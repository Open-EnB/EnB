/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_87acd0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87ACD0-0x87DE90 rebuilt so far (opcode 170); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_87ACD0_H  // NAME 1 [pkunknown_87acd0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_87ACD0_H

#include "pk.h"
#include <string>
#include <vector>

// opcode 168 (0x00A8, ?): vtable 0xB09F30, constructor 0x87AE80, 0x68 bytes
class Packet_00A8Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00A8 (its Print says 'ChatProxyLoginPacketClass')
{
public:
	Packet_00A8Class();	// 0x87ACD0
	Packet_00A8Class(RawPacketClass *raw);	// 0x87AE80
	virtual ~Packet_00A8Class();	// +0x00 0x87AE50 / 0x87B280
	virtual RawPacketClass *Build();	// +0x04 0x87B3E0
	virtual bool Process_Packet(void *client);	// +0x08 0x868DC0 (clientstub.cpp l.320)
	virtual int Get_Data_Size();	// +0x0C 0x87B5A0
	virtual int Print(char *buffer, int len);	// +0x10 0x87B640

	int Unknown0C;	// +0x0C (4 bytes as they are)
	std::string Unknown10;	// +0x10 (a 2-byte length, then the bytes)
	std::string Unknown20;	// +0x20
	int Unknown30;	// +0x30 (network order)
	char Unknown34;	// +0x34 (1 byte)
	__int64 Unknown38;	// +0x38 (network order)
	__int64 Unknown40;	// +0x40 (network order)
	std::vector<std::string> Unknown48;	// +0x48 (a 4-byte count, then the strings)
	std::vector<std::string> Unknown58;	// +0x58 (a 4-byte count, then the strings)
};

// opcode 170 (0x00AA, ?): vtable 0xB09F90, constructor 0x87C230, 0x14 bytes
class Packet_00AAClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AA
{
public:
	Packet_00AAClass(int unknown0_c, int unknown10);	// 0x87C190
	Packet_00AAClass(RawPacketClass *raw);	// 0x87C230
	virtual RawPacketClass *Build();	// +0x04 0x87C2D0
	virtual bool Process_Packet(void *client);	// +0x08 0x868D30 (clientstub.cpp l.311)
	virtual int Get_Data_Size();	// +0x0C 0x87C3A0
	virtual int Print(char *buffer, int len);	// +0x10 0x87C3C0

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

#endif
