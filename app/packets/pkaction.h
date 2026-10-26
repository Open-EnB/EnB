/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkaction.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x876360-0x8769F0 rebuilt so far (opcode 44); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKACTION_H  // NAME 1 [pkaction]: provisional, pk + its first named class's name
#define PKACTION_H

#include "pk.h"

// opcode 44 (0x002C, ACTION): vtable 0xB09C14, constructor 0x876400, 0x1C bytes
class ActionPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name ACTION (the Net-7 opcode table; no structure check)
{
public:
	ActionPacketClass(RawPacketClass *raw);	// 0x876400
	virtual RawPacketClass *Build();	// +0x04 0x8764C0
	virtual bool Process_Packet(void *client);	// +0x08 0x867DD0 (clientstub.cpp l.65)
	virtual int Get_Data_Size();	// +0x0C 0x8765B0
	virtual int Print(char *buffer, int len);	// +0x10 0x8765D0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Action;	// +0x10  // NAME 3: the client's Print text 'Action='
	int Unknown14;	// +0x14
	int Unknown18;	// +0x18
};

#endif
