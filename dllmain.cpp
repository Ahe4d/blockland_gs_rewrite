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

class Benchmarks 
{
public:
	void VectorAdd() {
		//start off with two vectors in a string.
		mathfu::vec3 vectorA(50.15f, 1585.5f, 185.6f);
		mathfu::vec3 vectorB(1.05f, 1.9556f, 1.5859f);
		mathfu::vec3 vectorC = vectorA + vectorB;
	}

	void VectorScale() {
		mathfu::vec3 vectorA(50.15f, 1585.5f, 185.6f);
		mathfu::vec3 scala(1.583f, 1.583f, 1.583f); //i'll make a vectorscale thing later lol
		vectorA *= scala;
	}

	void VectorDot() {
		mathfu::vec3 vectorA(50.15f, 1585.5f, 185.6f);
		mathfu::vec3 vectorB(1.05f, 1.9556f, 1.5859f);
		F32 dot = mathfu::vec3::DotProduct(vectorA, vectorB);
	}

/*
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
*/
	Benchmarks() 
	{
		//ns = namefuck;
		//fn = fnName;
	}
private:
	//const char* ns;
	//const char* fn;
};

BENCHMARK(Benchmarks, VectorAdd, 10, 10000) {
	Benchmarks().VectorAdd();
} 
BENCHMARK(Benchmarks, VectorScale, 10, 10000) {
	Benchmarks().VectorScale();
} 
BENCHMARK(Benchmarks, VectorDot, 10, 10000) {
	Benchmarks().VectorDot();
} 

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
	hayai::ConsoleOutputter consoleOutputter;

    hayai::Benchmarker::AddOutputter(consoleOutputter); 
	hayai::Benchmarker::RunAllTests();
}

void __fastcall Player__processTick_Hook(SimObject *this_, int edx, Move *move)
{
	const char* var = SimObject__getDataField(this_, "BLACDisable", StringTableEntry(""));
	if(this_ == nullptr) {
		return;
	}
	if(_stricmp(var, "") != 0 && !(this_->mFlags & 2)) {
		int yeah = atoi(var);
		if(!yeah) {
			const char* gross = SimObject__getDataField(this_, "aimLock", StringTableEntry(""));
			int enable = 0;
			if(_stricmp(gross, "") != 0) {
				enable = atoi(gross);
			}
			int spinme = 0;
			//Printf("calculating aim");
			WrappedPosData out = {false, mathfu::vec2(0.0f, 0.0f)};
			if(this_ != nullptr && move != nullptr) {
				out = ts__calcAim(this_->id);
			}
			else {
				return;
			}
			//Printf("done calculating");
			if(out.success) {
				//Printf("run all this stuff");
				mathfu::vec2 aimData = out.data;
				mathfu::vec2 diff(0.0f, 0.0f);
				mathfu::vec2 moveStuff(move->yaw, move->pitch);
				if(aimData[0] > moveStuff[0] && aimData[1] > moveStuff[1]) {
					diff = aimData - moveStuff;
				}
				else if(aimData[0] > moveStuff[0]) {
					diff = mathfu::vec2(aimData[0] - moveStuff[0], moveStuff[1] - aimData[1]);
				}
				else if(aimData[1] > moveStuff[1]) {
					diff = mathfu::vec2(moveStuff[0] - aimData[0], aimData[1] - moveStuff[1]);
				}
				else {
					diff = moveStuff - aimData;
				}
				//Printf("writing callback");
				if(diff[0] < 0.0005f && diff[1] < 0.0005f && diff[0] > 0 && diff[1] > 0 && enable == 0 && diff[0] != aimData[0] && diff[1] != aimData[1]) {
					if(moveStuff[0] > 0 || moveStuff[1] > 0) {
						const char *argv[3];
						argv[0] = "onPlayerSuspMove";
						if(this_ != nullptr) {
							argv[1] = Con__getIntArg(this_->id);
						}
						else {
							return;
						}
						char *writtenOut = StringStack__getArgBuffer(STR, 300);
						sprintf_s(writtenOut, 300, "%g %g %g %g %g %g", moveStuff[0], moveStuff[1], aimData[0], aimData[1], diff[0], diff[1]);
						argv[2] = writtenOut;
						Con__execute(3, argv);
					}
				}

				if(spinme) {
					float f1, f2, deltaView;
					float oldAn = aimData[1];
					if(oldAn < 0.0f)
						f1 = 360.0f + oldAn;
					else
						f1 = oldAn;

					if(aimData[0] < 0.0f)
						f2 = 360.0f + aimData[0];
					else
						f2 = aimData[0];

					if(f2 < f1)
						deltaView = abs(f2 - f1);
					else
						deltaView = 360.0f - abs(f1 - f2);

					deltaView = 360.0f - deltaView;
					//Printf("%d", cos(degreesToRadians(deltaView)) * move->x + cos(degreesToRadians(deltaView + 90.0f)));

					float fMove = cos(degreesToRadians(deltaView)) * move->x + cos(degreesToRadians(deltaView + 90.0f)) * move->y;
					float sMove = sin(degreesToRadians(deltaView)) * move->x + sin(degreesToRadians(deltaView + 90.0f)) * move->y;
					Printf("%f %f %f", deltaView, fMove, sMove);
					move->x = fMove;
					move->y = sMove;
					move->yaw = aimData[0];
				}
				else if(enable) {
					move->yaw = aimData[0];
					move->pitch = aimData[1];
				}

				ClampMove(move);
			}
		}
	}
	return Player__processTick_Detour->GetOriginalFunction()(this_, edx, move);
}

static int distribute = 1;

int init() {
	if(!torque_init()) {
		return 0;
	}

	if(distribute == 0) {
		Printf("PRGF | Init");
	}
	else
	{
		Printf("BLAC | Init");
	}
	//This should be called whenever..
	//base/main.cs.dso

	Player__processTick_Detour = new Detour<Player__processTickFn>(Player__processTick, Player__processTick_Hook);
	if(distribute == 0) {
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
	}
	//Benchmarking
	ConsoleFunction(NULL, "dll_runBench", ts__runBench, "() - Run a benchmark on the float performance.", 1, 1);
	//Anticheat
	//ConsoleFunction(NULL, "calculateAim", ts__calcAim, "(Player us) - calculate stuff..", 2, 2);
	if(distribute == 0) {
		Printf("PRGF | Loaded");
	}
	else
	{
		Printf("BLAC | Loaded");
	}
	return 1;
}

int deinit() {
	if(distribute == 0) {
		Printf("PRGF | Detached");
	}
	else
	{
		Printf("BLAC | Detached");
	}
	delete Player__processTick_Detour;
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