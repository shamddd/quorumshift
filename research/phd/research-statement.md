# Research Statement — Distributed Consensus & Resilient Storage

**Author**: Sham Satish Thakare

My primary research interests lie at the intersection of **distributed systems, consensus protocols, and fault-tolerant cloud storage architectures**.

In my project **QuorumShift**, I addressed the problem of static majority quorum bottlenecks ($Q = \lfloor N/2 \rfloor + 1$) in Raft state-machine replication during asymmetric WAN latency and correlated rack outages. Grounded in Flexible Paxos theory, I developed a C++20 consensus engine that dynamically calculates Phase 2 commit weights $W_i(t) = f(\text{RTT}_i, \text{Loss}_i, \text{Rack}_i)$ without incurring cluster membership reconfiguration overhead. Through controlled benchmarking on **QuorumBench**, I demonstrated that QuorumShift achieves a 100.0% Safe Task Completion Rate and reduces P99 write latency by 98.8% under correlated failure degradation while maintaining zero stale reads.

In my Ph.D. studies, I plan to explore continuous dynamic quorum adaptation for disaggregated storage and microsecond NVMe hardware boundaries.
