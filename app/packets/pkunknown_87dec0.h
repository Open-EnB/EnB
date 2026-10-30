/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_87dec0.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x87DEC0-0x87E3F0 (opcode 94).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_87DEC0_H  // NAME 1 [pkunknown_87dec0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_87DEC0_H

#include "pk.h"
#include "pkmessagechatstream.h"

// opcode 94 (0x005E, AVATAR_EMOTE): vtable 0xB0A020, constructor 0x87E010, 0x24 bytes
class AvatarEmotePacketClass : public PacketClass  // NAME 2: Net-7's opcode name AVATAR_EMOTE (the Net-7 opcode table; no structure check)
{
public:
	AvatarEmotePacketClass(int game_id, Unknown_ChatStreamClass *stream, char type, char *target_name);	// 0x87DEC0 (the stream's bytes and the name are the caller's, not copied)
	AvatarEmotePacketClass(RawPacketClass *raw);	// 0x87E010
	virtual ~AvatarEmotePacketClass();	// +0x00 0x87E130 / 0x87DFE0
	virtual RawPacketClass *Build();	// +0x04 0x87E1A0
	virtual bool Process_Packet(void *client);	// +0x08 0x8685E0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87E2C0
	virtual int Print(char *buffer, int len);	// +0x10 0x87E2E0
	virtual int Get_Channel() { return(-2); }	// +0x18 0x87DFC0
	virtual int Get_Priority() { return(0); }	// +0x1C 0x87DFA0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Type;	// +0x10  // NAME 3: the client's Print text 'Type='
	char *ChatString;	// +0x14 (ChatSize bytes)  // NAME 3: the client's Print text 'ChatString='
	short ChatSize;	// +0x18  // NAME 3: the client's Print text 'ChatSize='
	char *TargetName;	// +0x1C (TargetNameSize bytes, the name and its 0; printed as %d)  // NAME 3: the client's Print text 'TargetName='
	short TargetNameSize;	// +0x20  // NAME 2: what it holds
	bool Owned;	// +0x22 (the strings new[]'d by the packet: the destructor deletes them)  // NAME 2: what it means, as MessageStringPacketClass's
};

#endif
