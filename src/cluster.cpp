#include "dss/cluster.hpp"
#include "dss/network.hpp"
#include "dss/logger.hpp"

namespace dss {

Cluster::Cluster(std::size_t num_nodes) {
    initialize(num_nodes);
}

void Cluster::initialize(std::size_t num_nodes) {
    std::lock_guard<std::mutex> lock(mutex_);
    nodes_.clear();
    for (std::size_t i = 1; i <= num_nodes; ++i) {
        NodeId id = static_cast<NodeId>(i);
        nodes_[id] = std::make_shared<Node>(id, this);
        std::uint32_t rack_id = (i <= 3) ? 1 : 2; // Racks: Nodes 1-3 in Rack 1, 4+ in Rack 2
        quorum_engine_.register_node(id, rack_id);
    }
}

std::shared_ptr<Node> Cluster::get_node(NodeId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodes_.find(id);
    if (it != nodes_.end()) {
        return it->second;
    }
    return nullptr;
}

std::size_t Cluster::node_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return nodes_.size();
}

std::optional<RequestVoteReply> Cluster::send_request_vote(NodeId from, NodeId to, const RequestVoteArgs& args) {
    if (!NetworkSimulator::instance().is_reachable(from, to) || NetworkSimulator::instance().should_drop_packet()) {
        return std::nullopt;
    }

    auto target_node = get_node(to);
    if (!target_node || !target_node->is_alive()) {
        return std::nullopt;
    }

    return target_node->handle_request_vote(args);
}

std::optional<AppendEntriesReply> Cluster::send_append_entries(NodeId from, NodeId to, const AppendEntriesArgs& args) {
    if (!NetworkSimulator::instance().is_reachable(from, to) || NetworkSimulator::instance().should_drop_packet()) {
        return std::nullopt;
    }

    auto target_node = get_node(to);
    if (!target_node || !target_node->is_alive()) {
        return std::nullopt;
    }

    return target_node->handle_append_entries(args);
}

ClientWriteResponse Cluster::write(const std::string& key, const std::string& value) {
    auto leader_id_opt = find_leader();
    if (!leader_id_opt.has_value()) {
        return ClientWriteResponse{
            .success = false,
            .leader_id = 0,
            .error_message = "No active leader found in cluster",
            .latency_us = 0
        };
    }

    auto leader_node = get_node(leader_id_opt.value());
    if (!leader_node) {
        return ClientWriteResponse{
            .success = false,
            .leader_id = 0,
            .error_message = "Leader node not found",
            .latency_us = 0
        };
    }

    return leader_node->client_write(key, value);
}

ClientReadResponse Cluster::read(const std::string& key) {
    auto leader_id_opt = find_leader();
    NodeId target_id = leader_id_opt.value_or(1);

    auto target_node = get_node(target_id);
    if (!target_node) {
        return ClientReadResponse{
            .success = false,
            .value = std::nullopt,
            .leader_id = 0,
            .error_message = "Target read node not found",
            .latency_us = 0
        };
    }

    return target_node->client_read(key);
}

void Cluster::kill_node(NodeId id) {
    auto node = get_node(id);
    if (node) {
        node->crash();
        NetworkSimulator::instance().isolate_node(id);
    }
}

void Cluster::recover_node(NodeId id) {
    auto node = get_node(id);
    if (node) {
        NetworkSimulator::instance().unisolate_node(id);
        node->recover();
    }
}

void Cluster::partition(const std::set<NodeId>& groupA, const std::set<NodeId>& groupB) {
    NetworkSimulator::instance().partition(groupA, groupB);
}

void Cluster::heal() {
    NetworkSimulator::instance().heal_partitions();
}

void Cluster::tick_all() {
    std::vector<std::shared_ptr<Node>> current_nodes;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [id, node] : nodes_) {
            current_nodes.push_back(node);
        }
    }
    for (const auto& node : current_nodes) {
        node->tick_timer();
    }
}

std::optional<NodeId> Cluster::find_leader() const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [id, node] : nodes_) {
        if (node->role() == NodeRole::Leader && node->is_alive()) {
            return id;
        }
    }
    return std::nullopt;
}

std::vector<NodeStatus> Cluster::get_cluster_status() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<NodeStatus> statuses;
    for (const auto& [id, node] : nodes_) {
        statuses.push_back(node->get_status());
    }
    return statuses;
}

} // namespace dss
