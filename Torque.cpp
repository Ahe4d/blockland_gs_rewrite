#include "torque.h"
#include <psapi.h>
#include <iostream>
#include <cstdarg>
#include <map>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Global variables

static DWORD ImageBase; // start of the Blockland.exe module in memory
static DWORD ImageSize; // length of the Blockland.exe module

DWORD StringTable; // StringTable pointer
static DWORD GlobalVars; // global variable dictionary pointer

static std::map<const char*, Namespace::Entry*> cache;
static std::map<const char*, Namespace*> nscache;


						 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						 //Engine function declarations

						 //Con::printf
PrintfFn Printf;

//Con::lookupNamespace
LookupNamespaceFn LookupNamespace;

//StringTable::insert
initGameFn initGame;
StringTableInsertFn StringTableInsert;
Namespace__lookupFn Namespace__lookup;
CodeBlock__execFn CodeBlock__exec;
Sim__findObject_nameFn Sim__findObject_name;
Sim__findObject_idFn Sim__findObject_id;
Sim__postEventFn Sim__postEvent;
Sim__cancelEventFn Sim__cancelEvent;
SimObject__setDataFieldFn SimObject__setDataField;
SimObject__getDataFieldFn SimObject__getDataField;
SimObject__registerObjectFn SimObject__registerObject;
SimObject__registerReferenceFn SimObject__registerReference;
SimObject__unregisterReferenceFn SimObject__unregisterReference;
AbstractClassRep_create_classNameFn AbstractClassRep_create_className;
SimObject__deleteFn SimObject__delete;
//SimObject__setDataBlockFn SimObject__setDataBlock;
//fxDTSBrick__setDataBlockFn fxDTSBrick__setDataBlock;
//fxDTSBrick__plantFn fxDTSBrick__plant;
//fxDTSBrick__setTrustedFn fxDTSBrick__setTrusted;

//Namespace::addCommand overloads
BLFUNC(void, __thiscall, AddStringCommand, Namespace *ns, const char* name, StringCallback cb, const char *usage, int minArgs, int maxArgs);
BLFUNC(void, __thiscall, AddIntCommand, Namespace *ns, const char* name, IntCallback cb, const char *usage, int minArgs, int maxArgs);
BLFUNC(void, __thiscall, AddFloatCommand, Namespace *ns, const char* name, FloatCallback cb, const char *usage, int minArgs, int maxArgs);
BLFUNC(void, __thiscall, AddVoidCommand, Namespace *ns, const char* name, VoidCallback cb, const char *usage, int minArgs, int maxArgs);
BLFUNC(void, __thiscall, AddBoolCommand, Namespace *ns, const char* name, BoolCallback cb, const char *usage, int minArgs, int maxArgs);

//Exposing variables to torquescript
BLFUNC(void, __thiscall, AddVariable, DWORD dictionaryPtr, const char* name, int type, void* data);

//Executing code and calling torquescript functions
BLFUNC(const char*, , Evaluate, const char* string, bool echo, const char* fileName);

RawCallFn RawCall;
SetGlobalVariableFn SetGlobalVariable;
GetGlobalVariableFn GetGlobalVariable;
//BLFUNC(void, , SetGlobalVariable, const char *name, const char *value);
//BLFUNC(char *, , GetGlobalVariable, const char *name);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions

//Set the module start and length
void InitScanner()
{
	//Find the module
	// HMODULE module = GetModuleHandleA(moduleName);
	HMODULE module = GetModuleHandle(NULL);

	if (module)
	{
		//Retrieve information about the module
		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), module, &info, sizeof (MODULEINFO));

		//Store relevant information
		ImageBase = (DWORD)info.lpBaseOfDll;
		ImageSize = info.SizeOfImage;
	}
}

//Compare data at two locations for equality
bool CompareData(PBYTE data, PBYTE pattern, char* mask)
{
	//Iterate over the data, pattern and mask in parallel
	for (; *mask; ++data, ++pattern, ++mask)
	{
		//And check for equality at each unmasked byte
		if (*mask == 'x' && *data != *pattern)
			return false;
	}

	return (*mask) == NULL;
}

//Find a pattern in memory
DWORD FindPattern(DWORD imageBase, DWORD imageSize, PBYTE pattern, char* mask)
{
	//Iterate over the image
	for (DWORD i = imageBase; i < imageBase + imageSize; i++)
	{
		//And check for matching pattern at every byte
		if (CompareData((PBYTE)i, pattern, mask))
			return i;
	}

	return 0;
}

//Scan the module for a pattern
DWORD ScanFunc(char* pattern, char* mask)
{
	//Just search for the pattern in the module
	return FindPattern(ImageBase, ImageSize - strlen(mask), (PBYTE)pattern, mask);
}

//Change a byte at a specific location in memory
void PatchByte(BYTE* location, BYTE value)
{
	//Remove protection
	DWORD oldProtection;
	VirtualProtect(location, 1, PAGE_EXECUTE_READWRITE, &oldProtection);

	//Change value
	*location = value;

	//Restore protection
	VirtualProtect(location, 1, oldProtection, &oldProtection);
}

const char *StringTableEntry(const char *str, bool caseSensitive)
{
	return StringTableInsert(StringTable, str, caseSensitive);
}

//Register a torquescript function that returns a string. The function must look like this:
//const char* func(DWORD* obj, int argc, const char* argv[])
void ConsoleFunction(const char* nameSpace, const char* name, StringCallback callBack, const char* usage, int minArgs, int maxArgs)
{
	AddStringCommand(LookupNamespace(nameSpace), StringTableInsert(StringTable, name, false), callBack, usage, minArgs, maxArgs);
}

//Register a torquescript function that returns an int. The function must look like this:
//int func(DWORD* obj, int argc, const char* argv[])
void ConsoleFunction(const char* nameSpace, const char* name, IntCallback callBack, const char* usage, int minArgs, int maxArgs)
{
	AddIntCommand(LookupNamespace(nameSpace), StringTableInsert(StringTable, name, false), callBack, usage, minArgs, maxArgs);
}

//Register a torquescript function that returns a float. The function must look like this:
//float func(DWORD* obj, int argc, const char* argv[])
void ConsoleFunction(const char* nameSpace, const char* name, FloatCallback callBack, const char* usage, int minArgs, int maxArgs)
{
	AddFloatCommand(LookupNamespace(nameSpace), StringTableInsert(StringTable, name, false), callBack, usage, minArgs, maxArgs);
}

//Register a torquescript function that returns nothing. The function must look like this:
//void func(DWORD* obj, int argc, const char* argv[])
void ConsoleFunction(const char* nameSpace, const char* name, VoidCallback callBack, const char* usage, int minArgs, int maxArgs)
{
	AddVoidCommand(LookupNamespace(nameSpace), StringTableInsert(StringTable, name, false), callBack, usage, minArgs, maxArgs);
}

//Register a torquescript function that returns a bool. The function must look like this:
//bool func(DWORD* obj, int argc, const char* argv[])
void ConsoleFunction(const char* nameSpace, const char* name, BoolCallback callBack, const char* usage, int minArgs, int maxArgs)
{
	AddBoolCommand(LookupNamespace(nameSpace), StringTableInsert(StringTable, name, false), callBack, usage, minArgs, maxArgs);
}

//Expose an integer variable to torquescript
void ConsoleVariable(const char* name, int* data)
{
	AddVariable(GlobalVars, name, 4, data);
}

//Expose a boolean variable to torquescript
void ConsoleVariable(const char* name, bool* data)
{
	AddVariable(GlobalVars, name, 6, data);
}

//Expose a float variable to torquescript
void ConsoleVariable(const char* name, float* data)
{
	AddVariable(GlobalVars, name, 8, data);
}

//Expose a string variable to torquescript
void ConsoleVariable(const char* name, char* data)
{
	AddVariable(GlobalVars, name, 10, data);
}

//Evaluate a torquescript string in global scope
const char* Eval(const char* str)
{
	return Evaluate(str, false, NULL);
}

void rewrite__fatal() {
	Printf("!!! THIS SHOULD NEVER HAPPEN !!!");
}

void* ts__fastCall(Namespace::Entry* ourCall, SimObject* obj = NULL, int argc = 0, ...) {
	if(ourCall == NULL) {
		Printf("Invalid entry.");
		return nullptr;
	}
	const char* argv[argc + 1];
	va_list args;
	va_start(args, argc);
	argv[0] = ourCall->mFunctionName;
	for(int i = 0; i < argc; i++) {
		argv[i + 1] = va_arg(args, const char*);
	}
	argc++;
	va_end(args);
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
		actualObj = obj;
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

//Initialize the Torque Interface
bool torque_init()
{
	InitScanner();

	//Printf is required for debug output, so find it first
	Printf = (PrintfFn)ScanFunc("\x8B\x4C\x24\x04\x8D\x44\x24\x08\x50\x6A\x00\x6A\x00\xE8\x00\x00\x00\x00\x83\xC4\x0C\xC3", "xxxxxxxxxxxxxx????xxxx");

	//Do nothing if we don't find it :(
	if (!Printf)
		return false;

	ShapeNameHudOnRender = (ShapeNameHudOnRenderFn)ScanFunc("\x81\xec\x00\x00\x00\x00\x53\x8b\xd9\x8a\x83\xc9\x00\x00\x00\x84\xc0\x55\x56\x57\x89\x5c\x24\x14", "xx????xxxxxxxxxxxxxxxxxx");

	//First find all the functions
	BLSCAN(initGame, "\x56\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x6A\xFF", "xx????x????x????xx");
	BLSCAN(LookupNamespace, "\x8B\x44\x24\x04\x85\xC0\x75\x05", "xxxxxxxx");
	BLSCAN(StringTableInsert, "\x53\x8B\x5C\x24\x08\x55\x56\x57\x53", "xxxxxxxxx");
	BLSCAN(Namespace__lookup, "\x53\x56\x8B\xF1\x8B\x46\x24", "xxxxxxx");
	BLSCAN(CodeBlock__exec, "\x83\xEC\x44\x53\x55\x56\x8B\xE9", "xxxxxxxx");
	BLSCAN(Sim__findObject_name, "\x57\x8B\x7C\x24\x08\x8A\x07", "xxxxxxx");
	BLSCAN(Sim__findObject_id, "\x8B\x44\x24\x04\x8B\x0D\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\xC3", "xxxxxx????xx????x");
	BLSCAN(Sim__postEvent, "\x8B\x4C\x24\x08\x8B\x44\x24\x0C\x89\x41\x08", "xxxxxxxxxxx");
	BLSCAN(Sim__cancelEvent, "\x8B\x0D\x00\x00\x00\x00\x85\xC9\xB8\x00\x00\x00\x00\x74\x41", "xx????xxx????xx");
	BLSCAN(SimObject__setDataField, "\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC4\x53\x8B\x9C\x24\x00\x00\x00\x00\x55\x8B\xAC\x24\x00\x00\x00\x00\x56\x8B\xF1\x8B\x4E\x18", "xx????x????xxxxxx????xxxx????xxxxxx");
	BLSCAN(SimObject__getDataField, "\x53\x8B\x5C\x24\x0C\x55\x8B\x6C\x24\x0C\x56\x8B\xF1\x8B\x46\x18", "xxxxxxxxxxxxxxxx");
	BLSCAN(SimObject__registerObject, "\x53\x56\x8B\xF1\x83\x66\x18\xFC", "xxxxxxxx");
	BLSCAN(SimObject__registerReference, "\xA1\x00\x00\x00\x00\x85\xC0\x56\x8B\xF1", "x????xxxxx");
	BLSCAN(SimObject__unregisterReference, "\x8D\x41\x1C\x83\x38\x00\x74\x1C", "xxxxxxxx");
	BLSCAN(AbstractClassRep_create_className, "\x56\x8B\x35\x00\x00\x00\x00\x85\xF6\x57\x74\x1C", "xxx????xxxxx");

	//These are almost identical. Long sigs required
	BLSCAN(AddStringCommand,
		"\x8B\x44\x24\x04\x56\x50\xE8\x00\x00\x00\x00\x8B\xF0\xA1\x00\x00\x00\x00\x40\xB9\x00\x00\x00\x00\xA3"
		"\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B\x54\x24\x14\x8B\x44\x24\x18\x89\x4E\x18\x8B"
		"\x4C\x24\x0C\x89\x56\x10\x89\x46\x14\xC7\x46\x0C\x01\x00\x00\x00\x89\x4E\x28\x5E\xC2\x14\x00",
		"xxxxxxx????xxx????xx????x????x????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	BLSCAN(AddIntCommand,
		"\x8B\x44\x24\x04\x56\x50\xE8\x00\x00\x00\x00\x8B\xF0\xA1\x00\x00\x00\x00\x40\xB9\x00\x00\x00\x00\xA3"
		"\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B\x54\x24\x14\x8B\x44\x24\x18\x89\x4E\x18\x8B"
		"\x4C\x24\x0C\x89\x56\x10\x89\x46\x14\xC7\x46\x0C\x02\x00\x00\x00\x89\x4E\x28\x5E\xC2\x14\x00",
		"xxxxxxx????xxx????xx????x????x????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	BLSCAN(AddFloatCommand,
		"\x8B\x44\x24\x04\x56\x50\xE8\x00\x00\x00\x00\x8B\xF0\xA1\x00\x00\x00\x00\x40\xB9\x00\x00\x00\x00\xA3"
		"\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B\x54\x24\x14\x8B\x44\x24\x18\x89\x4E\x18\x8B"
		"\x4C\x24\x0C\x89\x56\x10\x89\x46\x14\xC7\x46\x0C\x03\x00\x00\x00\x89\x4E\x28\x5E\xC2\x14\x00",
		"xxxxxxx????xxx????xx????x????x????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	BLSCAN(AddVoidCommand,
		"\x8B\x44\x24\x04\x56\x50\xE8\x00\x00\x00\x00\x8B\xF0\xA1\x00\x00\x00\x00\x40\xB9\x00\x00\x00\x00\xA3"
		"\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B\x54\x24\x14\x8B\x44\x24\x18\x89\x4E\x18\x8B"
		"\x4C\x24\x0C\x89\x56\x10\x89\x46\x14\xC7\x46\x0C\x04\x00\x00\x00\x89\x4E\x28\x5E\xC2\x14\x00",
		"xxxxxxx????xxx????xx????x????x????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	BLSCAN(AddBoolCommand,
		"\x8B\x44\x24\x04\x56\x50\xE8\x00\x00\x00\x00\x8B\xF0\xA1\x00\x00\x00\x00\x40\xB9\x00\x00\x00\x00\xA3"
		"\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x10\x8B\x54\x24\x14\x8B\x44\x24\x18\x89\x4E\x18\x8B"
		"\x4C\x24\x0C\x89\x56\x10\x89\x46\x14\xC7\x46\x0C\x05\x00\x00\x00\x89\x4E\x28\x5E\xC2\x14\x00",
		"xxxxxxx????xxx????xx????x????x????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	BLSCAN(AddVariable, "\x8B\x44\x24\x04\x56\x8B\xF1\x80\x38\x24\x74\x1A", "xxxxxxxxxxxx");
	BLSCAN(Evaluate, "\x8A\x44\x24\x08\x84\xC0\x56\x57\x8B\x7C\x24\x0C", "xxxxxxxxxxxx");
	SimObject__delete = (SimObject__deleteFn)(void*)0x4b4e30;
	//32
	BLSCAN(RawCall, "\x8B\x0D\x00\x00\x00\x00\x56\x8B\x74\x24\x0C", "xx????xxxxx");
//	BLSCAN(fxDTSBrick__setDataBlock, "\x8B\x44\x24\x04\x56\x8B\x74\x24\x10\x50\xE8\x11\xE3\x00\x00\x83\xC4\x04\x5E\xC3\xCC\xCC\xCC\xCC", "xxxxxxxxxxxxx??xxxxxxxxx");
//	BLSCAN(fxDTSBrick__plant, "\x8B\x4C\x24\x04\xE9\x07\xEE\x00\x00\xCC\xCC\xCC\xCC\xCC\xCC\xCC", "xxxxxxx??xxxxxxx")
//		BLSCAN(SimObject__setDataBlock, "\x56\x8B\x74\x24\x10\x57\x8B\x7C\x24\x0C\xE8\x51\xFE\xFF\xFF\x5F\x5E\xC3\xCC\xCC\xCC\xCC\xCC\xCC", "xxxxxxxxxxxxxxxxxxxxxxx");
	BLSCAN(GetGlobalVariable, "\x56\x8b\x74\x24\x08\x85\xf6\x74\x00\x8a\x06\x84\xc0\x75", "xxxxxxxx?xxxxx");
	BLSCAN(SetGlobalVariable, "\x56\x8b\x74\x24\x08\x80\x3e\x24\x8b\xc6\x74\x00\x56\xe8", "xxxxxxxxxxx?xx");
	//since it's so small we need a long sig
//	BLSCAN(fxDTSBrick__setTrusted, "\x8B\x44\x24\x0C\x8B\x48\x08\x51\xE8\x13\xCE\xEC\x00\x8B\x54\x24\x08\x83\xC4\x04\x88\x82\xB6\x02", "xxxxxxxxxxxx?xxxxxxxxxxx");
	//The string table is used in lookupnamespace so we can get it's location
	StringTable = *(DWORD*)(*(DWORD*)((DWORD)LookupNamespace + 15));

	//Get the global variable dictionary pointer
	GlobalVars = *(DWORD*)(ScanFunc("\x8B\x44\x24\x0C\x8B\x4C\x24\x04\x50\x6A\x06", "xxxxxxxxxxx") + 13);
	GlobalNS = LookupNamespace(NULL); //look it up here..
	return true;
}
