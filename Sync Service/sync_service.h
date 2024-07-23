#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include "sync_service_handler.h"
#include "sync_service_config.h"
#include "stored_paths.h"
#include <string>
#include <filesystem>
#include <iostream>
#include "sqlite3.h"
/*
 * This class manages all operations related to the service boot process, including both initial and subsequent boots.
 * additionally, it handles service termination.
 *
 * for logic related to service interactions during runtime and its specific functionalities, refer to service_handler.h
 */
namespace fs = std::filesystem;
class SyncService {
public:
	SyncService();
	SyncService(fs::path path);
	SyncService(std::string path);
	~SyncService();
	ServiceHandler* get_handler();
	ServiceConfig* get_config();
	int instantiate_service();
	int print_config();
	int reset_service(); // this function will delete all service files and create them again
	int startup_routine();

private:
	ServiceHandler* handler;
	ServiceConfig* config;
	sqlite3* db;

	int create_files();
	int close_service();
	int check_service_validity(fs::path path);
	int find_existing_service(fs::path path);
	int create_db_schema();
	
	bool started;
	// if handler and config are pointers to the default constructor of both classes, then the service will use
	// the default configuration for both
};
#endif