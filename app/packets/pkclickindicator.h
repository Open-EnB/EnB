/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkclickindicator.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87E7D0-0x87EA30 rebuilt so far (opcode 105); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCLICKINDICATOR_H  // NAME 1 [pkclickindicator]: provisional, pk + its first named class's name
#define PKCLICKINDICATOR_H

#include "pk.h"

// opcode 105 (0x0069, ?): vtable 0xB0A080, constructor 0x87E870, 0x14 bytes
class ClickIndicatorPacketClass : public PacketClass
{
public:
	ClickIndicatorPacketClass(RawPacketClass *raw);	// 0x87E870
	virtual RawPacketClass *Build();	// +0x04 0x87E910
	virtual bool Process_Packet(void *client);	// +0x08 0x7393D0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x87E9E0
	virtual int Print(char *buffer, int len);	// +0x10 0x87EA00

	bool TurnOn;	// +0x0C  // NAME 3: the client's Print text 'TurnOn='
	char Unknown0D[0x3];	// +0x0D
	int ObjID;	// +0x10  // NAME 3: the client's Print text 'ObjID='
};

#endif
