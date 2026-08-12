# QuorumShift (`quorumshift`)

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Build & Test](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](scripts/build_and_test.sh)
[![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](LICENSE)
[![Research](https://img.shields.io/badge/Research-QuorumShift-orange.svg)](RESEARCH.md)

> A high-performance C++20 distributed storage engine and research prototype evaluating failure-domain-aware adaptive Raft consensus (**QuorumShift**).

---

## 🏛 Architecture

```mermaid
flowchart TD
    subgraph Client & Benchmark Harness
        CLI["dss-cli (Native C++ CLI)"]
        Bench["dss-bench (QuorumBench Harness)"]
    end

    subgraph Simulated Network & Fault Injector
        NetSim["Network Simulator\n(Asymmetric WAN RTT, Packet Loss, Partitions)"]
    end

    subgraph QuorumShift Consensus Engine
        Leader["Node 1 (Leader)\nDynamic Quorum Weight Evaluator"]
        Follower1["Node 2 (Rack A)\nWeight: 1.0"]
        Follower2["Node 3 (Rack A)\nWeight: 1.0"]
        Follower3["Node 4 (Rack B - Degraded)\nWeight: 0.15"]
        Follower4["Node 5 (Rack B - Degraded)\nWeight: 0.15"]
    end

    subgraph Storage Engine
        SE["Thread-Safe KV Engine\n(std::shared_mutex + WAL)"]
    end
```

---

## 🔬 Research & Benchmark (`QuorumShift`)

See [`RESEARCH.md`](RESEARCH.md) for full research documentation, literature review, novelty analysis, experimental benchmark metrics, and manuscript sources.

- **Primary Research Paper**: [`research/paper/main.tex`](research/paper/main.tex)
- **Experimental Protocol**: [`research/EXPERIMENT_PROTOCOL.md`](research/EXPERIMENT_PROTOCOL.md)
- **Literature Review**: [`research/literature-review.md`](research/literature-review.md)

---

## Author

**Sham Satish Thakare**  
GitHub: [https://github.com/shamddd](https://github.com/shamddd)
