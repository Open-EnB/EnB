// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85d7b0.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x85D7B0-0x85DA70 (opcode 30003).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85D7B0_H  // NAME 1 [pkunknown_85d7b0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85D7B0_H

#include "pk.h"

// 0x7533 (30003, not in the packet switch: sent only): vtable 0xB08DF8, constructor 0x85D850, 0x18 bytes
class Packet_7533Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7533 (Print 'GameEditorAdd')
{
public:
	Packet_7533Class(int game_id, int type, float radius);	// 0x85D7B0
	Packet_7533Class(RawPacketClass *raw);	// 0x85D850
	virtual RawPacketClass *Build();	// +0x04 0x85D910
	virtual bool Process_Packet(void *client);	// +0x08 0x85DA50
	virtual int Get_Data_Size();	// +0x0C 0x85D9F0
	virtual int Print(char *buffer, int len);	// +0x10 0x85DA10

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Type;	// +0x10  // NAME 3: the client's Print text 'Type='
	float Radius;	// +0x14  // NAME 3: the client's Print text 'Radius='
};

#endif
