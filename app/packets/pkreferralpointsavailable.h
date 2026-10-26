/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkreferralpointsavailable.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x878030-0x878250 rebuilt so far (opcode 233); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREFERRALPOINTSAVAILABLE_H  // NAME 1 [pkreferralpointsavailable]: provisional, pk + its first named class's name
#define PKREFERRALPOINTSAVAILABLE_H

#include "pk.h"

// opcode 233 (0x00E9, ?): vtable 0xB09DAC, constructor 0x878030, 0x14 bytes
class ReferralPointsAvailablePacketClass : public PacketClass
{
public:
	ReferralPointsAvailablePacketClass(RawPacketClass *raw);	// 0x878030
	virtual RawPacketClass *Build();	// +0x04 0x878120
	virtual bool Process_Packet(void *client);	// +0x08 0x73F3D0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8781F0
	virtual int Print(char *buffer, int len);	// +0x10 0x878210

	int PointsTotal;	// +0x0C  // NAME 3: the client's Print text 'PointsTotal='
	int PointsSpent;	// +0x10  // NAME 3: the client's Print text 'PointsSpent='
};

#endif
