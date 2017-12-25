#include "torque.h"

void ts__clientCmdMissionStartPhase1(SimObject* obj, int argc, const char* argv[]) { 
	Printf("*** New Mission (swag)");
	Printf("*** Phase 1: Download Datablocks & Targets");
	ts__fastCall(fastLookup("", "onMissionDownloadPhase1"), NULL, 0);
	char aa[9];
	giveUsATagMofo(aa, "MissionStartPhase1Ack");
	Printf("tag1: %s", aa);
	ts__fastCall(fastLookup("", "commandToServer"), NULL, 2, aa, StringTableEntry(argv[1])); //be sure to free this memory!
}
//Done

void ts__clientCmdMissionStartPhase2(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "onPhase1Complete"), NULL, 0);
	Printf("*** Phase 2: Download Ghost Objects");
	ts__fastCall(fastLookup("", "purgeResources"), NULL, 0);
	ts__fastCall(fastLookup("", "onMissionDownloadPhase2"), NULL, 0);
	char tag[9];
	giveUsATagMofo(tag, "MissionStartPhase2Ack");
	Printf("tag2: %s, argv[1]: %s", tag, argv[1]);
	ts__fastCall(fastLookup("", "commandToServer"), NULL, 2, tag, StringTableEntry(argv[1]));
}

void ts__clientCmdMissionStartPhase3(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "onPhase2Complete"), NULL, 0);
	Printf("*** Phase 3: Mission Lighting");
	SetGlobalVariable("MSeq", argv[1]);
	ts__fastCall(fastLookup("", "sceneLightingComplete"), NULL, 0);
}
