# Evaluated Baselines — AdaptiveReplica

**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Author**: Sham Satish Thakare

---

## Comparative Baselines ($B_0 - B_5$)

| Baseline ID | Name | Replication Factor ($R$) | Quorum Threshold | Placement Strategy |
| :-: | :--- | :-: | :-: | :--- |
| **B0** | Fixed $R=3$ Static Majority | 3 | $Q = 2$ | Fixed Node IDs 1, 2, 3 |
| **B1** | Fixed $R=5$ Static Majority | 5 | $Q = 3$ | Fixed Node IDs 1..5 |
| **B2** | Random Replica Placement | 3 | $Q = 2$ | Uniform random selection |
| **B3** | Latency-Aware Placement | 3 | $Q = 2$ | Lowest 3 RTT nodes |
| **B4** | Failure-Domain Static | 3 | $Q = 2$ | Static 1-per-rack distribution |
| **B5** | **AdaptiveReplica (Proposed)** | Dynamic $R \in [3, 7]$ | Dynamic $Q = \lfloor R/2 \rfloor + 1$ | Joint RTT & rack correlation optimization |
