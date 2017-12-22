///AAAAAAAAAAAAAAAAAAAA
#include "torque.h"
void ts__openALInit(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "OpenALShutdownDriver"), NULL, 0);
	Printf("");
	const char* lol = GetGlobalVariable("pref::Audio::Driver");
	Printf("OpenAL Driver Init: %s", lol);
	bool retVal = (bool)ts__fastCall(fastLookup("", "OpenALInitDriver"), NULL, 0);
	if(!retVal) {
		Printf("failed to init driver.");
		SetGlobalVariable("Audio::initFailed", "1");
	}
	else
	{
		Namespace::Entry* fc = fastLookup("", "alGetString");
		Printf("Vendor: %s", (const char*)ts__fastCall(fc, NULL, 1, "AL_VENDOR"));
		Printf("Version: %s", (const char*)ts__fastCall(fc, NULL, 1, "AL_VERSION"));
		Printf("Renderer: %s", (const char*)ts__fastCall(fc, NULL, 1, "AL_RENDERER"));
		Printf("Extensions: %s", (const char*)ts__fastCall(fc, NULL, 1, "AL_EXTENSIONS"));
		ts__fastCall(fastLookup("", "alxListenerf"), NULL, 2, "AL_GAIN_LINEAR", GetGlobalVariable("pref::Audio::masterVolume"));
		Namespace::Entry* alC = fastLookup("", "alxSetChannelVolume");
		for(int i = 1; i <= 8; i++) {
			char* blah;
			sprintf(blah, "pref::Audio::channelVolume%d", i);
			const char* lol = GetGlobalVariable(blah);
			if(_stricmp(lol, "") == 0) {
				lol = "0";
			}
			//Yes, I know. I can make this better by using a char* and sprintf.
			//I tried that. It crashes the game.
			//Don't lecture me on this <3
			switch(i) {
				case 1:
					ts__fastCall(alC, NULL, 2, "1", lol);
				case 2:
					ts__fastCall(alC, NULL, 2, "2", lol);
				case 3:
					ts__fastCall(alC, NULL, 2, "3", lol);
				case 4:
					ts__fastCall(alC, NULL, 2, "4", lol);
				case 5:
					ts__fastCall(alC, NULL, 2, "5", lol);
				case 6:
					ts__fastCall(alC, NULL, 2, "6", lol);
				case 7:
					ts__fastCall(alC, NULL, 2, "7", lol);
				case 8:
					ts__fastCall(alC, NULL, 2, "8", lol);
			}
		}
		Printf("");
	}
}

void ts__openALShutdown(SimObject* obj, int argc, const char* argv[]) {
	ts__fastCall(fastLookup("", "OpenALShutdownDriver"), NULL, 0);
}