#include "torque.h"
void ts__clientCmdMissionEnd(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "alxStopAll"), NULL, 0);
	SetGlobalVariable("lightingMission", "0");
	SetGlobalVariable("sceneLighting::terminateLighting", "1");
}

void ts__clientCmdMissionStart(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "ClearPhysicsCache"), NULL, 0);
}