// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgesave.cpp. Notes at the end of the file.
#include "pkgesave.h"
#include "..\..\shared\unknown_debug.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

GameEditorSaveSectorPacketClass::GameEditorSaveSectorPacketClass()
{
}

GameEditorSaveSectorPacketClass::GameEditorSaveSectorPacketClass(RawPacketClass *)
{
}

RawPacketClass *GameEditorSaveSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30027, Get_Data_Size());
	return(raw);
}

int GameEditorSaveSectorPacketClass::Get_Data_Size()
{
	return(0);
}

int GameEditorSaveSectorPacketClass::Print(char *buffer, int)
{
	strcpy(buffer, "GameEditorSaveSector {NO DATA}");
	return(strlen(buffer));
}

GameEditorSectorSavedPacketClass::GameEditorSectorSavedPacketClass(bool success, const char *message) :
	Success(success),
	Length(0),
	Message(NULL)
{
	if (message != NULL) {
		Message = nstrdup(message);
		Length = strlen(message) + 1;
	}
}

GameEditorSectorSavedPacketClass::GameEditorSectorSavedPacketClass(RawPacketClass *raw) :
	Success(false),
	Length(0),
	Message(NULL)
{
	raw->Get(&Success, sizeof(Success));
	raw->Get(&Length, sizeof(Length));
	if (Length > 0) {
		Message = new char[Length];























		G_ASSERT(Message);
		raw->Get(Message, Length);
	}
}

GameEditorSectorSavedPacketClass::~GameEditorSectorSavedPacketClass()
{
	delete [] Message;
}

RawPacketClass *GameEditorSectorSavedPacketClass::Build()
{
	RawPacketClass *packet = new RawPacketClass(30028, Get_Data_Size());

	G_ASSERT(packet);
	packet->Add(&Success, sizeof(Success));
	packet->Add(Length);
	packet->Add(Message, Length);
	return(packet);
}

int GameEditorSectorSavedPacketClass::Get_Data_Size()
{
	return(Length + sizeof(Success) + sizeof(Length));
}

int GameEditorSectorSavedPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Success=%s,Message=%s}", "GameEditorSectorSaved", Success ? "true" : "false", Message));
}

bool GameEditorSectorSavedPacketClass::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgesave.cpp)
**
**	* The client's app\Packets\gameedit\pkgesave.cpp (its asserts' __FILE__), 0x861DB0-0x862320 (0x861D80 is
**	  the previous object's guarded static, 0xC416FC; this one's is 0x862320). Opcode 30027 written by hand
**	  (builder P5, 2026-10-27, as pkunknown_861db0.cpp, renamed under D95(c)), as 30023's (gameedit\pkgeplan.cpp);
**	  the send-only 30028 (not in the packet switch: its Process_Packet returns true here) by builder A-opus
**	  (2026-10-27): the raw constructor's G_ASSERT(Message) on line 76, Build's G_ASSERT(packet) on line 90.
*/
