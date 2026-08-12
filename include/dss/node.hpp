#ifndef DSS_NODE_HPP
#define DSS_NODE_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "dss/common.hpp"
#include "dss/storage.hpp"

namespace dss {

class Cluster;

class Node : public std::enable_shared_from_this<Node> {
public:
    Node(NodeId id, Cluster* cluster);

    NodeId id() const { return id_; }
    NodeRole role() const;
    Term current_term() const;
    NodeStatus get_status() const;

    // RPC Interface
    RequestVoteReply handle_request_vote(const RequestVoteArgs& args);
    AppendEntriesReply handle_append_entries(const AppendEntriesArgs& args);

    // Client operations
    ClientWriteResponse client_write(const std::string& key, const std::string& value);
    ClientReadResponse client_read(const std::string& key);

    // Cluster lifecycle & failure injection
    void tick_timer();
    void set_peers(const std::vector<NodeId>& peer_ids);
    void crash();
    void recover();
    bool is_alive() const;

private:
    void start_election();
    void send_heartbeats();
    void convert_to_follower(Term term);

    NodeId id_;
    Cluster* cluster_;
    
    mutable std::mutex mutex_;
    NodeRole role_{NodeRole::Follower};
    Term current_term_{0};
    std::optional<NodeId> voted_for_;
    bool alive_{true};

    StorageEngine storage_;
    LogIndex commit_index_{0};

    // Leader tracking maps per peer NodeId
    std::unordered_map<NodeId, LogIndex> next_index_;
    std::unordered_map<NodeId, LogIndex> match_index_;

    std::uint32_t election_ticks_{0};
    std::uint32_t election_timeout_threshold_{3};
    std::uint32_t heartbeat_ticks_{0};
    std::uint32_t heartbeat_interval_{1};
};

} // namespace dss

#endif // DSS_NODE_HPP
