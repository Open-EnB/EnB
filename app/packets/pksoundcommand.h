/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pksoundcommand.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A4F20-0x8A54E0 rebuilt so far (opcode 107); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSOUNDCOMMAND_H  // NAME 1 [pksoundcommand]: provisional, pk + its first named class's name
#define PKSOUNDCOMMAND_H

#include "pk.h"

// opcode 107 (0x006B, ?): vtable 0xB0BAD8, constructor 0x8A5320, 0x14 bytes
class SoundCommandPacketClass : public PacketClass
{
public:
	SoundCommandPacketClass(int command, int channel);	// 0x8A5280
	SoundCommandPacketClass(RawPacketClass *raw);	// 0x8A5320
	virtual RawPacketClass *Build();	// +0x04 0x8A53C0
	virtual bool Process_Packet(void *client);	// +0x08 0x7384A0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A5490
	virtual int Print(char *buffer, int len);	// +0x10 0x8A54B0

	int Command;	// +0x0C  // NAME 3: the client's Print text 'Command='
	int Channel;	// +0x10  // NAME 3: the client's Print text 'Channel='
};

#endif
