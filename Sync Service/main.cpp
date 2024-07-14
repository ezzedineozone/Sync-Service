#include "cmd.h"
#include "sync_service.h"
SyncService* obj;
int main()
{
	obj = new SyncService();
	Cmd commands(obj->get_handler());
	std::cout << "Background Sync Service: 0.1.0\n" << "https://github.com/ezzedineozone/Sync-Service\n" <<
		"this is a console app, can be fully controlled through commands\n" << "compatible UI for ease of use can be found at: https://github.com/ezzedineozone/ProjectSync\n"
		<< "Ezzedine Al Ozon.\n";
	do {
		std::cout << "sync_service 0.1.0 (? or help for details): ";
	} while (commands.request_command() != 0);
	std::cout << "program ended \n";
};

