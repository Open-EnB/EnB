// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalhello.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x865020-0x8653C0 rebuilt so far (opcode 115); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALHELLO_H  // NAME 3 [pkglobalhello.h]: by convention from its class GlobalHelloPacketClass (thunk order) and its place in retail's object order (between pkglobalerror and pkgloballock)
#define PKGLOBALHELLO_H

#include "pk.h"

// opcode 115 (0x0073, ?): vtable 0xB094C0, constructor 0x865150, 0x68 bytes
class GlobalHelloPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GlobalHelloPacketClass(int unknown0c, int unknown10, int unknown14, const char *unknown18, int unknown60, int unknown5c, bool unknown64);	// 0x865020  // NAME 2: the stores (no caller left)
	GlobalHelloPacketClass(PacketClass *raw);	// 0x865150
	virtual PacketClass *Encode();	// +0x04 0x865230  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x8688E0 (clientstub.cpp l.242)
	virtual int Get_Data_Size();	// +0x0C 0x865370
	virtual int Print(char *buffer, int len);	// +0x10 0x865390
	virtual int Get_Type();	// +0x28 0x865350  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
	int Unknown14;	// +0x14
	char Unknown18[65];	// +0x18
	char Unknown59[0x3];	// +0x59
	int Unknown5C;	// +0x5C
	int Unknown60;	// +0x60
	short Unknown64;	// +0x64
	char Unknown66[0x2];	// +0x66
};

#endif
