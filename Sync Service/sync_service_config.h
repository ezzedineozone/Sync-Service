#ifndef SYNCSERVICECONFIG_H
#define SYNCSERVICECONFIG_H
#include <string>
#include <filesystem>
#include "stored_paths.h"
#include "json.hpp"
#include <fstream>
namespace fs = std::filesystem;
class ServiceConfig {
private:
	std::string version;
	std::string general_frequency; // format Number,day/month/year/hour
	std::string log_dir;
	bool log_enabled;
	int create_files;

	int from_json(const nlohmann::json& j, ServiceConfig& config);
	int to_json(nlohmann::json& j, const ServiceConfig& config);
	int save_json(const nlohmann::json& j);
	int load_json(nlohmann::json& j);

public:
	ServiceConfig(); 
	ServiceConfig(fs::path path_to_json);


	StoredPaths paths;



	std::string get_version();
	std::string get_general_frequency();
	std::string get_log_dir();
	bool get_log_enabled();
	void set_log_enabled(bool val);
	int print_config();
};
#endif