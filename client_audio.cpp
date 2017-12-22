///AAAAAAAAAAAAAAAAAAAA
#include "torque.h"
void ts__openALInit(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "OpenALShutdownDriver"), NULL, 0);
	Printf("");
	const char* lol = GetGlobalVariable("pref::Audio::Driver");
	Printf("OpenAL Driver Init: %s");
}