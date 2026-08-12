#ifndef DSS_NETWORK_HPP
#define DSS_NETWORK_HPP

#include <vector>
#include <mutex>
#include <set>
#include <random>
#include "dss/common.hpp"

namespace dss {

class NetworkSimulator {
public:
    static NetworkSimulator& instance();

    void set_latency_ms(std::uint32_t latency_ms);
    void set_drop_rate(double drop_rate);

    void isolate_node(NodeId node_id);
    void unisolate_node(NodeId node_id);
    
    void partition(const std::set<NodeId>& groupA, const std::set<NodeId>& groupB);
    void heal_partitions();

    bool is_reachable(NodeId from, NodeId to);
    bool should_drop_packet();

    void reset();

private:
    NetworkSimulator() = default;

    std::mutex mutex_;
    std::uint32_t latency_ms_{2};
    double drop_rate_{0.0};
    std::set<NodeId> isolated_nodes_;
    std::set<std::pair<NodeId, NodeId>> partitioned_pairs_;
    
    std::mt19937 rng_{42};
};

} // namespace dss

#endif // DSS_NETWORK_HPP
