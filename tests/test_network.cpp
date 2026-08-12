#include <cassert>
#include <iostream>
#include <set>
#include "dss/network.hpp"

using namespace dss;

void test_network_isolation_and_partitions() {
    auto& net = NetworkSimulator::instance();
    net.reset();

    assert(net.is_reachable(1, 2));
    assert(net.is_reachable(2, 1));

    net.isolate_node(2);
    assert(!net.is_reachable(1, 2));
    assert(!net.is_reachable(2, 3));
    assert(net.is_reachable(1, 3));

    net.unisolate_node(2);
    assert(net.is_reachable(1, 2));

    std::set<NodeId> g1 = {1, 2};
    std::set<NodeId> g2 = {3, 4, 5};
    net.partition(g1, g2);

    assert(net.is_reachable(1, 2));
    assert(net.is_reachable(3, 4));
    assert(!net.is_reachable(1, 3));
    assert(!net.is_reachable(2, 5));

    net.heal_partitions();
    assert(net.is_reachable(1, 3));
    assert(net.is_reachable(2, 5));
}

int main() {
    std::cout << "Running NetworkSimulator tests...\n";
    test_network_isolation_and_partitions();
    std::cout << "NetworkSimulator tests PASSED!\n";
    return 0;
}
