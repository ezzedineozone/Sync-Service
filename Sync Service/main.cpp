#include "cmd.h"
#include "sync_service.h"
#include "sync_module.h"
#include "dependencies/json/json.hpp"
SyncService* obj;
int main()
{
	obj = new SyncService();
	Cmd commands(obj);
	std::cout << "Background Sync Service: 0.1.0\n" << "https://github.com/ezzedineozone/Sync-Service\n" <<
		"this is a console app, can be fully controlled through commands\n" << "compatible UI for ease of use can be found at: https://github.com/ezzedineozone/ProjectSync\n"
		<< "Ezzedine Al Ozon.\n";
	//do {
	//	std::cout << "sync_service 0.1.0 (? or help for details): ";
	//} while (commands.request_command() != 0);
	SyncModule test_module("module3", fs::path("C:\\test_folder"), fs::path("C:\\test_folder"), "local", "one-way");
	nlohmann::json j = test_module.to_json();
	std::cout << j << "\n";
	SyncModule test_module_2(j);
	std::cout << test_module_2.to_string() << "\n";
	std::cout << "program ended \n";
};

