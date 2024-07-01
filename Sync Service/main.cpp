#include <iostream>
#include <string>
#include <limits>
#include "sync_service.h"
SyncService* obj;
int commandHandler(std::string command)
{
	if (command == "start")
		return obj->instantiate_service();
	else if (command == "stop")
		return 0;
	else
	{
		std::cout << "invalid command\n";
		return 1;
	}
};
int requestCommand() {
	std::string command;
	std::cout << "sync_service 0.1.0: ";
	std::getline(std::cin, command);
	int return_val = commandHandler(command);
	while ((getchar()) != '\n');
	return return_val;
};
int main()
{
	obj = new SyncService();
	std::cout << "Background Sync Service: 0.1.0\n" << "https://github.com/ezzedineozone/Sync-Service\n" <<
		"this is a console app, can be fully controlled through commands\n" << "compatible UI for ease of use can be found at: https://github.com/ezzedineozone/ProjectSync\n"
		<< "Ezzedine Al Ozon.\n";
	while (requestCommand() != 0)
	{
	}
	std::cout << "program ended \n";
};

