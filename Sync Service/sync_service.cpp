#include "sync_service.h"
#include <filesystem>
#include <ShlObj_core.h>
#include <iostream>
#include "sqlite3/sqlite3.h"
namespace fs = std::filesystem;


SyncService::SyncService() {
	db = nullptr;
};
int SyncService::instantiate_service(fs::path path) {
	fs::path existing_path = find_existing_service(path);
	if (existing_path.empty())
	{
		std::cout << "exiting path not found, creating a new one... \n";
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
			std::cout << "existing service folder invalid... \n deleting and creating a new one ... \n";
			bool removal_success = fs::remove_all(existing_path);
			if (removal_success)
			{
				std::cout << "existing service folder deleted \n creating a new one...\n";
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
		std::cout << "sqlite3_path" << sqlite3_path << "\n";
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
				std::cout << "something went wrong creating database... \n";
		}
		else
		{
			std::cout << "error creating sqlite3 directory";
		}

	}
	else
	{
		std::cout << "service not instantiated, please run the instantiate_service() function";
	}
	return 1;
};
int SyncService::create_db_schema() {
	std::cout << "begin creating of database schema...\n";
	if (db == nullptr)
		std::cout << "database connection not established, please run create_files() function";
	else
	{
		//code below is for the sync module table
		const char* sync_module_statement = "CREATE TABLE SYNCMODULE(id TEXT PRIMARY KEY, source VARCHAR(259) NOT NULL, destination VARCHAR(259) NOT NULL, type VARCHAR(15) NOT NULL, direction VARCHAR(15) NOT NULL);";
		char* error_msg;
		int sync_module_table_created = sqlite3_exec(db, sync_module_statement, nullptr, nullptr,&error_msg);
		if (sync_module_table_created == SQLITE_OK)
			std::cout << "Sync module table created succesfully \n";
		else
			std::cout << "Sync module table creation error \n" << error_msg << "\n";
	}
	return 1;
};
