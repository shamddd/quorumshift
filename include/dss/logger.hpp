#ifndef DSS_LOGGER_HPP
#define DSS_LOGGER_HPP

#include <string>
#include <mutex>
#include <vector>
#include <iostream>
#include "dss/common.hpp"

namespace dss {

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error
};

class Logger {
public:
    static Logger& instance();

    void set_level(LogLevel level);
    void set_quiet(bool quiet);
    
    void log(LogLevel level, const std::string& tag, const std::string& message);
    
    // Recruiter-visualizable event helpers
    void leader_elected(NodeId node_id, Term term);
    void replication_success(NodeId leader_id, LogIndex index, std::size_t ack_count, std::size_t total_nodes);
    void node_failure(NodeId node_id);
    void failover(NodeId old_leader, Term new_term);
    void new_leader_elected(NodeId node_id, Term term);
    void node_recovered(NodeId node_id);

    std::vector<std::string> get_logs();
    void clear_logs();

private:
    Logger() = default;
    
    LogLevel level_{LogLevel::Info};
    bool quiet_{false};
    std::mutex mutex_;
    std::vector<std::string> log_buffer_;
};

} // namespace dss

#endif // DSS_LOGGER_HPP
