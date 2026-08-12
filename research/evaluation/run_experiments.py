import os
import json
import numpy as np
from datetime import datetime
from evaluator import QuorumBenchEvaluator
from metrics import compute_quorumbench_metrics

def main():
    repo_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    raw_dir = os.path.join(repo_dir, "research", "results", "raw")
    processed_dir = os.path.join(repo_dir, "research", "results", "processed")
    tables_dir = os.path.join(repo_dir, "research", "tables")
    figures_dir = os.path.join(repo_dir, "research", "figures")

    for d in [raw_dir, processed_dir, tables_dir, figures_dir]:
        os.makedirs(d, exist_ok=True)

    scenarios = [
        {"id": "S01_SteadyState", "name": "Steady State LAN"},
        {"id": "S02_AsymmetricWAN", "name": "Asymmetric WAN Latency"},
        {"id": "S03_CorrelatedRackFailure", "name": "Correlated Rack Failure"},
        {"id": "S04_PacketLoss", "name": "Transient Packet Loss"},
        {"id": "S05_NetworkPartition", "name": "Network Partition"},
        {"id": "S06_RecoveryStorm", "name": "Recovery Storm"}
    ]

    baselines = ["B0_StaticMajority", "B1_StaticFlexible", "B2_QuorumShift"]
    seeds = [42, 1337, 2026]

    evaluator = QuorumBenchEvaluator(repo_dir)

    all_raw_results = []
    summary_by_baseline = {}

    print("==================================================")
    print("Executing QuorumBench Experiments across Baselines")
    print("==================================================")

    for baseline in baselines:
        baseline_episodes = []
        for seed in seeds:
            for sc in scenarios:
                res = evaluator.run_scenario(baseline, sc, seed)
                all_raw_results.append(res)
                baseline_episodes.append(res)
                print(f"[{baseline}] Seed {seed} | {sc['id']}: Success={res['success']}, P99={res['p99_latency_ms']}ms, Throughput={res['throughput_ops_sec']} ops/s")

        metrics = compute_quorumbench_metrics(baseline_episodes)
        summary_by_baseline[baseline] = metrics

    # Save raw results
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    raw_path = os.path.join(raw_dir, f"quorumbench_raw_seed42.json")
    with open(raw_path, "w") as f:
        json.dump(all_raw_results, f, indent=2)

    # Save summary results
    processed_path = os.path.join(processed_dir, f"quorumbench_summary_seed42.json")
    with open(processed_path, "w") as f:
        json.dump(summary_by_baseline, f, indent=2)

    print("\nSummary Results:")
    print(json.dumps(summary_by_baseline, indent=2))

    # Generate LaTeX Table
    latex_table_path = os.path.join(tables_dir, "main_results_table.tex")
    with open(latex_table_path, "w") as f:
        f.write("\\begin{table}[t]\n")
        f.write("\\centering\n")
        f.write("\\caption{QuorumBench Empirical Performance Comparison across Consensus Architectures.}\n")
        f.write("\\label{tab:main_results}\n")
        f.write("\\begin{tabular}{lcccc}\n")
        f.write("\\toprule\n")
        f.write("Architecture & STCR (\\% \\uparrow) & P95 Latency (ms \\downarrow) & P99 Latency (ms \\downarrow) & Throughput (ops/s \\uparrow) \\\\\n")
        f.write("\\midrule\n")
        for b_name, m in summary_by_baseline.items():
            clean_name = b_name.replace("_", " ")
            f.write(f"\\textbf{{{clean_name}}} & {m['stcr']}\\% & {m['p50_latency_ms' if 'p50_latency_ms' in m else 'p95_latency_ms']} & {m['p99_latency_ms']} & {m['throughput_ops_sec']} \\\\\n")
        f.write("\\bottomrule\n")
        f.write("\\end{tabular}\n")
        f.write("\\end{table}\n")

    print(f"\nLaTeX table written to: {latex_table_path}")

    # Generate Figure Summary Plot
    plot_path = os.path.join(figures_dir, "latency_vs_throughput.txt")
    with open(plot_path, "w") as f:
        f.write("QuorumBench Latency vs Throughput Summary\n")
        f.write("------------------------------------------\n")
        for b_name, m in summary_by_baseline.items():
            f.write(f"{b_name}: P99={m['p99_latency_ms']}ms | Throughput={m['throughput_ops_sec']} ops/sec | STCR={m['stcr']}%\n")
    print(f"Figure plot text written to: {plot_path}")

if __name__ == "__main__":
    main()
