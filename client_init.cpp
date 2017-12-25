#include "torque.h"
#include <algorithm>
void ts__initClient(SimObject* obj, int argc, const char* argv[]) { 
	//Just because it feels the same
	//Til the morning comes
	//Even roads feel the sun, so cold..
	//Just because we're not alone
	//You echo words you've heard
	//You let me go, just to watch me discern..
	Printf("\n--------- Initializing Base: Client ---------");
	if(_stricmp(GetGlobalVariable("pref::Video::Resolution"), "") == 0) {
		if(atoi(GetGlobalVariable("pref::Video::fullScreen"))) {
			const char* res = (const char*)ts__fastCall(fastLookup("", "getDesktopResolution"), NULL, 0);
			if(_stricmp(res, "") == 0) {
				SetGlobalVariable("pref::Video::resolution", "800 600 32");
			}
			else {
				SetGlobalVariable("pref::Video::resolution", res);
			}
		}
		else {
			char* aaaa;
			char* res = (char*)ts__fastCall(fastLookup("", "getDesktopResolution"), NULL, 0);
			char* desktopW = strtok(res, " ");
			char* desktopH = strtok(NULL, " ");
			char* desktopBpp = strtok(NULL, " ");
			int fudge = 30;
			if(atoi(desktopW) > 1680 + fudge && atoi(desktopH) > 1050 + fudge) {
				sprintf(aaaa, "1680 1050 %s", desktopBpp);
				SetGlobalVariable("pref::Video::Resolution", aaaa);
			}
			else {
				if(atoi(desktopW) > 1280 + fudge && atoi(desktopH) > 720 + fudge) {
					sprintf(aaaa, "1280 720 %s", desktopBpp);
					SetGlobalVariable("pref::Video::Resolution", aaaa);
				}
				else {
					sprintf(aaaa, "800 600 %s", desktopBpp);
					SetGlobalVariable("pref::Video::Resolution", aaaa);
				}
			}
		}
	}

	SetGlobalVariable("Server::Dedicated", "0");
	SetGlobalVariable("Client::GameTypeQuery", "Blockland");
	SetGlobalVariable("Client::MissionTypeQuery", "Any");
	ts__initBaseClient(NULL, 0, {});
	const char* args[] = {"initCanvas", "Blockland"};
	ts__initCanvas(NULL, 2, args);
	Namespace::Entry* exec = fastLookup("", "exec");
	ts__fastCall(exec, NULL, 1, "base/client/scripts/allClientScripts.cs");
	ts__fastCall(exec, NULL, 1, "base/client/ui/allClientGuis.gui");
	bool isFile = (bool)ts__fastCall(fastLookup("", "isFile"), NULL, 1, "config/client/config.cs");
	if(isFile) {
		ts__fastCall(exec, NULL, 1, "config/client/config.cs");
	}
	SimObject* jsg = Sim__findObject_name("JoinServerGui");
	if(jsg != NULL) {
		SimObject__setDataField(jsg, "lastQueryTime", StringTableEntry(""), StringTableEntry("0"));
	}
	Printf("\n--------- Loading Client Add-Ons ---------");
	//This shit slows down my fucking startup.
	//ts__fastCall(fastLookup("", "loadClientAddons"), NULL, 0);
	int* pac = (int*)ts__fastCall(fastLookup("", "getNumActivePackages"), NULL, 0);
	char fuckgod[9];
	sprintf(fuckgod, "%d", pac);
	SetGlobalVariable("numClientPackages", fuckgod);
	ts__fastCall(fastLookup("", "setNetPort"), NULL, 1, "28000");
	SimObject* options = Sim__findObject_name("optionsDlg");
	if(options != NULL) {
		ts__fastCall(fastLookup(options->mNameSpace->mName, "setShaderQuality"), options, 1, GetGlobalVariable("Pref::ShaderQuality"));
	}
	ts__fastCall(fastLookup("", "setDefaultFov"), NULL, 1, GetGlobalVariable("pref::Player::defaultFov"));
	ts__fastCall(fastLookup("", "setZoomSpeed"), NULL, 1, GetGlobalVariable("pref::Player::zoomSpeed"));
	//Printf("Loading main menu");
	ts__fastCall(fastLookup("", "loadMainMenu"), NULL, 0);
	//Printf("Bringing window to foreground");
	//ts__fastCall(fastLookup("", "BringWindowToForeground"), NULL, 0);
	//ts__fastCall(fastLookup("", "schedule"), NULL, 3, "1000", "0", "BringWindowToForeground");
	//Printf("Loading trust list");
	ts__fastCall(fastLookup("", "loadTrustList"), NULL, 0);
	//Printf("updating temp brick settings");
	ts__fastCall(fastLookup("", "updateTempBrickSettings"), NULL, 0);
}

//Brace yourselves for the grande finale!

void ts__loadMainMenu(SimObject* obj, int argc, const char* argv[]) {
	SimObject* canvas = Sim__findObject_name("Canvas");
	if(canvas != NULL) {
		ts__fastCall(fastLookup(canvas->mNameSpace->mName, "setContent"), canvas, 1, "MainMenuGui");
		ts__fastCall(fastLookup(canvas->mNameSpace->mName, "setCursor"), canvas, 1, "DefaultCursor");
	}
}