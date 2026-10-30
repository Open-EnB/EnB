/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pklogin.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x89A740-0x89AB00 (opcode 2).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKLOGIN_H  // NAME 1 [pklogin]: provisional, pk + its first named class's name
#define PKLOGIN_H

#include "pk.h"
#include "..\shared\unknown_login_data.h"

// opcode 2 (0x0002, LOGIN): vtable 0xB0B014, constructor 0x89A850, 0xB8 bytes
class LoginPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name LOGIN (the Net-7 opcode table), the client's Print text 'Login'
{
public:
	LoginPacketClass(const TicketClass &ticket, const char *name, int unknown);	// 0x89A740  // NAME 1 [name, unknown]: neutral (D10)
	LoginPacketClass(RawPacketClass *raw);	// 0x89A850
	virtual ~LoginPacketClass();	// +0x00 0x89A940 / 0x89A820
	virtual RawPacketClass *Build();	// +0x04 0x89A9B0
	virtual bool Process_Packet(void *client);	// +0x08 0x867AA0 (clientstub.cpp l.14)
	virtual int Get_Data_Size();	// +0x0C 0x89AAA0
	virtual int Print(char *buffer, int len);	// +0x10 0x89AAC0

	int Unknown0C;	// +0x0C (never set)  // NAME 1: neutral (D10)
	TicketClass Ticket;	// +0x10 (through RawPacketClass's Add / Get of its wire interface)  // NAME 2: what it holds
	unsigned int TimeSent;	// +0x58 (Build stamps it: Unknown_Get_Ticks)  // NAME 3: the client's Print text 'TimeSent='
	unsigned int TimeReceived;	// +0x5C (the raw constructor stamps it)  // NAME 3: the client's Print text 'TimeReceived='
	char Name[0x41];	// +0x60 (strncpy'd, 0x40 at most)  // NAME 1: neutral (D10)
	int UnknownA4;	// +0xA4  // NAME 1: neutral (D10)
	int UnknownA8;	// +0xA8 (0; not in the message)  // NAME 1: neutral (D10)
	int UnknownAC;	// +0xAC (0; not in the message)  // NAME 1: neutral (D10)
	int UnknownB0;	// +0xB0 (0; not in the message)  // NAME 1: neutral (D10)
	int UnknownB4;	// +0xB4 (never set: the class is 0xB8 bytes)  // NAME 1: neutral (D10)
};

#endif
