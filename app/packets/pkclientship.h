// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkclientship.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A4790-0x8A49C0 rebuilt so far (opcode 71); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCLIENTSHIP_H  // NAME 1 [pkclientship]: provisional, pk + its first named class's name
#define PKCLIENTSHIP_H

#include "pk.h"

// opcode 71 (0x0047, CLIENT_SHIP): vtable 0xB0BA18, constructor 0x8A4820, 0x10 bytes
class ClientShipPacketClass : public PacketClass
{
public:
	ClientShipPacketClass(int ship_id);	// 0x8A4790
	ClientShipPacketClass(RawPacketClass *raw);	// 0x8A4820
	virtual RawPacketClass *Build();	// +0x04 0x8A48B0
	virtual bool Process_Packet(void *client);	// +0x08 0x733660 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A4970
	virtual int Print(char *buffer, int len);	// +0x10 0x8A4990

	int ShipID;	// +0x0C  // NAME 3: the client's Print text 'ShipID='
};

#endif
