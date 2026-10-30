// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_87a940.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87A940-0x87ACA0 rebuilt so far (opcode 167); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_87A940_H  // NAME 1 [pkunknown_87a940]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_87A940_H

#include "pk.h"

// opcode 167 (0x00A7, ?): vtable 0xB09F00, constructor 0x87A9E0, 0x30 bytes
class Packet_00A7Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00A7
{
public:
	Packet_00A7Class();	// 0x87A940
	Packet_00A7Class(RawPacketClass *raw);	// 0x87A9E0
	virtual RawPacketClass *Build();	// +0x04 0x87AB60
	virtual bool Process_Packet(void *client);	// +0x08 0x868CD0 (clientstub.cpp l.305)
	virtual int Get_Data_Size();	// +0x0C 0x87AC30
	virtual int Print(char *buffer, int len);	// +0x10 0x87AC70

	int Unknown0C;	// +0x0C
	std::string Unknown10;	// +0x10
	std::string Unknown20;	// +0x20
};

#endif
