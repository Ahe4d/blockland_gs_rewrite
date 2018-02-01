#include <stdlib.h>
#include <string>
#include <map>
#include <windows.h>
#include <iostream>
#include <cstdarg>
#include "torque.h"
#include <hayai.hpp> 
#include "detours.h"
#include "mathfu/vector.h"
#include "mathfu/glsl_mappings.h"

using namespace MologieDetours;

Detour<Player__processTickFn> *Player__processTick_Detour;
//BENCHMARK(Benchmarks, CachedCall, 500, 10) {
//	Benchmarks("", "mPow").CachedCall();
//}

//BENCHMARK(Benchmarks, UnCachedCall, 500, 10) {
//	Benchmarks("", "mPow").UnCachedCall();
//}

typedef char *(__thiscall *StringStack__getArgBuffer_Type)(void *this_, unsigned int arg);
StringStack__getArgBuffer_Type StringStack__getArgBuffer = (StringStack__getArgBuffer_Type)0x004A0D10;

typedef const char *(*Con__execute_Type)(int argc, const char *argv[]);
Con__execute_Type Con__execute = (Con__execute_Type)0x004A7870;

typedef void (*Con__execute_o_Type)(void *object, int argc, const char *argv[]);
Con__execute_o_Type Con__execute_o = (Con__execute_o_Type)0x004A8B40;

void *STR = (void *)0x0070CDC8;

static void ts__runBench(SimObject* obj, int argc, const char* argv[]) {
}


int init() {
	if(!torque_init()) {
		return 0;
	}
		Printf("PRGF | Init");
	//This should be called whenever..
	//base/main.cs.dso

		ConsoleFunction(NULL, "onDatablocksDeleted", ts__onDatablocksDeleted, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "onDatablockLimitExceeded", ts__onDatablockLimitExceeded, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "initCommon", ts__initCommon, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "initBaseClient", ts__initBaseClient, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "initBaseServer", ts__initBaseServer, "implanted by rewrite", 1, 1);
		//base/client/audio.cs.dso
	
		ConsoleFunction(NULL, "OpenALInit", ts__openALInit, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "OpenALShutdown", ts__openALShutdown, "implanted by rewrite", 1, 1);
		//base/client/canvas.cs.dso
		ConsoleFunction(NULL, "initCanvas", ts__initCanvas, "implanted by rewrite", 2, 2);
		ConsoleFunction(NULL, "onWindowReactivate", ts__onWindowReactivate, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "resetCanvas", ts__resetCanvas, "implanted by rewrite", 1, 1);
		ConsoleFunction(NULL, "restartAudio", ts__restartAudio, "implanted by rewrite", 1, 1);
		//base/client/mission.cs.dso
		ConsoleFunction(NULL, "clientCmdMissionStart", ts__clientCmdMissionStart, "implanted by rewrite", 2, 2);
		ConsoleFunction(NULL, "clientCmdMissionEnd", ts__clientCmdMissionEnd, "implanted by rewrite", 2, 2);
		//base/client/message.cs.dso
		//For some stupid ass reason, addMessageCallback...initializes the video? Wtf?
		//ConsoleFunction(NULL, "addMessageCallback", ts__addMessageCallback, "implanted by rewrite", 3, 3);
		ConsoleFunction(NULL, "clientCmdChatMessage", ts__clientCmdChatMessage, "implanted by rewrite", 5, 15);
		//base/client/init.cs.dso
		ConsoleFunction(NULL, "initClient", ts__initClient, "implanted by rewrite", 1, 1);
		//base/client/missionDownload.cs.dso
		ConsoleFunction(NULL, "clientCmdMissionStartPhase1", ts__clientCmdMissionStartPhase1, "implanted by rewrite", 0, 2);
		ConsoleFunction(NULL, "clientCmdMissionStartPhase2", ts__clientCmdMissionStartPhase2, "implanted by rewrite", 0, 2);
		ConsoleFunction(NULL, "clientCmdMissionStartPhase3", ts__clientCmdMissionStartPhase3, "implanted by rewrite", 0, 5);

		Printf("PRGF | Loaded");
	return 1;
}

int deinit() {
	Printf("PRGF | Detached");

	deallocAll(); //Call this so we don't leak memory.
	return 1;
}

int __stdcall DllMain(HINSTANCE hInstance, unsigned long reason, void* reserved){
	if(reason == DLL_PROCESS_ATTACH)
		return init();
	else if (reason == DLL_PROCESS_DETACH)
		return deinit();

	return true;
}
