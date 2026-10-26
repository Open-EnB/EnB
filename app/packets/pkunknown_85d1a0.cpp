// Earth & Beyond client (reconstruction): app\packets\pkunknown_85d1a0.cpp. Notes at the end of the file.
#include "pkunknown_85d1a0.h"
#include <stdio.h>


Packet_0049Class::Packet_0049Class(RawPacketClass *raw)
{
	raw->Get(Unknown10);
	raw->Get(Unknown18);
	raw->Get(Unknown1C, sizeof(Unknown1C));
	raw->Get(Unknown60);
}

RawPacketClass *Packet_0049Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(73, Get_Data_Size());
	raw->Add(Unknown10);
	raw->Add(Unknown18);
	raw->Add(Unknown1C, sizeof(Unknown1C));
	raw->Add(Unknown60);
	return(raw);
}

int Packet_0049Class::Get_Data_Size()
{
	return(sizeof(Unknown10) + sizeof(Unknown18) + sizeof(Unknown1C) + sizeof(Unknown60));
}

int Packet_0049Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Delete Character"));
}


/*
**	Reconstruction notes (pkunknown_85d1a0.cpp)
**
**	* Client 0x85D1A0-0x85D480 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
