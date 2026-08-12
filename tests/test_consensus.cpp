#include <cassert>
#include <iostream>
#include "dss/cluster.hpp"
#include "dss/logger.hpp"

using namespace dss;

void test_leader_election() {
    Logger::instance().set_quiet(true);
    Cluster cluster(3);

    // Initial state: no leader
    assert(!cluster.find_leader().has_value());

    // Tick timers to trigger election
    for (int i = 0; i < 5; ++i) {
        cluster.tick_all();
    }

    auto leader_opt = cluster.find_leader();
    assert(leader_opt.has_value());
    NodeId leader_id = leader_opt.value();
    assert(leader_id >= 1 && leader_id <= 3);

    auto leader_node = cluster.get_node(leader_id);
    assert(leader_node->role() == NodeRole::Leader);
}

void test_vote_rejection_lower_term() {
    Cluster cluster(3);
    for (int i = 0; i < 5; ++i) cluster.tick_all();
    auto node = cluster.get_node(1);

    RequestVoteArgs old_args{.term = 0, .candidate_id = 2, .last_log_index = 0, .last_log_term = 0};
    auto reply = node->handle_request_vote(old_args);
    assert(!reply.vote_granted);
}

int main() {
    std::cout << "Running Consensus tests...\n";
    test_leader_election();
    test_vote_rejection_lower_term();
    std::cout << "Consensus tests PASSED!\n";
    return 0;
}
