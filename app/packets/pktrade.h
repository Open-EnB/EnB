/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pktrade.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AB290-0x8AB530 rebuilt so far (opcode 31); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKTRADE_H  // NAME 1 [pktrade]: provisional, pk + its first named class's name
#define PKTRADE_H

#include "pk.h"

// opcode 31 (0x001F, TRADE): vtable 0xB0C018, constructor 0x8AB370, 0x14 bytes
class TradePacketClass : public PacketClass
{
public:
	TradePacketClass(RawPacketClass *raw);	// 0x8AB370
	virtual RawPacketClass *Build();	// +0x04 0x8AB410
	virtual bool Process_Packet(void *client);	// +0x08 0x739DE0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8AB4E0
	virtual int Print(char *buffer, int len);	// +0x10 0x8AB500
	virtual int Get_Channel() { return(-2); }	// +0x18 0x8AB300
	virtual int Get_Priority() { return(0); }	// +0x1C 0x8AB2E0

	int PartnerID;	// +0x0C  // NAME 3: the client's Print text 'PartnerID='
	bool Type;	// +0x10  // NAME 3: the client's Print text 'Type='
	char Unknown11[0x3];	// +0x11
};

#endif
