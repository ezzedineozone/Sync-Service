#include "console.h"
#include "sync_service.h"
namespace fs = std::filesystem;
std::string Console::get_input() {
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
};
int Console::print_help_functions() {
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
int Console::check_args_validity(std::vector<std::string> args, std::string command)
{
	if (command == "add")
	{
		if (args.size() != 6)
		{
			std::cout << "Number of arguments invalid, type ? or help for more details.\n";
			return 0;
		}
		else if (std::find(service->get_handler()->types.begin(), service->get_handler()->types.end(), args.at(4)) == service->get_handler()->types.end())
		{
			std::cout << "Incorrect sync type, valid: local, cloud. \n";
			return 0;
		}
		else if (std::find(service->get_handler()->directions.begin(), service->get_handler()->directions.end(), args.at(5)) == service->get_handler()->directions.end())
		{
			std::cout << "Incorrect sync direction, valid: one-way, two-way, backup. \n";
			return 0;
		}
		return 1;
	}
	if (command == "remove")
	{
		if (args.size() != 2)
		{
			std::cout << "Number of arguments invalid, type ? or help for more details.\n";
			return 0;
		}

	}
	if (command == "update")
	{
		if (args.size() != 7)
		{
			std::cout << "Number of arguments invalid, type ? or help for more details.\n";
			return 0;
		}
	}
	if (command == "notify")
	{
		if (args.size() != 2)
		{
			std::cout << "Number of arguments invalid, type ? or help for more details.\n";
			return 0;
		}
	}
};
std::vector<std::string> Console::parse_arguments(std::string msg) {
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
};
int Console::command_handler_json(const nlohmann::json & j)
{
	std::string command = j["command"];
	if (command == "add")
	{
		SyncModule* sync_module = new SyncModule(j["data"]);
		service->get_handler()->add_sync_module(sync_module);
		return 1;
	}
	else if (command == "remove")
	{
		service->get_handler()->remove_sync_module(j["data"].dump());
		return 1;
	}
	else
	{
		return 0;
	}
}
int Console::command_handler(std::string msg)
{
	std::vector<std::string> args = Console::parse_arguments(msg);
	std::string command = args.at(0);
	if (command == "start")
		return service->instantiate_service();
	else if (command == "stop")
	{
		return service->instantiate_service();
	}
	else if (command == "add")
	{
		int valid = Console::check_args_validity(args, command);
		if (!valid)
			return 1;
		int module_added = service->get_handler()->add_sync_module(args.at(1), fs::path(args.at(2)), fs::path(args.at(3)), args.at(4), args.at(5));
		return module_added;
	}
	else if (command == "config")
	{
		return service->get_config()->print_config();
	}
	else if (command == "?" || command == "help")
	{
		return Console::print_help_functions();
	}
	else if (command == "list")
	{
		return service->get_handler()->print_all_modules();
	}
	else if (command == "remove")
	{
		int valid = Console::check_args_validity(args, command);
		if (!valid)
			return 1;
		int module_removed = service->get_handler()->remove_sync_module(args.at(1));
	}
	else if (command == "update")
	{
		int valid = Console::check_args_validity(args, command);
		auto* new_module = new SyncModule(args.at(2), fs::path(args.at(3)), fs::path(args.at(4)), args.at(5), args.at(6));
		if (valid)
			return service->get_handler()->update_sync_module(args.at(1), new_module);
		return 0;
	}
	else if (command == "reset")
	{
		
		return service->reset_service();
	}
	else if (command == "notify")
	{
		std::cout << args.at(1);
		return 1;
	}
	else
	{
		std::cout << "invalid command\n";
		return 1;
	}
};
int Console::request_command() {
	std::string msg = Console::get_input();
	return Console::command_handler(msg);
};
int Console::notify(const std::string& message) {
	std::cout << message;
	return 1;
}
int Console::notify_concurrent(const std::string& message)
{
	std::cout << message << "\nsync_service 0.1.0 (? or help for details): ";
	return 1;
}
SyncService* Console::service = nullptr;
