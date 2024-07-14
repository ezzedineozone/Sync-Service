#ifndef SYNCSERVICECONFIG_H
#define SYNCSERVICECONFIG_H
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
class ServiceConfig {
private:
	std::string version;
	std::string general_frequency; // format Number,day/month/year/hour
	std::string log_dir;

	bool log_enabled;

public:
	ServiceConfig(); 
	ServiceConfig(fs::path path_to_json);
	std::string get_version();
	std::string get_general_frequency();
	std::string get_log_dir();
	bool get_log_enabled();
	void set_log_enabled(bool val);
	void print_config();
};
#endif