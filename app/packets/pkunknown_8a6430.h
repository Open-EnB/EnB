/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_8a6430.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A6430-0x8A65E0 rebuilt so far (opcode 61); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_8A6430_H  // NAME 1 [pkunknown_8a6430]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_8A6430_H

#include "pk.h"

// opcode 61 (0x003D, ?): vtable 0xB0BBD0, constructor 0x8A64C0, 0xC bytes
class Packet_003DClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x003D
{
public:
	Packet_003DClass();	// 0x8A6430
	Packet_003DClass(RawPacketClass *raw);	// 0x8A64C0
	virtual RawPacketClass *Build();	// +0x04 0x8A6500
	virtual bool Process_Packet(void *client);	// +0x08 0x867E90 (clientstub.cpp l.77)
	virtual int Get_Data_Size();	// +0x0C 0x8A6590
	virtual int Print(char *buffer, int len);	// +0x10 0x8A65B0

};

#endif
