#include <cassert>
#include <iostream>
#include "dss/adaptive_replica.hpp"
#include "dss/cluster.hpp"

void test_adaptive_replica_baselines() {
    dss::AdaptiveReplicaController ctrl(dss::AdaptationStrategy::FixedR3);
    for (std::size_t i = 1; i <= 5; ++i) {
        ctrl.register_node(static_cast<dss::NodeId>(i), static_cast<std::uint32_t>((i - 1) % 3 + 1));
    }

    auto decision_r3 = ctrl.evaluate_policy(5);
    assert(decision_r3.replication_factor == 3);
    assert(decision_r3.required_quorum == 2);
    assert(decision_r3.active_replicas.size() == 5);

    ctrl.set_strategy(dss::AdaptationStrategy::FixedR5);
    auto decision_r5 = ctrl.evaluate_policy(5);
    assert(decision_r5.replication_factor == 5);
    assert(decision_r5.required_quorum == 3);

    std::cout << "[PASS] test_adaptive_replica_baselines" << std::endl;
}

void test_adaptive_replica_dynamic_scaling() {
    dss::AdaptiveReplicaController ctrl(dss::AdaptationStrategy::AdaptiveReplicaProposed);
    for (std::size_t i = 1; i <= 7; ++i) {
        ctrl.register_node(static_cast<dss::NodeId>(i), static_cast<std::uint32_t>((i - 1) % 3 + 1));
    }

    // Steady state low failure
    auto d1 = ctrl.evaluate_policy(7);
    assert(d1.replication_factor == 3);
    assert(d1.required_quorum == 2);

    // High unhealthy ratio (>0.5)
    for (std::size_t i = 1; i <= 4; ++i) {
        ctrl.update_telemetry(static_cast<dss::NodeId>(i), 999.0, 1.0, 0.0, false, 0);
    }
    auto d2 = ctrl.evaluate_policy(7);
    assert(d2.replication_factor >= 3);
    assert(d2.required_quorum >= 2);

    std::cout << "[PASS] test_adaptive_replica_dynamic_scaling" << std::endl;
}

void test_adaptive_replica_quorum_check() {
    dss::AdaptiveReplicaController ctrl(dss::AdaptationStrategy::AdaptiveReplicaProposed);
    for (std::size_t i = 1; i <= 5; ++i) {
        ctrl.register_node(static_cast<dss::NodeId>(i), static_cast<std::uint32_t>((i - 1) % 3 + 1));
    }

    auto decision = ctrl.evaluate_policy(5);
    std::vector<dss::NodeId> acks_valid = {1, 2};
    std::vector<dss::NodeId> acks_insufficient = {1};

    assert(ctrl.is_quorum_satisfied(acks_valid, decision) == true);
    assert(ctrl.is_quorum_satisfied(acks_insufficient, decision) == false);

    std::cout << "[PASS] test_adaptive_replica_quorum_check" << std::endl;
}

int main() {
    test_adaptive_replica_baselines();
    test_adaptive_replica_dynamic_scaling();
    test_adaptive_replica_quorum_check();
    std::cout << "All AdaptiveReplica tests passed successfully!" << std::endl;
    return 0;
}
