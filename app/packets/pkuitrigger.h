/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkuitrigger.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AB800-0x8ABA50 rebuilt so far (opcode 101); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUITRIGGER_H  // NAME 1 [pkuitrigger]: provisional, pk + its first named class's name
#define PKUITRIGGER_H

#include "pk.h"

// opcode 101 (0x0065, UI_TRIGGER): vtable 0xB0C078, constructor 0x8AB880, 0x14 bytes
class UiTriggerPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name UI_TRIGGER (the Net-7 opcode table; no structure check)
{
public:
	UiTriggerPacketClass(int id, int action);	// 0x8AB800
	UiTriggerPacketClass(RawPacketClass *raw);	// 0x8AB880
	virtual RawPacketClass *Build();	// +0x04 0x8AB920
	virtual bool Process_Packet(void *client);	// +0x08 0x868820 (clientstub.cpp l.230)
	virtual int Get_Data_Size();	// +0x0C 0x8AB9F0
	virtual int Print(char *buffer, int len);	// +0x10 0x8ABA10

	int ID;	// +0x0C  // NAME 3: the client's Print text 'ID='
	int Action;	// +0x10  // NAME 3: the client's Print text 'Action='
};

#endif
