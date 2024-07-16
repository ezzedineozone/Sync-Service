#include "sync_service_config.h"
#include <iostream>
ServiceConfig::ServiceConfig() {
	this->general_frequency = "1/day";
	this->log_dir = "";
	this->log_enabled = false;
	this->version = "0.1.0";
	this->paths = StoredPaths();
};
ServiceConfig::ServiceConfig(fs::path path) {
	this->general_frequency = "1/day";
	this->log_dir = "";
	this->log_enabled = false;
	this->version = "0.1.0";
	this->paths = StoredPaths(path);
};
int ServiceConfig::to_json(nlohmann::json& j, const ServiceConfig& config) {
	j = nlohmann::json{
		{"version", config.version},
		{"general_frequency", config.general_frequency},
		{"log_dir", config.log_dir},
		{"log_enabled", config.log_enabled},
		{"create_files", config.create_files}
	};
	return 1;
};

int ServiceConfig::from_json(const nlohmann::json& j, ServiceConfig& config) {
	j.at("version").get_to(config.version);
	j.at("general_frequency").get_to(config.general_frequency);
	j.at("log_dir").get_to(config.log_dir);
	j.at("log_enabled").get_to(config.log_enabled);
	j.at("create_files").get_to(config.create_files);
	return 1;
};
int ServiceConfig::save_json(const nlohmann::json& j)
{
	std::ofstream file(this->paths.configPath().c_str());
	if (file.is_open())
	{
		file << std::setw(4) << j << std::endl;
		std::cout << "JSON saved" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Something went wrong saving config to json file\n";
		return 0;
	}
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
int ServiceConfig::print_config() {
	std::cout << "Version: " << this->get_version() << "\n"
		<< "General Frequency: " << this->get_general_frequency() << "\n"
		<< "Log Enabled: " << (this->get_log_enabled() ? "true" : "false") << "\n"
		<< "Log Directory: " << this->get_log_dir() << std::endl;
	return 1;
}
