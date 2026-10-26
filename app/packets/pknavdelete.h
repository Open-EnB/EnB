/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pknavdelete.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89E8C0-0x89EEF0 rebuilt so far (opcode 154); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKNAVDELETE_H  // NAME 1 [pknavdelete]: provisional, pk + its first named class's name
#define PKNAVDELETE_H

#include "pk.h"

// opcode 154 (0x009A, ?): vtable 0xB0B4D4, constructor 0x89ED50, 0x14 bytes
class NavDeletePacketClass : public PacketClass
{
public:
	NavDeletePacketClass(RawPacketClass *raw);	// 0x89ED50
	virtual RawPacketClass *Build();	// +0x04 0x89EDE0
	virtual bool Process_Packet(void *client);	// +0x08 0x73A710 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x89EEA0
	virtual int Print(char *buffer, int len);	// +0x10 0x89EEC0
	virtual int Get_Channel() { return(Unknown10); }	// +0x18 0x89ECE0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Unknown10;	// +0x10
};

#endif
