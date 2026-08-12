#ifndef DSS_CLUSTER_HPP
#define DSS_CLUSTER_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <set>
#include <optional>
#include "dss/common.hpp"
#include "dss/node.hpp"

namespace dss {

class Cluster {
public:
    explicit Cluster(std::size_t num_nodes = 3);
    ~Cluster() = default;

    void initialize(std::size_t num_nodes);

    std::shared_ptr<Node> get_node(NodeId id);
    std::size_t node_count() const;

    // RPC dispatch with network simulator checks
    std::optional<RequestVoteReply> send_request_vote(NodeId from, NodeId to, const RequestVoteArgs& args);
    std::optional<AppendEntriesReply> send_append_entries(NodeId from, NodeId to, const AppendEntriesArgs& args);

    // Cluster control commands
    ClientWriteResponse write(const std::string& key, const std::string& value);
    ClientReadResponse read(const std::string& key);

    void kill_node(NodeId id);
    void recover_node(NodeId id);
    
    void partition(const std::set<NodeId>& groupA, const std::set<NodeId>& groupB);
    void heal();

    void tick_all();
    std::optional<NodeId> find_leader() const;
    std::vector<NodeStatus> get_cluster_status() const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<NodeId, std::shared_ptr<Node>> nodes_;
};

} // namespace dss

#endif // DSS_CLUSTER_HPP
