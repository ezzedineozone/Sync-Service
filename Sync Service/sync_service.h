#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include <filesystem>
#include "stored_paths.h"
#include "dependencies/sqlite3/sqlite3.h"
#include "sync_service_config.h"
#include "sync_module.h"
namespace fs = std::filesystem;
class SyncService {
public:
	StoredPaths paths;
	SyncServiceConfig config;
	sqlite3* db;
	std::vector<SyncModule> sync_modules;
	std::vector<std::string> types;
	std::vector<std::string> directions;

	bool started = false;

	SyncService();


	int instantiate_service();
	int instantiate_service(fs::path path);
	int close_service();
	int create_files();
	int add_sync_module(std::string name, fs::path source, fs::path destination, std::string type, std::string direction);
	int add_sync_module(SyncModule module);
	int print_all_modules();
	int remove_sync_module(SyncModule module);
	int remove_sync_module(std::string name);
	int update_sync_module(SyncModule module_old, SyncModule module_new);
private:
	fs::path find_existing_service(fs::path path);
	int check_service_validity(fs::path path);
	int create_db_schema();
	int load_sync_modules();
	int remove_sync_module_vector(std::string name);
};
#endif // !SYNCSERVICE_H
