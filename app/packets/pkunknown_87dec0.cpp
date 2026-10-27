// Earth & Beyond client (reconstruction): app\packets\pkunknown_87dec0.cpp. Notes at the end of the file.
#include "pkunknown_87dec0.h"
#include <stdio.h>
#include <string.h>

AvatarEmotePacketClass::AvatarEmotePacketClass(int game_id, Unknown_ChatStreamClass *stream, char type, char *target_name) :
	GameID(game_id),
	Type(type),
	ChatString(NULL),
	ChatSize(0),
	TargetName(NULL),
	TargetNameSize(0),
	Owned(false)
{
	ChatSize = stream->Unknown_04();
	ChatString = (char *)stream->Unknown04;
	if (target_name != NULL) {
		TargetNameSize = strlen(target_name) + 1;
		TargetName = target_name;
	}
}

AvatarEmotePacketClass::AvatarEmotePacketClass(RawPacketClass *raw) :
	TargetName(NULL),
	Owned(true)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Type, sizeof(Type));
	raw->Get(&ChatSize, sizeof(ChatSize));
	ChatString = new char[ChatSize];
	raw->Get(ChatString, ChatSize);
	raw->Get(&TargetNameSize, sizeof(TargetNameSize));
	if (TargetNameSize != 0) {
		TargetName = new char[TargetNameSize];
		raw->Get(TargetName, TargetNameSize);
	}
}

AvatarEmotePacketClass::~AvatarEmotePacketClass()
{
	if (Owned) {
		if (ChatString != NULL) {
			delete [] ChatString;
			ChatString = NULL;
		}
		if (TargetName != NULL) {
			delete [] TargetName;
			TargetName = NULL;
		}
	}
}

RawPacketClass *AvatarEmotePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(94, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Type, sizeof(Type));
	raw->Add(&ChatSize, sizeof(ChatSize));
	raw->Add(ChatString, ChatSize);
	raw->Add(&TargetNameSize, sizeof(TargetNameSize));
	if (TargetName != NULL) {
		raw->Add(TargetName, TargetNameSize);
	}
	return(raw);
}

int AvatarEmotePacketClass::Get_Data_Size()
{
	return(TargetNameSize + sizeof(GameID) + sizeof(Type) + sizeof(ChatSize) + sizeof(TargetNameSize) + ChatSize);
}

int AvatarEmotePacketClass::Print(char *buffer, int)
{
	Unknown_ChatStreamClass stream(ChatString, ChatSize);
	char text[512] = "";
	stream.Unknown_4C48C0(text);
	return(sprintf(buffer, "%s {GameID=%d Type=%d ChatSize=%hd ChatString=%s TargetName=%d }", "Chatstream", GameID, Type, ChatSize, text, TargetName));
}


/*
**	Reconstruction notes (pkunknown_87dec0.cpp)
**
**	* Client 0x87DEC0-0x87E3F0 (0x87DE90 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x87E3F0): opcode 94, written by hand (builder P5, 2026-10-27), the value constructor first; the shape
**	  of MESSAGE_CHATSTREAM 95's (pkmessagechatstream.cpp) with a target name. Print shows the name's
**	  pointer with %d, as the client does.
**	* The file name is provisional (no assert names it).
*/
