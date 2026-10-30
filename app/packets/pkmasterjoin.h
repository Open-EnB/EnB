/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmasterjoin.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8974C0-0x8977D0 (opcode 53).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMASTERJOIN_H  // NAME 1 [pkmasterjoin]: provisional, pk + its first named class's name
#define PKMASTERJOIN_H

#include "pk.h"
#include "..\shared\unknown_login_data.h"

// opcode 53 (0x0035, MASTER_JOIN): vtable 0xB0AFB4, constructor 0x897590, 0x60 bytes
class MasterJoinPacketClass : public PacketClass  // NAME 2: Net-7's opcode name MASTER_JOIN (the Net-7 opcode table), the client's Print text 'Master Join'
{
public:
	MasterJoinPacketClass(const TicketClass &ticket);	// 0x8974C0
	MasterJoinPacketClass(RawPacketClass *raw);	// 0x897590
	virtual ~MasterJoinPacketClass();	// +0x00 0x897560 / 0x897650
	virtual RawPacketClass *Build();	// +0x04 0x8976C0
	virtual bool Process_Packet(void *client);	// +0x08 0x8683A0 (clientstub.cpp l.158)
	virtual int Get_Data_Size();	// +0x0C 0x897780
	virtual int Print(char *buffer, int len);	// +0x10 0x8977A0

	int Unknown0C;	// +0x0C (never set)  // NAME 1: neutral (D10)
	TicketClass Ticket;	// +0x10 (through RawPacketClass's Add / Get of its wire interface)  // NAME 2: what it holds
	int Unknown58;	// +0x58 (0; not in the message)  // NAME 1: neutral (D10)
	int Unknown5C;	// +0x5C (never set: the class is 0x60 bytes)  // NAME 1: neutral (D10)
};

#endif
