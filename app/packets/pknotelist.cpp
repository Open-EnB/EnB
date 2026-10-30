// Earth & Beyond client (reconstruction): app\packets\pknotelist.cpp. Notes at the end of the file.
#include "pknotelist.h"
#include "nstrdup.h"
#include <stdio.h>

NoteListPacketClass::NoteListPacketClass()
{
}

NoteListPacketClass::NoteListPacketClass(RawPacketClass *raw)
{
	while (raw->Data.Get_Size() - raw->Data.Get_Pos() >= sizeof(NoteListEntryStruct)) {
		NoteListEntryStruct note;
		raw->Get(note.Unknown00);
		raw->Get(note.Unknown04);
		raw->Get(note.Unknown08, sizeof(note.Unknown08));
		Notes.push_back(note);
	}
}

NoteListPacketClass::~NoteListPacketClass()
{
}

RawPacketClass *NoteListPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(183, Get_Data_Size());
	for (std::list<NoteListEntryStruct>::iterator it = Notes.begin(); it != Notes.end(); it++) {
		raw->Add(it->Unknown00);
		raw->Add(it->Unknown04);
		raw->Add(it->Unknown08, sizeof(it->Unknown08));
	}
	return(raw);
}

int NoteListPacketClass::Get_Data_Size()
{
	return(Notes.size() * sizeof(NoteListEntryStruct));
}

int NoteListPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "NoteList:"));
}

bool NoteListPacketClass::Add_Note(int unknown00, int unknown04, const char *unknown08)
{
	if (Notes.size() > 15) {
		return(false);
	}
	NoteListEntryStruct note;
	note.Unknown00 = unknown00;
	note.Unknown04 = unknown04;
	Unknown_4A1590(note.Unknown08, sizeof(note.Unknown08), unknown08);
	Notes.push_back(note);
	return(true);
}

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
**	  is at 0x87F2F0): NoteList 183 (a std::list of 0x4C-byte notes:
**	  builder P6, 2026-10-27), then NoteContent 184 (builder P4, 2026-10-26), written by hand.
**	* The file name is provisional (no assert names it).
*/
