#ifndef DSS_STORAGE_HPP
#define DSS_STORAGE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <optional>
#include "dss/common.hpp"

namespace dss {

class StorageEngine {
public:
    StorageEngine() = default;

    // Concurrent KV operations
    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    bool remove(const std::string& key);

    // Write-Ahead Log (WAL) / Raft State Machine operations
    LogIndex append_log(Term term, CommandType type, const std::string& key, const std::string& value);
    std::optional<LogEntry> get_log_entry(LogIndex index) const;
    std::vector<LogEntry> get_log_range(LogIndex start_index) const;
    
    LogIndex last_log_index() const;
    Term last_log_term() const;
    LogIndex commit_index() const;

    void apply_committed(LogIndex up_to_index);
    void truncate_logs_after(LogIndex index);

    std::size_t key_count() const;
    void clear();

private:
    mutable std::shared_mutex rw_mutex_;
    std::unordered_map<std::string, std::string> kv_data_;
    std::vector<LogEntry> wal_log_;
    LogIndex last_applied_index_{0};
    LogIndex commit_index_{0};
};

} // namespace dss

#endif // DSS_STORAGE_HPP
