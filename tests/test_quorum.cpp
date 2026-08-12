#include <cassert>
#include <iostream>
#include "dss/quorum_engine.hpp"

using namespace dss;

void test_static_majority_quorum() {
    QuorumEngine engine(QuorumMode::StaticMajority);
    engine.register_node(1, 1);
    engine.register_node(2, 1);
    engine.register_node(3, 1);
    engine.register_node(4, 2);
    engine.register_node(5, 2);

    // 2 acks out of 5 -> Failure (Threshold is 3)
    std::vector<NodeId> acks_2 = {1, 2};
    assert(!engine.is_quorum_satisfied(acks_2, 5));

    // 3 acks out of 5 -> Success
    std::vector<NodeId> acks_3 = {1, 2, 3};
    assert(engine.is_quorum_satisfied(acks_3, 5));

    std::cout << "test_static_majority_quorum PASSED!\n";
}

void test_quorum_shift_dynamic_adaptation() {
    QuorumEngine engine(QuorumMode::QuorumShiftDynamic);
    engine.register_node(1, 1); // Rack 1
    engine.register_node(2, 1); // Rack 1
    engine.register_node(3, 1); // Rack 1
    engine.register_node(4, 2); // Rack 2
    engine.register_node(5, 2); // Rack 2

    // Degrade Rack 2 nodes with +100ms RTT latency
    engine.update_telemetry(4, 102.0, 0.2, true);
    engine.update_telemetry(5, 102.0, 0.2, true);

    // Local Rack 1 nodes (1, 2, 3) remain fast (2ms RTT)
    engine.update_telemetry(1, 2.0, 0.0, true);
    engine.update_telemetry(2, 2.0, 0.0, true);
    engine.update_telemetry(3, 2.0, 0.0, true);

    // Ack from fast nodes (1, 2) satisfies dynamic weight threshold (> 45% capacity)
    std::vector<NodeId> fast_acks = {1, 2};
    assert(engine.is_quorum_satisfied(fast_acks, 5));

    // Check dynamic weights
    auto weights = engine.get_all_weights();
    assert(weights[1] > weights[4]);
    assert(weights[2] > weights[5]);

    std::cout << "test_quorum_shift_dynamic_adaptation PASSED!\n";
}

int main() {
    std::cout << "Running QuorumEngine tests...\n";
    test_static_majority_quorum();
    test_quorum_shift_dynamic_adaptation();
    std::cout << "QuorumEngine tests PASSED!\n";
    return 0;
}
