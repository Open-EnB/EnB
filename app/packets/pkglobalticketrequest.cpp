// Earth & Beyond client (reconstruction): app\packets\pkglobalticketrequest.cpp. Notes at the end of the file.
#include "pkglobalticketrequest.h"
#include <stdio.h>


GlobalTicketRequestPacketClass::GlobalTicketRequestPacketClass(int unknown0_c)
{
	Unknown0C = unknown0_c;
}

GlobalTicketRequestPacketClass::GlobalTicketRequestPacketClass(RawPacketClass *raw)
{
	raw->Get(Unknown0C);
}

RawPacketClass *GlobalTicketRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(110, Get_Data_Size());
	raw->Add(Unknown0C);
	return(raw);
}

int GlobalTicketRequestPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C));
}

int GlobalTicketRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Global Ticket Request"));
}


/*
**	Reconstruction notes (pkglobalticketrequest.cpp)
**
**	* Client 0x867860-0x867A70 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
