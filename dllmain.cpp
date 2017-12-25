#include <stdlib.h>
#include <string>
#include <map>
#include <windows.h>
#include <iostream>
#include <cstdarg>
#include "torque.h"
#include <hayai.hpp> 
#include "detours.h"

using namespace MologieDetours;

Detour<Player__processTickFn> *Player__processTick_Detour;

class Benchmarks 
{
public:
	void CachedCall() {
		Namespace::Entry* us = fastLookup(ns, fn);
		ts__fastCall(us, NULL, 2, "55", "123");
	} 

	void UnCachedCall() { 
		Namespace* ourNs;
		if(_stricmp(ns, "") == 0) { //fastLookup also does this internally.
			ourNs = LookupNamespace(NULL);
		}
		else {
			ourNs = LookupNamespace(ns);
			if(ourNs == NULL) {
				Printf("Benchmarks::UnCachedCall()- found invalid namespace");
				return;
			}
		}
		Namespace::Entry* ourEntry = Namespace__lookup(ourNs, StringTableEntry(fn));
		if(ourEntry == NULL) {
			Printf("Benchmarks::UnCachedCall()- found invalid Namespace::Entry...");
			return;
		}
		ts__fastCall(ourEntry, NULL, 2, "55", "123");
	}

	Benchmarks(const char* namefuck, const char* fnName) 
	{
		ns = namefuck;
		fn = fnName;
	}
private:
	const char* ns;
	const char* fn;
};

BENCHMARK(Benchmarks, CachedCall, 500, 10) {
	Benchmarks("", "mPow").CachedCall();
}

BENCHMARK(Benchmarks, UnCachedCall, 500, 10) {
	Benchmarks("", "mPow").UnCachedCall();
}

static void ts__runBench(SimObject* obj, int argc, const char* argv[]) {
	hayai::ConsoleOutputter consoleOutputter;

    hayai::Benchmarker::AddOutputter(consoleOutputter); 
	hayai::Benchmarker::RunAllTests();
}

void __fastcall Player__processTick_Hook(SimObject *this_, int edx, Move *move)
{
	//Pull the trigger, yeah
	//Bullseye.
	const char* var = SimObject__getDataField(this_, "enable", StringTableEntry(""));
	if(_stricmp(var, "") != 0) {
		int yeah = atoi(var);
		if(yeah) {
			float blahx, blahy;
			char id1[9];
			//char id2[9];
			const char* target = SimObject__getDataField(this_, "target", StringTableEntry(""));
			sprintf(id1, "%d", this_->id);
			const char* args[] = {"ts__calcAim", id1, target};
			ts__calcAim(NULL, 3, args);
			const char* eek = SimObject__getDataField(this_, "stuff", StringTableEntry(""));
			sscanf(eek, "%g %g", &blahx, &blahy);
			Printf("Client: %g %g, expected: %g %g", move->yaw, move->pitch, blahx, blahy);
			float diffx, diffy;
			if(move->yaw == blahx) {
				Printf("YAW MATCHED SERVER AIMBOT! %g", move->yaw);
			}
			else if(move->pitch == blahy) {
				Printf("PITCH MATCHED SERVER AIMBOT! %g", move->pitch);
			}
			if(blahx > move->yaw && blahy > move->pitch) {
				diffx = blahx - move->yaw;
				diffy = blahy - move->pitch;
					//Printf("Pitch and yaw were off by %g and %g", blahx - move->yaw, blahy - move->pitch);
			}
			else if(blahx > move->yaw) {
				diffx = blahx - move->yaw;
				diffy = move->pitch - blahy;
					//Printf("Pitch and yaw were off by %g and %g", blahx - move->yaw, move->pitch - blahy);
			}
			else if(blahy > move->pitch) {
				diffx = move->yaw - blahx;
				diffy = blahy - move->pitch;
				//Printf("Pitch and yaw were off by %g and %g", move->yaw - blahx, blahy - move->pitch);
			}
			else {
				diffx = move->yaw - blahx;
				diffy = move->pitch - blahy;
				//Printf("Pitch and yaw were off by %g and %g", move->yaw - blahx, move->pitch - blahy);
			}
			Printf("Pitch and yaw were off by %g and %g", diffx, diffy);
			if(diffx < 0.005f && diffy < 0.005f && diffy > 0 && diffx > 0) {
				Printf("Found a suspicious move..");
			}
			//SimObject__setDataField(this_, "enable", StringTableEntry(""), StringTableEntry("0"));		
			//move->yaw = blahx;
			//move->pitch = blahy;
			ClampMove(move);
		}
	}
	return Player__processTick_Detour->GetOriginalFunction()(this_, edx, move);
}

int init() {
	if(!torque_init()) {
		return 0;
	}

	Printf("PRGF | Init");
	//This should be called whenever..
	//base/main.cs.dso
	Player__processTick_Detour = new Detour<Player__processTickFn>(Player__processTick, Player__processTick_Hook);
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
//	ConsoleFunction(NULL, "addMessageCallback", ts__addMessageCallback, "implanted by rewrite", 3, 3);
	ConsoleFunction(NULL, "clientCmdChatMessage", ts__clientCmdChatMessage, "implanted by rewrite", 5, 15);
	//base/client/init.cs.dso
	ConsoleFunction(NULL, "initClient", ts__initClient, "implanted by rewrite", 1, 1);
	//base/client/missionDownload.cs.dso
	ConsoleFunction(NULL, "clientCmdMissionStartPhase1", ts__clientCmdMissionStartPhase1, "implanted by rewrite", 0, 2);
	ConsoleFunction(NULL, "clientCmdMissionStartPhase2", ts__clientCmdMissionStartPhase2, "implanted by rewrite", 0, 2);
	ConsoleFunction(NULL, "clientCmdMissionStartPhase3", ts__clientCmdMissionStartPhase3, "implanted by rewrite", 0, 5);
//Benchmarking
	ConsoleFunction(NULL, "dll_runBench", ts__runBench, "implanted by rewrite", 1, 1);
//Anticheat
	ConsoleFunction(NULL, "calculateAim", ts__calcAim, "implanted by rewrite", 3, 3);
	Printf("PRGF | Loaded");
	return 1;
}

int deinit() {
	delete Player__processTick_Detour;
	deallocAll(); //Call this so we don't leak memory.
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