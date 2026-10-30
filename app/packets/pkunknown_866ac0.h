// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_866ac0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x866AC0-0x866FE0 (opcodes 119, 120).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_866AC0_H  // NAME 1 [pkunknown_866ac0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_866AC0_H

#include "pk.h"
#include <string>

// opcode 119 (0x0077, ?): vtable 0xB04170, constructor 0x866C20, 0x20 bytes
class Packet_0077Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0077 (Print: 'Global SetReferrer')
{
public:
	Packet_0077Class(const char *name);	// 0x866AC0 (LoginTaskClass::Send_Referrer)
	Packet_0077Class(RawPacketClass *raw);	// 0x866C20
	virtual ~Packet_0077Class() {}	// +0x00 0x76B470 (inline: the client's copy is tsk_login.cpp's)
	virtual RawPacketClass *Build();	// +0x04 0x866CC0
	virtual bool Process_Packet(void *client);	// +0x08 0x8695D0 (clientstub.cpp l.449)
	virtual int Get_Data_Size();	// +0x0C 0x866D80
	virtual int Print(char *buffer, int len);	// +0x10 0x866DA0

	std::string Name;	// +0x0C  // NAME 2: the client's Print text 'ReferrerName='
	int Unknown1C;	// +0x1C (0; not in the message)  // NAME 1: neutral (D10)
};

// opcode 120 (0x0078, ?): vtable 0xB09628, constructor 0x866DF0, 0x10 bytes
class Packet_0078Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0078
{
public:
	Packet_0078Class(RawPacketClass *raw);	// 0x866DF0
	virtual RawPacketClass *Build();	// +0x04 0x866ED0
	virtual bool Process_Packet(void *client);	// +0x08 0x76BB70 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x866F90
	virtual int Print(char *buffer, int len);	// +0x10 0x866FB0

	short Result;	// +0x0C  // NAME 3: the client's Print text 'Result='
	char Unknown0E[0x2];	// +0x0E
};

#endif
