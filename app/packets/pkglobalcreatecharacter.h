// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalcreatecharacter.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x864090-0x864780 (opcode 114).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALCREATECHARACTER_H  // NAME 1 [pkglobalcreatecharacter]: provisional, pk + its first named class's name
#define PKGLOBALCREATECHARACTER_H

#include "pk.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\shipcust_desc.h"

// opcode 114 (0x0072, GLOBAL_CREATE_CHARACTER): vtable 0xB09430, constructor 0x864380, 0x2C8 bytes
class GlobalCreateCharacterPacketClass : public PacketClass  // NAME 2: Net-7's opcode name GLOBAL_CREATE_CHARACTER (the Net-7 opcode table), the client's Print text 'Global Create Character'
{
public:
	GlobalCreateCharacterPacketClass(int galaxy, int slot, const char *name, const AvatarDescClass *desc, const ShipCustDescClass *ship, unsigned char unknown);	// 0x864090 (LoginTaskClass::Unknown_48)  // NAME 1 [galaxy, slot, unknown]: what the login task passes
	GlobalCreateCharacterPacketClass(RawPacketClass *raw);	// 0x864380
	virtual ~GlobalCreateCharacterPacketClass();	// +0x00 0x864590 / 0x864350
	virtual RawPacketClass *Build();	// +0x04 0x864610
	virtual bool Process_Packet(void *client);	// +0x08 0x868880 (clientstub.cpp l.236)
	virtual int Get_Data_Size();	// +0x0C 0x864720
	virtual int Print(char *buffer, int len);	// +0x10 0x864750

	int Galaxy;	// +0x0C  // NAME 1: the login task's galaxy ID
	int Slot;	// +0x10  // NAME 1: the login task's selected slot
	int Unknown14;	// +0x14 (the value constructor's byte)  // NAME 1: neutral (D10)
	char Name[0x41];	// +0x18 (0x41 bytes on the wire; the raw constructor lowers its case)  // NAME 1: the login task's user name
	AvatarDescClass Desc;	// +0x5C  // NAME 2: what it holds
	ShipCustDescClass Ship;	// +0x168  // NAME 2: what it holds
};

#endif
