// Earth & Beyond client (reconstruction): app\packets\pknotelist.cpp. Notes at the end of the file.
#include "pknotelist.h"
#include "nstrdup.h"
#include <stdio.h>

NoteContentPacketClass::NoteContentPacketClass(int unknown0c, const char *unknown10, const char *unknown54) :
	Unknown0C(unknown0c),
	Unknown54(0)
{
	Unknown_4A1590(Unknown10, sizeof(Unknown10), unknown10);
	Unknown54 = nstrdup(unknown54);
}

NoteContentPacketClass::NoteContentPacketClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown54(0)
{
	raw->Get(Unknown0C);
	raw->Get(Unknown10, sizeof(Unknown10));
	Unknown54 = raw->Get_String();
}

NoteContentPacketClass::~NoteContentPacketClass()
{
	delete [] Unknown54;
}

RawPacketClass *NoteContentPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(184, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Unknown10, sizeof(Unknown10));
	raw->Add_String(Unknown54);
	return(raw);
}

int NoteContentPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Unknown54) + sizeof(Unknown0C) + sizeof(Unknown10));
}

int NoteContentPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "NoteContent:"));
}


/*
**	Reconstruction notes (pknotelist.cpp)
**
**	* Client 0x87EA50-0x87F2F0 (0x87EA30 is the previous object's guarded static, 0xC416FC; this one's
**	  is at 0x87F2F0): NoteList 183 (a std::list of 0x4C-byte notes, not rebuilt yet), then NoteContent
**	  184, written by hand (builder P4, 2026-10-26).
**	* The file name is provisional (no assert names it).
*/
