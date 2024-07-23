#include "sync_info.h"


//SyncInfo(std::string name);
//SyncInfo(std::string name, std::string frequency);
//SyncInfo(std::string name, int last_unix_sync_date, std::string frequency, int dirty);

SyncInfo::SyncInfo(std::string name, int last_sync_date, std::string frequency, int dirty) : name(name), last_sync_date_unix(last_sync_date), frequency(frequency), dirty(dirty) {};
SyncInfo::SyncInfo(std::string name, std::string frequency) : SyncInfo(name, get_current_unix_time(), std::string(""), 1) {};
SyncInfo::SyncInfo(std::string name) : SyncInfo(name, std::string("")) {};


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
