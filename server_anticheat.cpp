//hah stupid aimbot code
#include "torque.h"
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <utility> 
#include "mathfu/vector.h"
#include "mathfu/glsl_mappings.h"

//yuh...
//yuh...
//lmao

static std::map<int, velRecorded> lastVel;
static int firstRun = 1;

/*
F32 clamp(F32 in, F32 min, F32 max) {
	return std::max(std::min(in, max), min);
}

void normalize(F32& x, F32& y, F32& z) {
	F32 squared = x * x + y * y + z * z;
	if(squared != 0.0f) {
		float factor = 1.0f / sqrt(squared);
		x *= factor;
		y *= factor;
		z *= factor;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
		z = 1.0f;
	}
}
*/

int SimSet__getCount(DWORD set)
{
	return *(DWORD*)(set + 0x34);
}


SimObject* SimSet__getObject(DWORD set, int index)
{
	DWORD ptr1 = *(DWORD*)(set + 0x3C);
	SimObject* ptr2 = (SimObject*)*(DWORD*)(ptr1 + 0x4 * index);
	return ptr2;
}

mathfu::vec3 Player__getPosition(DWORD playerPtr)
{
	float x = *((float *)(playerPtr + 0x68));
	float y = *((float *)(playerPtr + 0x78));
	float z = *((float *)(playerPtr + 0x88));

	mathfu::vec3 pos(x, y, z);
	return pos;
}

mathfu::vec3 Player__getVelocity(DWORD player)
{
	float velX = (*reinterpret_cast<float*>((player + 2044)));
	float velY = (*reinterpret_cast<float*>((player + 2048)));
	float velZ = (*reinterpret_cast<float*>((player + 2052)));
	mathfu::vec3 vel(velX, velY, velZ);
	return vel;
}

mathfu::vec3 extrapolate(mathfu::vec3 pos, mathfu::vec3 vel) {
	return pos + (vel * 32);
}

SimObject* getNearestClient(SimObject* us) {
	SimObject* cg = *cachedObjFind("ClientGroup");
	int closest, dist;
	closest = 99999;
	dist = 99999999;
	if(cg != NULL && us != NULL) {
		//int count = (int)ts__fastCall(fastLookup(cg->mNameSpace->mName, "getCount"), cg, 1, id3);
		//Printf("Get count");
		int count = SimSet__getCount((DWORD)cg);
		if(count < 2) { //Only work when a client can get a target onto another..
			return nullptr;
		}
		//Printf("Get the nearest..");
		SimObject* theNearest = nullptr;
		for(int wtflol = 0; wtflol < count; wtflol++) {
			SimObject* obj = SimSet__getObject((DWORD)cg, wtflol);
			if(obj != NULL){
				const char* player = SimObject__getDataField(obj, "player", StringTableEntry(""));
				if(_stricmp(player, "") != 0) {
					//Printf("Getting player pointer..");
					SimObject* pl = nullptr;
					SimObject** theboys = cachedObjFind(atoi(player));
					if(theboys != nullptr) {
						pl = *theboys;
					}
					else {
						return nullptr;
					}
					//Printf("Making sure that it isn't null.");
					if(pl != NULL && obj != NULL && pl != nullptr && us != nullptr && obj != nullptr) {
						//Printf("Making sure that their id isn't ours..");
						if(pl->id != us->id) {
							//Printf("Getting player positions");
							mathfu::vec3 qos = Player__getPosition((DWORD)pl);
							mathfu::vec3 pos = Player__getPosition((DWORD)us);
							mathfu::vec3 len = qos - pos;
							float vecDist = len.Length();
							if(vecDist <= dist) {
								//Printf("Comparing positions");
								dist = vecDist;
								closest = wtflol;
								theNearest = pl;
							}
						}
					}
				}
			}
		}
		return theNearest;
	}
	return nullptr;
}


WrappedPosData ts__calcAim(U32 id) {
	//Do the shit here.
	//Printf("Running...");

	if(firstRun) {
		firstRun = 0;
	}
	SimObject* a = *cachedObjFind(id);
	//Printf("getting nearest client");
	SimObject* b = getNearestClient(a);
	if(a == nullptr || b == nullptr || a == NULL || b == NULL) {
		return {false, mathfu::vec2(0.0f, 0.0f)};
	}
	//Printf("Looking up EyeVector..");
	Namespace::Entry* getEyeV = fastLookup(a->mNameSpace->mName, "getEyeVector");
	if(getEyeV == NULL && getEyeV == nullptr) {
		return {false, mathfu::vec2(0.0f, 0.0f)};
	}
	int itaend = 0;
	int itbend = 0;
	std::map<int, velRecorded>::iterator ita = lastVel.find(a->id);
	std::map<int, velRecorded>::iterator itb = lastVel.find(b->id);
	const char* id1 = Con__getIntArg(a->id);
	//Printf("Getting position..");
	mathfu::vec3 posA(0.0f, 0.0f, 0.0f);
	mathfu::vec3 posB(0.0f, 0.0f, 0.0f);
	if(a != nullptr && b != nullptr) {
		posA = Player__getPosition((DWORD)a);
		posB = Player__getPosition((DWORD)b);
	}
	else
	{
		return {false, mathfu::vec2(0.0f, 0.0f)};
	}
	//Printf("Getting eyevec");
	mathfu::vec3 eyeVec(0.0f, 0.0f, 0.0f); //Extract their eyevector here..
	if(a != nullptr) {
		const char* eye = (const char*)ts__fastCall(getEyeV, a, 1, id1);
		sscanf(eye, "%f %f %f", &eyeVec[0], &eyeVec[1], &eyeVec[2]);
	}
	else {
		return {false, mathfu::vec2(0.0f, 0.0f)};
	}

	mathfu::vec3 oldVecA(0.0f, 0.0f, 0.0f);
	mathfu::vec3 oldVecB(0.0f, 0.0f, 0.0f);
	//Printf("Getting stored velocity");
	U32 loaded = 0;
	if(ita != lastVel.end()) {
		loaded = ita->second.time;
		oldVecA = ita->second.vel;
	}
	else {
		itaend = 1;
		loaded = 0;
	}

	if(itb != lastVel.end()) {
		oldVecB = itb->second.vel;
	}
	else
	{
		itbend = 1;
	}
	//Printf("Getting new velocity");
	mathfu::vec3 velA(0.0f, 0.0f, 0.0f);
	mathfu::vec3 velB(0.0f, 0.0f, 0.0f);
	if(a != nullptr && b != nullptr) {
		velA = Player__getVelocity((DWORD)a);
		velB = Player__getVelocity((DWORD)b);
	}
	else {
		return {false, mathfu::vec2(0.0f, 0.0f)};
	}

	if(oldVecB[0] == 0.0f && oldVecB[1] == 0.0f && oldVecB[2] == 0.0f) {
		oldVecB = velB;
		oldVecA = velA;
	}

	U32 time = GetTickCount();
	U32 timeDif = time - loaded;
	if(timeDif > 500) {
		//Velocity is too old. Discard it.
		//oldVecB = velB;
		//oldVecA = velA;
		timeDif = time;
	}

	if(timeDif < 1) {
		timeDif = 1;
	}

	int routine1 = 1;
	//Printf("Time dif: %d", timeDif);
	float yaw = 0.0f;
	float pitch = 0.0f;

	//const char* spinwhee = SimObject__getDataField(a, "spinBot", StringTableEntry(""));
	//if(_stricmp(spinwhee, "1") == 0) {
		//Printf("WHEE!");
	//	routine1 = 2;
	//}

	if(routine1 == 2) {
		float rads = atan2(eyeVec[0], eyeVec[1]);
		float degres = rads * 180.0 / M_PI;
		
		double rate =  360.0/1.618033988749895;
		float idkreally = fmod(rate * 32, 360.0);
		yaw = idkreally;
		pitch = degres;
		//Printf("our degrees: %f, radians: %f", deg, aaaa);
	}
	else if(routine1 == 1){
		mathfu::vec3 aa = (velA - oldVecA) / timeDif;
		mathfu::vec3 bb = (velB - oldVecB) / timeDif;
		mathfu::vec3 gg = posB - posA;
		float tt = gg.Length() / 95;
		//float vx, vy, vz;
		//Printf("doing aimbot math");
		mathfu::vec3 v = bb - aa;
		v *= 0.5;
		v *= tt;
		v *= tt;
		v += velB - velA;
		v *= tt;
		v += gg;
		v.Normalize();
		//eyeVec.Normalize();
		yaw = atan2(v[0], v[1]) - atan2(eyeVec[0], eyeVec[1]);
		pitch = atan2(eyeVec[2], eyeVec.xy().Length()) - atan2(v[2], v.xy().Length());
	}
	else if(routine1 == 0) {
		mathfu::vec3 predLoc(0.0f, 0.0f, 0.0f);
		predLoc = (posB - posA) + ((velB - oldVecB) * timeDif) + (0.5 * timeDif * timeDif);
		predLoc.Normalize();
		yaw = atan2(predLoc[0], predLoc[1]) - atan2(eyeVec[0], eyeVec[1]);
		pitch = atan2(eyeVec[2], sqrt(eyeVec[0] * eyeVec[0] + eyeVec[1] * eyeVec[1])) - atan2(predLoc[2], sqrt(predLoc[0] * predLoc[0] + predLoc[1] * predLoc[1]));
	}
	//Printf("Clamping move");
	Move* eek = new Move; //Construct a new move so it can clamp it CORRECTLY...
	eek->yaw = yaw;
	eek->pitch = pitch;
	ClampMove(eek);
	yaw = eek->yaw;
	pitch = eek->pitch;
	mathfu::vec2 ret(yaw, pitch);
	delete eek;

	//Printf("Storing in map");
    if(!itaend) {
     // Printf("erasing");
       lastVel.erase(ita);
    }
    
   // Printf("insert1");
    if(a != nullptr && a != NULL) {
    	velRecorded vendetta = {time, velA};
    	lastVel.insert(lastVel.end(), std::make_pair(a->id, vendetta));
    }

    if(!itbend) {
    	lastVel.erase(itb);
    }

    //Printf("insert2");
    if(b != nullptr && b != NULL) {
    	velRecorded second = {time, velB};
    	lastVel.insert(lastVel.end(), std::make_pair(b->id, second));
    }

    //Printf("We're done here.");
	return {true, ret};
}
