#include "dss/quorum_engine.hpp"
#include <cmath>
#include <numeric>

namespace dss {

QuorumEngine::QuorumEngine(QuorumMode mode) : mode_(mode) {}

void QuorumEngine::set_mode(QuorumMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    mode_ = mode;
}

QuorumMode QuorumEngine::get_mode() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return mode_;
}

void QuorumEngine::register_node(NodeId id, std::uint32_t rack_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_[id] = NodeMetric{.id = id, .rack_id = rack_id, .rtt_ms = 2.0, .packet_loss_rate = 0.0, .is_healthy = true};
}

void QuorumEngine::update_telemetry(NodeId id, double rtt_ms, double packet_loss_rate, bool is_healthy) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (metrics_.find(id) != metrics_.end()) {
        metrics_[id].rtt_ms = rtt_ms;
        metrics_[id].packet_loss_rate = packet_loss_rate;
        metrics_[id].is_healthy = is_healthy;
    }
}

double QuorumEngine::calculate_node_weight(NodeId id) const {
    auto it = metrics_.find(id);
    if (it == metrics_.end() || !it->second.is_healthy) {
        return 0.0;
    }

    if (mode_ == QuorumMode::StaticMajority || mode_ == QuorumMode::StaticFlexible) {
        return 1.0;
    }

    // QuorumShift Dynamic Weighting Formula:
    // W_i = 1.0 / (1.0 + 0.05 * RTT_ms + 2.0 * LossRate)
    double rtt_penalty = 0.05 * std::max(0.0, it->second.rtt_ms - 2.0);
    double loss_penalty = 2.0 * std::clamp(it->second.packet_loss_rate, 0.0, 1.0);
    
    return 1.0 / (1.0 + rtt_penalty + loss_penalty);
}

bool QuorumEngine::is_quorum_satisfied(const std::vector<NodeId>& acked_nodes, std::size_t total_nodes) const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (mode_ == QuorumMode::StaticMajority) {
        std::size_t static_threshold = (total_nodes / 2) + 1;
        return acked_nodes.size() >= static_threshold;
    }

    if (mode_ == QuorumMode::StaticFlexible) {
        // Phase 2 flexible quorum requires 2 acknowledgements for 5-node cluster
        std::size_t flexible_threshold = std::max<std::size_t>(2, total_nodes / 2);
        return acked_nodes.size() >= flexible_threshold;
    }

    // QuorumShift Dynamic Mode:
    double total_possible_weight = 0.0;
    for (const auto& [id, metric] : metrics_) {
        if (metric.is_healthy) {
            total_possible_weight += 1.0; // Base total possible capacity
        }
    }

    double acked_weight = 0.0;
    for (NodeId id : acked_nodes) {
        auto it = metrics_.find(id);
        if (it != metrics_.end() && it->second.is_healthy) {
            // Dynamic weight calculation
            double rtt_penalty = 0.05 * std::max(0.0, it->second.rtt_ms - 2.0);
            double loss_penalty = 2.0 * std::clamp(it->second.packet_loss_rate, 0.0, 1.0);
            double weight = 1.0 / (1.0 + rtt_penalty + loss_penalty);
            acked_weight += weight;
        }
    }

    // Require > 35% of total dynamic cluster weight capacity for Phase 2 commit
    double dynamic_threshold = 0.35 * total_possible_weight;
    return acked_weight >= dynamic_threshold && !acked_nodes.empty();
}

std::uint32_t QuorumEngine::get_node_rack(NodeId id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(id);
    if (it != metrics_.end()) {
        return it->second.rack_id;
    }
    return 1;
}

std::unordered_map<NodeId, double> QuorumEngine::get_all_weights() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_map<NodeId, double> weights;
    for (const auto& [id, metric] : metrics_) {
        if (!metric.is_healthy) {
            weights[id] = 0.0;
        } else {
            double rtt_penalty = 0.05 * std::max(0.0, metric.rtt_ms - 2.0);
            double loss_penalty = 2.0 * std::clamp(metric.packet_loss_rate, 0.0, 1.0);
            weights[id] = 1.0 / (1.0 + rtt_penalty + loss_penalty);
        }
    }
    return weights;
}

} // namespace dss
