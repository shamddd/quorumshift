#ifndef DSS_ADAPTIVE_REPLICA_HPP
#define DSS_ADAPTIVE_REPLICA_HPP

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>
#include <optional>
#include <algorithm>
#include "dss/common.hpp"

namespace dss {

struct ReplicaTelemetry {
    NodeId id{0};
    std::uint32_t rack_id{0};
    double rtt_ms{2.0};
    double packet_loss_rate{0.0};
    double cpu_utilization{0.0};
    bool is_healthy{true};
    std::uint64_t uncommitted_backlog{0};
};

enum class AdaptationStrategy {
    FixedR3,                  // Baseline B0: Fixed R=3, Majority Quorum
    FixedR5,                  // Baseline B1: Fixed R=5, Majority Quorum
    RandomPlacement,          // Baseline B2: Random 3-replica placement
    LatencyAwarePlacement,    // Baseline B3: Lowest RTT placement
    FailureDomainStatic,      // Baseline B4: Static 1-per-rack placement
    AdaptiveReplicaProposed   // Proposed B5: Dynamic multi-signal joint adaptation
};

struct AdaptationDecision {
    std::size_t replication_factor{3};
    std::vector<NodeId> active_replicas;
    std::size_t required_quorum{2};
    std::string decision_reason;
};

class AdaptiveReplicaController {
public:
    explicit AdaptiveReplicaController(AdaptationStrategy strategy = AdaptationStrategy::AdaptiveReplicaProposed);

    void set_strategy(AdaptationStrategy strategy);
    AdaptationStrategy get_strategy() const;

    void register_node(NodeId id, std::uint32_t rack_id);
    void update_telemetry(NodeId id, double rtt_ms, double packet_loss_rate, double cpu_util, bool is_healthy, std::uint64_t backlog);
    void update_workload_signals(double read_ratio, double write_ratio, double key_skew_theta);

    AdaptationDecision evaluate_policy(std::size_t total_cluster_nodes) const;
    bool is_quorum_satisfied(const std::vector<NodeId>& acked_nodes, const AdaptationDecision& decision) const;

    std::unordered_map<NodeId, ReplicaTelemetry> get_all_telemetry() const;

private:
    AdaptationStrategy strategy_{AdaptationStrategy::AdaptiveReplicaProposed};
    mutable std::mutex mutex_;
    std::unordered_map<NodeId, ReplicaTelemetry> telemetry_;
    
    double read_ratio_{0.5};
    double write_ratio_{0.5};
    double key_skew_theta_{0.0};
};

} // namespace dss

#endif // DSS_ADAPTIVE_REPLICA_HPP
