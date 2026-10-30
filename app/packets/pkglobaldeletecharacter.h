// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobaldeletecharacter.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8647B0-0x8649E0 (opcode 113).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALDELETECHARACTER_H  // NAME 1 [pkglobaldeletecharacter]: provisional, pk + its first named class's name
#define PKGLOBALDELETECHARACTER_H

#include "pk.h"

// opcode 113 (0x0071, GLOBAL_DELETE_CHARACTER): vtable 0xB09460, constructor 0x864820, 0x14 bytes
class GlobalDeleteCharacterPacketClass : public PacketClass  // NAME 2: Net-7's opcode name GLOBAL_DELETE_CHARACTER (the Net-7 opcode table; no structure check)
{
public:
	GlobalDeleteCharacterPacketClass(int slot);	// 0x8647B0 (LoginTaskClass's delete step)  // NAME 1 [slot]: the login task's selected slot
	GlobalDeleteCharacterPacketClass(RawPacketClass *raw);	// 0x864820
	virtual RawPacketClass *Build();	// +0x04 0x8648D0
	virtual bool Process_Packet(void *client);	// +0x08 0x8688B0 (clientstub.cpp l.239)
	virtual int Get_Data_Size();	// +0x0C 0x864990
	virtual int Print(char *buffer, int len);	// +0x10 0x8649B0

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10 (0; not in the message)  // NAME 1: neutral (D10)
};

#endif
