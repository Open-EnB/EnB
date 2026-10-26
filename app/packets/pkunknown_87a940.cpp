// Earth & Beyond client (reconstruction): app\packets\pkunknown_87a940.cpp. Notes at the end of the file.
#include "pkunknown_87a940.h"
#include <stdio.h>


Packet_00A7Class::Packet_00A7Class() : Unknown0C(0)
{
}

Packet_00A7Class::Packet_00A7Class(RawPacketClass *raw) : Unknown0C(0)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10);
	raw->Get(&Unknown20);
}

RawPacketClass *Packet_00A7Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(167, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(Unknown10);
	raw->Add(Unknown20);
	return(raw);
}

int Packet_00A7Class::Get_Data_Size()
{
	return(sizeof(Unknown0C) + RawPacketClass::Get_Size(Unknown10) + RawPacketClass::Get_Size(Unknown20));
}

int Packet_00A7Class::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatBasicRequestPacketClass"));
}


/*
**	Reconstruction notes (pkunknown_87a940.cpp)
**
**	* Client 0x87A940-0x87ACA0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
