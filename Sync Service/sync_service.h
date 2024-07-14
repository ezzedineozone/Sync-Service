#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H
#include "sync_service_handler.h"
#include "sync_service_config.h"
class SyncService {
public:
	SyncService();
	ServiceHandler* get_handler();
	ServiceConfig* get_config();
private:
	ServiceHandler* handler;
	ServiceConfig* config;
};
#endif