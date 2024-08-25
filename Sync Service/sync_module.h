#ifndef SYNCMODULE_H
#define SYNCMODULE_H
#include <filesystem>
#include "sync_info.h"
#include <iostream>
namespace fs = std::filesystem;
class SyncModule {
public:
	fs::path source;
	fs::path destination;
	std::string type;
	std::string direction;
	std::string name;


	SyncInfo info;



	SyncModule();
	SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction);
	SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, std::string frequency);
	SyncModule(std::string name, fs::path source, fs::path destination, std::string type, std::string direction, const SyncInfo& info);
	SyncModule(const SyncModule& module);


	fs::path get_source();
	fs::path get_destination();
	std::string get_name();
	std::wstring get_source_wstr();
	std::wstring get_destination_wstr();
	std::string get_type();
	std::string get_direction();

	void set_name(std::string name);
	void set_source(fs::path path);
	void set_destination(fs::path path);
	void set_type(std::string type);
	void set_direction(std::string direction);
	
	bool operator==(const SyncModule other);
private:

};
#endif