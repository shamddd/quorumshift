#!/usr/bin/env python3
"""
Benchmark runner script for Distributed Storage Simulator.
Executes dss-bench binary and prints markdown summary table.
"""

import json
import subprocess
from pathlib import Path


def main() -> None:
    project_root = Path(__file__).resolve().parent.parent
    bench_bin = project_root / "build" / "dss-bench"

    print("Running Distributed Storage Simulator Benchmark Suite...")
    subprocess.run([str(bench_bin), "1000"], check=True, cwd=str(project_root))

    results_file = project_root / "benchmark_results.json"
    if results_file.exists():
        with open(results_file, "r") as f:
            data = json.load(f)

        print("\n### Performance Benchmark Results\n")
        header = (
            "| Cluster Size | Total Operations | Throughput (ops/sec) "
            "| p50 Latency (µs) | p95 Latency (µs) | p99 Latency (µs) |"
        )
        print(header)
        print(
            "|:-------------|:-----------------|:---------------------"
            "|:-----------------|:-----------------|:-----------------|"
        )
        for item in data:
            size_str = f"{item['cluster_size']} Nodes"
            ops = item["total_ops"]
            tput = f"{item['ops_per_sec']:.2f}"
            p50 = f"{item['p50_us']:.1f}"
            p95 = f"{item['p95_us']:.1f}"
            p99 = f"{item['p99_us']:.1f}"
            print(f"| {size_str} | {ops} | {tput} | {p50} | {p95} | {p99} |")
        print("\nBenchmark execution complete.")


if __name__ == "__main__":
    main()
