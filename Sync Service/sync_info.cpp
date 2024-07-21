#include "sync_info.h"
// Constructor
SyncInfo::SyncInfo(std::string name) : name(name), last_sync_date_unix(this->get_current_unix_time()), frequency(""), dirty(1){}
SyncInfo::SyncInfo(std::string name, std::string frequency) : name(name), last_sync_date_unix(this->get_current_unix_time()), frequency(frequency), dirty(1) {};
SyncInfo::SyncInfo(std::string name, int last_sync_unix, std::string frequency, int dirty) : name(name), last_sync_date_unix(last_sync_unix), frequency(frequency), dirty(dirty) {};
// Getters
int SyncInfo::get_last_sync_date_unix() const {
    return last_sync_date_unix;
}

std::string SyncInfo::get_frequency() const {
    return frequency;
}

int SyncInfo::get_dirty() const {
    return dirty;
}

// Setters
void SyncInfo::set_last_sync_date_unix(int lastSyncDateUnix) {
    last_sync_date_unix = lastSyncDateUnix;
}

void SyncInfo::set_frequency(std::string frequency) {
    this->frequency = frequency;
}

void SyncInfo::set_dirty(int dirty) {
    this->dirty = dirty;
}

int SyncInfo::get_current_unix_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(now);
    int unix_timestamp_int = static_cast<int>(unix_timestamp);


    return unix_timestamp_int;
}
