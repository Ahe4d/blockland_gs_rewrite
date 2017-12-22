#include "torque.h"

void ts__clientCmdMissionStartPhase1(SimObject* obj, int argc, const char* argv[]) { 
	Printf("*** New Mission (swag)");
	Printf("*** Phase 1: Download Datablocks & Targets");
	ts__fastCall(fastLookup("", "onMissionDownloadPhase1"), NULL, 0);
	ts__fastCall(fastLookup("", "commandToServer"), NULL, 2, StringTableEntry(atoi("MissionStartPhase1Ack" + 1)), argv[1]);
}
//Todo. Need to find the address of the StringHandle constructor (shouldn't be too hard.)
