// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgmdestroyitem.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8714F0-0x871A00 (opcode 236).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGMDESTROYITEM_H  // NAME 1 [pkgmdestroyitem]: provisional, pk + its class's name
#define PKGMDESTROYITEM_H

#include "pk.h"

// opcode 236 (0x00EC, ?): vtable 0xB09994, constructor 0x8714F0, 0x24 bytes
class GMDestroyItemPacketClass : public PacketClass  // NAME 3: the client's Print text 'GM Destroy Item {'
{
public:
	GMDestroyItemPacketClass(int unknown0c, const char *item_name, bool unknown20);	// 0x8716D0
	GMDestroyItemPacketClass(RawPacketClass *raw);	// 0x8714F0
	virtual ~GMDestroyItemPacketClass();	// +0x00 0x8716A0 / 0x871840
	virtual RawPacketClass *Build();	// +0x04 0x8718B0
	virtual bool Process_Packet(void *client);	// +0x08 0x869570 (clientstub.cpp l.443)
	virtual int Get_Data_Size();	// +0x0C 0x8719A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8719C0

	int Unknown0C;	// +0x0C (network order)
	std::string ItemName;	// +0x10 (sent as a string)  // NAME 3: the client's Print text 'Item Name ='
	bool Unknown20;	// +0x20 (sent as a network-order int)
};

#endif
