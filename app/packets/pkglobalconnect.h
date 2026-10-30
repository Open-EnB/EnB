// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalconnect.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x863400-0x8636F0 (opcode 109).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALCONNECT_H  // NAME 1 [pkglobalconnect]: provisional, pk + its class's name
#define PKGLOBALCONNECT_H

#include "pk.h"

// opcode 109 (0x006D, GLOBAL_CONNECT): vtable 0xB09398, constructor 0x8634D0, 0x14 bytes
class GlobalConnectPacketClass : public PacketClass  // NAME 2: Net-7's opcode name GLOBAL_CONNECT (the Net-7 opcode table; no structure check)
{
public:
	GlobalConnectPacketClass(const char *ticket, const char *password);	// 0x863400
	GlobalConnectPacketClass(RawPacketClass *raw);	// 0x8634D0
	virtual ~GlobalConnectPacketClass();	// +0x00 0x8634A0 / 0x863570
	virtual RawPacketClass *Build();	// +0x04 0x8635B0
	virtual bool Process_Packet(void *client);	// +0x08 0x868850 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x863680
	virtual int Print(char *buffer, int len);	// +0x10 0x8636C0

	char *Ticket;	// +0x0C (nstrdup'd; sent as a string)  // NAME 2: what tsk_login.cpp passes (its EATicket)
	char *Password;	// +0x10 (nstrdup'd; sent as a string)  // NAME 2: what tsk_login.cpp passes ('123456')
};

#endif
