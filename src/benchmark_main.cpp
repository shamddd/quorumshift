#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>
#include "dss/cluster.hpp"
#include "dss/logger.hpp"

using namespace dss;

struct BenchResult {
    std::size_t cluster_size{0};
    std::size_t total_ops{0};
    std::size_t successful_ops{0};
    double duration_sec{0.0};
    double ops_per_sec{0.0};
    double p50_us{0.0};
    double p90_us{0.0};
    double p95_us{0.0};
    double p99_us{0.0};
};

BenchResult run_benchmark(std::size_t cluster_size, std::size_t num_requests, double write_ratio) {
    Cluster cluster(cluster_size);
    Logger::instance().set_quiet(true); // Suppress log spam during benchmark

    // Tick to establish initial leader
    for (int i = 0; i < 5; ++i) cluster.tick_all();

    std::vector<std::uint64_t> latencies_us;
    latencies_us.reserve(num_requests);

    std::size_t success_count = 0;
    std::size_t num_writes = static_cast<std::size_t>(num_requests * write_ratio);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (std::size_t i = 0; i < num_requests; ++i) {
        std::string key = "key_" + std::to_string(i);
        std::string val = "val_" + std::to_string(i);

        if (i < num_writes) {
            auto res = cluster.write(key, val);
            if (res.success) success_count++;
            latencies_us.push_back(res.latency_us);
        } else {
            auto res = cluster.read(key);
            if (res.success) success_count++;
            latencies_us.push_back(res.latency_us);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_sec = std::chrono::duration<double>(end_time - start_time).count();

    std::sort(latencies_us.begin(), latencies_us.end());

    BenchResult result;
    result.cluster_size = cluster_size;
    result.total_ops = num_requests;
    result.successful_ops = success_count;
    result.duration_sec = duration_sec;
    result.ops_per_sec = (duration_sec > 0) ? (num_requests / duration_sec) : 0.0;

    if (!latencies_us.empty()) {
        result.p50_us = latencies_us[static_cast<std::size_t>(latencies_us.size() * 0.50)];
        result.p90_us = latencies_us[static_cast<std::size_t>(latencies_us.size() * 0.90)];
        result.p95_us = latencies_us[static_cast<std::size_t>(latencies_us.size() * 0.95)];
        result.p99_us = latencies_us[static_cast<std::size_t>(latencies_us.size() * 0.99)];
    }

    return result;
}

int main(int argc, char* argv[]) {
    std::size_t num_requests = 1000;
    if (argc >= 2) {
        num_requests = std::stoul(argv[1]);
    }

    std::cout << "=======================================================\n";
    std::cout << "   DISTRIBUTED STORAGE SIMULATOR — BENCHMARK SUITE    \n";
    std::cout << "=======================================================\n";
    std::cout << "Executing " << num_requests << " operations across cluster configurations...\n\n";

    std::vector<std::size_t> cluster_sizes = {3, 5, 7};
    std::vector<BenchResult> results;

    std::cout << std::left << std::setw(15) << "Cluster Size"
              << std::setw(15) << "Total Ops"
              << std::setw(15) << "Throughput (ops/s)"
              << std::setw(12) << "p50 (us)"
              << std::setw(12) << "p95 (us)"
              << std::setw(12) << "p99 (us)" << "\n";
    std::cout << "------------------------------------------------------------------------------\n";

    for (std::size_t size : cluster_sizes) {
        auto res = run_benchmark(size, num_requests, 0.7); // 70% writes, 30% reads
        results.push_back(res);

        std::cout << std::left << std::setw(15) << (std::to_string(size) + " Nodes")
                  << std::setw(15) << res.total_ops
                  << std::setw(15) << std::fixed << std::setprecision(2) << res.ops_per_sec
                  << std::setw(12) << res.p50_us
                  << std::setw(12) << res.p95_us
                  << std::setw(12) << res.p99_us << "\n";
    }

    std::cout << "================================================------------------------------\n";

    // Write results to JSON file
    std::ofstream json_out("benchmark_results.json");
    json_out << "[\n";
    for (std::size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        json_out << "  {\n"
                 << "    \"cluster_size\": " << r.cluster_size << ",\n"
                 << "    \"total_ops\": " << r.total_ops << ",\n"
                 << "    \"successful_ops\": " << r.successful_ops << ",\n"
                 << "    \"duration_sec\": " << r.duration_sec << ",\n"
                 << "    \"ops_per_sec\": " << r.ops_per_sec << ",\n"
                 << "    \"p50_us\": " << r.p50_us << ",\n"
                 << "    \"p95_us\": " << r.p95_us << ",\n"
                 << "    \"p99_us\": " << r.p99_us << "\n"
                 << "  }" << (i + 1 < results.size() ? "," : "") << "\n";
    }
    json_out << "]\n";
    json_out.close();

    std::cout << "Benchmark results written to benchmark_results.json\n\n";
    return 0;
}
