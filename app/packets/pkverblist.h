/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkverblist.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AEAD0-0x8B15C0 rebuilt so far (opcode 92); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKVERBLIST_H  // NAME 1 [pkverblist]: provisional, pk + its first named class's name
#define PKVERBLIST_H

#include "pk.h"
#include <set>
#include <string>

// A verb (the client's Value_To_String 0x84C470 names it from its table 0xBB1218 as 'VerbEnum':
// the high 16 bits pick the verb, the low 16 bits are an argument)
enum VerbEnum  // NAME 4: the client's own label 'VerbEnum' (0x84C470's table name)
{
	VERB_NONE = 0	// NAME 1: neutral placeholder (D10)
};

char *Value_To_String(char *buf, int size, VerbEnum verb);	// 0x84C470 (not rebuilt)

// opcode 92 (0x005C, VERB_UPDATE): vtable 0xB0C29C, constructor 0x8AFD10, 0x30 bytes
class VerbUpdatePacketClass : public PacketClass
{
public:
	VerbUpdatePacketClass(RawPacketClass *raw);	// 0x8AFD10
	virtual RawPacketClass *Build();	// +0x04 0x8AFF40
	virtual bool Process_Packet(void *client);	// +0x08 0x7388B0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8B0130
	virtual int Print(char *buffer, int len);	// +0x10 0x8B0150
	virtual int Get_Type() { return(92); }	// +0x28 0x8B0110

	int GameID;	// +0x0C (network order)  // NAME 3: the client's Print text 'GameID='
	std::set<VerbEnum> Adds;	// +0x10  // NAME 3: the client's Print text 'Adds='
	std::set<VerbEnum> Drops;	// +0x20  // NAME 3: the client's Print text 'Drops='
};

#endif
