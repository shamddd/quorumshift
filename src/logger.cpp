#include "dss/logger.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>

namespace dss {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::set_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    level_ = level;
}

void Logger::set_quiet(bool quiet) {
    std::lock_guard<std::mutex> lock(mutex_);
    quiet_ = quiet;
}

void Logger::log(LogLevel level, const std::string& tag, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (level < level_) return;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count()
       << " [" << tag << "] " << message;

    std::string formatted = ss.str();
    log_buffer_.push_back(formatted);

    if (!quiet_) {
        std::cout << formatted << std::endl;
    }
}

void Logger::leader_elected(NodeId node_id, Term term) {
    log(LogLevel::Info, "LEADER_ELECTED", "Node " + std::to_string(node_id) + " elected leader for Term " + std::to_string(term));
}

void Logger::replication_success(NodeId leader_id, LogIndex index, std::size_t ack_count, std::size_t total_nodes) {
    log(LogLevel::Info, "REPLICATION_SUCCESS", "Index " + std::to_string(index) + " replicated from Leader " + 
        std::to_string(leader_id) + " to " + std::to_string(ack_count) + "/" + std::to_string(total_nodes) + " nodes (Quorum reached)");
}

void Logger::node_failure(NodeId node_id) {
    log(LogLevel::Warn, "NODE_FAILURE", "Detected failure / disconnect on Node " + std::to_string(node_id));
}

void Logger::failover(NodeId old_leader, Term new_term) {
    log(LogLevel::Warn, "FAILOVER", "Heartbeat timeout from Leader " + std::to_string(old_leader) + ". Initiating election for Term " + std::to_string(new_term));
}

void Logger::new_leader_elected(NodeId node_id, Term term) {
    log(LogLevel::Info, "NEW_LEADER_ELECTED", "Node " + std::to_string(node_id) + " successfully established as New Leader for Term " + std::to_string(term));
}

void Logger::node_recovered(NodeId node_id) {
    log(LogLevel::Info, "NODE_RECOVERED", "Node " + std::to_string(node_id) + " recovered and rejoining cluster");
}

std::vector<std::string> Logger::get_logs() {
    std::lock_guard<std::mutex> lock(mutex_);
    return log_buffer_;
}

void Logger::clear_logs() {
    std::lock_guard<std::mutex> lock(mutex_);
    log_buffer_.clear();
}

} // namespace dss
