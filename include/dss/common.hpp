#ifndef DSS_COMMON_HPP
#define DSS_COMMON_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <optional>

namespace dss {

using NodeId = std::uint32_t;
using Term = std::uint64_t;
using LogIndex = std::uint64_t;

enum class NodeRole {
    Follower,
    Candidate,
    Leader,
    Offline
};

inline std::string to_string(NodeRole role) {
    switch (role) {
        case NodeRole::Follower: return "Follower";
        case NodeRole::Candidate: return "Candidate";
        case NodeRole::Leader: return "Leader";
        case NodeRole::Offline: return "Offline";
    }
    return "Unknown";
}

enum class CommandType {
    Put,
    Delete,
    NoOp
};

struct LogEntry {
    Term term{0};
    LogIndex index{0};
    CommandType type{CommandType::Put};
    std::string key;
    std::string value;
    std::uint64_t timestamp_ms{0};
};

struct RequestVoteArgs {
    Term term{0};
    NodeId candidate_id{0};
    LogIndex last_log_index{0};
    Term last_log_term{0};
};

struct RequestVoteReply {
    Term term{0};
    bool vote_granted{false};
};

struct AppendEntriesArgs {
    Term term{0};
    NodeId leader_id{0};
    LogIndex prev_log_index{0};
    Term prev_log_term{0};
    std::vector<LogEntry> entries;
    LogIndex leader_commit{0};
};

struct AppendEntriesReply {
    Term term{0};
    bool success{false};
    LogIndex match_index{0};
};

struct ClientWriteRequest {
    std::string key;
    std::string value;
};

struct ClientWriteResponse {
    bool success{false};
    NodeId leader_id{0};
    std::string error_message;
    std::uint64_t latency_us{0};
};

struct ClientReadRequest {
    std::string key;
    bool require_linearizable{true};
};

struct ClientReadResponse {
    bool success{false};
    std::optional<std::string> value;
    NodeId leader_id{0};
    std::string error_message;
    std::uint64_t latency_us{0};
};

struct NodeStatus {
    NodeId id{0};
    NodeRole role{NodeRole::Offline};
    Term current_term{0};
    LogIndex last_log_index{0};
    LogIndex commit_index{0};
    std::optional<NodeId> voted_for;
    bool is_alive{false};
};

} // namespace dss

#endif // DSS_COMMON_HPP
