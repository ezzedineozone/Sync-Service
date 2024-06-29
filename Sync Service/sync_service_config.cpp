#include "sync_service_config.h"
#include "sqlite3/sqlite3.h"
#include <iostream>
SyncServiceConfig::SyncServiceConfig(sqlite3* db) {
	const char* config_query = "SELECT * FROM SYNCSERVICECONFIG";
	char* error_msg;
	int config_result_status = sqlite3_exec(db, config_query, nullptr, nullptr, &error_msg);
	if (config_result_status == SQLITE_OK)
		std::cout << "query succesful";
};
SyncServiceConfig::SyncServiceConfig() {
	this->general_frequency = "1/day";
	this->log_dir = "";
	this->log_enabled = false;
	this->version = "0.1.0";
};
bool SyncServiceConfig::get_log_enabled() {
	return this->log_enabled;
};
std::string SyncServiceConfig::get_version() {
	return this->version;
};
std::string SyncServiceConfig::get_general_frequency() {
	return this->general_frequency;
};
std::string SyncServiceConfig::get_log_dir() {
	return this->log_dir;
};
void SyncServiceConfig::set_log_enabled(bool val) {
	this->log_enabled = val;
};
void SyncServiceConfig::print_config() {
	std::cout << "Version: " << this->get_version() << "\n"
		<< "General Frequency: " << this->get_general_frequency() << "\n"
		<< "Log Enabled: " << (this->get_log_enabled() ? "true" : "false") << "\n"
		<< "Log Directory: " << this->get_log_dir() << std::endl;
}
