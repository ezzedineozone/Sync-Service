#include "sync_service.h"
SyncService::SyncService() {
	this->config = new ServiceConfig();
	this->handler = new ServiceHandler(this->config, this->db, this->started);
	this->started = false;
};
SyncService::SyncService(fs::path path) {
	this->config = new ServiceConfig(path);
	this->handler = new ServiceHandler(this->config, this->db, this->started);
	this->started = false;
};
SyncService::~SyncService() {
	// Destructor implementation
	if (db) {
		sqlite3_close(db); // Close the database connection if it is open
		db = nullptr; // Set to nullptr to avoid dangling pointer issues
	}

	// Delete the handler if it was allocated dynamically
	if (handler) {
		delete handler;
		handler = nullptr; // Set to nullptr to avoid dangling pointer issues
	}

	// Delete the config if it was allocated dynamically
	if (config) {
		delete config;
		config = nullptr; // Set to nullptr to avoid dangling pointer issues
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
		std::cout << "service already started! \n";
		return 1;
	}
	if (!find_existing_service(this->config->paths.servicePath()))
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
		bool dir_created = fs::create_directory(this->config->paths.servicePath());
		if (dir_created)
		{
			std::cout << "created succesfully! \n";
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
		int valid = this->check_service_validity(this->config->paths.servicePath());
		if (valid)
		{
			std::cout << "existing service folder valid \n";
			std::cout << "loading config\n";
			int config_loaded = this->config->json_handler();
			if (!config_loaded)
				std::cout << "something went wrong loading config";
			int opened = sqlite3_open(reinterpret_cast<const char*>(this->config->paths.sqlite3Path().string().c_str()), &db);
			if (opened == SQLITE_OK)
			{
				std::cout << "connection to database opened\n";
				this->started = true;
			}

		}
		else
		{
			std::cout << "existing service folder invalid... \ndeleting and creating a new one ... \n";
			bool removal_success = fs::remove_all(this->config->paths.servicePath());
			if (removal_success)
			{
				std::cout << "existing service folder deleted \ncreating a new one...\n";
				instantiate_service();
			}
		}
		//set service_path to given path if validity check works
		//currently checkServiceValidity is empty returns 1 each time, implement next commit
	}
	this->started = true;
	return startup_routine();
};
int SyncService::startup_routine() {
	int modules_loaded = this->handler->load_sync_modules();
	return 1;
};
int SyncService::close_service() {
	if (this->started == false)
	{
		std::cout << "service not started\n";
		return 1;
	}
	int con_closed = sqlite3_close(this->db);
	if (con_closed != SQLITE_OK)
	{
		std::cout << "an unexpected error occured \n";
		return 0;
	}
	else
	{
		std::cout << "service stopped\n";
		this->started = false;
		return 1;
	}
};
int SyncService::create_files()
{
	std::cout << "begin file creation... \n";
	if (this->config->paths.servicePath() != fs::path())
	{
		std::cout << "service path is created... \n";
		std::cout << "begin creation of database... \n";
		fs::path sqlite3_path = this->config->paths.servicePath() / "sqlite3";
		bool sqlite3_dir_created = fs::create_directory(sqlite3_path);
		if (sqlite3_dir_created)
		{
			fs::path sqlite3_file_path = this->config->paths.sqlite3Path();
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
	int config_loaded = this->config->json_handler();
	if (!config_loaded)
		std::cout << "something went wrong loading config\n";
	return 1;
};
int SyncService::find_existing_service(fs::path path) {
	fs::directory_entry entry = fs::directory_entry(path);
	if (entry.exists())
		return 1;
	return 0;
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
		const char* sync_module_statement = "CREATE TABLE SYNCMODULE(name TEXT PRIMARY KEY, source VARCHAR(259) NOT NULL, destination VARCHAR(259) NOT NULL, type VARCHAR(15) NOT NULL, direction VARCHAR(15) NOT NULL);";
		char* sync_module_error_msg;
		int sync_module_table_created = sqlite3_exec(db, sync_module_statement, nullptr, nullptr, &sync_module_error_msg);
		if (sync_module_table_created == SQLITE_OK)
			std::cout << "Sync module table created succesfully \n";
		else
		{
			std::cout << "Sync module table creation error\n" << sync_module_error_msg;
			return 0;
		}
	};
	if (db == nullptr)
	{
		std::cout << "database connection not established, please run create_files() function";
		return 0;
	}
	else
	{
		//code below is for the SYNCMODULE table
		const char* sync_info_statement = "CREATE TABLE SYNCINFO(name TEXT PRIMARY KEY,last_sync_date_unix INT, frequency varchar(20), dirty INT,FOREIGN KEY(name) REFERENCES SYNCMODULE(name));";
		char* sync_info_error_msg;
		int sync_module_table_created = sqlite3_exec(db, sync_info_statement, nullptr, nullptr, &sync_info_error_msg);
		if (sync_module_table_created == SQLITE_OK)
			std::cout << "Sync info table created succesfully \n";
		else
		{
			std::cout << "Sync module table creation error\n" << sync_info_error_msg;
			return 0;
		}
	};
	return 1;
};
SyncService::SyncService(std::string path) : SyncService(fs::path(path)) {
};
ServiceConfig* SyncService::get_config() {
	return this->config;
};
ServiceHandler* SyncService::get_handler() {
	return this->handler;
};
int SyncService::print_config() {
	return this->config->print_config();
};
int SyncService::reset_service() {
	if (this->started)
		if (sqlite3_close(this->db))
			return 0;
		else
			this->started = false;
	if (!fs::directory_entry(this->config->paths.servicePath()).exists())
	{
		std::cout << "succesful\n";
		return 1;
	}
	bool removal_success = fs::remove_all(this->config->paths.servicePath());
	if (removal_success)
	{
		std::cout << "existing service folder deleted \ncreating a new one...\n";
		return this->instantiate_service();
	}
	else
		return 0;
};