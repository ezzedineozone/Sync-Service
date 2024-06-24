#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include <filesystem>
class SyncService {
public:
	SyncService();
	int instantiate_service();
	int instantiate_service(std::filesystem::path path);
};
#endif // !SYNCSERVICE_H
