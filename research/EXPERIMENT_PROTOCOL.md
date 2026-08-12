# Frozen Experimental Protocol — QuorumShift Benchmarking

**Author**: Sham Satish Thakare (`151498087+shamddd@users.noreply.github.com`)  
**Repository**: `distributed-storage-simulator` (`quorumshift`)

---

## 1. Experimental Environment & Seed Control

- **Execution Engine**: Native C++20 compiled binary (`QuorumShift` Engine) + Python 3.12 Evaluation Harness.
- **Random Seeds**: `{42, 1337, 2026}` (3 random seeds per scenario for statistical error bars).
- **Workload Concurrency**: 50 concurrent client threads, 10,000 write operations per seed per scenario.
- **Cluster Configurations**: $N = 5$ nodes across 2 rack failure domains (Rack A: 3 nodes, Rack B: 2 nodes).

---

## 2. Benchmark Scenarios (`QuorumBench`)

| Scenario ID | Name | Network Conditions | Failure Fault Injected | Expected Outcome |
| :-: | :--- | :--- | :--- | :--- |
| **S01** | Steady State | Uniform 2ms LAN RTT | None | Baseline P50/P99 latency parity. |
| **S02** | Asymmetric WAN Latency | Rack B nodes +100ms RTT | None | QuorumShift shifts $Q_2$ to Rack A (P99 latency down >30%). |
| **S03** | Correlated Rack Failure | Uniform 2ms RTT | Rack B completely powered down | QuorumShift re-weights Rack A to maintain quorum. |
| **S04** | Transient Packet Loss | Rack B 25% packet loss | None | QuorumShift down-weights lossy nodes without election. |
| **S05** | Network Partition | 3 vs 2 partition | 2 minority nodes isolated | Majority partition maintains writes; linearizability preserved. |
| **S06** | Recovery Storm | Stale nodes reconnecting | 2 nodes reconnect after 5s | Catch-up sync completed without log corruption. |

---

## 3. Evaluation Metrics

1. **Safe Task Completion Rate ($STCR$)**: Percentage of operations committed without linearizability violations.
2. **P99 Commit Latency ($\text{ms}$)**: 99th percentile write transaction latency.
3. **Throughput ($\text{ops/sec}$)**: Total committed transactions per second.
4. **Stale Read Count ($SRC$)**: Number of reads returning non-linearizable stale values (Target: 0).
