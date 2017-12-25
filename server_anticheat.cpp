//hah stupid aimbot code
#include "torque.h"
#include <windows.h>

void ts__calcAim(SimObject* obj, int argc, const char* argv[]) {
	//Do the shit here.
	if(argc != 3) {
		Printf("Boi wtf r u doin");
		return;
	}

	SimObject* a = Sim__findObject_id(atoi(argv[1]));
	SimObject* b = Sim__findObject_id(atoi(argv[2]));
	if(a == NULL || b == NULL) {
		Printf("Could not find an object.");
		return;
	}

		//They're the same class..
		Namespace::Entry* getTransform = fastLookup(a->mNameSpace->mName, "getTransform");
		Namespace::Entry* getTransformB = fastLookup(b->mNameSpace->mName, "getTransform");
		//This'll spit out a MatrixF..
		//G-g-get low...
		float posx, posy, posz, angAxisX, angAxisY, angAxisZ, angle; //All the fields in a matrixf.
		
		float bosx, bosy, bosz, bngAxisX, bngAxisY, bngAxisZ, bngle;
		//Impacto.

		char id1[9];
		char id2[9];
		sprintf(id1, "%d", a->id);
		sprintf(id2, "%d", b->id);
		const char* trans = (const char*)ts__fastCall(getTransform, a, 1, id1);
		sscanf(trans, "%g %g %g %g %g %g %g", &posx, &posy, 
			&posz, &angAxisZ, &angAxisY, &angAxisZ, &angle);

		const char* trans2 = (const char*)ts__fastCall(getTransformB, b, 1, id2);
		sscanf(trans2, "%g %g %g %g %g %g %g", &bosx, &bosy, 
			&bosz, &bngAxisZ, &bngAxisY, &bngAxisZ, &bngle); 

		//Find the best object.
		SimObject* cg = Sim__findObject_name("ClientGroup");
		
		if(cg != NULL) { //Just some basic sanity checking.	
			char id3[9];
			sprintf(id3, "%d", cg->id);
			int count = (int)ts__fastCall(fastLookup(cg->mNameSpace->mName, "getCount"), cg, 1, id3);
			Printf("Found %d in ClientGroup.", count);
			Printf("Hopefully entering loop..");
			int wtflol = 0;
			for(wtflol < count; wtflol++;) {
				Printf("Entered loop.");
				char fuck[9];
				sprintf(fuck, "%d", wtflol);
				Printf("Sprintf succeeded");
				SimObject* obj = Sim__findObject_id((int)ts__fastCall(fastLookup(cg->mNameSpace->mName, "getObject"), cg, 1, fuck));
				Printf("Sim__findObject_id succeeded");
				if(obj != NULL){
					Printf("Found obj %d in ClientGroup.", obj->id);
				}
				else
				{
					Printf("Encountered a null object at index %d", wtflol);
				}
			}
		}
}