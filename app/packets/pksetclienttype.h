/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pksetclienttype.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87FCF0-0x87FF30 (opcode 60).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSETCLIENTTYPE_H  // NAME 1 [pksetclienttype]: provisional, pk + its class's name
#define PKSETCLIENTTYPE_H

#include "pk.h"

char *Sector_Type_Name(int sector_type);	// 0x84A3B0 (sector_types.cpp, not rebuilt; also pkstarbasetow.h)  // NAME 2: what it returns

// opcode 60 (0x003C, CLIENT_TYPE): vtable 0xB0A170, constructor 0x87FD80, 0x10 bytes
class SetClientTypePacketClass : public PacketClass
{
public:
	SetClientTypePacketClass(int type);	// 0x87FCF0
	SetClientTypePacketClass(RawPacketClass *raw);	// 0x87FD80
	virtual RawPacketClass *Build();	// +0x04 0x87FE10
	virtual bool Process_Packet(void *client);	// +0x08 0x737E10 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x87FED0
	virtual int Print(char *buffer, int len);	// +0x10 0x87FEF0

	int Type;	// +0x0C (a sector type: Sector_Type_Name)  // NAME 3: the client's Print text 'Type='
};

#endif
