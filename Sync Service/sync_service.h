#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include <filesystem>
#include "storedPaths.h"
#include "sqlite3/sqlite3.h"
namespace fs = std::filesystem;
class SyncService {
public:
	StoredPaths paths;
	sqlite3* db;



	SyncService();


	int instantiate_service();
	int instantiate_service(fs::path path);
	int create_files();
private:
	fs::path find_existing_service(fs::path path);
	int check_service_validity(fs::path path);
};
#endif // !SYNCSERVICE_H
