// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_861b30.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x861B30-0x861D80 (opcode 30004).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_861B30_H  // NAME 1 [pkunknown_861b30]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_861B30_H

#include "pk.h"

// 0x7534 (30004, not in the packet switch: sent only): vtable 0xB09278, constructor 0x861BC0, 0x10 bytes
class Packet_7534Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7534 (Print 'GameEditorRemove')
{
public:
	Packet_7534Class(int game_id);	// 0x861B30
	Packet_7534Class(RawPacketClass *raw);	// 0x861BC0
	virtual RawPacketClass *Build();	// +0x04 0x861C50
	virtual bool Process_Packet(void *client);	// +0x08 0x861D60
	virtual int Get_Data_Size();	// +0x0C 0x861D10
	virtual int Print(char *buffer, int len);	// +0x10 0x861D30

	int GameID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'GameID='
};

#endif
