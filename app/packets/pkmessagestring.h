/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmessagestring.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89E110-0x89E5B0 (opcode 29).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMESSAGESTRING_H  // NAME 1 [pkmessagestring]: provisional, pk + its class's name
#define PKMESSAGESTRING_H

#include "pk.h"

// opcode 29 (0x001D, MESSAGE_STRING): vtable 0xB0B444, constructor 0x89E310, 0x14 bytes
class MessageStringPacketClass : public PacketClass
{
public:
	MessageStringPacketClass(unsigned char color, char *text);	// 0x89E120 (String is text's, not copied)
	MessageStringPacketClass(unsigned char color, const std::string &text);	// 0x89E200 (String a copy)
	MessageStringPacketClass(RawPacketClass *raw);	// 0x89E310
	virtual ~MessageStringPacketClass();	// +0x00 0x89E1D0 / 0x89E3E0
	virtual RawPacketClass *Build();	// +0x04 0x89E420
	virtual bool Process_Packet(void *client);	// +0x08 0x736760 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89E500
	virtual int Print(char *buffer, int len);	// +0x10 0x89E520
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89E1B0
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89E190

	char *String;	// +0x0C (Size bytes, the text and its 0)  // NAME 3: the client's Print text 'String='
	short Size;	// +0x10  // NAME 3: the client's Print text 'Size='
	unsigned char Color;	// +0x12  // NAME 3: the client's Print text 'Color='
	bool Owned;	// +0x13 (String new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means
};

#endif
