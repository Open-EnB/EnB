// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_861760.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x861760-0x861B00 (opcode 30002).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_861760_H  // NAME 1 [pkunknown_861760]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_861760_H

#include "pk.h"
#include "matrix3d.h"

// 0x7532 (30002, not in the packet switch: sent only): vtable 0xB09248, constructor 0x861830, 0x40 bytes
class Packet_7532Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7532 (Print 'GameEditorPositionalUpdate')
{
public:
	Packet_7532Class(int game_id, const Matrix3D &transform);	// 0x861760
	Packet_7532Class(RawPacketClass *raw);	// 0x861830 (a rotation quaternion and a position in the message)
	virtual RawPacketClass *Build();	// +0x04 0x861980
	virtual bool Process_Packet(void *client);	// +0x08 0x861AE0
	virtual int Get_Data_Size();	// +0x0C 0x861A90
	virtual int Print(char *buffer, int len);	// +0x10 0x861AB0

	int GameID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'GameID='
	Matrix3D Transform;	// +0x10  // NAME 2: what it holds
};

#endif
