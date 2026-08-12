# Limitations & Scientific Scope — AdaptiveReplica

**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Author**: Sham Satish Thakare

---

## Recognized Scientific Limitations

1. **In-Memory Simulation Boundary**: The current evaluation simulates network RPCs and memory WAL logs; evaluation on physical NVMe storage devices with fsync delays is left for future hardware testbeds.
2. **Re-Configuration Frequency Throttling**: Very rapid adaptation intervals ($\Delta t < 5\text{ms}$) can induce temporary election timer instability if threshold safety limits are bypassed.
3. **Byzantine Fault Non-Coverage**: AdaptiveReplica assumes fail-stop and network drop threat models, not malicious Byzantine behaviors.
