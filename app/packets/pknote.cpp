// Earth & Beyond client (reconstruction): app\packets\pknote.cpp. Notes at the end of the file.
#include "pknote.h"
#include <stdio.h>

Packet_006CClass::Packet_006CClass(__int64 unknown10, __int64 unknown18) :
	Unknown10(unknown10),
	Unknown18(unknown18),
	Unknown20(0),
	Unknown24(0)
{
}

Packet_006CClass::Packet_006CClass(RawPacketClass *raw) :
	Unknown10(0),
	Unknown18(0),
	Unknown20(0),
	Unknown24(0)
{
	raw->Get(Unknown10);
	raw->Get(Unknown18);
}

RawPacketClass *Packet_006CClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(108, Get_Data_Size());
	raw->Add(Unknown10);
	raw->Add(Unknown18);
	return(raw);
}

int Packet_006CClass::Get_Data_Size()
{
	return(sizeof(Unknown10) + sizeof(Unknown18));
}

int Packet_006CClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Note:"));
}


/*
**	Reconstruction notes (pknote.cpp)
**
**	* Client 0x89EF20-0x89F1C0 (0x89EEF0 is the previous object's guarded static, 0xC416FC; this one's is 0x89F1C0):
**	  opcode 108, written by hand (builder P4, 2026-10-27). The value constructor comes first.
**	* The file name is provisional (no assert names it).
*/
