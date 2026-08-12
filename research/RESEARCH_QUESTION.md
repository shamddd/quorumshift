# Research Question & Hypothesis — AdaptiveReplica

**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Author**: Sham Satish Thakare (`151498087+shamddd@users.noreply.github.com`)  
**Repository Branch**: `research/adaptive-replica`

---

## Primary Research Question

> **Can a failure-aware adaptive replication strategy improve the availability–latency–resource-cost tradeoff compared with fixed replication under dynamic workloads and correlated failure patterns?**

---

## Formal Falsifiable Hypotheses

1. **$H_1$ (Availability & Tail Latency under Correlated Failures)**: Under correlated rack degradation ($50\%$ rack node loss + $10\%$ packet loss), `AdaptiveReplica` achieves $\ge 99.9\%$ Availability ($A$) and $\le 15\text{ms}$ P99 write latency, outperforming fixed $R=3$ static majority Raft ($A \le 96.0\%$, P99 latency $\ge 200\text{ms}$).
2. **$H_2$ (Resource Cost Efficiency)**: Under steady-state low-failure conditions, `AdaptiveReplica` automatically reduces replication factor to $R=3$, achieving $\ge 30\%$ lower network replication traffic compared to static $R=5$ placement ($1.15\times$ vs $1.67\times$) without compromising read/write throughput.
3. **$H_3$ (Falsification Criteria)**: If network re-configuration overhead during rapid adaptation intervals ($\Delta t < 10\text{ms}$) introduces leader election churn exceeding 5 elections/sec, `AdaptiveReplica`'s P99 latency will degrade below static $R=5$ Raft.
