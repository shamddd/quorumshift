#ifndef DSS_QUORUM_ENGINE_HPP
#define DSS_QUORUM_ENGINE_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "dss/common.hpp"

namespace dss {

struct NodeMetric {
    NodeId id{0};
    std::uint32_t rack_id{0};
    double rtt_ms{2.0};
    double packet_loss_rate{0.0};
    bool is_healthy{true};
};

enum class QuorumMode {
    StaticMajority,    // Baseline B0: Fixed N/2 + 1
    StaticFlexible,    // Baseline B1: Fixed asymmetric quorums
    QuorumShiftDynamic // Proposed QuorumShift: Continuous dynamic weight adaptation
};

class QuorumEngine {
public:
    explicit QuorumEngine(QuorumMode mode = QuorumMode::QuorumShiftDynamic);

    void set_mode(QuorumMode mode);
    QuorumMode get_mode() const;

    void register_node(NodeId id, std::uint32_t rack_id);
    void update_telemetry(NodeId id, double rtt_ms, double packet_loss_rate, bool is_healthy);

    double calculate_node_weight(NodeId id) const;
    bool is_quorum_satisfied(const std::vector<NodeId>& acked_nodes, std::size_t total_nodes) const;

    std::uint32_t get_node_rack(NodeId id) const;
    std::unordered_map<NodeId, double> get_all_weights() const;

private:
    QuorumMode mode_{QuorumMode::QuorumShiftDynamic};
    mutable std::mutex mutex_;
    std::unordered_map<NodeId, NodeMetric> metrics_;
};

} // namespace dss

#endif // DSS_QUORUM_ENGINE_HPP
