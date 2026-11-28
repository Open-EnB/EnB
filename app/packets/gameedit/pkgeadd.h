// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\gameedit\pkgeadd.h (the name by convention: see the .cpp)
**
**	The packet class of client 0x85D7B0-0x85DA70 (opcode 30003).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGEADD_H  // NAME 3 [pkgeadd.h]: by convention from its class GameEditorAddPacketClass (thunk order) and its place in retail's object order (before pkgeaset)
#define PKGEADD_H

#include "..\pk.h"

// 0x7533 (30003, not in the packet switch: sent only): vtable 0xB08DF8, constructor 0x85D850, 0x18 bytes
class GameEditorAddPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GameEditorAddPacketClass(int game_id, int type, float radius);	// 0x85D7B0
	GameEditorAddPacketClass(PacketClass *raw);	// 0x85D850
	virtual PacketClass *Build();	// +0x04 0x85D910
	virtual bool Process_Packet(void *client);	// +0x08 0x85DA50
	virtual int Get_Data_Size();	// +0x0C 0x85D9F0
	virtual int Print(char *buffer, int len);	// +0x10 0x85DA10

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Type;	// +0x10  // NAME 3: the client's Print text 'Type='
	float Radius;	// +0x14  // NAME 3: the client's Print text 'Radius='
};

#endif
