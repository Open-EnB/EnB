// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalplayerstatus.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x8667A0-0x866A90 rebuilt so far (opcode 116); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALPLAYERSTATUS_H  // NAME 3 [pkglobalplayerstatus.h]: by convention from its class GlobalPlayerStatusPacketClass (thunk order) and its place in retail's object order (after pkgloballogin)
#define PKGLOBALPLAYERSTATUS_H

#include "pk.h"

// opcode 116 (0x0074, ?): vtable 0xB095F4, constructor 0x866870, 0x68 bytes
class GlobalPlayerStatusPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GlobalPlayerStatusPacketClass(int unknown0c, const char *unknown10, __int64 unknown58, int unknown60, int unknown64);	// 0x8667A0  // NAME 2: the stores (no caller left)
	GlobalPlayerStatusPacketClass(PacketClass *raw);	// 0x866870
	virtual PacketClass *Encode();	// +0x04 0x866930  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x868910 (clientstub.cpp l.245)
	virtual int Get_Data_Size();	// +0x0C 0x866A40
	virtual int Print(char *buffer, int len);	// +0x10 0x866A60
	virtual int Get_Type();	// +0x28 0x866A20  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Unknown0C;	// +0x0C
	char Unknown10[65];	// +0x10
	char Unknown51[0x7];	// +0x51
	__int64 Unknown58;	// +0x58
	int Unknown60;	// +0x60
	int Unknown64;	// +0x64
};

#endif
