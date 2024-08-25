#ifndef CMD_H
#define CMD_H
#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <vector>
#include "sync_service.h"
#include <filesystem>
class Cmd {
public:
	int print_help_functions();
	int request_command();
	Cmd(SyncService* obj);
private:
	SyncService* obj;
	std::string get_input();
	int check_args_validity(std::vector<std::string> args, std::string command);
	std::vector<std::string> parse_arguments(std::string msg);
	int command_handler(std::string msg);
};
#endif