# Reproducibility Guide — AdaptiveReplica

**Target Venue**: IEEE Transactions on Parallel and Distributed Systems (TPDS)  
**Author**: Sham Satish Thakare

---

## 1. Quick Start Commands

```bash
# Build C++20 AdaptiveReplica binary & run unit test suites
./scripts/build_and_test.sh

# Execute 16 failure scenarios across 6 baselines & generate LaTeX tables/plots
python3 research/evaluation/run_adaptivereplica_bench.py
```

---

## 2. Environment Verification

- **Compiler**: `clang++ -std=c++20` (Apple LLVM 15.0+)
- **Python**: Python 3.12+ (NumPy, SciPy)
- **Random Seeds**: Fixed seeds `{42, 1337, 2026}` guaranteed deterministic execution.
