#ifndef CMD_H
#define CMD_H
class SyncService;
#include <string>
#include <vector>
class Console {
public:
	static int print_help_functions();
	static int request_command(SyncService& service);
	static int notify(const std::string& message);
	static int notify_concurrent(const std::string& message);
private:
	static std::string get_input();
	static int check_args_validity(std::vector<std::string> args, std::string command,SyncService& obj);
	static std::vector<std::string> parse_arguments(std::string msg);
	static int command_handler(std::string msg,SyncService& obj);
};
#endif