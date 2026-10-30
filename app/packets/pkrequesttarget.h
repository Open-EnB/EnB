/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkrequesttarget.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A7EA0-0x8A85E0 rebuilt so far (opcode 23, 24, 25); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREQUESTTARGET_H  // NAME 1 [pkrequesttarget]: provisional, pk + its first named class's name
#define PKREQUESTTARGET_H

#include "pk.h"

// opcode 23 (0x0017, REQUEST_TARGET): vtable 0xB0BCF0, constructor 0x8A7F40, 0x14 bytes
class RequestTargetPacketClass : public PacketClass  // NAME 2: Net-7's opcode name REQUEST_TARGET (the Net-7 opcode table; no structure check)
{
public:
	RequestTargetPacketClass(int game_id, int target_id);	// 0x8A7EA0
	RequestTargetPacketClass(RawPacketClass *raw);	// 0x8A7F40
	virtual RawPacketClass *Build();	// +0x04 0x8A7FE0
	virtual bool Process_Packet(void *client);	// +0x08 0x867CE0 (clientstub.cpp l.50)
	virtual int Get_Data_Size();	// +0x0C 0x8A80B0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A80D0
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8A7EF0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int TargetID;	// +0x10  // NAME 3: the client's Print text 'TargetID='
};

// opcode 24 (0x0018, REQUEST_TARGETS_TARGET): vtable 0xB0BD20, constructor 0x8A81B0, 0x14 bytes
class RequestTargetsTargetPacketClass : public PacketClass  // NAME 2: Net-7's opcode name REQUEST_TARGETS_TARGET (the Net-7 opcode table; no structure check)
{
public:
	RequestTargetsTargetPacketClass(int game_id, int target_id);	// 0x8A8110
	RequestTargetsTargetPacketClass(RawPacketClass *raw);	// 0x8A81B0
	virtual RawPacketClass *Build();	// +0x04 0x8A8250
	virtual bool Process_Packet(void *client);	// +0x08 0x8689D0 (clientstub.cpp l.257)
	virtual int Get_Data_Size();	// +0x0C 0x8A8320
	virtual int Print(char *buffer, int len);	// +0x10 0x8A8340
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8A8160

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int TargetID;	// +0x10  // NAME 3: the client's Print text 'TargetID='
};

// opcode 25 (0x0019, SET_TARGET): vtable 0xB0BD50, constructor 0x8A8420, 0x14 bytes
class SetTargetPacketClass : public PacketClass
{
public:
	SetTargetPacketClass(int target_id, int unknown10);	// 0x8A8380
	SetTargetPacketClass(RawPacketClass *raw);	// 0x8A8420
	virtual RawPacketClass *Build();	// +0x04 0x8A84C0
	virtual bool Process_Packet(void *client);	// +0x08 0x7363E0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A8590
	virtual int Print(char *buffer, int len);	// +0x10 0x8A85B0

	int TargetID;	// +0x0C  // NAME 3: the client's Print text 'TargetID='
	int Unknown10;	// +0x10
};

#endif
