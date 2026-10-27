/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkopeninterface.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89F1F0-0x89F460 rebuilt so far (opcode 102); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKOPENINTERFACE_H  // NAME 1 [pkopeninterface]: provisional, pk + its first named class's name
#define PKOPENINTERFACE_H

#include "pk.h"

// opcode 102 (0x0066, OPEN_INTERFACE): vtable 0xB0B534, constructor 0x89F290, 0x14 bytes
class OpenInterfacePacketClass : public PacketClass
{
public:
	OpenInterfacePacketClass(int ui_change, int ui_type);	// 0x89F1F0
	OpenInterfacePacketClass(RawPacketClass *raw);	// 0x89F290
	virtual RawPacketClass *Build();	// +0x04 0x89F330
	virtual bool Process_Packet(void *client);	// +0x08 0x7397D0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89F400
	virtual int Print(char *buffer, int len);	// +0x10 0x89F420

	int UIChange;	// +0x0C  // NAME 3: the client's Print text 'UIChange='
	int UIType;	// +0x10  // NAME 3: the client's Print text 'UIType='
};

#endif
