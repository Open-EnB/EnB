// Earth & Beyond client (reconstruction): app\packets\pkaction.cpp. Notes at the end of the file.
#include "pkaction.h"
#include <stdio.h>


ActionPacketClass::ActionPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&Unknown14, sizeof(Unknown14));
	raw->Get(&Unknown18, sizeof(Unknown18));
}

RawPacketClass *ActionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(44, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Action, sizeof(Action));
	raw->Add(&Unknown14, sizeof(Unknown14));
	raw->Add(&Unknown18, sizeof(Unknown18));
	return(raw);
}

int ActionPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Action) + sizeof(Unknown14) + sizeof(Unknown18));
}

int ActionPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Action=%d Target %d OptionalVar %d}", "Action", GameID, Action, Unknown14, Unknown18));
}


/*
**	Reconstruction notes (pkaction.cpp)
**
**	* Client 0x876360-0x8769F0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 45 (declared in pk_classes.h).
*/
