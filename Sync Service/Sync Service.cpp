// Sync Service.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "sync_service.h"

int main()
{
	SyncService* obj = new SyncService();
	obj->instantiate_service();
};

