/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkremove.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A1660-0x8A1920 rebuilt so far (opcode 7); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREMOVE_H  // NAME 1 [pkremove]: provisional, pk + its first named class's name
#define PKREMOVE_H

#include "pk.h"

// opcode 7 (0x0007, REMOVE): vtable 0xB0B6E8, constructor 0x8A1780, 0x14 bytes
class RemovePacketClass : public PacketClass
{
public:
	RemovePacketClass(RawPacketClass *raw);	// 0x8A1780
	virtual RawPacketClass *Build();	// +0x04 0x8A1810
	virtual bool Process_Packet(void *client);	// +0x08 0x7362F0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A18D0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A18F0
	virtual int Get_Channel() { return(Unknown10); }	// +0x18 0x8A16F0
	virtual bool Close_Channel() { return(true); }	// +0x24 0x8A1710

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Unknown10;	// +0x10
};

#endif
