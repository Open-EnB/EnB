/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkserverredirect.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8A1500-0x8A1630 (opcode 54).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSERVERREDIRECT_H  // NAME 1 [pkserverredirect]: provisional, pk + its first named class's name
#define PKSERVERREDIRECT_H

#include "pkserverready.h"

// Server_Ready 59's members and Build / Get_Data_Size (0x8A62A0, 0x8A63A0), its own type and Print.
// opcode 54 (0x0036, SERVER_REDIRECT): vtable 0xB0B6B4, constructor 0x8A1590, 0x18 bytes
class ServerRedirectPacketClass : public Packet_003BClass  // NAME 2: the reference decoder's opcode name SERVER_REDIRECT (the Net-7 opcode table; the client's Print text 'Client_Redirect')
{
public:
	ServerRedirectPacketClass(int sector, int ip, unsigned short port);	// 0x8A1500
	ServerRedirectPacketClass(RawPacketClass *raw);	// 0x8A1590
	virtual bool Process_Packet(void *client);	// +0x08 0x766FC0 (tsk_game.cpp)
	virtual int Print(char *buffer, int len);	// +0x10 0x8A15E0
	virtual int Get_Type();	// +0x28 0x8A15C0
};

#endif
