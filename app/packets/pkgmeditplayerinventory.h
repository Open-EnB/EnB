// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgmeditplayerinventory.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x86F630-0x86FCD0 (opcode 234).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGMEDITPLAYERINVENTORY_H  // NAME 1 [pkgmeditplayerinventory]: provisional, pk + its class's name
#define PKGMEDITPLAYERINVENTORY_H

#include "pk.h"
#include <vector>

// opcode 234 (0x00EA, ?): vtable 0xB0983C, constructor 0x86F630, 0x20 bytes
class GMEditPlayerInventoryPacketClass : public PacketClass  // NAME 3: the client's Print text 'GM Edit Player Inventory'
{
public:
	GMEditPlayerInventoryPacketClass(int unknown0c, const std::vector<std::string> &strings);	// 0x86F870
	GMEditPlayerInventoryPacketClass(RawPacketClass *raw);	// 0x86F630
	virtual ~GMEditPlayerInventoryPacketClass();	// +0x00 0x86F840 / 0x86FAA0
	virtual RawPacketClass *Build();	// +0x04 0x86FB30
	virtual bool Process_Packet(void *client);	// +0x08 0x73F2E0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C PKGMEDITPLAYERINVENTORY_HDS
	virtual int Print(char *buffer, int len);	// +0x10 0x86FCA0

	int Unknown0C;	// +0x0C (network order)
	std::vector<std::string> Strings;	// +0x10 (a network-order count, then the strings)  // NAME 2: what it holds
};

#endif
