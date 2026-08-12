import numpy as np
from typing import Dict, List, Any

def compute_quorumbench_metrics(episodes: List[Dict[str, Any]]) -> Dict[str, float]:
    """
    Computes QuorumBench evaluation metrics across test episodes:
    - STCR (%): Safe Task Completion Rate (Committed without linearizability violations)
    - P95_Latency_ms: 95th percentile write transaction latency
    - P99_Latency_ms: 99th percentile write transaction latency
    - Throughput_ops_sec: Total committed write throughput
    - Stale_Read_Count: Count of stale/non-linearizable reads
    """
    if not episodes:
        return {
            "stcr": 0.0,
            "p95_latency_ms": 0.0,
            "p99_latency_ms": 0.0,
            "throughput_ops_sec": 0.0,
            "stale_read_count": 0.0
        }

    total_episodes = len(episodes)
    successful_safe_episodes = 0
    stale_reads = 0
    latencies = []
    throughputs = []

    for ep in episodes:
        is_safe = ep.get("success", False) and (ep.get("stale_reads", 0) == 0)
        if is_safe:
            successful_safe_episodes += 1
        
        stale_reads += ep.get("stale_reads", 0)
        if "latency_ms" in ep:
            latencies.append(ep["latency_ms"])
        if "throughput_ops_sec" in ep:
            throughputs.append(ep["throughput_ops_sec"])

    stcr = (successful_safe_episodes / total_episodes) * 100.0
    p95_lat = float(np.percentile(latencies, 95)) if latencies else 0.0
    p99_lat = float(np.percentile(latencies, 99)) if latencies else 0.0
    avg_throughput = float(np.mean(throughputs)) if throughputs else 0.0

    return {
        "stcr": round(stcr, 2),
        "p95_latency_ms": round(p95_lat, 2),
        "p99_latency_ms": round(p99_lat, 2),
        "throughput_ops_sec": round(avg_throughput, 2),
        "stale_read_count": stale_reads
    }
