#include "sync_service.h"
#include <filesystem>
#include <ShlObj_core.h>
#include <iostream>
namespace fs = std::filesystem;

fs::path SyncService::default_path = fs::path();

SyncService::SyncService() {
	PWSTR roaming_path_pwstr;
	HRESULT res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &roaming_path_pwstr);
	fs::path roaming_path = fs::path(std::wstring(roaming_path_pwstr));
	default_path = roaming_path;
};
int SyncService::instantiate_service(fs::path path) {
	fs::path existing_path = find_existing_service(path);
	if (existing_path.empty())
	{
		fs::path service_relative_path = "sync_service";
		fs::path combined_paths = default_path / service_relative_path;
		bool dir_created = fs::create_directory(combined_paths);
		if (dir_created)
		{
			std::cout << "created succesfully! \n";
			this->default_path = combined_paths;
		}
		else
			std::cout << "something went wrong creating service directory \n";
		 // create directory for service at given path and if succesful, set service_path to given path
	}
	else
	{
		std::cout << "existing path non empty \n";
		int valid = this->checkServiceValidity(existing_path);
		if (valid)
		{
			std::cout << "existing service folder valid \n";
			this->service_path = existing_path;
		}

		//set service_path to given path if validity check works
		//currently checkServiceValidity is empty returns 1 each time, implement next commit
	}
	return 1;
};
int SyncService::instantiate_service(){
	this->instantiate_service(default_path);
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
int SyncService::checkServiceValidity(fs::path path) {
	//returns 0 if service files are valid, 1 otherwise, could maybe be used as a future update checker for the service???
	return 1;
}