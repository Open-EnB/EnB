// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeclon.cpp. Notes at the end of the file.
#include "pkgeclon.h"
#include "..\..\shared\unknown_debug.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

Packet_7545Class::Packet_7545Class(int game_id, const char *new_name, const Matrix3D &transform) :
	GameID(game_id),
	Transform(transform)
{


	G_ASSERT(new_name);
	NewNameSize = strlen(new_name);
	NewName = nstrdup(new_name);
}

Packet_7545Class::Packet_7545Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Transform, sizeof(Transform));
	raw->Get(&NewNameSize, sizeof(NewNameSize));
	NewName = new char[NewNameSize];
	raw->Get(NewName, NewNameSize);
}

RawPacketClass *Packet_7545Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30021, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Transform, sizeof(Transform));
	raw->Add(&NewNameSize, sizeof(NewNameSize));
	raw->Add(NewName, NewNameSize);
	return(raw);
}

int Packet_7545Class::Get_Data_Size()
{
	return(NewNameSize + sizeof(GameID) + sizeof(Transform) + sizeof(NewNameSize));
}

int Packet_7545Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d, NewName=%s}", "GameEditorClone", GameID, NewName));
}

Packet_7546Class::Packet_7546Class(int original_game_id, int clone_game_id, const char *error_msg) :
	CloneSuccess(true),
	OriginalGameID(original_game_id),
	CloneGameID(clone_game_id),
	Size(0),
	ErrorMsg(NULL)
{
	if (error_msg != NULL) {
		CloneSuccess = false;
		ErrorMsg = nstrdup(error_msg);
		Size = strlen(error_msg) + 1;
	}
}

Packet_7546Class::Packet_7546Class(RawPacketClass *raw) :
	CloneSuccess(true),
	OriginalGameID(-1),
	CloneGameID(-1),
	Size(0),
	ErrorMsg(NULL)
{
	raw->Get(&CloneSuccess, sizeof(CloneSuccess));
	raw->Get(&OriginalGameID, sizeof(OriginalGameID));
	raw->Get(&CloneGameID, sizeof(CloneGameID));
	raw->Get(&Size, sizeof(Size));
	if (Size > 0) {
		ErrorMsg = new char[Size];















		G_ASSERT(ErrorMsg);
		raw->Get(ErrorMsg, Size);
	}
}

Packet_7546Class::~Packet_7546Class()
{
	if (ErrorMsg != NULL) {
		delete [] ErrorMsg;
	}
}

RawPacketClass *Packet_7546Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30022, Get_Data_Size());
	raw->Add(&CloneSuccess, sizeof(CloneSuccess));
	raw->Add(&OriginalGameID, sizeof(OriginalGameID));
	raw->Add(&CloneGameID, sizeof(CloneGameID));
	raw->Add(&Size, sizeof(Size));
	if (Size > 0) {
		raw->Add(ErrorMsg, Size);
	} else {
		char zero = 0;
		raw->Add(&zero, 1);
	}
	return(raw);
}

int Packet_7546Class::Get_Data_Size()
{
	int size = Size;
	if (size <= 0) {
		size = 1;
	}
	return(size + 13);
}

int Packet_7546Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {CloneSuccess=%s, OriginalGameID=%d, CloneGameID=%d, Size=%d, ErrorMsg=%s}", "GameEditorCloneResponse",
		CloneSuccess ? "true" : "false", OriginalGameID, CloneGameID, Size, ErrorMsg ? ErrorMsg : "NULL"));
}

bool Packet_7546Class::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgeclon.cpp)
**
**	* The client's app\Packets\gameedit\pkgeclon.cpp (its asserts' __FILE__), 0x85DE30-0x85E610 (0x85DE00 is
**	  the previous object's guarded static, 0xC416FC; this one's is 0x85E610), written by hand (builder P5,
**	  2026-10-27): 30021 'GameEditorClone' (G_ASSERT(new_name) on line 14; NewName is never deleted) and
**	  the send-only 30022 'GameEditorCloneResponse' (not in the packet switch: its Process_Packet returns
**	  true here; G_ASSERT(ErrorMsg) on line 90; an empty message is sent as one 0 byte).
*/
