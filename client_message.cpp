#include "torque.h"
void ts__addMessageCallback(SimObject* obj, int argc, const char* argv[]) {
	int i = 0;
	char* afunc;
	char* eh;
	sprintf(eh, "MSGCB[%s,%d]", argv[1], i);
	afunc = GetGlobalVariable(eh);
	while(_stricmp(GetGlobalVariable(eh), "") == 1) {
		if(_stricmp(argv[2], afunc) == 0) {
			return;
		}
		i++;
		sprintf(eh, "MSGCB[%s,%d]", argv[1], i);
		afunc = GetGlobalVariable(eh);
	}
	SetGlobalVariable(eh, argv[2]);
}

void ts__clientCmdChatMessage(SimObject* obj, int argc, const char* argv[]) {
	const char* msg = (const char*)ts__fastCall(fastLookup("", "detag"), NULL, 1, argv[4]);
	ts__fastCall(fastLookup("", "onChatMessage"), NULL, 3, msg, argv[2], argv[3]);
}