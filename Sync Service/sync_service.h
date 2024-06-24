#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include <filesystem>
namespace fs = std::filesystem;
class SyncService {
public:
	static fs::path default_path;
	fs::path service_path;


	SyncService();


	int instantiate_service();
	int instantiate_service(fs::path path);
private:
	std::filesystem::path find_existing_service(fs::path path);
	int checkServiceValidity(fs::path path);
};
#endif // !SYNCSERVICE_H
