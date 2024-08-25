#include "sync_service_config.h"
ServiceConfig::ServiceConfig() {
	this->general_frequency = "1/day";
	this->log_dir = "";
	this->log_enabled = false;
	this->version = "0.1.0";
	this->paths = StoredPaths();
	json_handler();
};
ServiceConfig::ServiceConfig(fs::path path) {
	this->general_frequency = "1/day";
	this->log_dir = "";
	this->log_enabled = false;
	this->version = "0.1.0";
	this->paths = StoredPaths(path);
	json_handler();
};
int ServiceConfig::to_json(nlohmann::json& j, const ServiceConfig& config) {
	j = nlohmann::json{
		{"version", config.version},
		{"general_frequency", config.general_frequency},
		{"log_dir", config.log_dir},
		{"log_enabled", config.log_enabled}
	};
	return 1;
};

int ServiceConfig::from_json(const nlohmann::json& j, ServiceConfig& config) {
	std::ifstream file(this->paths.configPath());
	this->j = nlohmann::json::parse(file);
	try {
		j.at("general_frequency").get_to(config.general_frequency);
		j.at("log_dir").get_to(config.log_dir);
		j.at("log_enabled").get_to(config.log_enabled);
		j.at("version").get_to(config.version);
	}
	catch (const nlohmann::json::exception& e)
	{
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
		file.close();
		return 0;
	}
	file.close();
	return 1;

};
int ServiceConfig::json_handler()
{
	if (!fs::exists(this->paths.configPath()))
	{
		to_json(this->j, *this);
		std::ofstream file(this->paths.configPath());
		if (!file.is_open())
		{
			std::cout << "could not open config.json for writing\n";
		}
		file << j;
		file.close();
		return 1;
	};
	if (from_json(this->j, *this) == 1)
		return 1;
	else
	{
		std::cout << "existing json file invalid, deleting and creating a new one\n";
		int json_removed = fs::remove(this->paths.configPath());
		if (!json_removed)
		{
			std::cout << "something went wrong removing existing config.json\n";
			return 0;
		}
		this->json_handler();
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
	json_handler();
	std::cout << "Version: " << this->get_version() << "\n"
		<< "General Frequency: " << this->get_general_frequency() << "\n"
		<< "Log Enabled: " << (this->get_log_enabled() ? "true" : "false") << "\n"
		<< "Log Directory: " << this->get_log_dir() << std::endl;
	return 1;
}
