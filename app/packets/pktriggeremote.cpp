// Earth & Beyond client (reconstruction): app\packets\pktriggeremote.cpp. Notes at the end of the file.
#include "pktriggeremote.h"
#include <stdio.h>

static char *EmoteNames[] = {	// 0xBB4450 (Print's Emote; the sound names 'wave.wav', ... follow it)  // NAME 2: what it holds
	"Wave",
	"Loop",
	"Dive",
	"Right Roll",
	"Left Roll",
	"Spin Right",
	"Spin Left"
};

TriggerEmotePacketClass::TriggerEmotePacketClass(int game_id, int emote) :
	GameID(game_id),
	Emote(emote)
{
}

TriggerEmotePacketClass::TriggerEmotePacketClass(RawPacketClass *raw)
{
	raw->Get(GameID);
	raw->Get(&Emote, sizeof(Emote));
}

RawPacketClass *TriggerEmotePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(161, Get_Data_Size());
	raw->Add(GameID);
	raw->Add(&Emote, sizeof(Emote));
	return(raw);
}

int TriggerEmotePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Emote));
}

int TriggerEmotePacketClass::Print(char *buffer, int)
{
	char *emote = EmoteNames[Emote];
	return(sprintf(buffer, "%s {GameID=%d Emote=%s}", "TriggerEmote", GameID, emote));
}

NotifyEmotePacketClass::NotifyEmotePacketClass(int game_id, int emote) :
	GameID(game_id),
	Emote(emote),
	Channel(-1)
{
	Channel = Unknown_14(game_id, -1);
}

NotifyEmotePacketClass::NotifyEmotePacketClass(RawPacketClass *raw) :
	Channel(-1)
{
	raw->Get(GameID);
	raw->Get(&Emote, sizeof(Emote));
}

RawPacketClass *NotifyEmotePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(162, Get_Data_Size());
	raw->Add(GameID);
	raw->Add(&Emote, sizeof(Emote));
	return(raw);
}

int NotifyEmotePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Emote));
}

int NotifyEmotePacketClass::Print(char *buffer, int)
{
	char *emote = EmoteNames[Emote];
	return(sprintf(buffer, "%s {GameID=%d Emote=%s}", "NotifyEmote", GameID, emote));
}


/*
**	Reconstruction notes (pktriggeremote.cpp)
**
**	* Client 0x886000-0x886550 (0x885FD0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x886550): TRIGGER_EMOTE 161 and NOTIFY_EMOTE 162, written by hand (builder P4, 2026-10-26).
**	  Their implicit destructors are copies (0x747F80) or this object's (0x886360).
**	* The file name is provisional (no assert names it).
*/
