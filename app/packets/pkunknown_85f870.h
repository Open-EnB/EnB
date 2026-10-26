/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85f870.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x85F870-0x860D00 rebuilt so far (opcode 30015, 30009, 30010); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85F870_H  // NAME 1 [pkunknown_85f870]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85F870_H

#include "pk.h"

// opcode 30015 (0x753F, ?): vtable 0xB09068, constructor 0x85FE60, 0xC bytes
class Packet_753FClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753F
{
public:
	Packet_753FClass();	// 0x85FDF0
	Packet_753FClass(RawPacketClass *raw);	// 0x85FE60
	virtual RawPacketClass *Build();	// +0x04 0x85FEC0
	virtual bool Process_Packet(void *client);	// +0x08 0x868220 (clientstub.cpp l.134)
	virtual int Get_Data_Size();	// +0x0C 0x85FF50
	virtual int Print(char *buffer, int len);	// +0x10 0x85FF70

};

// opcode 30009 (0x7539, ?): vtable 0xB09098, constructor 0x860030, 0x10 bytes
class Packet_7539Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7539
{
public:
	Packet_7539Class(int game_id);	// 0x85FFA0
	Packet_7539Class(RawPacketClass *raw);	// 0x860030
	virtual RawPacketClass *Build();	// +0x04 0x8600C0
	virtual bool Process_Packet(void *client);	// +0x08 0x868250 (clientstub.cpp l.137)
	virtual int Get_Data_Size();	// +0x0C 0x860180
	virtual int Print(char *buffer, int len);	// +0x10 0x8601A0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

// opcode 30010 (0x753A, ?): vtable 0xB090C8, constructor 0x8602C0, 0x40 bytes
class Packet_753AClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753A
{
public:
	Packet_753AClass(RawPacketClass *raw);	// 0x8602C0
	virtual RawPacketClass *Build();	// +0x04 0x860360
	virtual bool Process_Packet(void *client);	// +0x08 0x868280 (clientstub.cpp l.140)
	virtual int Get_Data_Size();	// +0x0C 0x860430
	virtual int Print(char *buffer, int len);	// +0x10 0x860450

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Unknown10[48];	// +0x10
};

#endif
