#include "torque.h"

void ts__onDatablockLimitExceeded(SimObject* obj, int argc, const char* argv[]) {
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

void ts__onDatablocksDeleted(SimObject* obj, int argc, const char* argv[]) {
	SetGlobalVariable("datablockExceededCount", "0");
}

void ts__initCommon(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "setRandomSeed"), NULL, 0);
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/client/canvas.cs");
	ts__fastCall(exec, NULL, 1, "base/client/audio.cs");
}

void ts__initBaseClient(SimObject* obj, int argc, const char* argv[]) {
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/client/message.cs");
	ts__fastCall(exec, NULL, 1, "base/client/mission.cs");
    ts__fastCall(exec, NULL, 1, "base/client/missionDownload.cs");
	ts__fastCall(exec, NULL, 1, "base/client/actionMap.cs");
}

void ts__initBaseServer(SimObject* obj, int argc, const char* argv[]) {
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/server/mainServer.cs");
}