#ifndef STOREDPATHS_H
#define STOREDPATHS_H
#include <filesystem>
#include <shlobj_core.h>	
namespace fs = std::filesystem;
class StoredPaths {
private:
	static fs::path default_path;
	fs::path service_path;
	fs::path sqlite3_path;
public:
	StoredPaths() {
		PWSTR roaming_path_pwstr;
		HRESULT res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &roaming_path_pwstr);
		fs::path roaming_path = fs::path(std::wstring(roaming_path_pwstr));
		this->default_path = roaming_path;
		this->service_path = fs::path();
		this->sqlite3_path = fs::path();
	};
	fs::path getDefaultPath() { return default_path; };
	fs::path& getServicePath() { return service_path; };
	fs::path& getSqlite3Path() { return sqlite3_path; };
};
#endif