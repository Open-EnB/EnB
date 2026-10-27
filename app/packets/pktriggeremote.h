/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pktriggeremote.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x886000-0x886550 (opcodes 161 and 162).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKTRIGGEREMOTE_H  // NAME 1 [pktriggeremote]: provisional, pk + its first class's name
#define PKTRIGGEREMOTE_H

#include "pk.h"

// opcode 161 (0x00A1, TRIGGER_EMOTE): vtable 0xB0A52C, constructor 0x8860A0, 0x14 bytes
class TriggerEmotePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name TRIGGER_EMOTE (the Net-7 opcode table; no structure check)
{
public:
	TriggerEmotePacketClass(int game_id, int emote);	// 0x886000
	TriggerEmotePacketClass(RawPacketClass *raw);	// 0x8860A0
	virtual RawPacketClass *Build();	// +0x04 0x886140
	virtual bool Process_Packet(void *client);	// +0x08 0x868C40 (clientstub.cpp l.296)
	virtual int Get_Data_Size();	// +0x0C 0x886210
	virtual int Print(char *buffer, int len);	// +0x10 0x886230
	virtual int Get_Priority() { return(0); }	// +0x1C 0x886050

	int GameID;	// +0x0C (network order)  // NAME 3: the client's Print text 'GameID='
	int Emote;	// +0x10 (EmoteNames' index)  // NAME 3: the client's Print text 'Emote='
};

// opcode 162 (0x00A2, NOTIFY_EMOTE): vtable 0xB0A55C, constructor 0x886380, 0x18 bytes
class NotifyEmotePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name NOTIFY_EMOTE (the Net-7 opcode table; no structure check)
{
public:
	NotifyEmotePacketClass(int game_id, int emote);	// 0x886270
	NotifyEmotePacketClass(RawPacketClass *raw);	// 0x886380
	virtual RawPacketClass *Build();	// +0x04 0x886420
	virtual bool Process_Packet(void *client);	// +0x08 0x73A8E0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8864F0
	virtual int Print(char *buffer, int len);	// +0x10 0x886510
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x886310

	int GameID;	// +0x0C (network order)  // NAME 3: the client's Print text 'GameID='
	int Emote;	// +0x10 (EmoteNames' index)  // NAME 3: the client's Print text 'Emote='
	int Channel;	// +0x14 (PacketClass::Unknown_14 of the game id; -1 when received)  // NAME 2: Get_Channel returns it
};

#endif
