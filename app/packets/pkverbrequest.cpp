// Earth & Beyond client (reconstruction): app\packets\pkverbrequest.cpp. Notes at the end of the file.
#include "pkverbrequest.h"
#include <stdio.h>


VerbRequestPacketClass::VerbRequestPacketClass(int subject_id, int object_id, int action) : SubjectID(subject_id), ObjectID(object_id), Action(action)
{
}

VerbRequestPacketClass::VerbRequestPacketClass(RawPacketClass *raw)
{
	raw->Get(SubjectID);
	raw->Get(ObjectID);
	raw->Get(&Action, sizeof(Action));
}

RawPacketClass *VerbRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(SubjectID);
	raw->Add(ObjectID);
	raw->Add(&Action, sizeof(Action));
	return(raw);
}

int VerbRequestPacketClass::Get_Type()
{
	return(90);
}

int VerbRequestPacketClass::Get_Data_Size()
{
	return(sizeof(SubjectID) + sizeof(ObjectID) + sizeof(Action));
}

int VerbRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s { SubjectID=%d ObjectID=%d Action=%d}", "VerbRequest", SubjectID, ObjectID, Action));
}


/*
**	Reconstruction notes (pkverbrequest.cpp)
**
**	* Client 0x8AEAE0-0x8B15C0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 91, 92 (declared in pk_classes.h).
*/
