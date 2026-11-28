// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\gameedit\pkgerem.h (the name by convention: see the .cpp)
**
**	The packet class of client 0x861B30-0x861D80 (opcode 30004).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGEREM_H  // NAME 3 [pkgerem.h]: by convention from its class GameEditorRemovePacketClass (thunk order) and its place in retail's object order (between pkgeplan and pkgesave)
#define PKGEREM_H

#include "..\pk.h"

// 0x7534 (30004, not in the packet switch: sent only): vtable 0xB09278, constructor 0x861BC0, 0x10 bytes
class GameEditorRemovePacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GameEditorRemovePacketClass(int game_id);	// 0x861B30
	GameEditorRemovePacketClass(PacketClass *raw);	// 0x861BC0
	virtual PacketClass *Encode();	// +0x04 0x861C50  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x861D60
	virtual int Get_Data_Size();	// +0x0C 0x861D10
	virtual int Print(char *buffer, int len);	// +0x10 0x861D30

	int GameID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'GameID='
};

#endif
