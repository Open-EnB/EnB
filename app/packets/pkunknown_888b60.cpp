// Earth & Beyond client (reconstruction): app\packets\pkunknown_888b60.cpp. Notes at the end of the file.
#include "pkunknown_888b60.h"
#include <stdio.h>


Packet_00AFClass::Packet_00AFClass(int unknown0_c, int unknown10) : Unknown0C(unknown0_c), Unknown10(unknown10)
{
}

Packet_00AFClass::Packet_00AFClass(RawPacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(Unknown0C);
	raw->Get(&Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_00AFClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(175, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(&Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_00AFClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10));
}

int Packet_00AFClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerResponsePacketClass"));
}


/*
**	Reconstruction notes (pkunknown_888b60.cpp)
**
**	* Client 0x888B60-0x88BD10 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Not rebuilt yet: opcode 173, 174, 176, 177 (declared in pk_classes.h).
*/
