/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobaldeletecharacter.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8647B0-0x8649E0 rebuilt so far (opcode 113); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALDELETECHARACTER_H  // NAME 1 [pkglobaldeletecharacter]: provisional, pk + its first named class's name
#define PKGLOBALDELETECHARACTER_H

#include "pk.h"

// opcode 113 (0x0071, GLOBAL_DELETE_CHARACTER): vtable 0xB09460, constructor 0x864820, 0x14 bytes
class GlobalDeleteCharacterPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name GLOBAL_DELETE_CHARACTER (the Net-7 opcode table; no structure check)
{
public:
	GlobalDeleteCharacterPacketClass(RawPacketClass *raw);	// 0x864820
	virtual RawPacketClass *Build();	// +0x04 0x8648D0
	virtual bool Process_Packet(void *client);	// +0x08 0x8688B0 (clientstub.cpp l.239)
	virtual int Get_Data_Size();	// +0x0C 0x864990
	virtual int Print(char *buffer, int len);	// +0x10 0x8649B0

	int Unknown0C;	// +0x0C
	char Unknown10[0x4];	// +0x10
};

#endif
