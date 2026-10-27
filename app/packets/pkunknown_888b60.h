/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_888b60.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x888B60-0x88BD10 rebuilt so far (opcode 174, 175); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_888B60_H  // NAME 1 [pkunknown_888b60]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_888B60_H

#include "pk.h"

// opcode 174 (0x00AE, ?): vtable 0xB0A7C4, constructor 0x889940, 0x28 bytes
class Packet_00AEClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AE (its Print says 'GroupServerRequestPacketClass')
{
public:
	Packet_00AEClass(int unknown0c, int unknown10, __int64 unknown18, __int64 unknown20);	// 0x889870
	Packet_00AEClass(RawPacketClass *raw);	// 0x889940
	virtual RawPacketClass *Build();	// +0x04 0x889A10
	virtual bool Process_Packet(void *client);	// +0x08 0x868E80 (clientstub.cpp l.332)
	virtual int Get_Data_Size();	// +0x0C 0x889AF0
	virtual int Print(char *buffer, int len);	// +0x10 0x889B10

	int Unknown0C;	// +0x0C (in network order)
	int Unknown10;	// +0x10
	__int64 Unknown18;	// +0x18 (in network order)
	__int64 Unknown20;	// +0x20 (in network order)
};

// opcode 175 (0x00AF, ?): vtable 0xB0A7F4, constructor 0x889BE0, 0x14 bytes
class Packet_00AFClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AF
{
public:
	Packet_00AFClass(int unknown0_c, int unknown10);	// 0x889B40
	Packet_00AFClass(RawPacketClass *raw);	// 0x889BE0
	virtual RawPacketClass *Build();	// +0x04 0x889C80
	virtual bool Process_Packet(void *client);	// +0x08 0x868EB0 (clientstub.cpp l.335)
	virtual int Get_Data_Size();	// +0x0C 0x889D50
	virtual int Print(char *buffer, int len);	// +0x10 0x889D70

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

#endif
