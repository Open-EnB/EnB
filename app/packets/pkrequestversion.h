// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkrequestversion.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x8A2B70-0x8A2D40 rebuilt so far (opcode 69); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREQUESTVERSION_H  // NAME 3 [pkrequestversion.h]: by convention from its class RequestVersionPacketClass (thunk order) and its place in retail's object order (after pkrequesttransformchange)
#define PKREQUESTVERSION_H

#include "pk.h"

// opcode 69 (0x0045, ?): vtable 0xB0B838, constructor 0x8A2C20, 0xC bytes
class RequestVersionPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	RequestVersionPacketClass();	// 0x8A2B70
	RequestVersionPacketClass(PacketClass *raw);	// 0x8A2C20
	virtual PacketClass *Build();	// +0x04 0x8A2C60
	virtual bool Process_Packet(void *client);	// +0x08 0x867EF0 (clientstub.cpp l.83)
	virtual int Get_Data_Size();	// +0x0C 0x8A2CF0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A2D10
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8A2BB0

};

#endif
