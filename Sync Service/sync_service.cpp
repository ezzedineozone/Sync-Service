#include "sync_service.h"
SyncService::SyncService() {
	this->config = new ServiceConfig();
	this->handler = new ServiceHandler();
};
ServiceConfig* SyncService::get_config() {
	return this->config;
};
ServiceHandler* SyncService::get_handler() {
	return this->handler;
};