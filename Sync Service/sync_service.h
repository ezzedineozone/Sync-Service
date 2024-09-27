#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include "dependencies/asio/asio.hpp"
#include "sync_service_handler.h"
#include "sync_service_config.h"
#include "stored_paths.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <ShlObj_core.h>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include <chrono>
#include "dependencies/sqlite3/sqlite3.h"
#include "dependencies/json/json.hpp"
#include "tcp_server.h"
#include <memory>
#include <thread>
/*
 * This class manages all operations related to the service boot process, including both initial and subsequent boots.
 * additionally, it handles service termination.
 *
 * for logic related to service interactions during runtime and its specific functionalities, go to service_handler.h
 */
namespace fs = std::filesystem;
class SyncService {
public:
	SyncService();
	SyncService(fs::path path);
	SyncService(std::string path);
	SyncService(const SyncService& other);
	~SyncService();
	ServiceHandler* get_handler() const;
	ServiceConfig* get_config() const;
	int instantiate_service();
	int print_config();
	int reset_service(); // this function will delete all service files and create them again
	int startup_routine();

private:
	ServiceHandler* handler;
	ServiceConfig* config;
	sqlite3* db;
	tcp_server* tcp_server_;
	std::unique_ptr<std::thread> tcp_thread_;
	asio::io_context tcp_io_context_;
	bool tcp_server_started;

	int create_files();
	int create_db_schema();
	int check_service_validity(fs::path path);
	int find_existing_service(fs::path path);
	int start_tcp_server();
	int close_service();


	
	bool started;
	// if handler and config are pointers to the default constructor of both classes, then the service will use
	// the default configuration for both
};
#endif