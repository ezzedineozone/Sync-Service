#ifndef SYNCINFO_H
#define SYNCINFO_H
#include <string>
#include <chrono>
#include "dependencies/json/json.hpp"
class SyncInfo {
public:
    SyncInfo();
    SyncInfo(std::string name);
    SyncInfo(std::string name, std::string frequency);
    SyncInfo(std::string name, int last_unix_sync_date, std::string frequency, int dirty);
    SyncInfo(const nlohmann::json& j);

    std::string name;
    int get_last_sync_date_unix() const;
    std::string get_frequency() const;
    int get_dirty() const;


    void set_last_sync_date_unix(int lastSyncDateUnix);
    void set_frequency(std::string frequency);
    void set_dirty(int dirty);
    nlohmann::json to_json();

private:
	int last_sync_date_unix;
	std::string frequency;
	int dirty;
    int get_current_unix_time();
};
#endif