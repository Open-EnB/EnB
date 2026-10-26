/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pknamedecal.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8805C0-0x8812D0 rebuilt so far (opcode 178); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKNAMEDECAL_H  // NAME 1 [pknamedecal]: provisional, pk + its first named class's name
#define PKNAMEDECAL_H

#include "pk.h"

// opcode 178 (0x00B2, NAME_DECAL): vtable 0xB0A230, constructor 0x880EE0, 0x40 bytes
class NameDecalPacketClass : public PacketClass
{
public:
	NameDecalPacketClass(RawPacketClass *raw);	// 0x880EE0
	virtual RawPacketClass *Build();	// +0x04 0x880FA0
	virtual bool Process_Packet(void *client);	// +0x08 0x735FC0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x881080
	virtual int Print(char *buffer, int len);	// +0x10 0x8810A0
	virtual int Get_Channel() { return(Unknown3C); }	// +0x18 0x880E70

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Name[32];	// +0x10  // NAME 3: the client's Print text 'Name='
	float RGB[3];	// +0x30  // NAME 3: the client's Print text 'RGB='
	int Unknown3C;	// +0x3C
};

#endif
