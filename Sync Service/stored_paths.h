#ifndef STOREDPATHS_H
#define STOREDPATHS_H
#include <filesystem>
#include <shlobj_core.h>	
namespace fs = std::filesystem;
class StoredPaths {
private:
	fs::path service_path;
	fs::path config_path;
	fs::path sqlite3_path;
public:
	StoredPaths() {
		PWSTR roaming_path_pwstr;
		HRESULT res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &roaming_path_pwstr);
		fs::path roaming_path = fs::path(std::wstring(roaming_path_pwstr));
		this->service_path = roaming_path / "sync_service";
		this->sqlite3_path = this->servicePath() / "sqlite3" / "database.db";
	};
	StoredPaths(fs::path path)
	{
		this->service_path = path / "sync_service";
		this->sqlite3_path = this->servicePath() / "sqlite3" / "database.db";
		this->config_path = this->servicePath() / "config.json";
	};
	fs::path& servicePath() { return service_path; };
	fs::path& sqlite3Path() { return sqlite3_path; };
	fs::path& configPath() { return config_path; };
};
#endif