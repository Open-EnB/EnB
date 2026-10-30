// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_867010.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x867010-0x8674F0 rebuilt so far (opcode 179); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_867010_H  // NAME 1 [pkunknown_867010]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_867010_H

#include "pk.h"

// opcode 179 (0x00B3, ?): vtable 0xB09658, constructor 0x867250, 0x20 bytes
class Packet_00B3Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00B3
{
public:
	Packet_00B3Class(RawPacketClass *raw);	// 0x867250
	virtual RawPacketClass *Build();	// +0x04 0x8673C0
	virtual bool Process_Packet(void *client);	// +0x08 0x868BB0 (clientstub.cpp l.287)
	virtual int Get_Data_Size();	// +0x0C 0x867490
	virtual int Print(char *buffer, int len);	// +0x10 0x8674B0
	virtual int Get_Channel() { return(-1); }	// +0x18 0x867190
	virtual int Get_Priority() { return(0); }	// +0x1C 0x867170

	std::string Name;	// +0x0C  // NAME 3: the client's Print text 'Name='
	int Unknown1C;	// +0x1C
};

#endif
