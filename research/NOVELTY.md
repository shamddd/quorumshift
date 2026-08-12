# Novelty Analysis & Hostile Peer Review — QuorumShift

**Author**: Sham Satish Thakare (`151498087+shamddd@users.noreply.github.com`)  
**Repository**: `distributed-storage-simulator` (`quorumshift`)

---

## 1. Technical Gap & Core Novelty

| Dimension | Existing Approaches (Raft, Paxos, Flexible Paxos) | Proposed QuorumShift |
| :--- | :--- | :--- |
| **Quorum Thresholds** | Static majority ($Q = \lfloor N/2 \rfloor + 1$) or static asymmetric allocation. | Dynamic telemetry-aware Phase 2 commit quorum allocation ($Q_2(t)$). |
| **Failure Domain Awareness** | Topology-agnostic node counts. | Explicit rack/availability-zone affinity matrix weighting. |
| **Transient Latency Jitter** | Leader blocks on slow nodes or triggers unnecessary election timeouts. | Dynamic weight shift downscales slow node weights without cluster reconfiguration. |
| **Reconfiguration Overhead** | Heavyweight joint consensus ($C_{\text{old}} \to C_{\text{new}}$) required for every adjustment. | Zero-reconfiguration transient weight shifting within safety invariant boundaries. |

---

## 2. Hostile Peer Reviewer Challenge (Novelty Attack)

### Attack Vector (Reviewer 2 - Systems Security & Consensus)
> *"The authors claim that adjusting quorum sizes is novel. However, Flexible Paxos (Howard et al., 2016) proved nearly a decade ago that Phase 2 commit quorums do not need to intersect with each other as long as Phase 1 election quorums intersect with Phase 2. Implementing Flexible Paxos in a C++ Raft simulator is a straightforward software engineering exercise, not an algorithmic computer science contribution."*

### Author Defense & Refined Scientific Gap
We explicitly acknowledge Flexible Paxos as our theoretical foundation. However, Flexible Paxos assumes **statically configured** asymmetric quorums established at system deployment. 

**Our Refined Research Contribution**:  
`QuorumShift` introduces a **continuous runtime weight adaptation algorithm** $W_i(t) = f(\text{RTT}_i, \text{Loss}_i, \text{Domain}_i)$ that computes the minimal valid Phase 2 commit quorum dynamically during log replication:
$$\sum_{i \in Q_2(t)} W_i(t) \ge \Theta_{\text{commit}}$$
subject to the non-negotiable safety constraint:
$$\forall Q_1, Q_2(t): Q_1 \cap Q_2(t) \neq \emptyset$$
This enables sub-millisecond adaptive latency optimization under asymmetric network degradation without triggering joint consensus protocol state transitions.

---

## 3. Testable Falsifiable Hypotheses

- **$H_1$ (Latency Resilience)**: Under $100\text{ms}$ asymmetric WAN latency injected into $40\%$ of cluster nodes, `QuorumShift` reduces P99 write commit latency by $\ge 30\%$ compared to static majority Raft, with zero stale-read or linearizability violations.
- **$H_2$ (Correlated Fault Availability)**: Under simultaneous failure of all nodes within a single rack failure domain, `QuorumShift` maintains $\ge 99.9\%$ write transaction availability, whereas static majority Raft suffers complete execution stalls.
