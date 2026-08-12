# Threat & Failure Model — AdaptiveReplica

**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Author**: Sham Satish Thakare

---

## 1. System Topology & Failure Domains

We model a distributed storage cluster composed of $N$ nodes $\mathcal{N} = \{n_1, n_2, \dots, n_N\}$ partitioned across $M$ distinct physical failure domains (racks/availability zones) $\mathcal{D} = \{d_1, d_2, \dots, d_M\}$.

- **Nodes**: Replicated state machines executing C++20 Raft consensus.
- **Network**: Asynchronous RPC transport subject to dynamic latency jitter ($\text{RTT} \in [2\text{ms}, 250\text{ms}]$), packet drops ($L \in [0.0, 0.25]$), and bi-partition splits.

---

## 2. Failure Classes

1. **Independent Fail-Stop**: Transient crashes of single nodes (e.g. power supply loss, process crash).
2. **Correlated Rack Outages**: Simultaneous power or Top-of-Rack (ToR) switch failures disabling all nodes in domain $d_m$ concurrently.
3. **Network Degradation**: Asymmetric cross-rack packet loss and latency spikes.
4. **Cascading Node Degradation**: Sequential failure of multiple replicas under high write load.
5. **Recovery Storms**: Simultaneous recovery of lagging replicas attempting WAL log catch-up.
