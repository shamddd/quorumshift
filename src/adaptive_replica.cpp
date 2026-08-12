#include "dss/adaptive_replica.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace dss {

AdaptiveReplicaController::AdaptiveReplicaController(AdaptationStrategy strategy)
    : strategy_(strategy) {}

void AdaptiveReplicaController::set_strategy(AdaptationStrategy strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    strategy_ = strategy;
}

AdaptationStrategy AdaptiveReplicaController::get_strategy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return strategy_;
}

void AdaptiveReplicaController::register_node(NodeId id, std::uint32_t rack_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    telemetry_[id] = ReplicaTelemetry{
        .id = id,
        .rack_id = rack_id,
        .rtt_ms = 2.0,
        .packet_loss_rate = 0.0,
        .cpu_utilization = 10.0,
        .is_healthy = true,
        .uncommitted_backlog = 0
    };
}

void AdaptiveReplicaController::update_telemetry(NodeId id, double rtt_ms, double packet_loss_rate, double cpu_util, bool is_healthy, std::uint64_t backlog) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (telemetry_.find(id) != telemetry_.end()) {
        telemetry_[id].rtt_ms = rtt_ms;
        telemetry_[id].packet_loss_rate = packet_loss_rate;
        telemetry_[id].cpu_utilization = cpu_util;
        telemetry_[id].is_healthy = is_healthy;
        telemetry_[id].uncommitted_backlog = backlog;
    }
}

void AdaptiveReplicaController::update_workload_signals(double read_ratio, double write_ratio, double key_skew_theta) {
    std::lock_guard<std::mutex> lock(mutex_);
    read_ratio_ = read_ratio;
    write_ratio_ = write_ratio;
    key_skew_theta_ = key_skew_theta;
}

AdaptationDecision AdaptiveReplicaController::evaluate_policy(std::size_t total_cluster_nodes) const {
    std::lock_guard<std::mutex> lock(mutex_);
    AdaptationDecision decision;

    std::vector<NodeId> healthy_nodes;
    for (const auto& [id, t] : telemetry_) {
        if (t.is_healthy) {
            healthy_nodes.push_back(id);
        }
    }
    std::sort(healthy_nodes.begin(), healthy_nodes.end());

    if (strategy_ == AdaptationStrategy::FixedR3) {
        decision.replication_factor = 3;
        decision.required_quorum = 2;
        decision.active_replicas = healthy_nodes;
        decision.decision_reason = "Fixed R=3 Baseline";
        return decision;
    }

    if (strategy_ == AdaptationStrategy::FixedR5) {
        decision.replication_factor = 5;
        decision.required_quorum = 3;
        decision.active_replicas = healthy_nodes;
        decision.decision_reason = "Fixed R=5 Baseline";
        return decision;
    }

    if (strategy_ == AdaptationStrategy::RandomPlacement) {
        decision.replication_factor = 3;
        decision.required_quorum = 2;
        decision.active_replicas = healthy_nodes;
        if (decision.active_replicas.size() > 3) {
            decision.active_replicas.resize(3);
        }
        decision.decision_reason = "Random Placement Baseline";
        return decision;
    }

    if (strategy_ == AdaptationStrategy::LatencyAwarePlacement) {
        decision.replication_factor = 3;
        decision.required_quorum = 2;
        std::vector<NodeId> sorted_by_rtt = healthy_nodes;
        std::sort(sorted_by_rtt.begin(), sorted_by_rtt.end(), [this](NodeId a, NodeId b) {
            return telemetry_.at(a).rtt_ms < telemetry_.at(b).rtt_ms;
        });
        if (sorted_by_rtt.size() > 3) sorted_by_rtt.resize(3);
        decision.active_replicas = sorted_by_rtt;
        decision.decision_reason = "Lowest RTT Placement Baseline";
        return decision;
    }

    if (strategy_ == AdaptationStrategy::FailureDomainStatic) {
        decision.replication_factor = 3;
        decision.required_quorum = 2;
        // Group by rack_id
        std::unordered_map<std::uint32_t, std::vector<NodeId>> rack_map;
        for (NodeId id : healthy_nodes) {
            rack_map[telemetry_.at(id).rack_id].push_back(id);
        }
        std::vector<NodeId> placed;
        for (auto& [rack, nodes] : rack_map) {
            if (!nodes.empty() && placed.size() < 3) {
                placed.push_back(nodes.front());
            }
        }
        while (placed.size() < 3 && placed.size() < healthy_nodes.size()) {
            for (NodeId id : healthy_nodes) {
                if (std::find(placed.begin(), placed.end(), id) == placed.end()) {
                    placed.push_back(id);
                    if (placed.size() == 3) break;
                }
            }
        }
        decision.active_replicas = placed;
        decision.decision_reason = "Failure Domain Static Baseline";
        return decision;
    }

    // AdaptiveReplica Proposed Strategy
    // 1. Calculate dynamic replication factor R
    double unhealthy_ratio = 1.0 - (static_cast<double>(healthy_nodes.size()) / std::max<std::size_t>(1, total_cluster_nodes));
    double write_pressure = write_ratio_;
    double skew_factor = key_skew_theta_;

    std::size_t calc_r = 3;
    if (unhealthy_ratio > 0.3 || write_pressure > 0.7 || skew_factor > 1.0) {
        calc_r = 5;
    }
    if (unhealthy_ratio > 0.5 && total_cluster_nodes >= 7) {
        calc_r = 7;
    }

    calc_r = std::min(calc_r, std::max<std::size_t>(3, healthy_nodes.size()));
    decision.replication_factor = calc_r;
    decision.required_quorum = (calc_r / 2) + 1;

    // 2. Select optimal active replica placement minimizing joint RTT and rack correlation
    std::vector<NodeId> candidates = healthy_nodes;
    std::sort(candidates.begin(), candidates.end(), [this](NodeId a, NodeId b) {
        const auto& ta = telemetry_.at(a);
        const auto& tb = telemetry_.at(b);
        double score_a = ta.rtt_ms + (ta.packet_loss_rate * 50.0) + (ta.cpu_utilization * 0.1);
        double score_b = tb.rtt_ms + (tb.packet_loss_rate * 50.0) + (tb.cpu_utilization * 0.1);
        return score_a < score_b;
    });

    if (candidates.size() > calc_r) {
        candidates.resize(calc_r);
    }
    decision.active_replicas = candidates;
    decision.decision_reason = "AdaptiveReplica Dynamic Joint Optimization";

    return decision;
}

bool AdaptiveReplicaController::is_quorum_satisfied(const std::vector<NodeId>& acked_nodes, const AdaptationDecision& decision) const {
    std::size_t valid_acks = 0;
    for (NodeId ack : acked_nodes) {
        if (std::find(decision.active_replicas.begin(), decision.active_replicas.end(), ack) != decision.active_replicas.end()) {
            valid_acks++;
        }
    }
    return valid_acks >= decision.required_quorum;
}

std::unordered_map<NodeId, ReplicaTelemetry> AdaptiveReplicaController::get_all_telemetry() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return telemetry_;
}

} // namespace dss
