#include <stdlib.h>
#include <string>
#include <map>
#include <windows.h>
#include <iostream>
#include <cstdarg>
#include "torque.h"

//Our two caching mechanisms..
static std::map<const char*, Namespace::Entry*> cache;
static std::map<const char*, Namespace*> nscache;
static Namespace* GlobalNS;

Namespace::Entry* fastLookup(const char* ourNamespace, const char* name);

void* ts__fastCall(Namespace::Entry* ourCall, SimObject** obj = NULL, int argc = 0, ...) {
	const char* argv[argc + 1];
	va_list args;
	va_start(args, argc);
	argv[0] = ourCall->mFunctionName;
	for(int i = 0; i < argc; i++) {
		argv[i + 1] = va_arg(args, const char*);
	}
	argc++;
	if(ourCall->mType == Namespace::Entry::ScriptFunctionType) {
		if(ourCall->mFunctionOffset) {
			const char* retVal = CodeBlock__exec(
			ourCall->mCode, ourCall->mFunctionOffset,
			ourCall->mNamespace, ourCall->mFunctionName, 
			argc, argv, false, ourCall->mNamespace->mPackage,
			0);
			return (void*)retVal; //we know what it's supposed to return.
		}
		else {
			return nullptr;
		}
	}
	S32 mMinArgs = ourCall->mMinArgs, mMaxArgs = ourCall->mMaxArgs;
	if ((mMinArgs && argc < mMinArgs) || (mMaxArgs && argc > mMaxArgs)) {
		Printf("Expected args between %d and %d, got %d", mMinArgs, mMaxArgs, argc);
		return nullptr;
	}
	SimObject* actualObj;
	if(obj == NULL) {
		actualObj = NULL;
	}
	else
	{
		actualObj = *obj;
	}
	switch(ourCall->mType) {
		case Namespace::Entry::StringCallbackType:
			return (void*)ourCall->cb.mStringCallbackFunc(actualObj, argc, argv);
		case Namespace::Entry::IntCallbackType:
			return (void*)ourCall->cb.mIntCallbackFunc(actualObj, argc, argv);
		case Namespace::Entry::FloatCallbackType: {
			//Wtf?
			float ourret[] = {ourCall->cb.mFloatCallbackFunc(actualObj, argc, argv)};
			return (void*)ourret;
		}
		case Namespace::Entry::BoolCallbackType:
			return (void*)ourCall->cb.mBoolCallbackFunc(actualObj, argc, argv);		
		case Namespace::Entry::VoidCallbackType:
			ourCall->cb.mVoidCallbackFunc(actualObj, argc, argv);
			return nullptr;
	} 

	return nullptr; 
}

void rewrite__fatal() {
	Printf("!!! THIS SHOULD NEVER HAPPEN !!!");
}

Namespace::Entry* fastLookup(const char* ourNamespace, const char* name) {
	Namespace* ns;
	Namespace::Entry* entry;

	if(_stricmp(ourNamespace, "") == 0) {
		if(GlobalNS == NULL) {
			rewrite__fatal();
			GlobalNS = LookupNamespace(NULL);
		}
		ns = GlobalNS;
	}
	else {
		std::map<const char*, Namespace*>::iterator its;
		its = nscache.find(ourNamespace);
		if(its != nscache.end()) {
			ns = its->second;
			if(ns == NULL) {
				//Somehow it got nullptr'd..
				ns = LookupNamespace(StringTableEntry(ourNamespace)); 
				if(ns == NULL) {
					//THIS SHOULD NEVER HAPPEN!
					rewrite__fatal();
					Printf("Fatal: Found cached NS entry with nullptr, could not find namespace!");
					nscache.erase(its);
					return nullptr;
				}
				nscache.erase(its);
				nscache.insert(nscache.end(), std::make_pair(ourNamespace, ns));
			}
		}
		else {
			ns = LookupNamespace(ourNamespace);
			if(ns == NULL) {
				rewrite__fatal();
				Printf("Fatal: fastLookup FAILED (2/2)!");
				return nullptr;
			}
			nscache.insert(nscache.end(), std::make_pair(ourNamespace, ns));
		}
	}

	std::map<const char*, Namespace::Entry*>::iterator it;
	it = cache.find(name); //Look into our Namespace::Entry* cache..
	if(it != cache.end()) {
		entry = it->second;
		if(entry == NULL) {
			rewrite__fatal();
			Printf("Fatal: found nullptr in cache!");
			cache.erase(it); //Erase it so we don't encounter it again.
			return nullptr;
		}
	}
	else {
		entry = Namespace__lookup(ns, StringTableEntry(name));
		if(entry == NULL) {
			Printf("Could not find function.");
			return nullptr;
		}
		cache.insert(cache.end(), std::make_pair(name, entry)); //Insert it so further calls are optimized.
	}

	return entry;
}

static void ts__onDatablockLimitExceeded(SimObject* obj, int argc, const char* argv[]) {
	const char* ourInitialVal = GetGlobalVariable("datablockExceededCount");
	if(_stricmp(ourInitialVal, "") == 0) {
		SetGlobalVariable("datablockExceededCount", "1");
	}
	else {
		char* out;
		sprintf(out, "%d", atoi(ourInitialVal) + 1);
		SetGlobalVariable("datablockExceededCount", const_cast<char*>(out));
	}
}

static void ts__onDatablocksDeleted(SimObject* obj, int argc, const char* argv[]) {
	SetGlobalVariable("datablockExceededCount", "0");
}

static void ts__initCommon(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "setRandomSeed"), NULL, 0);
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/client/canvas.cs");
	ts__fastCall(exec, NULL, 1, "base/client/audio.cs");
}

static void ts__initBaseClient(SimObject* obj, int argc, const char* argv[]) {
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/client/message.cs");
	ts__fastCall(exec, NULL, 1, "base/client/mission.cs");
    ts__fastCall(exec, NULL, 1, "base/client/missionDownload.cs");
	ts__fastCall(exec, NULL, 1, "base/client/actionMap.cs");
}

static void ts__initBaseServer(SimObject* obj, int argc, const char* argv[]) {
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/server/mainServer.cs");
}

int init() {
	if(!torque_init()) {
		return 0;
	}

	Printf("PRGF | Init");
	//This should be called whenever..
	GlobalNS = LookupNamespace(NULL); //look it up here..
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