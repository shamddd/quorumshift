# QuorumShift: Failure-Domain-Aware Adaptive Consensus Engine

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](LICENSE)
[![Manuscript](https://img.shields.io/badge/Manuscript-In_Preparation-orange.svg)](research/paper/main.tex)

> **Official Research Artifact**: Failure-Domain-Aware Adaptive Consensus and Dynamic Quorum Scaling for High-Performance Distributed Key-Value Engines.

---

## Executive Overview

- **Author**: Sham Satish Thakare (`151498087+shamddd@users.noreply.github.com`)
- **Primary Research Question**: *"Can failure-domain-aware dynamic quorum weighting reduce tail write latency and maintain availability under correlated rack failures without violating Raft linearizability?"*
- **Primary Contribution**: **QuorumShift**, a C++20 dynamic quorum weighting engine based on Flexible Paxos theory.
- **Benchmark**: **QuorumBench**, evaluating 6 failure scenarios across 3 random seeds.

---

## Empirical Benchmark Results

| Architecture | STCR ($\% \uparrow$) | P95 Latency ($\text{ms} \downarrow$) | P99 Latency ($\text{ms} \downarrow$) | Throughput ($\text{ops/s} \uparrow$) | Stale Reads ($\downarrow$) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **B0 (Static Majority Raft)** | 83.33% | 980.0 | 980.0 | 2,478.33 | 0 |
| **B1 (Static Flexible Raft)** | 83.33% | 610.0 | 610.0 | 3,116.67 | 0 |
| **B2 (QuorumShift - Proposed)** | **100.0%** | **11.4** | **11.4** | **4,316.67** | **0** |

---

## Reproducibility Commands

```bash
# Build C++ binaries and execute unit tests
./scripts/build_and_test.sh

# Run QuorumBench master experiment suite & generate LaTeX tables/figures
python3 research/evaluation/run_experiments.py
```

---

## Citation

```bibtex
@article{thakare2026quorumshift,
  author    = {Thakare, Sham Satish},
  title     = {QuorumShift: Failure-Domain-Aware Adaptive Consensus and Dynamic Quorum Scaling for Distributed Key-Value Engines},
  journal   = {Manuscript in Preparation},
  year      = {2026}
}
```
