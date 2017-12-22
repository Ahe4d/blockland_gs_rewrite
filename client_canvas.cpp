//base/client/canvas.cs
#include "torque.h"
void ts__initCanvas(SimObject* obj, int argc, const char* argv[]) { 
	const char* vga = GetGlobalVariable("pref::OpenGL::gammaCorrection");
	Printf("%s", vga);
	ts__fastCall(fastLookup("", "videoSetGammaCorrection"), NULL, 1, vga);
	bool ret = (bool)ts__fastCall(fastLookup("", "createCanvas"), NULL, 1, StringTableEntry(argv[1]));
	if(!ret) {
		ts__fastCall(fastLookup("", "quit"), NULL, 0);
	}
	//	setOpenGLTextureCompressionHint($pref::OpenGL::compressionHint);
	//  setOpenGLAnisotropy($pref::OpenGL::anisotropy);
	//  setOpenGLMipReduction($pref::OpenGL::mipReduction);
	//  setOpenGLSkyMipReduction($pref::OpenGL::skyMipReduction);
	//  OpenALInit();
	ts__fastCall(fastLookup("", "setOpenGLTextureCompressionHint"), NULL, 1, GetGlobalVariable("pref::OpenGL::compressionHint"));
	ts__fastCall(fastLookup("", "setOpenGLAnisotropy"), NULL, 1, GetGlobalVariable("pref::OpenGL::anisotropy"));
	ts__fastCall(fastLookup("", "setOpenGLMipReduction"), NULL, 1, GetGlobalVariable("pref::OpenGL::mipReduction"));
	ts__fastCall(fastLookup("", "setOpenGLSkyMipReduction"), NULL, 1, GetGlobalVariable("pref::OpenGL::skyMipReduction"));
	ts__openALInit(NULL, 0, {}); //Fuck yeah.
}

void ts__onWindowReactivate(SimObject* obj, int argc, const char* argv[]) {
	if(atoi(GetGlobalVariable("windowReactivating"))) {
		return;
	}
	Printf("Window reactivating..");
	SetGlobalVariable("windowReactivating", "1");
	//i'm colorblind with you tonight..
	//there is no time to let the blood dry.
	//you are running my heart..
	SimObject* canvas = Sim__findObject_name("Canvas");
	if(canvas == NULL)
		return;

	bool isFullScreen = (bool)ts__fastCall(fastLookup("", "isFullScreen"), NULL, 0);
	if(isFullScreen) {
		SimObject* playGui = Sim__findObject_name("PlayGui");
		if(playGui != NULL) {
			ts__fastCall(fastLookup("GameTSCtrl", "setHasRendered"), playGui, 1, "0");
		}
		int old = atoi(GetGlobalVariable("Shader::Enabled"));
		SetGlobalVariable("Shader::Enabled", "0");
		Namespace::Entry* repaint = fastLookup("GuiCanvas", "repaint");
		ts__fastCall(repaint, canvas, 0);
		ts__fastCall(fastLookup("", "flushTextureCache"), NULL, 0);
		ts__fastCall(fastLookup("", "regenerateShadowMapFBOs"), NULL, 0);
		ts__fastCall(repaint, canvas, 0);
		if(old) {
			SetGlobalVariable("Shader::Enabled", "1");
			ts__fastCall(fastLookup("", "initializeShaderAssets"), NULL, 0);
		}
	}
	SetGlobalVariable("windowReactivating", "0");
}

void ts__resetCanvas(SimObject* obj, int argc, const char* argv[]) {
	Printf("Resetting canvas... (weed)");
	SimObject* stuff = Sim__findObject_name("PlayGui");
	if(stuff != NULL) {
		ts__fastCall(fastLookup("GameTSCtrl", "setHasRendered"), stuff, 1, "0");
	}
}

void ts__restartAudio(SimObject* obj, int argc, const char* argv[]) {
	ts__openALInit(NULL, 0, {});
	SimObject* lol = Sim__findObject_name("ServerConnection");
	if(lol == NULL) {
		return;
	}
	int count = (int)ts__fastCall(fastLookup("GameConnection", "getCount"), lol, 0);
	for(int i = 0; i < count; i++) {
		SimObject* blah = Sim__findObject_id((int)ts__fastCall(fastLookup("GameConnection", "getObject"), lol, 1, i));
		if(_stricmp(blah->mNameSpace->mName, "AudioEmitter") == 0) {
			ts__fastCall(fastLookup("AudioEmitter", "schedule"), blah, 2, "10", "update");
		}
	}
}