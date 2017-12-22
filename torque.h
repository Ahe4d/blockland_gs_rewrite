#pragma once
#include <windows.h>
#include <iostream>
#include <cstdarg>

typedef unsigned int U32;
typedef signed int S32;
typedef float F32;
typedef double F64;
typedef U32 SimObjectId;

struct SimObject;

typedef const char *(*StringCallback)(SimObject *obj, int argc, const char* argv[]);
typedef int(*IntCallback)   (SimObject *obj, int argc, const char* argv[]);
typedef float(*FloatCallback) (SimObject *obj, int argc, const char* argv[]);
typedef void(*VoidCallback)  (SimObject *obj, int argc, const char* argv[]);
typedef bool(*BoolCallback)  (SimObject *obj, int argc, const char* argv[]);

struct Namespace
{
	const char* mName;
	const char* mPackage;

	Namespace *mParent;
	Namespace *mNext;
	void *mClassRep;
	U32 mRefCountToParent;
	struct Entry
	{
		enum
		{
			GroupMarker = -3,
			OverloadMarker = -2,
			InvalidFunctionType = -1,
			ScriptFunctionType,
			StringCallbackType,
			IntCallbackType,
			FloatCallbackType,
			VoidCallbackType,
			BoolCallbackType
		};

		Namespace *mNamespace;
		//char _padding1[4];
		Entry *mNext;
		const char *mFunctionName;
		S32 mType;
		S32 mMinArgs;
		S32 mMaxArgs;
		const char *mUsage;
		const char *mPackage;
		void *mCode; // CodeBlock *mCode;
		U32 mFunctionOffset;
		union {
			StringCallback mStringCallbackFunc;
			IntCallback mIntCallbackFunc;
			VoidCallback mVoidCallbackFunc;
			FloatCallback mFloatCallbackFunc;
			BoolCallback mBoolCallbackFunc;
			const char* mGroupName;
		} cb;
	};
	Entry *mEntryList;
	Entry **mHashTable;
	U32 mHashSize;
	U32 mHashSequence;  ///< @note The hash sequence is used by the autodoc console facility
						///        as a means of testing reference state.
	char * lastUsage;
};
static Namespace* GlobalNS;
struct ConsoleObject
{
};

struct SimObject
{
	enum {
#define BIT(x) (1 << (x))
		Deleted = BIT(0),   ///< This object is marked for deletion.
		Removed = BIT(1),   ///< This object has been unregistered from the object system.
		Added = BIT(3),   ///< This object has been registered with the object system.
		Selected = BIT(4),   ///< This object has been marked as selected. (in editor)
		Expanded = BIT(5),   ///< This object has been marked as expanded. (in editor)
							 //ModStaticFields = BIT(6),    ///< The object allows you to read/modify static fields
							 //ModDynamicFields = BIT(7)     ///< The object allows you to read/modify dynamic fields
							 // why are these different?
							 ModStaticFields = BIT(7),    ///< The object allows you to read/modify static fields
							 ModDynamicFields = BIT(8)     ///< The object allows you to read/modify dynamic fields
	};

	char _padding1[4];
	const char *objectName;
	// char _padding2[24];
	// SimObjectId mId;
	SimObject *nextNameObject;
	SimObject *nextManagerNameObject;
	SimObject *nextIdObject;
	void *mGroup;
	unsigned int mFlags;
	void *mNotifyList;
	SimObjectId id;
	Namespace *mNameSpace;
	unsigned int mTypeMask;
	void *mFieldDictionary;
	void setDatablock(SimObject* obj, const char* datablock);
};

struct SimEvent
{
	SimEvent *nextEvent;
	unsigned int startTime;
	unsigned int time;
	unsigned int sequenceCount;
	SimObject *destObject;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Macros

//Typedef an engine function to use it later
#define BLFUNC(returnType, convention, name, ...)         \
	typedef returnType (convention*name##Fn)(__VA_ARGS__); \
	static name##Fn name;

#define FUCKME(returnType, convention, name) \
	extern returnType name(SimObject* obj, int argc, const char** argv); \
	typedef returnType (convention*name##Fn)(SimObject* obj, int argc, const char** argv);

//Typedef an exported engine function to use it later
#define BLFUNC_EXTERN(returnType, convention, name, ...)  \
	typedef returnType (convention*name##Fn)(__VA_ARGS__); \
	extern name##Fn name;

//Search for an engine function in blockland
#define BLSCAN(target, pattern, mask)            \
	target = (target##Fn)ScanFunc(pattern, mask); \
 /////////////////////////////////////////////////////////////////////////////////
//Engine function declarations

//Con::printf
BLFUNC_EXTERN(void, , Printf, const char* format, ...);

extern const char *StringTableEntry(const char *str, bool caseSensitive = false);
void* ts__fastCall(Namespace::Entry* ourCall, SimObject* obj, int argc, ...);
Namespace::Entry* fastLookup(const char* ourNamespace, const char* name);

extern DWORD StringTable;
extern bool setDatablock;
BLFUNC_EXTERN(bool, , initGame, int argc, const char **argv);
BLFUNC_EXTERN(Namespace *, , LookupNamespace, const char *ns);
BLFUNC_EXTERN(const char *, __thiscall, StringTableInsert, DWORD stringTablePtr, const char* val, const bool caseSensitive)
BLFUNC_EXTERN(Namespace::Entry *, __thiscall, Namespace__lookup, Namespace *this_, const char *name)
//BLFUNC_EXTERN(void *, __thiscall, CodeBlock__exec, void *this_, U32 offset, const char *fnName, Namespace *ns, U32 argc, const char **argv, bool noCalls, const char *packageName, int setFrame)
BLFUNC_EXTERN(const char *, __thiscall, CodeBlock__exec, void *this_, U32 offset, Namespace *ns, const char *fnName, U32 argc, const char **argv, bool noCalls, const char *packageName, int setFrame)
BLFUNC_EXTERN(SimObject *, , Sim__findObject_name, const char *name);
BLFUNC_EXTERN(SimObject *, , Sim__findObject_id, unsigned int id);
BLFUNC_EXTERN(unsigned int, , Sim__postEvent, SimObject *destObject, SimEvent *event, U32 time);
BLFUNC_EXTERN(void, , Sim__cancelEvent, unsigned int eventSequence);
BLFUNC_EXTERN(void, __thiscall, SimObject__setDataField, SimObject *this_, const char *name, const char *arr, const char *value)
BLFUNC_EXTERN(const char *, __thiscall, SimObject__getDataField, SimObject *this_, const char *name, const char *arr);
BLFUNC_EXTERN(bool, __thiscall, SimObject__registerObject, SimObject *this_);
BLFUNC_EXTERN(void, __thiscall, SimObject__registerReference, SimObject *this_, SimObject **ptr);
BLFUNC_EXTERN(void, __thiscall, SimObject__unregisterReference, SimObject *this_, SimObject **ptr);
BLFUNC_EXTERN(ConsoleObject *, , AbstractClassRep_create_className, const char *className);
/*
BLFUNC_EXTERN(void, , SimObject__setDataBlock, SimObject *this_, const char *datablock);
BLFUNC_EXTERN(bool, , fxDTSBrick__plant, SimObject *this_);
BLFUNC_EXTERN(void, , fxDTSBrick__setTrusted, SimObject *this_, const char* kappa);
*/
BLFUNC_EXTERN(void, __thiscall, SimObject__delete, SimObject *this_);
//This function is really ..odd.
//Help.
//new metario to the rescue!! fuck that!! we're making the user do it on his/her/idk about your pronouns own!!
//BLFUNC_EXTERN(void, __thiscall, fxDTSBrick__setDataBlock, SimObject *this_, const char *dataBlock);
/* DWORD ScanFunc(char* pattern, char* mask);
void PatchByte(BYTE* location, BYTE value); */

void ConsoleFunction(const char* scope, const char* name, StringCallback callBack, const char* usage, int minArgs, int maxArgs);
void ConsoleFunction(const char* scope, const char* name, IntCallback callBack, const char* usage, int minArgs, int maxArgs);
void ConsoleFunction(const char* scope, const char* name, FloatCallback callBack, const char* usage, int minArgs, int maxArgs);
void ConsoleFunction(const char* scope, const char* name, VoidCallback callBack, const char* usage, int minArgs, int maxArgs);
void ConsoleFunction(const char* scope, const char* name, BoolCallback callBack, const char* usage, int minArgs, int maxArgs);

void ConsoleVariable(const char *name, int *data);
void ConsoleVariable(const char *name, bool *data);
void ConsoleVariable(const char *name, float *data);
void ConsoleVariable(const char *name, char *data);

//Evaluate a torquescript string in global scope
const char* Eval(const char* str);
//Motherfucker.
FUCKME(void, , ts__onDatablockLimitExceeded);
FUCKME(void, , ts__onDatablocksDeleted);
FUCKME(void, , ts__initCommon);
FUCKME(void, , ts__initBaseClient);
FUCKME(void, , ts__initBaseServer);
//base/client/audio.cs.dso
FUCKME(void, , ts__openALInit);
FUCKME(void, , ts__openALShutdown);
//base/client/canvas.cs.dso
FUCKME(void, , ts__initCanvas);
FUCKME(void, , ts__onWindowReactivate);
FUCKME(void, , ts__resetCanvas);
FUCKME(void, , ts__restartAudio);
//base/client/mission.cs.dso
FUCKME(void, , ts__clientCmdMissionStart);
FUCKME(void, , ts__clientCmdMissionEnd);
//base/client/message.cs.dso
FUCKME(void, , ts__addMessageCallback);
FUCKME(void, , ts__clientCmdChatMessage);
//base/client/init.cs.dso
FUCKME(void, , ts__initClient);
//base/client/missionDownload.cs.dso
FUCKME(void, , ts__clientCmdMissionStartPhase1);
//Call a function
BLFUNC_EXTERN(void, , RawCall, S32 argc, const char* argv);

BLFUNC_EXTERN(void, , SetGlobalVariable, const char *name, const char *value);
BLFUNC_EXTERN(char *, , GetGlobalVariable, const char *name);

typedef void(__thiscall *ShapeNameHudOnRenderFn)(DWORD* obj, DWORD arg1, DWORD arg2, DWORD arg3);
// typedef void(__fastcall *ShapeNameHudOnRenderFn)(SimObject *this_, int, int arg1, int arg2);
static ShapeNameHudOnRenderFn ShapeNameHudOnRender;

bool torque_init();
