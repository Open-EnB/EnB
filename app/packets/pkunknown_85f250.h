/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85f250.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x85F250-0x85F840 rebuilt so far (opcode 30000, 30001); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85F250_H  // NAME 1 [pkunknown_85f250]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85F250_H

#include "pk.h"

// opcode 30000 (0x7530, ?): vtable 0xB08F78, constructor 0x85F2C0, 0x10 bytes
class Packet_7530Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7530
{
public:
	Packet_7530Class(int editor_protocol_version);	// 0x85F250
	Packet_7530Class(RawPacketClass *raw);	// 0x85F2C0
	virtual RawPacketClass *Build();	// +0x04 0x85F370
	virtual bool Process_Packet(void *client);	// +0x08 0x867F20 (clientstub.cpp l.86)
	virtual int Get_Data_Size();	// +0x0C 0x85F430
	virtual int Print(char *buffer, int len);	// +0x10 0x85F450

	int EditorProtocolVersion;	// +0x0C  // NAME 3: the client's Print text 'EditorProtocolVersion='
};

// opcode 30001 (0x7531, ?): vtable 0xB08FA8, constructor 0x85F480, 0xC bytes
class Packet_7531Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7531
{
public:
	Packet_7531Class(RawPacketClass *raw);	// 0x85F480
	virtual RawPacketClass *Build();	// +0x04 0x85F510
	virtual bool Process_Packet(void *client);	// +0x08 0x868190 (clientstub.cpp l.125)
	virtual int Get_Data_Size();	// +0x0C 0x85F5A0
	virtual int Print(char *buffer, int len);	// +0x10 0x85F5C0

};

#endif
