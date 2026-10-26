/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalticketrequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x867860-0x867A70 rebuilt so far (opcode 110); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALTICKETREQUEST_H  // NAME 1 [pkglobalticketrequest]: provisional, pk + its first named class's name
#define PKGLOBALTICKETREQUEST_H

#include "pk.h"

// opcode 110 (0x006E, GLOBAL_TICKET_REQUEST): vtable 0xB096B8, constructor 0x8678D0, 0x10 bytes
class GlobalTicketRequestPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name GLOBAL_TICKET_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	GlobalTicketRequestPacketClass(int unknown0_c);	// 0x867860
	GlobalTicketRequestPacketClass(RawPacketClass *raw);	// 0x8678D0
	virtual RawPacketClass *Build();	// +0x04 0x867960
	virtual bool Process_Packet(void *client);	// +0x08 0x868940 (clientstub.cpp l.248)
	virtual int Get_Data_Size();	// +0x0C 0x867A20
	virtual int Print(char *buffer, int len);	// +0x10 0x867A40

	int Unknown0C;	// +0x0C
};

#endif
