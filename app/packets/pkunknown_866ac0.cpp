// Earth & Beyond client (reconstruction): app\packets\pkunknown_866ac0.cpp. Notes at the end of the file.
#include "pkunknown_866ac0.h"
#include <stdio.h>


Packet_0078Class::Packet_0078Class(RawPacketClass *raw)
{
	raw->Get(Result);
}

RawPacketClass *Packet_0078Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(120, Get_Data_Size());
	raw->Add(Result);
	return(raw);
}

int Packet_0078Class::Get_Data_Size()
{
	return(sizeof(Result));
}

int Packet_0078Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s : Result=%d", "Global SetReferrerResponse", Result));
}


/*
**	Reconstruction notes (pkunknown_866ac0.cpp)
**
**	* Client 0x866AC0-0x866FE0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Not rebuilt yet: opcode 119 (declared in pk_classes.h).
*/
