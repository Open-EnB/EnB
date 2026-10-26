/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkincapacitancerequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x871F70-0x873770 rebuilt so far (opcode 141, 143); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKINCAPACITANCEREQUEST_H  // NAME 1 [pkincapacitancerequest]: provisional, pk + its first named class's name
#define PKINCAPACITANCEREQUEST_H

#include "pk.h"

// opcode 141 (0x008D, INCAPACITANCE_REQUEST): vtable 0xB099F4, constructor 0x871FE0, 0x10 bytes
class IncapacitanceRequestPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name INCAPACITANCE_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	IncapacitanceRequestPacketClass(int game_id);	// 0x871F70
	IncapacitanceRequestPacketClass(RawPacketClass *raw);	// 0x871FE0
	virtual RawPacketClass *Build();	// +0x04 0x872070
	virtual bool Process_Packet(void *client);	// +0x08 0x868A30 (clientstub.cpp l.263)
	virtual int Get_Data_Size();	// +0x0C 0x872130
	virtual int Print(char *buffer, int len);	// +0x10 0x872150

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

// opcode 143 (0x008F, ?): vtable 0xB09A54, constructor 0x872450, 0x10 bytes
class Packet_008FClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x008F
{
public:
	Packet_008FClass(int game_id);	// 0x8723E0
	Packet_008FClass(RawPacketClass *raw);	// 0x872450
	virtual RawPacketClass *Build();	// +0x04 0x8724E0
	virtual bool Process_Packet(void *client);	// +0x08 0x868A90 (clientstub.cpp l.269)
	virtual int Get_Data_Size();	// +0x0C 0x8725A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8725C0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

#endif
