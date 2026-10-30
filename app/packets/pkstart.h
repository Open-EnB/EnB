/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkstart.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A6610-0x8A6A50 rebuilt so far (opcode 5, 6); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSTART_H  // NAME 1 [pkstart]: provisional, pk + its first named class's name
#define PKSTART_H

#include "pk.h"

// opcode 5 (0x0005, START): vtable 0xB0BC00, constructor 0x8A66A0, 0x10 bytes
class StartPacketClass : public PacketClass
{
public:
	StartPacketClass(RawPacketClass *raw);	// 0x8A66A0
	virtual RawPacketClass *Build();	// +0x04 0x8A6730
	virtual bool Process_Packet(void *client);	// +0x08 0x733440 (cpacket.cpp, register-only residue)
	virtual int Get_Data_Size();	// +0x0C 0x8A67F0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A6810

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
};

// opcode 6 (0x0006, START_ACK): vtable 0xB0BC30, constructor 0x8A68B0, 0x10 bytes
class StartAckPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name START_ACK (the Net-7 opcode table; no structure check)
{
public:
	StartAckPacketClass(int player_id);	// 0x8A6840
	StartAckPacketClass(RawPacketClass *raw);	// 0x8A68B0
	virtual RawPacketClass *Build();	// +0x04 0x8A6940
	virtual bool Process_Packet(void *client);	// +0x08 0x867B90 (clientstub.cpp l.29)
	virtual int Get_Data_Size();	// +0x0C 0x8A6A00
	virtual int Print(char *buffer, int len);	// +0x10 0x8A6A20

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
};

#endif
