// Earth & Beyond client (reconstruction): app\packets\pkunknown_87acd0.cpp. Notes at the end of the file.
#include "pkunknown_87acd0.h"
#include <stdio.h>


Packet_00AAClass::Packet_00AAClass(int unknown0_c, int unknown10) : Unknown0C(unknown0_c), Unknown10(unknown10)
{
}

Packet_00AAClass::Packet_00AAClass(RawPacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_00AAClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(170, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_00AAClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10));
}

int Packet_00AAClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatProxyResponsePacketClass"));
}


/*
**	Reconstruction notes (pkunknown_87acd0.cpp)
**
**	* Client 0x87ACD0-0x87DE90 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Not rebuilt yet: opcode 168, 169, 171, 172 (declared in pk_classes.h).
*/
