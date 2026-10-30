/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalticket.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x867520-0x867830 (opcode 111).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALTICKET_H  // NAME 1 [pkglobalticket]: provisional, pk + its first named class's name
#define PKGLOBALTICKET_H

#include "pk.h"
#include "..\shared\unknown_login_data.h"

// opcode 111 (0x006F, GLOBAL_TICKET): vtable 0xB09688, constructor 0x867650, 0x58 bytes
class GlobalTicketPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name GLOBAL_TICKET (the Net-7 opcode table), the client's Print text 'Global Ticket'
{
public:
	GlobalTicketPacketClass(int status, const TicketClass &ticket);	// 0x867520
	GlobalTicketPacketClass(RawPacketClass *raw);	// 0x867650
	virtual RawPacketClass *Build();	// +0x04 0x867710
	virtual bool Process_Packet(void *client);	// +0x08 0x76B730 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8677E0
	virtual int Print(char *buffer, int len);	// +0x10 0x867800

	int Status;	// +0x0C  // NAME 1: tsk_login.cpp's 'Got the global ticket: %d'
	TicketClass Ticket;	// +0x10 (through RawPacketClass's Add / Get of its wire interface)  // NAME 1: copied to the login task's Ticket
};

#endif
