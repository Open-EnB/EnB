// Earth & Beyond client (reconstruction): app\packets\pkunknown_85d4b0.cpp. Notes at the end of the file.
#include "pkunknown_85d4b0.h"
#include <stdio.h>
#include <string.h>

Packet_004DClass::Packet_004DClass(int unknown0c, const char *unknown10, __int64 unknown58) :
	Unknown0C(unknown0c),
	Unknown58(unknown58)
{
	strcpy(Unknown10, unknown10);
}

Packet_004DClass::Packet_004DClass(RawPacketClass *raw)
{
	int value;
	raw->Get(value);
	raw->Get(Unknown10, sizeof(Unknown10));
	raw->Get(Unknown58);
	Unknown0C = value;
}

RawPacketClass *Packet_004DClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(77, Get_Data_Size());
	int value = Unknown0C;
	raw->Add(value);
	raw->Add(Unknown10, sizeof(Unknown10));
	raw->Add(Unknown58);
	return(raw);
}

int Packet_004DClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10) + sizeof(Unknown58));
}

int Packet_004DClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Registration_Result"));
}


/*
**	Reconstruction notes (pkunknown_85d4b0.cpp)
**
**	* Client 0x85D4B0-0x85D780 (0x85D480 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x85D780): opcode 77, written by hand (builder P5, 2026-10-27), the value constructor first. The raw
**	  constructor reads the first int into a local (the raw pointer's dead slot) and stores it last.
**	* The file name is provisional (no assert names it).
*/
