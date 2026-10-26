// Earth & Beyond client (reconstruction): app\packets\pknavdelete.cpp. Notes at the end of the file.
#include "pknavdelete.h"
#include <stdio.h>


NavDeletePacketClass::NavDeletePacketClass(RawPacketClass *raw) : GameID(0), Unknown10(-1)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *NavDeletePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(154, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int NavDeletePacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int NavDeletePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "NavDeletePacketClass", GameID));
}


/*
**	Reconstruction notes (pknavdelete.cpp)
**
**	* Client 0x89E8C0-0x89EEF0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 153 (declared in pk_classes.h).
*/
