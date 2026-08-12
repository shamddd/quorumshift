#include "dss/node.hpp"
#include "dss/cluster.hpp"
#include "dss/logger.hpp"
#include "dss/network.hpp"
#include <algorithm>

namespace dss {

Node::Node(NodeId id, Cluster* cluster)
    : id_(id), cluster_(cluster) {}

NodeRole Node::role() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return role_;
}

Term Node::current_term() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_term_;
}

NodeStatus Node::get_status() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return NodeStatus{
        .id = id_,
        .role = role_,
        .current_term = current_term_,
        .last_log_index = storage_.last_log_index(),
        .commit_index = commit_index_,
        .voted_for = voted_for_,
        .is_alive = alive_
    };
}

void Node::crash() {
    std::lock_guard<std::mutex> lock(mutex_);
    alive_ = false;
    role_ = NodeRole::Offline;
    Logger::instance().node_failure(id_);
}

void Node::recover() {
    std::lock_guard<std::mutex> lock(mutex_);
    alive_ = true;
    role_ = NodeRole::Follower;
    election_ticks_ = 0;
    voted_for_ = std::nullopt;
    Logger::instance().node_recovered(id_);
}

bool Node::is_alive() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return alive_;
}

void Node::convert_to_follower(Term term) {
    role_ = NodeRole::Follower;
    current_term_ = term;
    voted_for_ = std::nullopt;
    election_ticks_ = 0;
}

void Node::tick_timer() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!alive_) return;

    if (role_ == NodeRole::Leader) {
        heartbeat_ticks_++;
        if (heartbeat_ticks_ >= heartbeat_interval_) {
            heartbeat_ticks_ = 0;
            lock.unlock();
            send_heartbeats();
        }
    } else {
        election_ticks_++;
        if (election_ticks_ >= election_timeout_threshold_) {
            election_ticks_ = 0;
            lock.unlock();
            start_election();
        }
    }
}

void Node::start_election() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!alive_) return;

    role_ = NodeRole::Candidate;
    current_term_++;
    voted_for_ = id_;
    Term election_term = current_term_;
    LogIndex last_log_idx = storage_.last_log_index();
    Term last_log_t = storage_.last_log_term();

    std::size_t votes_granted = 1;
    std::size_t total_nodes = cluster_->node_count();
    std::size_t quorum = (total_nodes / 2) + 1;

    lock.unlock();

    RequestVoteArgs args{
        .term = election_term,
        .candidate_id = id_,
        .last_log_index = last_log_idx,
        .last_log_term = last_log_t
    };

    for (NodeId peer_id = 1; peer_id <= static_cast<NodeId>(total_nodes); ++peer_id) {
        if (peer_id == id_) continue;

        auto reply_opt = cluster_->send_request_vote(id_, peer_id, args);
        if (reply_opt.has_value() && reply_opt->vote_granted) {
            votes_granted++;
        }
    }

    lock.lock();
    if (role_ == NodeRole::Candidate && current_term_ == election_term) {
        if (votes_granted >= quorum) {
            role_ = NodeRole::Leader;
            Logger::instance().leader_elected(id_, current_term_);

            // Initialize nextIndex and matchIndex for all peers
            for (NodeId peer_id = 1; peer_id <= static_cast<NodeId>(total_nodes); ++peer_id) {
                if (peer_id != id_) {
                    next_index_[peer_id] = storage_.last_log_index() + 1;
                    match_index_[peer_id] = 0;
                }
            }
        }
    }
}

void Node::send_heartbeats() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!alive_ || role_ != NodeRole::Leader) return;

    Term term = current_term_;
    LogIndex commit_idx = commit_index_;
    std::size_t total_nodes = cluster_->node_count();

    lock.unlock();

    for (NodeId peer_id = 1; peer_id <= static_cast<NodeId>(total_nodes); ++peer_id) {
        if (peer_id == id_) continue;

        LogIndex prev_idx = storage_.last_log_index();
        Term prev_t = storage_.last_log_term();

        AppendEntriesArgs args{
            .term = term,
            .leader_id = id_,
            .prev_log_index = prev_idx,
            .prev_log_term = prev_t,
            .entries = {},
            .leader_commit = commit_idx
        };

        cluster_->send_append_entries(id_, peer_id, args);
    }
}

RequestVoteReply Node::handle_request_vote(const RequestVoteArgs& args) {
    std::lock_guard<std::mutex> lock(mutex_);
    RequestVoteReply reply{.term = current_term_, .vote_granted = false};

    if (!alive_) return reply;

    if (args.term < current_term_) {
        return reply;
    }

    if (args.term > current_term_) {
        convert_to_follower(args.term);
    }

    if (args.term == current_term_ && (!voted_for_.has_value() || voted_for_.value() == args.candidate_id)) {
        LogIndex last_idx = storage_.last_log_index();
        Term last_t = storage_.last_log_term();

        if (args.last_log_term > last_t || 
           (args.last_log_term == last_t && args.last_log_index >= last_idx)) {
            voted_for_ = args.candidate_id;
            reply.vote_granted = true;
            election_ticks_ = 0;
        }
    }

    reply.term = current_term_;
    return reply;
}

AppendEntriesReply Node::handle_append_entries(const AppendEntriesArgs& args) {
    std::lock_guard<std::mutex> lock(mutex_);
    AppendEntriesReply reply{.term = current_term_, .success = false};

    if (!alive_) return reply;

    if (args.term < current_term_) {
        return reply;
    }

    if (args.term > current_term_ || role_ == NodeRole::Candidate) {
        convert_to_follower(args.term);
    }

    election_ticks_ = 0; // Reset election timer on receiving heartbeat

    // Append new entries if any
    for (const auto& entry : args.entries) {
        storage_.append_log(entry.term, entry.type, entry.key, entry.value);
    }

    if (args.leader_commit > commit_index_) {
        commit_index_ = std::min(args.leader_commit, storage_.last_log_index());
        storage_.apply_committed(commit_index_);
    }

    reply.success = true;
    reply.term = current_term_;
    reply.match_index = storage_.last_log_index();
    return reply;
}

ClientWriteResponse Node::client_write(const std::string& key, const std::string& value) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::unique_lock<std::mutex> lock(mutex_);

    ClientWriteResponse resp;
    if (!alive_) {
        resp.success = false;
        resp.error_message = "Node is offline";
        return resp;
    }

    if (role_ != NodeRole::Leader) {
        resp.success = false;
        resp.leader_id = cluster_->find_leader().value_or(0);
        resp.error_message = "Not the leader";
        return resp;
    }

    Term term = current_term_;
    LogIndex index = storage_.append_log(term, CommandType::Put, key, value);

    std::vector<NodeId> acked_nodes = {id_};
    std::size_t total_nodes = cluster_->node_count();

    lock.unlock();

    AppendEntriesArgs args{
        .term = term,
        .leader_id = id_,
        .prev_log_index = index - 1,
        .prev_log_term = term,
        .entries = {LogEntry{.term = term, .index = index, .type = CommandType::Put, .key = key, .value = value}},
        .leader_commit = commit_index_
    };

    for (NodeId peer_id = 1; peer_id <= static_cast<NodeId>(total_nodes); ++peer_id) {
        if (peer_id == id_) continue;

        auto reply_opt = cluster_->send_append_entries(id_, peer_id, args);
        if (reply_opt.has_value() && reply_opt->success) {
            acked_nodes.push_back(peer_id);
        }
    }

    lock.lock();
    bool quorum_satisfied = cluster_->quorum_engine().is_quorum_satisfied(acked_nodes, total_nodes);
    if (quorum_satisfied) {
        commit_index_ = index;
        storage_.apply_committed(commit_index_);
        Logger::instance().replication_success(id_, index, acked_nodes.size(), total_nodes);
        resp.success = true;
    } else {
        resp.success = false;
        resp.error_message = "Quorum lost during replication";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    resp.latency_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    resp.leader_id = id_;
    return resp;
}

ClientReadResponse Node::client_read(const std::string& key) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(mutex_);

    ClientReadResponse resp;
    if (!alive_) {
        resp.success = false;
        resp.error_message = "Node is offline";
        return resp;
    }

    resp.value = storage_.get(key);
    resp.success = true;
    resp.leader_id = (role_ == NodeRole::Leader) ? id_ : cluster_->find_leader().value_or(0);

    auto end_time = std::chrono::high_resolution_clock::now();
    resp.latency_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    return resp;
}

} // namespace dss
