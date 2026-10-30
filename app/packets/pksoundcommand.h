// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pksoundcommand.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A4F20-0x8A54E0 rebuilt (opcodes 106, 107; 106 written by hand,
**	builder P, 2026-10-26).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSOUNDCOMMAND_H  // NAME 1 [pksoundcommand]: provisional, pk + its first named class's name
#define PKSOUNDCOMMAND_H

#include "pk.h"

// opcode 106 (0x006A, CLIENT_SOUND): vtable 0xB0BAA8, constructor 0x8A5050, 0x1C bytes
class ClientSoundPacketClass : public PacketClass  // NAME 3: the client's Print text 'Client Sound Packet {'
{
public:
	ClientSoundPacketClass(const char *sound_name, int channel, bool queue);	// 0x8A4F20
	ClientSoundPacketClass(RawPacketClass *raw);	// 0x8A5050
	virtual ~ClientSoundPacketClass();	// +0x00 0x8A5010 / 0x8A4FE0
	virtual RawPacketClass *Build();	// +0x04 0x8A5130
	virtual bool Process_Packet(void *client);	// +0x08 0x7383D0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A5220
	virtual int Print(char *buffer, int len);	// +0x10 0x8A5240

	char *SoundName;	// +0x0C (nstrdup / new[] Length bytes)  // NAME 3: the client's Print text 'SoundName='
	int Length;	// +0x10 (the name's bytes with its 0)  // NAME 2: what it holds
	int Channel;	// +0x14  // NAME 3: the client's Print text 'Channel='
	bool Queue;	// +0x18  // NAME 3: the client's Print text 'Queue='
};

// opcode 107 (0x006B, ?): vtable 0xB0BAD8, constructor 0x8A5320, 0x14 bytes
class SoundCommandPacketClass : public PacketClass
{
public:
	SoundCommandPacketClass(int command, int channel);	// 0x8A5280
	SoundCommandPacketClass(RawPacketClass *raw);	// 0x8A5320
	virtual RawPacketClass *Build();	// +0x04 0x8A53C0
	virtual bool Process_Packet(void *client);	// +0x08 0x7384A0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A5490
	virtual int Print(char *buffer, int len);	// +0x10 0x8A54B0

	int Command;	// +0x0C  // NAME 3: the client's Print text 'Command='
	int Channel;	// +0x10  // NAME 3: the client's Print text 'Channel='
};

#endif
