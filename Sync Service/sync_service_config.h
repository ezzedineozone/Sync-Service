#ifndef SYNCSERVICEMETADA_H
#define SYNCSERVICEMETADA_H
#include <string>
#include "dependencies/sqlite3/sqlite3.h"
class SyncServiceConfig {
private:
	std::string version;
	std::string general_frequency; // format Number,day/month/year/hour
	std::string log_dir;

	bool log_enabled;

public:
	SyncServiceConfig();
	SyncServiceConfig(sqlite3* db);
	std::string get_version();
	std::string get_general_frequency();
	std::string get_log_dir();
	bool get_log_enabled();
	void set_log_enabled(bool val);
	void print_config();
};
#endif