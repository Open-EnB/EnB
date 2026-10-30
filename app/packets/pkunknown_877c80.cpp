// Earth & Beyond client (reconstruction): app\packets\pkunknown_877c80.cpp. Notes at the end of the file.
#include "pkunknown_877c80.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

Packet_0063Class::Packet_0063Class(char *text, int unknown14) :
	Unknown14(unknown14)
{
	Size = (short)(strlen(text) + 1);
	String = nstrdup(text);
}

Packet_0063Class::Packet_0063Class(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	String = new char[Size];
	raw->Get(String, Size);
	raw->Get(Unknown14);
}

Packet_0063Class::~Packet_0063Class()
{
	delete [] String;
	String = 0;
}

RawPacketClass *Packet_0063Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(99, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(String, Size);
	raw->Add(Unknown14);
	return(raw);
}

int Packet_0063Class::Get_Data_Size()
{
	return(sizeof(Size) + sizeof(Unknown14) + Size);
}

int Packet_0063Class::Print(char *buffer, int)
{
	char text[128];
	short len = Size;
	if (len >= 128) {
		len = 128;
	}
	strncpy(text, String, len);
	return(sprintf(buffer, "%s {String=%s Size=%hd}", "Broadcast_Packet", text, Size));
}

/*
**	Reconstruction notes (pkunknown_877c80.cpp)
**
**	* Client 0x877C50-0x878000 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P3, 2026-10-26), 6 of 6 exact in appfile.py
**	  match: a length-counted string (Size counts its 0), nstrdup'd by the value constructor, new[]'d by
**	  the raw one, deleted by the destructor either way; the same shape as opcode 98's.
**	* The file name is provisional (no assert names it).
*/
