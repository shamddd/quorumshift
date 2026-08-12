#include "dss/network.hpp"

namespace dss {

NetworkSimulator& NetworkSimulator::instance() {
    static NetworkSimulator inst;
    return inst;
}

void NetworkSimulator::set_latency_ms(std::uint32_t latency_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    latency_ms_ = latency_ms;
}

void NetworkSimulator::set_drop_rate(double drop_rate) {
    std::lock_guard<std::mutex> lock(mutex_);
    drop_rate_ = drop_rate;
}

void NetworkSimulator::isolate_node(NodeId node_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    isolated_nodes_.insert(node_id);
}

void NetworkSimulator::unisolate_node(NodeId node_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    isolated_nodes_.erase(node_id);
}

void NetworkSimulator::partition(const std::set<NodeId>& groupA, const std::set<NodeId>& groupB) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (NodeId a : groupA) {
        for (NodeId b : groupB) {
            partitioned_pairs_.insert({a, b});
            partitioned_pairs_.insert({b, a});
        }
    }
}

void NetworkSimulator::heal_partitions() {
    std::lock_guard<std::mutex> lock(mutex_);
    partitioned_pairs_.clear();
    isolated_nodes_.clear();
}

bool NetworkSimulator::is_reachable(NodeId from, NodeId to) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isolated_nodes_.count(from) || isolated_nodes_.count(to)) {
        return false;
    }
    if (partitioned_pairs_.count({from, to})) {
        return false;
    }
    return true;
}

bool NetworkSimulator::should_drop_packet() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (drop_rate_ <= 0.0) return false;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng_) < drop_rate_;
}

void NetworkSimulator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    latency_ms_ = 2;
    drop_rate_ = 0.0;
    isolated_nodes_.clear();
    partitioned_pairs_.clear();
}

} // namespace dss
