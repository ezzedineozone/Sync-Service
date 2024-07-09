#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <vector>
#include <algorithm>
#include "sync_service.h"
SyncService* obj;

std::string getInput() {
	std::string input;
	char c;
	while (std::cin.get(c)) {
		if (c != '\n') {
			std::cin.unget(); // Put back the non-newline character
			break;
		}
	}
	std::getline(std::cin, input);
	return input;
}
int printHelpFunctions() {
	std::cout << "View the following commands to use the service:\n\n"
		<< "start\n"
		<< "    starts the service - required to interact with app\n\n"
		<< "stop\n"
		<< "    stops the service\n\n"
		<< "add [name] [source] [destination] [type] [direction]\n"
		<< "    example: add example_module \"C:\\ProgramFiles\" \"D:\\ProgramFiles\" local one-way\n\n"
		<< "remove [name]\n"
		<< "    example: remove example_module\n\n";
	return 1;
};
int checkArgsValidity(std::vector<std::string> args, std::string command)
{
	if (command == "add")
	{
		if (args.size() != 6)
		{
			std::cout << "Number of arguments too small, type ? or help for more details.\n";
			return 0;
		}
		else if (std::find(obj->types.begin(), obj->types.end(), args.at(4)) == obj->types.end())
		{
			std::cout << "Incorrect sync type, valid: local, cloud. \n";
			return 0;
		}
		else if (std::find(obj->directions.begin(), obj->directions.end(), args.at(5)) == obj->directions.end())
		{
			std::cout << "Incorrect sync direction, valid: one-way, two-way, backup. \n";
			return 0;
		}
		return 1;
	}
};
std::vector<std::string> parseArguments(std::string msg) {
	std::istringstream stream(msg);
	std::vector<std::string> args;
	std::string arg;
	bool quotes = false;
	std::string currentArg;

	while (stream >> std::ws) {
		char c = stream.peek();
		if (c == '\"') {
			quotes = !quotes;
			stream.get();  
			if (quotes) {
				std::getline(stream, arg, '\"');
				currentArg = arg;
				while (currentArg.back() == '\\') {
					currentArg.pop_back();  
					currentArg += "\"";  
					std::getline(stream, arg, '\"');
					currentArg += arg;  
				}
				args.push_back(currentArg);
				quotes = !quotes;
			}
		}
		else {
			stream >> arg;
			args.push_back(arg);
		}
	}

	if (quotes) {
		std::cerr << "Error: mismatched quotes in input string." << std::endl;
	}

	return args;
}
int commandHandler(std::string msg)
{
	std::vector<std::string> args = parseArguments(msg);
	std::string command = args.at(0);
	if (command == "start")
		return obj->instantiate_service();
	else if (command == "stop")
	{
		return obj->close_service();
	}
	else if (command == "add")
	{
		int valid = checkArgsValidity(args, command);
		if (!valid)
			return 1;
		int module_added = obj->add_sync_module(args.at(1), fs::path(args.at(2)), fs::path(args.at(3)), args.at(4), args.at(5));
		return module_added;
	}else if (command == "?" || command == "help")
	{
		return printHelpFunctions();
	}
	else if (command == "list")
	{
		return obj->print_all_modules();
	}
	else
	{
		std::cout << "invalid command\n";
		return 1;
	}
};
int requestCommand() {
	std::string msg = getInput();
	return commandHandler(msg);
};
int main()
{
	obj = new SyncService();
	std::cout << "Background Sync Service: 0.1.0\n" << "https://github.com/ezzedineozone/Sync-Service\n" <<
		"this is a console app, can be fully controlled through commands\n" << "compatible UI for ease of use can be found at: https://github.com/ezzedineozone/ProjectSync\n"
		<< "Ezzedine Al Ozon.\n";
	do {
		std::cout << "sync_service 0.1.0 (? or help for details): ";
	} while (requestCommand() != 0);
	std::cout << "program ended \n";
};

