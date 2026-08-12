#include <cassert>
#include <iostream>
#include "dss/cluster.hpp"
#include "dss/logger.hpp"

using namespace dss;

void test_cluster_failover_and_replication() {
    Logger::instance().set_quiet(true);
    Cluster cluster(5);

    // Initial ticks to elect leader
    for (int i = 0; i < 5; ++i) cluster.tick_all();

    auto leader_opt = cluster.find_leader();
    assert(leader_opt.has_value());
    NodeId leader1 = leader_opt.value();

    // Perform quorum write
    auto w_res = cluster.write("alpha", "beta");
    assert(w_res.success);
    assert(w_res.leader_id == leader1);

    // Read back
    auto r_res = cluster.read("alpha");
    assert(r_res.success);
    assert(r_res.value.has_value());
    assert(r_res.value.value() == "beta");

    // Fail leader
    cluster.kill_node(leader1);
    assert(!cluster.get_node(leader1)->is_alive());

    // Tick timers to elect new leader
    for (int i = 0; i < 5; ++i) cluster.tick_all();

    auto new_leader_opt = cluster.find_leader();
    assert(new_leader_opt.has_value());
    NodeId leader2 = new_leader_opt.value();
    assert(leader2 != leader1);

    // Write on new leader
    auto w_res2 = cluster.write("gamma", "delta");
    assert(w_res2.success);

    // Recover old leader
    cluster.recover_node(leader1);
    assert(cluster.get_node(leader1)->is_alive());

    // Verify read
    auto r_res2 = cluster.read("gamma");
    assert(r_res2.success);
    assert(r_res2.value.value() == "delta");
}

int main() {
    std::cout << "Running Cluster integration tests...\n";
    test_cluster_failover_and_replication();
    std::cout << "Cluster integration tests PASSED!\n";
    return 0;
}
