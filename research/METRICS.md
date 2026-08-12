# Measured Metrics & Formulations — AdaptiveReplica

**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Author**: Sham Satish Thakare

---

## Metric Definitions

1. **Availability ($A \% \uparrow$)**:
   $$A = \frac{N_{\text{successful\_writes}} + N_{\text{successful\_reads}}}{N_{\text{total\_requests}}} \times 100\%$$
2. **P99 Commit Latency ($\text{ms} \downarrow$)**: 99th percentile write commit duration from client dispatch to quorum acknowledgment.
3. **Throughput ($\text{ops/s} \uparrow$)**: Total completed key-value operations per second.
4. **Relative Traffic Overhead ($\times \downarrow$)**: Ratio of inter-node network RPC messages relative to single-replica operations.
5. **Linearizability & Stale Read Count**: Number of read operations returning values strictly older than the last committed write.
