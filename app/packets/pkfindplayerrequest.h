/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkfindplayerrequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x886C70-0x887280 (opcodes 181 and 182).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKFINDPLAYERREQUEST_H  // NAME 1 [pkfindplayerrequest]: provisional, pk + its first class's name
#define PKFINDPLAYERREQUEST_H

#include "pk.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1 [Unknown_4A1590]: neutral placeholder (D10)

// opcode 181 (0x00B5, ?): vtable 0xB0A5BC, constructor 0x886D40, 0x54 bytes
class FindPlayerRequestPacketClass : public PacketClass  // NAME 3: the client's Print label 'FindPlayerRequest'
{
public:
	FindPlayerRequestPacketClass(int unknown0c, const char *name, bool unknown51);	// 0x886C70
	FindPlayerRequestPacketClass(RawPacketClass *raw);	// 0x886D40
	virtual RawPacketClass *Build();	// +0x04 0x886E30
	virtual bool Process_Packet(void *client);	// +0x08 0x868BE0 (clientstub.cpp l.290)
	virtual int Get_Data_Size();	// +0x0C 0x886F10
	virtual int Print(char *buffer, int len);	// +0x10 0x886F30

	int Unknown0C;	// +0x0C  // NAME 1: neutral (D10)
	char Name[0x41];	// +0x10 (0x41 bytes sent, the last cleared on receipt)  // NAME 2: a player name copied with the bounded strcpy
	bool Unknown51;	// +0x51 (sent as an int)  // NAME 1: neutral (D10)
};

// opcode 182 (0x00B6, ?): vtable 0xB0A5EC, constructor 0x887030, 0x58 bytes
class FindPlayerResponsePacketClass : public PacketClass  // NAME 3: the client's Print label 'FindPlayerResponse'
{
public:
	FindPlayerResponsePacketClass(int unknown0c, const char *name, bool unknown51, int unknown54);	// 0x886F60
	FindPlayerResponsePacketClass(RawPacketClass *raw);	// 0x887030
	virtual RawPacketClass *Build();	// +0x04 0x887120
	virtual bool Process_Packet(void *client);	// +0x08 0x868C10 (clientstub.cpp l.293)
	virtual int Get_Data_Size();	// +0x0C 0x887210
	virtual int Print(char *buffer, int len);	// +0x10 0x887230

	int Unknown0C;	// +0x0C  // NAME 1: neutral (D10)
	char Name[0x41];	// +0x10 (0x41 bytes sent, the last cleared on receipt)  // NAME 2: a player name copied with the bounded strcpy
	bool Unknown51;	// +0x51 (sent as an int)  // NAME 1: neutral (D10)
	int Unknown54;	// +0x54  // NAME 1: neutral (D10)
};

#endif
