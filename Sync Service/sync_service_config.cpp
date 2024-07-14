#include "sync_service_config.h"
#include <iostream>
ServiceConfig::ServiceConfig() {
	this->general_frequency = "1/day";
	this->log_dir = "";
	this->log_enabled = false;
	this->version = "0.1.0";
};
bool ServiceConfig::get_log_enabled() {
	return this->log_enabled;
};
std::string ServiceConfig::get_version() {
	return this->version;
};
std::string ServiceConfig::get_general_frequency() {
	return this->general_frequency;
};
std::string ServiceConfig::get_log_dir() {
	return this->log_dir;
};
void ServiceConfig::set_log_enabled(bool val) {
	this->log_enabled = val;
};
void ServiceConfig::print_config() {
	std::cout << "Version: " << this->get_version() << "\n"
		<< "General Frequency: " << this->get_general_frequency() << "\n"
		<< "Log Enabled: " << (this->get_log_enabled() ? "true" : "false") << "\n"
		<< "Log Directory: " << this->get_log_dir() << std::endl;
}
