# AdaptiveReplica: Failure-Aware Dynamic Replication for Distributed Storage

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](LICENSE)
[![Manuscript](https://img.shields.io/badge/Manuscript-IEEE_TPDS_Preparation-orange.svg)](research/paper/main.tex)

> **Official Research Artifact**: Failure-Aware Dynamic Replication and Placement Optimization for Distributed Consensus Storage.

---

## Executive Overview

- **Author**: Sham Satish Thakare (`151498087+shamddd@users.noreply.github.com`)
- **Target Journal**: IEEE Transactions on Parallel and Distributed Systems (TPDS)
- **Primary Research Question**: *"Can a failure-aware adaptive replication strategy improve the availability–latency–resource-cost tradeoff compared with fixed replication under dynamic workloads and correlated failure patterns?"*
- **Primary Contribution**: **AdaptiveReplica**, a multi-signal dynamic replication controller ($R(t) \in [3, 7]$) with failure-domain-aware placement optimization while preserving Raft consensus invariants ($Q_e \cap Q_w \neq \emptyset$).

---

## Empirical Benchmark Results (Mean $\pm$ 95% CI)

| Replication Strategy | Availability ($\% \uparrow$) | P99 Latency ($\text{ms} \downarrow$) | Throughput ($\text{ops/s} \uparrow$) | Relative Traffic | Stale Reads |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **B0 (Fixed $R=3$ Static Majority)** | $95.86\% \pm 1.66\%$ | 215.81 | 3587.44 | $1.00\times$ | 0 |
| **B1 (Fixed $R=5$ Static Majority)** | $98.86\% \pm 0.60\%$ | 120.48 | 3418.83 | $1.67\times$ | 0 |
| **B2 (Random Placement)** | $96.57\% \pm 1.16\%$ | 135.26 | 3565.72 | $1.00\times$ | 0 |
| **B3 (Latency-Aware Placement)** | $98.44\% \pm 0.94\%$ | 56.95 | 4507.04 | $1.00\times$ | 0 |
| **B4 (Failure-Domain Static)** | $99.15\% \pm 0.30\%$ | 56.62 | 4144.59 | $1.00\times$ | 0 |
| **B5 (AdaptiveReplica Proposed)** | **$99.97\% \pm 0.00\%$** | **13.50** | **4742.73** | **$1.15\times$** | **0** |

---

## Reproducibility Commands

```bash
# Build C++20 AdaptiveReplica core & run all 6 unit test suites
./scripts/build_and_test.sh

# Execute 16 failure scenarios across 6 baselines & generate LaTeX tables/figures
python3 research/evaluation/run_adaptivereplica_bench.py
```

---

## Citation

```bibtex
@article{thakare2026adaptivereplica,
  author    = {Thakare, Sham Satish},
  title     = {AdaptiveReplica: Failure-Aware Dynamic Replication for Distributed Storage under Correlated Faults},
  journal   = {IEEE Transactions on Parallel and Distributed Systems (Under Preparation)},
  year      = {2026}
}
```
