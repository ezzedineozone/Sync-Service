#include "sync_service.h"
#include <filesystem>
#include <ShlObj_core.h>
#include <iostream>
SyncService::SyncService() {

};
int SyncService::instantiate_service(std::filesystem::path path) {

};
int SyncService::instantiate_service() {
	PWSTR roaming_path_pwstr;
	HRESULT res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &roaming_path_pwstr);
	std::cout << roaming_path_pwstr;
};