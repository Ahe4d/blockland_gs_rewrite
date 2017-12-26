//hah stupid aimbot code
#include "torque.h"
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <cmath>

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

void ts__calcAim(SimObject* obj, int argc, const char* argv[]) {
	//Do the shit here.
	if(argc != 3) {
		Printf("Boi wtf r u doin");
		return;
	}

	SimObject* a = *cachedObjFind(atoi(argv[1]));
	SimObject* b = *cachedObjFind(atoi(argv[2]));
	if(a == NULL || b == NULL) {
		Printf("Could not find an object.");
		return;
	}
		Namespace::Entry* getTransform = fastLookup(a->mNameSpace->mName, "getTransform");
		Namespace::Entry* getTransformB = fastLookup(b->mNameSpace->mName, "getTransform");
		
		Namespace::Entry* getEyeV = fastLookup(a->mNameSpace->mName, "getEyeVector");
		
		Namespace::Entry* getVel = fastLookup(a->mNameSpace->mName, "getVelocity");
		Namespace::Entry* getVel2 = fastLookup(b->mNameSpace->mName, "getVelocity");
		if(getVel == NULL || getVel2 == NULL || getTransform == NULL || getEyeV == NULL || getTransformB == NULL) {
			return;
		}
		float posx, posy, posz, angAxisX, angAxisY, angAxisZ, angle; //All the fields in a matrixf.
		
		float bosx, bosy, bosz, bngAxisX, bngAxisY, bngAxisZ, bngle;

		float eyeVecX, eyeVecY, eyeVecZ;
		//Impacto.

		char id1[9];
		char id2[9];
		sprintf(id1, "%d", a->id);
		sprintf(id2, "%d", b->id);
		const char* trans = (const char*)ts__fastCall(getTransform, a, 1, id1);
		sscanf(trans, "%f %f %f %f %f %f %f", &posx, &posy, 
			&posz, &angAxisX, &angAxisY, &angAxisZ, &angle);

		const char* trans2 = (const char*)ts__fastCall(getTransformB, b, 1, id2);
		sscanf(trans2, "%f %f %f %f %f %f %f", &bosx, &bosy, 
			&bosz, &bngAxisX, &bngAxisY, &bngAxisZ, &bngle); 

		const char* eye = (const char*)ts__fastCall(getEyeV, a, 1, id1);
		sscanf(eye, "%g %g %g", &eyeVecX, &eyeVecY, &eyeVecZ);
		float aelx, aely, aelz;
		float belx, bely, belz;
		//float lastVelX, lastVelY, lastVelZ;
		//float lastVelThemX, lastVelThemY, lastVelThemZ;
		const char* ael = (const char*)ts__fastCall(getVel, a, 1, id1);
		const char* bel = (const char*)ts__fastCall(getVel2, b, 1, id2);
		//const char* lastVel = SimObject__getDataField(a, "lastVel", StringTableEntry(""));
		//const char* lastVelThem = SimObject__getDataField(b, "lastVel", StringTableEntry(""));
		sscanf(ael, "%f %f %f", &aelx, &aely, &aelz);
		sscanf(bel, "%f %f %f", &belx, &bely, &belz);
		float xx, yy, zz;
		xx = (bosx - posx);
		yy = (bosy - posy);
		zz = (bosz - posz);
		float tt = sqrt(xx * xx + yy * yy + zz * zz) / 95;

		float vx, vy, vz;
		vx = (belx - aelx) * tt + xx;
		vy = (bely - aely) * tt + yy;
		vz = (belz - aelz) * tt + zz;

		//Find the best object.
		normalize(vx, vy, vz);
		//normalize(eyeVecX, eyeVecY, eyeVecZ);
		//Printf("Eye vec: %f %f %f, position: %f %f %f", eyeVecX, eyeVecY, eyeVecZ, vx, vy, vz);
		float yaw = atan2(vx, vy) - atan2(eyeVecX, eyeVecY);

		float pitch = atan2(eyeVecZ, sqrt(eyeVecX * eyeVecX + eyeVecY * eyeVecY)) - atan2(vz, sqrt(vx * vx + vy * vy));
		//Clamp these fuckers
		Move* newMoves = new Move();
		newMoves->yaw = yaw;
		newMoves->pitch = pitch;
		ClampMove(newMoves);
		char blah[500];
		sprintf(blah, "%f %f", newMoves->yaw, newMoves->pitch);
		SimObject__setDataField(a, "stuff", StringTableEntry(""), StringTableEntry(blah));
		//SimObject__setDataField(a, "enable", StringTableEntry(""), StringTableEntry("1"));
		delete newMoves;
		//Printf("Calculated yaw: %f, pitch: %f", yaw, pitch);
}
