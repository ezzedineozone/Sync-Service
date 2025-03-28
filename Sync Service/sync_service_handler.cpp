#include "sync_service.h"
#include "console.h"
namespace fs = std::filesystem;

ServiceHandler::ServiceHandler(ServiceConfig* config, sqlite3*& db, bool& started, tcp_server*& server)
	: config(config), db(db), started(started), tcp_server_(server) {
	this->sync_modules = std::vector<SyncModule*>();
	this->types = { "local", "cloud" };
	this->directions = { "one-way", "two-way", "backup" };
}
ServiceHandler::~ServiceHandler() {
	if (config) {
		delete config;
		config = nullptr;
	}
	for (auto module : sync_modules) {
		delete module;
	}
	sync_modules.clear();
};
int ServiceHandler::load_sync_modules() {

	const char* sql_stmt = "SELECT * FROM SYNCMODULE;";
	sqlite3_stmt* ppStmt;
	int sync_modules_querry = sqlite3_prepare_v2(db, sql_stmt, -1, &ppStmt, nullptr);
	if (sync_modules_querry == SQLITE_OK)
	{

		Console::notify("sync modules querried succesfully\n");
		int sync_modules_querried = sqlite3_step(ppStmt);
		while (sync_modules_querried == SQLITE_ROW)
		{
			std::string name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 0)));
			fs::path source = fs::path(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 1)));
			fs::path destination = fs::path(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 2)));
			std::string type = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 3)));
			std::string direction = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmt, 4)));
			SyncInfo* info;
			sqlite3_stmt* ppStmtInfo;
			int sync_info_querry = sqlite3_prepare_v2(db, (std::string("SELECT last_sync_date_unix, frequency, dirty from syncinfo where name = '") + name + std::string("';")).c_str(), -1, &ppStmtInfo, nullptr);
			if (sync_info_querry == SQLITE_OK)
			{
				int sync_info_querried = sqlite3_step(ppStmtInfo);
				int unix_time = sqlite3_column_int(ppStmtInfo, 0);
				std::string frequency = std::string(reinterpret_cast<const char*>(sqlite3_column_text(ppStmtInfo, 1)));
				int dirty = sqlite3_column_int(ppStmtInfo, 2);
				info = new SyncInfo(name, unix_time, frequency, dirty);
				this->sync_modules.push_back(new SyncModule(name, source, destination, type, direction, *info));
			}
			else
			{
				Console::notify("something went wrong querrying from syncinfo \n" + std::string(sqlite3_errmsg(db)) + "\n");
				return 0;
			}
			sync_modules_querried = sqlite3_step(ppStmt);
		}
		if (sync_modules_querried == SQLITE_DONE)
		{
			sqlite3_finalize(ppStmt);
			return 1;
		}
		return 0;
	}
	Console::notify("Error preparing SQL statement: " + std::string(sqlite3_errmsg(db)) + "\n");
	return 0;
};
int ServiceHandler::add_sync_module(SyncModule* module) {
	if (*module == SyncModule())
	{
		Console::notify("module not inserted \n");
		return 0;
	}
	if (!this->started)
	{
		Console::notify("Please start the service first, ? or help for more details\n");
		return 0;
	}
	std::ostringstream str;
	char* err_msg;
	str << "INSERT INTO SYNCMODULE VALUES ('" << module->name << "', '"
		<< module->source.string() << "', '"
		<< module->destination.string() << "', '"
		<< module->type << "', '"
		<< module->direction << "');";
	int sync_module_added = sqlite3_exec(db, str.str().c_str(), nullptr, nullptr, &err_msg);
	if (sync_module_added == SQLITE_OK)
	{
		Console::notify("Module added succesfully \n");
		std::ostringstream sync_info_str;
		char* sync_info_err;
		int unix_timestamp = this->get_current_unix_time();
		sync_info_str << "INSERT INTO SYNCINFO VALUES ('" << module->info.name << "', "
			<< module->info.get_last_sync_date_unix() << ", '' ," << 0 << ");";
		int sync_info_added = sqlite3_exec(db, sync_info_str.str().c_str(), nullptr, nullptr, &sync_info_err);
		if (sync_info_added != SQLITE_OK)
		{
			Console::notify("something went wrong adding module\n" + std::string(sync_info_err) + "\n");
			this->tcp_server_->notify_failure("add", "something went wrong adding info\n" + std::string(sync_info_err) + "\n");
			this->remove_sync_module(*module);
			return 0;
		}
		else
		{
			this->sync_modules.push_back(module);
			this->tcp_server_->notify_add(*module);
		}
	}
	else
	{
		this->tcp_server_->notify_failure("add", "something went wrong adding module\n" + std::string(err_msg) + "\n");
		return 0;
	}
	return 1;
};
int ServiceHandler::add_sync_module(std::string name, fs::path source, fs::path destination, std::string type, std::string direction)
{
	SyncModule* module = new SyncModule(name, source, destination, type, direction);
	return add_sync_module(module);
};
int ServiceHandler::remove_sync_module(std::string name)
{
	if (!this->started)
	{
		Console::notify("Please start the service first, ? or help for more details\n");
		return 1;
	};
	std::ostringstream str;
	char* err_msg;
	str << "DELETE FROM SYNCMODULE WHERE name = \'" << name << "\';";
	int module_deleted = sqlite3_exec(db, str.str().c_str(), nullptr, nullptr, &err_msg);
	if (module_deleted == SQLITE_OK)
	{
		int changes = sqlite3_changes(db);
		if (changes == 0)
		{
			Console::notify("Module not found \n");
			return 0;
		}
		int sync_module_removed_vector = this->remove_sync_module_vector(name);
		Console::notify("module deleted succesfully\n");
		tcp_server_->notify_removal(name);
		char* info_err_msg;
		int info_deleted = sqlite3_exec(db, reinterpret_cast<const char*>((std::string("delete from syncinfo where name == '") + name + std::string("';")).c_str()), nullptr, nullptr, &info_err_msg);
		changes = sqlite3_changes(db);
		if (changes == 0)
		{
			Console::notify("something went wrong deleting info\n");
			return 0;
		}
		return 1;
	}
	else
	{
		Console::notify("something went wrong removing module\n" + std::string(err_msg) + "\n");
		this->tcp_server_->notify_failure("remove", "something went wrong removing module\n" + std::string(err_msg) + "\n");
	}	
	return 0;
};
SyncModule ServiceHandler::remove_sync_module_and_keep_copy(const SyncModule& module)
{
	return remove_sync_module_and_keep_copy(module.name);
};
SyncModule ServiceHandler::remove_sync_module_and_keep_copy(std::string name)
{
	if (!this->started)
	{
		Console::notify("Please start the service first, ? or help for more details\n");
	};
	std::ostringstream str;
	char* err_msg;
	str << "DELETE FROM SYNCMODULE WHERE name == '" << name << "';";
	int module_deleted = sqlite3_exec(db, str.str().c_str(), nullptr, nullptr, &err_msg);
	SyncModule module_to_return = SyncModule();
	if (module_deleted == SQLITE_OK)
	{
		int changes = sqlite3_changes(db);
		if (changes == 0)
		{
			Console::notify("Module not found \n");
		}
		if (name != "")
			module_to_return = SyncModule(*get_module(name));
		int sync_module_removed_vector = this->remove_sync_module_vector(name);
		Console::notify("module deleted succesfully\n");
		tcp_server_->notify_removal(name);
		char* info_err_msg;
		int info_deleted = sqlite3_exec(db, reinterpret_cast<const char*>((std::string("delete from syncinfo where name == '") + name + std::string("';")).c_str()), nullptr, nullptr, &info_err_msg);
		changes = sqlite3_changes(db);
		if (changes == 0)
		{
			Console::notify("something went wrong deleting info\n");
		}
	}
	else
		Console::notify("something went wrong removing module\n" + std::string(err_msg) + "\n");
	return module_to_return;
};
int ServiceHandler::remove_sync_module(const SyncModule& module)
{
	return remove_sync_module(module.name);
};
int ServiceHandler::remove_sync_module_vector(std::string name) {
	auto iterator = this->sync_modules.begin();
	while (iterator != this->sync_modules.end())
	{
		SyncModule* module = *iterator;
		if (module->name == name)
		{
			delete module;
			iterator = this->sync_modules.erase(iterator);
		}
		else
			iterator++;
	}
	return 1;
};
int ServiceHandler::print_all_modules() {

	if (!this->started)
	{
		Console::notify("Please start the service first, ? or help for more details\n");
		return 1;
	}


	Console::notify("-----------------------------------------\n");
	for (const auto& module : this->sync_modules) {
		Console::notify("Name: " + module->name + "\n" + "Source: " + module->source.string() + "\n"
			+ "Destination: " + module->destination.string() + "\n" + "Type: " + module->type + "\n"
			+ "Direction: " + module->direction + "\n----------------------------------\n");
	}
	return 1;
};
int ServiceHandler::update_sync_module(std::string name, SyncModule* module) {
	SyncModule* old_module = get_module(name);
	if (!old_module)
	{
		Console::notify("Old module not found\n");
		return 0;
	}
	if (*module == SyncModule())
	{
		Console::notify("new module invalid\n");
		return 0;
	}
	SyncModule temp_module = remove_sync_module_and_keep_copy(old_module->name);
	if (temp_module == SyncModule())
	{
		Console::notify("something went wrong adding new module\n");
		this->tcp_server_->notify_failure("add", "something went wrong adding new module\n");
		return 0;
	}
	else
	{
		int sync_module_added = add_sync_module(module);
		if (!sync_module_added)
		{
			Console::notify("something went wrong removing old module\n");
			this->tcp_server_->notify_failure("add", "something went wrong removing old module\n");
			remove_sync_module(module->name);
			add_sync_module(temp_module.name, temp_module.source, temp_module.destination, temp_module.type, temp_module.direction);
			return 1;
		}
	}
	return 1;
};
SyncModule* ServiceHandler::get_module(std::string name) {
	for (auto module : sync_modules)
	{
		if (module->name == name)
			return module;
	}
	return new SyncModule();
};
int ServiceHandler::get_current_unix_time() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	return static_cast<int>(seconds);
};
