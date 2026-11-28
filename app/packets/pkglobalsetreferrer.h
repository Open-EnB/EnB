// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalsetreferrer.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x866AC0-0x866FE0 (opcodes 119, 120).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALSETREFERRER_H  // NAME 3 [pkglobalsetreferrer.h]: by convention from its class GlobalSetReferrer / Response (thunk order) and its place in retail's object order (before pkglobalticket)
#define PKGLOBALSETREFERRER_H

#include "pk.h"
#include <string>

// opcode 119 (0x0077, ?): vtable 0xB04170, constructor 0x866C20, 0x20 bytes
class GlobalSetReferrerPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GlobalSetReferrerPacketClass(const char *name);	// 0x866AC0 (LoginTaskClass::Send_Referrer)
	GlobalSetReferrerPacketClass(PacketClass *raw);	// 0x866C20
	virtual ~GlobalSetReferrerPacketClass() {}	// +0x00 0x76B470 (inline: the client's copy is tsk_login.cpp's)
	virtual PacketClass *Encode();	// +0x04 0x866CC0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x8695D0 (clientstub.cpp l.449)
	virtual int Get_Data_Size();	// +0x0C 0x866D80
	virtual int Print(char *buffer, int len);	// +0x10 0x866DA0

	std::string Name;	// +0x0C  // NAME 2: the client's Print text 'ReferrerName='
	int Unknown1C;	// +0x1C (0; not in the message)  // NAME 1: neutral (D10)
};

// opcode 120 (0x0078, ?): vtable 0xB09628, constructor 0x866DF0, 0x10 bytes
class GlobalSetReferrerResponsePacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GlobalSetReferrerResponsePacketClass(PacketClass *raw);	// 0x866DF0
	virtual PacketClass *Encode();	// +0x04 0x866ED0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x76BB70 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x866F90
	virtual int Print(char *buffer, int len);	// +0x10 0x866FB0

	short Result;	// +0x0C  // NAME 3: the client's Print text 'Result='
	char Unknown0E[0x2];	// +0x0E
};

#endif
