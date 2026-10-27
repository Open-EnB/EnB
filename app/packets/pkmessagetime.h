/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmessagetime.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x89E5E0-0x89E890 (opcode 96).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMESSAGETIME_H  // NAME 1 [pkmessagetime]: provisional, pk + its Print text 'Message_Time'
#define PKMESSAGETIME_H

#include "pk.h"
#include <time.h>

// opcode 96 (0x0060, ?): vtable 0xB0B474, constructor 0x89E6A0, 0x14 bytes
class MessageTimePacketClass : public PacketClass
{
public:
	MessageTimePacketClass(unsigned char color, time_t time);	// 0x89E5E0
	MessageTimePacketClass(RawPacketClass *raw);	// 0x89E6A0
	virtual RawPacketClass *Build();	// +0x04 0x89E760
	virtual bool Process_Packet(void *client);	// +0x08 0x736800 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x89E830
	virtual int Print(char *buffer, int len);	// +0x10 0x89E850
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89E650
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89E630

	time_t Time;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Time='
	unsigned char Color;	// +0x10 (1 byte as it is)  // NAME 3: the client's Print text 'Color='
};

#endif
