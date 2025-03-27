#include "sync_service.h"
#include "console.h"
SyncService::SyncService() {
	this->config = new ServiceConfig();
	this->started = false;
	this->tcp_server_started = false;
	this->handler = new ServiceHandler(this->config, this->db, this->started, this->tcp_server_);
};
SyncService::SyncService(fs::path path) : tcp_io_context_() {
	this->config = new ServiceConfig(path);
	this->started = false;
	this->tcp_server_started = false;
	this->tcp_server_ = nullptr;
	this->handler = new ServiceHandler(this->config, this->db, this->started, this->tcp_server_);
};
SyncService::~SyncService() {
	if (db) {
		sqlite3_close(db);
		db = nullptr;
	}
	if (handler) {
		delete handler;
		handler = nullptr;
	}
	if (config) {
		delete config;
		config = nullptr;
	}
	if (tcp_server_)
	{
		delete tcp_server_;
		tcp_server_ = nullptr;
	}
	if (tcp_thread_ && tcp_thread_->joinable()) {
		tcp_thread_->join();
	}
}
int SyncService::check_service_validity(fs::path path) {
	//returns 0 if service files are valid, 1 otherwise, could maybe be used as a future update checker for the service???
	return 1;
};
int SyncService::instantiate_service() {
	fs::path path = this->config->paths.servicePath();
	if (this->started)
	{
		Console::notify("service already started!\n");
		return 1;
	}
	if (!find_existing_service(this->config->paths.servicePath()))
	{
		Console::notify("Looks like sync service isn't currently installed on your pc, would you like to install it? (Y/N):");
		std::string user_answer;
		std::cin >> user_answer;
		while (_stricmp(user_answer.c_str(), "y") != 0 && _stricmp(user_answer.c_str(), "n") != 0)
		{
			Console::notify("Please enter Y for yes, N for no to answer install service:");
			std::cin >> user_answer;
		}
		if (_stricmp(user_answer.c_str(), "n") == 0)
			return 0;
		this->config->json_handler();
		bool dir_created = fs::create_directory(this->config->paths.servicePath());
		if (dir_created)
		{
			Console::notify("created successfully!\n");
			int files_created = create_files();
			int schema_created = create_db_schema();
		}
		else
			Console::notify("something went wrong creating service directory\n");
	}
	else
	{
		Console::notify("existing path non empty\n");
		Console::notify("checking validity for existing path...\n");
		int valid = this->check_service_validity(this->config->paths.servicePath());
		if (valid)
		{
			Console::notify("existing service folder valid\n");
			Console::notify("loading config\n");
			int config_loaded = this->config->json_handler();
			if (!config_loaded)
				Console::notify("something went wrong loading config\n");
			int opened = sqlite3_open(reinterpret_cast<const char*>(this->config->paths.sqlite3Path().string().c_str()), &db);
			if (opened == SQLITE_OK)
			{
				Console::notify("connection to database opened\n");
				this->started = true;
			}
		}
		else
		{
			Console::notify("existing service folder invalid... \ndeleting and creating a new one...\n");
			bool removal_success = fs::remove_all(this->config->paths.servicePath());
			if (removal_success)
			{
				Console::notify("existing service folder deleted \ncreating a new one...\n");
				instantiate_service();
			}
		}
	}
	this->started = true;
	return startup_routine();
};
int SyncService::startup_routine() {
	int modules_loaded = this->handler->load_sync_modules();
	start_tcp_server();
	return 1;
};
int SyncService::close_service() {
	if (this->started == false)
	{
		Console::notify("service not started\n");
		return 1;
	}
	int con_closed = sqlite3_close(this->db);
	if (con_closed != SQLITE_OK)
	{
		Console::notify("an unexpected error occurred\n");
		return 0;
	}
	else
	{
		Console::notify("service stopped\n");
		this->started = false;
		return 1;
	}
};
int SyncService::create_files()
{
	Console::notify("begin file creation...\n");
	if (this->config->paths.servicePath() != fs::path())
	{
		Console::notify("service path is created...\n");
		Console::notify("begin creation of database...\n");
		fs::path sqlite3_path = this->config->paths.servicePath() / "sqlite3";
		bool sqlite3_dir_created = fs::create_directory(sqlite3_path);
		if (sqlite3_dir_created)
		{
			fs::path sqlite3_file_path = this->config->paths.sqlite3Path();
			int result = sqlite3_open(reinterpret_cast<const char*>(sqlite3_file_path.string().c_str()), &db);
			if (result == SQLITE_OK)
			{
				Console::notify("database file created...\n");
			}
			else
			{
				Console::notify("something went wrong creating database...\n");
				return 0;
			}
		}
		else
		{
			Console::notify("error creating sqlite3 directory\n");
			return 0;
		}
	}
	else
	{
		Console::notify("service not instantiated, please run the instantiate_service() function\n");
		return 0;
	}
	int config_loaded = this->config->json_handler();
	if (!config_loaded)
		Console::notify("something went wrong loading config\n");
	return 1;
};
int SyncService::find_existing_service(fs::path path) {
	fs::directory_entry entry = fs::directory_entry(path);
	if (entry.exists())
		return 1;
	return 0;
};
int SyncService::create_db_schema() {
	Console::notify("begin creating of database schema...\n");
	if (db == nullptr)
	{
		Console::notify("database connection not established, please run create_files() function\n");
		return 0;
	}
	else
	{
		const char* sync_module_statement = "CREATE TABLE SYNCMODULE(name TEXT PRIMARY KEY COLLATE NOCASE, source VARCHAR(259) NOT NULL, destination VARCHAR(259) NOT NULL, type VARCHAR(15) NOT NULL, direction VARCHAR(15) NOT NULL);";
		char* sync_module_error_msg;
		int sync_module_table_created = sqlite3_exec(db, sync_module_statement, nullptr, nullptr, &sync_module_error_msg);
		if (sync_module_table_created == SQLITE_OK)
			Console::notify("Sync module table created successfully\n");
		else
		{
			Console::notify("Sync module table creation error\n");
			Console::notify(sync_module_error_msg);
			return 0;
		}
	}
	if (db == nullptr)
	{
		Console::notify("database connection not established, please run create_files() function\n");
		return 0;
	}
	else
	{
		const char* sync_info_statement = "CREATE TABLE SYNCINFO(name TEXT PRIMARY KEY,last_sync_date_unix INT, frequency varchar(20), dirty INT,FOREIGN KEY(name) REFERENCES SYNCMODULE(name));";
		char* sync_info_error_msg;
		int sync_module_table_created = sqlite3_exec(db, sync_info_statement, nullptr, nullptr, &sync_info_error_msg);
		if (sync_module_table_created == SQLITE_OK)
			Console::notify("Sync info table created successfully\n");
		else
		{
			Console::notify("Sync info table creation error\n");
			Console::notify(sync_info_error_msg);
			return 0;
		}
	}
	return 1;
};
SyncService::SyncService(std::string path) : SyncService(fs::path(path)) {
};
ServiceConfig* SyncService::get_config() const {
	return this->config;
};
ServiceHandler* SyncService::get_handler() const {
	return this->handler;
};
int SyncService::print_config() {
	return this->config->print_config();
};
int SyncService::reset_service() {
	if (this->started)
	{
		auto connection_closed = sqlite3_close(this->db);
		if (connection_closed != SQLITE_OK)
			return 0;
		else
			this->started = false;
	}
	if (!fs::directory_entry(this->config->paths.servicePath()).exists())
	{
		Console::notify("successful\n");
		return 1;
	}
	bool removal_success = fs::remove_all(this->config->paths.servicePath());
	if (removal_success)
	{
		Console::notify("existing service folder deleted \ncreating a new one...\n");
		return this->instantiate_service();
	}
	else
		return 0;
};
int SyncService::start_tcp_server() {
	Console::notify("starting tcp server...\n");
	try {
		tcp_server_ = new tcp_server(tcp_io_context_, *this);
		this->tcp_thread_ = std::make_unique<std::thread>([this] {
			tcp_io_context_.run();
			});
		Console::notify("tcp server started\n");
		return 1;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		Console::notify("something went wrong while starting tcp server...\n");
	}
	return 0;
};
