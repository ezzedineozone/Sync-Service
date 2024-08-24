#ifndef SYNCSERVICEHANDLER_H
#define SYNCSERVICEHANDLER_H
#include <filesystem>
#include "stored_paths.h"
#include "dependencies/sqlite3/sqlite3.h"
#include "sync_service_config.h"
#include "sync_module.h"
/*
 * This class handles all interactions with the service during its runtime, including defining  functionalities.
 *
 * For operations related to the boot process and termination of the service, refer to sync_service.h.
 */

namespace fs = std::filesystem;
class ServiceHandler {
public:
	ServiceConfig* config;
	sqlite3*& db;
	std::vector<SyncModule*> sync_modules;
	std::vector<std::string> types;
	std::vector<std::string> directions;

	bool& started;

	ServiceHandler(ServiceConfig* config, sqlite3*& db, bool& started);
	~ServiceHandler();

	int add_sync_module(std::string name, fs::path source, fs::path destination, std::string type, std::string direction);
	int add_sync_module(SyncModule* module);
	int print_all_modules();
	int remove_sync_module(SyncModule module);
	int remove_sync_module(std::string name);
	int load_sync_modules();
	int update_sync_module(std::string name, SyncModule* module);
	SyncModule* get_module(std::string name);
private:
	fs::path find_existing_service(fs::path path);
	int check_service_validity(fs::path path);
	int create_db_schema();
	int remove_sync_module_vector(std::string name);
	int get_current_unix_time();
	SyncModule get_sync_module(std::string name);
	SyncInfo querySyncInfo(std::string name);

};
#endif // !SYNCSERVICE_H
