#include "sync_service.h"
#include <filesystem>
#include <ShlObj_core.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include "sqlite3.h"
namespace fs = std::filesystem;

ServiceHandler::ServiceHandler(ServiceConfig* config, sqlite3*& db, bool& started) : config(config) , db(db), started(started) {
	this->sync_modules = std::vector<SyncModule>();
	this->types = { "local", "cloud" };
	this->directions = { "one-way", "two-way", "backup" };
};
int ServiceHandler::load_sync_modules() {
	//// code below adds a dummy sync module
	//// temp code remove before release
	//const char* add_stmt = "INSERT INTO SYNCMODULE VALUES ('test_module', 'C:\\Users\\Administrator\\Documents\\VS Projects\\Sync Service', 'C:\\test_folder', 'local', 'one-way');";
	//char* add_err_msg;
	//int dummy_inserted = sqlite3_exec(db, add_stmt, nullptr, nullptr, &add_err_msg);
	//if (dummy_inserted == SQLITE_OK)
	//{
	//	std::cout << "dummy syncmodule inserted succesfully \n";
	//}
	//else
	//{
	//	std::cout << "something went wrong inserting dummy \n" << add_err_msg << "\n";
	//	return 0;
	//}



	const char* sql_stmt = "SELECT * FROM SYNCMODULE;";
	sqlite3_stmt* ppStmt;
	int sync_modules_querry = sqlite3_prepare_v2(db, sql_stmt, -1, &ppStmt, nullptr);
	if (sync_modules_querry == SQLITE_OK)
	{
		std::cout << "sync modules querried succesfully\n";
		int sync_modules_querried = sqlite3_step(ppStmt);
		while (sync_modules_querried == SQLITE_ROW)
		{
			std::string name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 0)));
			fs::path source = fs::path(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 1)));
			fs::path destination = fs::path(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 2)));
			std::string type = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 3)));
			std::string direction = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 4)));
			this->sync_modules.push_back(SyncModule(name, source, destination, type, direction));
			sync_modules_querried = sqlite3_step(ppStmt);
		}
		if (sync_modules_querried == SQLITE_DONE)
		{
			sqlite3_finalize(ppStmt);
			return 1;
		}
		return 0;
	}
	std::cout << "Error preparing SQL statement: " << sqlite3_errmsg(db) << "\n";
	return 0;
};
int ServiceHandler::add_sync_module(SyncModule module) {
	if (module == SyncModule())
	{
		std::cout << "module not inserted \n";
		return 1;
	}
	if (!this->started)
	{
		std::cout << "Please start the service first, ? or help for more details\n";
		return 1;
	}
	std::ostringstream str;
	char* err_msg;
	str << "INSERT INTO SYNCMODULE VALUES ('" << module.name << "', '"
		<< module.source.string() << "', '"
		<< module.destination.string() << "', '"
		<< module.type << "', '"
		<< module.direction << "');";
	int sync_module_added = sqlite3_exec(db, str.str().c_str(), nullptr, nullptr, &err_msg);
	if (sync_module_added == SQLITE_OK)
	{
		std::cout << "Module added succesfully \n";
		this->sync_modules.push_back(module);
	}
	else
	{
		std::cout << "Something went wrong inserting module \n" << err_msg << "\n";
		return 0;
	}
	return 1;
};
int ServiceHandler::add_sync_module(std::string name, fs::path source, fs::path destination, std::string type, std::string direction)
{
	SyncModule module(name, source, destination, type, direction);
	return add_sync_module(module);
};
int ServiceHandler::remove_sync_module(std::string name)
{
	if (!this->started)
	{
		std::cout << "Please start the service first, ? or help for more details\n";
		return 1;
	};
	std::ostringstream str;
	char* err_msg;
	str << "DELETE FROM SYNCMODULE WHERE name == '" << name << "';";
	int module_deleted = sqlite3_exec(db, str.str().c_str(), nullptr, nullptr, &err_msg);
	if (module_deleted == SQLITE_OK)
	{
		int changes = sqlite3_changes(db);
		if (changes == 0)
		{
			std::cout << "Module not found \n";
			return 1;
		}
		int sync_module_removed_vector = this->remove_sync_module_vector(name);
		std::cout << "module deleted succesfully\n";
		return 1;
	}
	else
		std::cout << "something went wrong removing module\n" << err_msg << "\n";
	return 0;
};
int ServiceHandler::remove_sync_module(SyncModule module)
{
	return remove_sync_module(module.name);
};
int ServiceHandler::remove_sync_module_vector(std::string name) {
	auto logical_end_it = std::remove_if(sync_modules.begin(), sync_modules.end(), [&name](const SyncModule& module) {
		return module.name == name;
		});
	sync_modules.erase(logical_end_it, sync_modules.end());
	return 1;
};
int ServiceHandler::print_all_modules() {
	if (!this->started)
	{
		std::cout << "Please start the service first, ? or help for more details\n";
		return 1;
	}


	// Print each module
	std::cout << "-----------------------------------------\n";
	for (const auto& module : sync_modules) {
		std::cout << "Name: " << module.name << "\n" << "Source: " << module.source << "\n"
			<< "Destination: " << module.destination << "\n" << "Type: " << module.type << "\n"
			<< "Direction: " << module.direction << "\n----------------------------------\n";
	}
	return 1; // Return value as per your original function signature
};
