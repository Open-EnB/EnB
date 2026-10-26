/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_866ac0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x866AC0-0x866FE0 rebuilt so far (opcode 120); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_866AC0_H  // NAME 1 [pkunknown_866ac0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_866AC0_H

#include "pk.h"

// opcode 120 (0x0078, ?): vtable 0xB09628, constructor 0x866DF0, 0x10 bytes
class Packet_0078Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0078
{
public:
	Packet_0078Class(RawPacketClass *raw);	// 0x866DF0
	virtual RawPacketClass *Build();	// +0x04 0x866ED0
	virtual bool Process_Packet(void *client);	// +0x08 0x76BB70 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x866F90
	virtual int Print(char *buffer, int len);	// +0x10 0x866FB0

	short Result;	// +0x0C  // NAME 3: the client's Print text 'Result='
	char Unknown0E[0x2];	// +0x0E
};

#endif
