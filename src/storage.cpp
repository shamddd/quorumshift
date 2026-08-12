#include "dss/storage.hpp"
#include <chrono>

namespace dss {

void StorageEngine::put(const std::string& key, const std::string& value) {
    std::unique_lock lock(rw_mutex_);
    kv_data_[key] = value;
}

std::optional<std::string> StorageEngine::get(const std::string& key) const {
    std::shared_lock lock(rw_mutex_);
    auto it = kv_data_.find(key);
    if (it != kv_data_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool StorageEngine::remove(const std::string& key) {
    std::unique_lock lock(rw_mutex_);
    return kv_data_.erase(key) > 0;
}

LogIndex StorageEngine::append_log(Term term, CommandType type, const std::string& key, const std::string& value) {
    std::unique_lock lock(rw_mutex_);
    LogIndex index = wal_log_.size() + 1;
    auto now_ms = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );

    wal_log_.push_back(LogEntry{
        .term = term,
        .index = index,
        .type = type,
        .key = key,
        .value = value,
        .timestamp_ms = now_ms
    });
    return index;
}

std::optional<LogEntry> StorageEngine::get_log_entry(LogIndex index) const {
    std::shared_lock lock(rw_mutex_);
    if (index == 0 || index > wal_log_.size()) {
        return std::nullopt;
    }
    return wal_log_[index - 1];
}

std::vector<LogEntry> StorageEngine::get_log_range(LogIndex start_index) const {
    std::shared_lock lock(rw_mutex_);
    if (start_index == 0 || start_index > wal_log_.size()) {
        return {};
    }
    return std::vector<LogEntry>(wal_log_.begin() + (start_index - 1), wal_log_.end());
}

LogIndex StorageEngine::last_log_index() const {
    std::shared_lock lock(rw_mutex_);
    return wal_log_.size();
}

Term StorageEngine::last_log_term() const {
    std::shared_lock lock(rw_mutex_);
    if (wal_log_.empty()) {
        return 0;
    }
    return wal_log_.back().term;
}

LogIndex StorageEngine::commit_index() const {
    std::shared_lock lock(rw_mutex_);
    return commit_index_;
}

void StorageEngine::apply_committed(LogIndex up_to_index) {
    std::unique_lock lock(rw_mutex_);
    if (up_to_index > wal_log_.size()) {
        up_to_index = wal_log_.size();
    }
    
    while (last_applied_index_ < up_to_index) {
        last_applied_index_++;
        const auto& entry = wal_log_[last_applied_index_ - 1];
        if (entry.type == CommandType::Put) {
            kv_data_[entry.key] = entry.value;
        } else if (entry.type == CommandType::Delete) {
            kv_data_.erase(entry.key);
        }
    }
    commit_index_ = up_to_index;
}

void StorageEngine::truncate_logs_after(LogIndex index) {
    std::unique_lock lock(rw_mutex_);
    if (index < wal_log_.size()) {
        wal_log_.resize(index);
    }
}

std::size_t StorageEngine::key_count() const {
    std::shared_lock lock(rw_mutex_);
    return kv_data_.size();
}

void StorageEngine::clear() {
    std::unique_lock lock(rw_mutex_);
    kv_data_.clear();
    wal_log_.clear();
    last_applied_index_ = 0;
    commit_index_ = 0;
}

} // namespace dss
