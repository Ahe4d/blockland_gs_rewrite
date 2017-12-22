#include <stdlib.h>
#include <string>
#include <map>
#include <windows.h>
#include <iostream>
#include <cstdarg>
#include "torque.h"

int init() {
	if(!torque_init()) {
		return 0;
	}

	Printf("PRGF | Init");
	//This should be called whenever..
	ConsoleFunction(NULL, "onDatablocksDeleted", ts__onDatablocksDeleted, "implanted by rewrite", 1, 1);
	ConsoleFunction(NULL, "onDatablockLimitExceeded", ts__onDatablockLimitExceeded, "implanted by rewrite", 1, 1);
	ConsoleFunction(NULL, "initCommon", ts__initCommon, "implanted by rewrite", 1, 1);
	ConsoleFunction(NULL, "initBaseClient", ts__initBaseClient, "implanted by rewrite", 1, 1);
	ConsoleFunction(NULL, "initBaseServer", ts__initBaseServer, "implanted by rewrite", 1, 1);

	Printf("PRGF | Loaded");
	return 1;
}

int deinit() {
	Printf("PRGF | Detached");
	return 1;
}

int __stdcall DllMain(HINSTANCE hInstance, unsigned long reason, void* reserved){
	if(reason == DLL_PROCESS_ATTACH)
		return init();
	else if (reason == DLL_PROCESS_DETACH)
		return deinit();

	return true;
}