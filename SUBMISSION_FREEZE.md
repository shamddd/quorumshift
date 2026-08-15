# Submission Freeze Record — quorumshift (AdaptiveReplica)

**Date**: 2026-08-14  
**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Submission System**: IEEE Author Portal (ScholarOne)  
**Commit SHA**: e41a976bd3f044c5d0d37cb38f93489bfd0c0e05  

## Frozen Experimental Artifacts & Benchmark Configuration
* **System Implemented**: `AdaptiveReplica` (Dynamic Quorum Adaptation over Raft Joint Consensus)
* **Random Seeds**: 42, 43, 44, 45, 46 ($N = 5$ independent runs)
* **Fault Injection Baseline**: 50ms asymmetric network latency degradation across minority replica nodes
* **Primary Key Metrics**:
  - Availability ($A$): $99.97\% \pm 0.01\%$
  - Write p99 Latency: $13.50\text{ms} \pm 0.42\text{ms}$ (vs $120.48\text{ms}$ static $R=5$ majority; $88.8\%$ reduction)
  - Stale Reads ($S_{\text{stale}}$): Exactly $0$ ($C = 100.0\%$)
* **Code Reproducibility**: 100% Pass rate (`uv run pytest tests/`)
