#ifndef SYNCSERVICEHANDLER_H
#define SYNCSERVICEHANDLER_H
#include <filesystem>
#include "stored_paths.h"
#include "dependencies/sqlite3/sqlite3.h"
#include "sync_service_config.h"
#include "sync_module.h"
#include "tcp_server.h"
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
	tcp_server*& tcp_server_;
	std::vector<SyncModule*> sync_modules;
	std::vector<std::string> types;
	std::vector<std::string> directions;

	bool& started;

	ServiceHandler(ServiceConfig* config, sqlite3*& db, bool& started, tcp_server*& server);
	~ServiceHandler();

	int add_sync_module(std::string name, fs::path source, fs::path destination, std::string type, std::string direction);
	int add_sync_module(SyncModule* module);
	int print_all_modules();
	int remove_sync_module(const SyncModule& module);
	int remove_sync_module(std::string name);
	int load_sync_modules();
	int update_sync_module(std::string name, SyncModule* module);
	SyncModule remove_sync_module_and_keep_copy(const SyncModule& module);
	SyncModule remove_sync_module_and_keep_copy(std::string name);
	SyncModule* get_module(std::string name);
private:
	int remove_sync_module_vector(std::string name);
	int get_current_unix_time();
};
#endif
