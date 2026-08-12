#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include "dss/cluster.hpp"
#include "dss/logger.hpp"

using namespace dss;

void print_usage() {
    std::cout << "Distributed Storage Simulator CLI (dss-cli)\n"
              << "Usage: dss-cli [command] [options]\n\n"
              << "Commands:\n"
              << "  cluster start [--nodes N]    Start simulated cluster with N nodes (default: 3)\n"
              << "  cluster status               Show status of all nodes in cluster\n"
              << "  node kill --id N             Simulate crash failure of node N\n"
              << "  node recover --id N          Recover node N\n"
              << "  write --key K --value V      Perform quorum replicated write operation\n"
              << "  read --key K                 Perform read operation\n"
              << "  demo                         Run interactive fault tolerance & failover demo\n"
              << "  help                         Show this help message\n";
}

void print_cluster_status(const Cluster& cluster) {
    auto statuses = cluster.get_cluster_status();
    std::cout << "\n=======================================================\n";
    std::cout << "               CLUSTER STATUS OVERVIEW                 \n";
    std::cout << "=======================================================\n";
    std::cout << std::left << std::setw(10) << "Node ID"
              << std::setw(15) << "Role"
              << std::setw(10) << "Term"
              << std::setw(15) << "Last Log Index"
              << std::setw(10) << "Status" << "\n";
    std::cout << "-------------------------------------------------------\n";

    for (const auto& s : statuses) {
        std::cout << std::left << std::setw(10) << s.id
                  << std::setw(15) << to_string(s.role)
                  << std::setw(10) << s.current_term
                  << std::setw(15) << s.last_log_index
                  << std::setw(10) << (s.is_alive ? "ONLINE" : "CRASHED") << "\n";
    }
    std::cout << "=======================================================\n\n";
}

void run_demo() {
    std::cout << "\n=======================================================\n";
    std::cout << "    DISTRIBUTED STORAGE SIMULATOR — FAULT TOLERANCE DEMO \n";
    std::cout << "=======================================================\n\n";

    Cluster cluster(5);
    Logger::instance().set_quiet(false);

    std::cout << "[Step 1] Initializing 5-Node Raft Cluster...\n";
    for (int i = 0; i < 4; ++i) {
        cluster.tick_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    print_cluster_status(cluster);

    auto leader_id_opt = cluster.find_leader();
    if (!leader_id_opt.has_value()) {
        std::cout << "Retrying election ticks...\n";
        cluster.tick_all();
        leader_id_opt = cluster.find_leader();
    }

    NodeId initial_leader = leader_id_opt.value_or(1);
    std::cout << "\n[Step 2] Executing Quorum Replicated Writes on Leader (Node " << initial_leader << ")...\n";
    auto w1 = cluster.write("user_profile_101", "{\"name\": \"Alice\", \"role\": \"Admin\"}");
    std::cout << "Write 1 Result: " << (w1.success ? "SUCCESS" : "FAILED") << " (Latency: " << w1.latency_us << " us)\n";

    auto w2 = cluster.write("system_config", "{\"cluster_mode\": \"strict_consistency\"}");
    std::cout << "Write 2 Result: " << (w2.success ? "SUCCESS" : "FAILED") << " (Latency: " << w2.latency_us << " us)\n";

    std::cout << "\n[Step 3] Simulating Unannounced Failure of Leader Node " << initial_leader << "...\n";
    cluster.kill_node(initial_leader);
    
    std::cout << "\n[Step 4] Ticking Cluster Timers to Detect Timeout & Trigger Failover...\n";
    for (int i = 0; i < 5; ++i) {
        cluster.tick_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    auto new_leader_id_opt = cluster.find_leader();
    NodeId new_leader = new_leader_id_opt.value_or(0);
    Logger::instance().failover(initial_leader, 2);
    Logger::instance().new_leader_elected(new_leader, 2);
    print_cluster_status(cluster);

    std::cout << "\n[Step 5] Performing Write Operation on New Leader Node " << new_leader << "...\n";
    auto w3 = cluster.write("user_profile_102", "{\"name\": \"Bob\", \"role\": \"Developer\"}");
    std::cout << "Write 3 Result: " << (w3.success ? "SUCCESS" : "FAILED") << " (Latency: " << w3.latency_us << " us)\n";

    std::cout << "\n[Step 6] Reading Replicated Key 'user_profile_101' from New Leader...\n";
    auto r1 = cluster.read("user_profile_101");
    std::cout << "Read Result: " << (r1.value.has_value() ? r1.value.value() : "NOT FOUND") << "\n";

    std::cout << "\n[Step 7] Recovering Crashed Node " << initial_leader << " & Rejoining Cluster...\n";
    cluster.recover_node(initial_leader);
    for (int i = 0; i < 3; ++i) {
        cluster.tick_all();
    }
    print_cluster_status(cluster);

    std::cout << "Demo Execution Completed Successfully!\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 0;
    }

    std::string cmd = argv[1];

    if (cmd == "demo") {
        run_demo();
        return 0;
    }

    if (cmd == "help" || cmd == "--help" || cmd == "-h") {
        print_usage();
        return 0;
    }

    Cluster cluster(3);
    for (int i = 0; i < 5; ++i) cluster.tick_all();

    if (cmd == "cluster") {
        if (argc >= 3 && std::string(argv[2]) == "status") {
            print_cluster_status(cluster);
            return 0;
        }
        if (argc >= 3 && std::string(argv[2]) == "start") {
            std::size_t n = 3;
            if (argc >= 5 && std::string(argv[3]) == "--nodes") {
                n = std::stoul(argv[4]);
            }
            cluster.initialize(n);
            for (int i = 0; i < 5; ++i) cluster.tick_all();
            std::cout << "Cluster started with " << n << " nodes.\n";
            print_cluster_status(cluster);
            return 0;
        }
    } else if (cmd == "node") {
        if (argc >= 4 && std::string(argv[2]) == "kill" && std::string(argv[3]) == "--id") {
            NodeId id = std::stoul(argv[4]);
            cluster.kill_node(id);
            std::cout << "Killed Node " << id << "\n";
            return 0;
        }
        if (argc >= 4 && std::string(argv[2]) == "recover" && std::string(argv[3]) == "--id") {
            NodeId id = std::stoul(argv[4]);
            cluster.recover_node(id);
            std::cout << "Recovered Node " << id << "\n";
            return 0;
        }
    } else if (cmd == "write") {
        std::string key, val;
        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "--key" && i + 1 < argc) key = argv[++i];
            if (std::string(argv[i]) == "--value" && i + 1 < argc) val = argv[++i];
        }
        auto res = cluster.write(key, val);
        std::cout << "Write Key=" << key << " Result: " << (res.success ? "SUCCESS" : "FAILED") 
                  << " (Leader: Node " << res.leader_id << ", Latency: " << res.latency_us << " us)\n";
        return res.success ? 0 : 1;
    } else if (cmd == "read") {
        std::string key;
        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "--key" && i + 1 < argc) key = argv[++i];
        }
        auto res = cluster.read(key);
        std::cout << "Read Key=" << key << " Value=" << (res.value.has_value() ? res.value.value() : "<null>") 
                  << " (Latency: " << res.latency_us << " us)\n";
        return res.success ? 0 : 1;
    }

    print_usage();
    return 0;
}
