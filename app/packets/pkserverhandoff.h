/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkserverhandoff.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8943C0-0x8949D0 (opcode 58).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSERVERHANDOFF_H  // NAME 1 [pkserverhandoff]: provisional, pk + its first named class's name
#define PKSERVERHANDOFF_H

#include "pk.h"
#include "..\shared\unknown_login_data.h"
#include <string>

// opcode 58 (0x003A, SERVER_HANDOFF): vtable 0xB0ADD4, constructor 0x8946B0, 0x98 bytes
class ServerHandoffPacketClass : public PacketClass  // NAME 3: the client's Print text 'Server_Handoff'
{
public:
	ServerHandoffPacketClass(const TicketClass &ticket, const std::string &source_sector, const std::string &source_system,
		const std::string &name, const std::string &system);	// 0x8943C0
	ServerHandoffPacketClass(RawPacketClass *raw);	// 0x8946B0
	virtual RawPacketClass *Build();	// +0x04 0x8947F0
	virtual bool Process_Packet(void *client);	// +0x08 0x737CB0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8948E0
	virtual int Print(char *buffer, int len);	// +0x10 0x894950

	int Unknown0C;	// +0x0C (never set)  // NAME 1: neutral (D10)
	TicketClass Ticket;	// +0x10 (through RawPacketClass's Add / Get of its wire interface; Print's 'Dest Sector' its +0x18)  // NAME 2: what it holds
	std::string Source_Sector;	// +0x58  // NAME 3: the client's Print text 'Source Sector='
	std::string Source_System;	// +0x68  // NAME 3: the client's Print text 'Source System='
	std::string Name;	// +0x78  // NAME 3: the client's Print text 'Name='
	std::string System;	// +0x88  // NAME 3: the client's Print text 'System='
};

#endif
