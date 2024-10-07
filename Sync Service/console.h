#ifndef CMD_H
#define CMD_H
class SyncService;
#include <string>
#include <vector>
#include "dependencies/json/json.hpp"
class Console {
public:
	static int print_help_functions();
	static int request_command();
	static int notify(const std::string& message);
	static int notify_concurrent(const std::string& message);
	static int command_handler_json(const nlohmann::json& j);
	static SyncService* service;
private:
	static std::string get_input();
	static int command_handler(std::string msg);
	static int check_args_validity(std::vector<std::string> args, std::string command);
	static std::vector<std::string> parse_arguments(std::string msg);

};
#endif