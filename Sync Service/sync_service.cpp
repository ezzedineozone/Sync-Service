#include "sync_service.h"
#include <filesystem>
#include <ShlObj_core.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include "sqlite3/sqlite3.h"
namespace fs = std::filesystem;


SyncService::SyncService() {
	db = nullptr;
	this->paths = StoredPaths();
	this->config = SyncServiceConfig();
};
int SyncService::instantiate_service(fs::path path) {
	fs::path existing_path = find_existing_service(path);
	if (existing_path.empty())
	{
		std::cout << "Looks like sync service isnt currently installed on your pc, would you like to install it? (Y/N): ";
		std::string user_answer;
		std::cin >> user_answer;
		while (_stricmp(user_answer.c_str(), "y") != 0 && _stricmp(user_answer.c_str(), "n") != 0)
		{
			std::cout << "Please enter Y for yes, N for no to answer install service: ";
			std::cin >> user_answer;
		}
		if (_stricmp(user_answer.c_str(), "n") == 0)
			return 0;
		fs::path service_relative_path = "sync_service";
		fs::path combined_paths = this->paths.getDefaultPath() / service_relative_path;
		bool dir_created = fs::create_directory(combined_paths);
		if (dir_created)
		{
			std::cout << "created succesfully! \n";
			paths.getServicePath() = combined_paths;
			int files_created = create_files();
			int schema_created = create_db_schema();
		}
		else
			std::cout << "something went wrong creating service directory \n";
		 // create directory for service at given path and if succesful, set service_path to given path
	}
	else
	{
		std::cout << "existing path non empty \n";
		std::cout << "checking validity for existing path... \n";
		int valid = this->check_service_validity(existing_path);
		if (valid)
		{
			std::cout << "existing service folder valid \n";
			paths.getServicePath() = existing_path;
		}
		else
		{
			std::cout << "existing service folder invalid... \ndeleting and creating a new one ... \n";
			bool removal_success = fs::remove_all(existing_path);
			if (removal_success)
			{
				std::cout << "existing service folder deleted \ncreating a new one...\n";
				instantiate_service(path);
			}
		}
		//set service_path to given path if validity check works
		//currently checkServiceValidity is empty returns 1 each time, implement next commit
	}
	return 1;
};
int SyncService::instantiate_service(){
	std::cout << "crearing directory at default_path... \n";
	this->instantiate_service(paths.getDefaultPath());
	return 1;
};

fs::path SyncService::find_existing_service(fs::path path) {
	fs::directory_iterator roaming_dir_iterator = fs::directory_iterator(path);
	fs::directory_iterator end = fs::directory_iterator();
	fs::path relative_service_path = "sync_service";
	fs::path combined_paths = path / relative_service_path;
	while (roaming_dir_iterator != end)
	{
		fs::directory_entry entry = *roaming_dir_iterator;
		fs::path entry_path = entry.path();
		if (entry_path.wstring() == combined_paths.wstring())
			return entry_path;
		roaming_dir_iterator++;
	}
	return fs::path();
};
int SyncService::check_service_validity(fs::path path) {
	//returns 0 if service files are valid, 1 otherwise, could maybe be used as a future update checker for the service???
	return 0;
};


int SyncService::create_files()
{
	std::cout << "begin file creation... \n";
	if(paths.getServicePath() != fs::path())
	{
		std::cout << "service path is created... \n";
		std::cout << "begin creation of database... \n";
		fs::path sqlite3_path = paths.getServicePath() / "sqlite3";
		bool sqlite3_dir_created = fs::create_directory(sqlite3_path);
		if (sqlite3_dir_created)
		{
			fs::path sqlite3_file_path = sqlite3_path /  fs::path("database.db");
			int result = sqlite3_open(reinterpret_cast<const char*>(sqlite3_file_path.string().c_str()), &db);
			if (result == SQLITE_OK)
			{
				std::cout << "database file created... \n";
			}
			else
			{
				std::cout << "something went wrong creating database... \n";
				return 0;
			}
		}
		else
		{
			std::cout << "error creating sqlite3 directory";
			return 0;
		}
	}
	else
	{
		std::cout << "service not instantiated, please run the instantiate_service() function";
		return 0;
	}
	return 1;
};
int SyncService::create_db_schema() {
	std::cout << "begin creating of database schema...\n";
	if (db == nullptr)
	{
		std::cout << "database connection not established, please run create_files() function";
		return 0;
	}
	else
	{
		//code below is for the SYNCMODULE table
		const char* sync_module_statement = "CREATE TABLE SYNCMODULE(id TEXT PRIMARY KEY, source VARCHAR(259) NOT NULL, destination VARCHAR(259) NOT NULL, type VARCHAR(15) NOT NULL, direction VARCHAR(15) NOT NULL);";
		char* sync_module_error_msg;
		int sync_module_table_created = sqlite3_exec(db, sync_module_statement, nullptr, nullptr,&sync_module_error_msg);
		if (sync_module_table_created == SQLITE_OK)
			std::cout << "Sync module table created succesfully \n";
		else
		{
			std::cout << "Sync module table creation error\n"<<sync_module_error_msg;
			return 0;
		}

		//code below is for SYNCSERVICECONFIG table
		//the hard coded config is in the sync_service_config class, the default constructor

		const char* sync_config_statement = "CREATE TABLE CONFIG(version VARCHAR(10),log_dir VARCHAR(259), log_enabled INT, general_frequency VARCHAR(20));";
		char* sync_config_creation_error;
		int sync_config_table_created = sqlite3_exec(db, sync_config_statement, nullptr, nullptr, &sync_config_creation_error);
		if (sync_config_table_created == SQLITE_OK)
			std::cout << "SYNCCONFIGTABLE created succesfully \n";
		else
		{
			std::cout << "something went wrong creating SYNCCONFIG TABLE\n" << sync_config_creation_error;
			return -1;
		}

		std::ostringstream oss;
		oss << "INSERT INTO CONFIG VALUES ("
			<< "'" << config.get_version() << "', "
			<< "'" << config.get_log_dir() << "', "
			<< ((config.get_log_enabled() ? 1 : 0)) << ", "
			<< "'" << config.get_general_frequency() << "');";
		std::string sync_config_insert = oss.str();
		char* sync_config_insertion_error;
		int config_inserted = sqlite3_exec(db, sync_config_insert.c_str(), nullptr, nullptr, &sync_config_insertion_error);
		if (config_inserted == SQLITE_OK)
			std::cout << "config created succesfully \n";
		else
		{
			std::cout << "something went wrong inserting default config\n" << sync_config_insertion_error;
			return -1;
		}
	}
};
