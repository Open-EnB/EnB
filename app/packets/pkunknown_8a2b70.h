// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_8a2b70.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A2B70-0x8A2D40 rebuilt so far (opcode 69); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_8A2B70_H  // NAME 1 [pkunknown_8a2b70]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_8A2B70_H

#include "pk.h"

// opcode 69 (0x0045, ?): vtable 0xB0B838, constructor 0x8A2C20, 0xC bytes
class Packet_0045Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0045
{
public:
	Packet_0045Class();	// 0x8A2B70
	Packet_0045Class(RawPacketClass *raw);	// 0x8A2C20
	virtual RawPacketClass *Build();	// +0x04 0x8A2C60
	virtual bool Process_Packet(void *client);	// +0x08 0x867EF0 (clientstub.cpp l.83)
	virtual int Get_Data_Size();	// +0x0C 0x8A2CF0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A2D10
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8A2BB0

};

#endif
