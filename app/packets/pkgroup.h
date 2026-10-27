/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgroup.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x888690-0x888B30 (opcode 30).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGROUP_H  // NAME 1 [pkgroup]: provisional, pk + its class's name
#define PKGROUP_H

#include "pk.h"

// opcode 30 (0x001E, GROUP): vtable 0xB0A734, constructor 0x888890, 0x14 bytes
class GroupPacketClass : public PacketClass
{
public:
	GroupPacketClass(unsigned char type, char *text);	// 0x8886A0 (String is text's, not copied)
	GroupPacketClass(unsigned char type, const std::string &text);	// 0x888780 (String a copy)
	GroupPacketClass(RawPacketClass *raw);	// 0x888890
	virtual ~GroupPacketClass();	// +0x00 0x888750 / 0x888960
	virtual RawPacketClass *Build();	// +0x04 0x8889A0
	virtual bool Process_Packet(void *client);	// +0x08 0x736930 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x888A80
	virtual int Print(char *buffer, int len);	// +0x10 0x888AA0
	virtual int Get_Channel() { return(-2); }	// +0x18 0x888730
	virtual int Get_Priority() { return(0); }	// +0x1C 0x888710

	char *String;	// +0x0C (Size bytes, the text and its 0)  // NAME 3: the client's Print text 'String='
	short Size;	// +0x10  // NAME 3: the client's Print text 'Size='
	unsigned char Type;	// +0x12  // NAME 3: the client's Print text 'Type='
	bool Owned;	// +0x13 (String new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means
};

#endif
