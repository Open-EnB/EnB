/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgalaxymap.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x870E90-0x8714C0 (opcodes 151, 152): the galaxy map message and
**	the request derived from it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGALAXYMAP_H  // NAME 1 [pkgalaxymap]: provisional, pk + its first named class's name
#define PKGALAXYMAP_H

#include "pk.h"

// opcode 151 (0x0097, GALAXY_MAP): vtable 0xB0992C, constructor 0x870FD0, 0x1C bytes
class GalaxyMapPacketClass : public PacketClass
{
public:
	GalaxyMapPacketClass(int type, LinearBufferClass &buffer, int channel);	// 0x870E90
	GalaxyMapPacketClass(RawPacketClass *raw);	// 0x870FD0
	virtual ~GalaxyMapPacketClass();	// +0x00 0x871180 / 0x870FA0
	virtual RawPacketClass *Build();	// +0x04 0x8710A0
	virtual bool Process_Packet(void *client);	// +0x08 0x73AC60 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8711C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8711E0
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x870F60
	virtual int Get_Type() { return(151); }	// +0x28 0x870F80  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Type;	// +0x0C  // NAME 3: the client's Print text 'Type='
	char *Data;	// +0x10 (new[] Size bytes)  // NAME 3: the client's Print text 'Data='
	int Size;	// +0x14  // NAME 3: the client's Print text 'Size='
	int Channel;	// +0x18 (the player id; read and sent by GalaxyMapRequestPacketClass only)  // NAME 3: the client's Print text 'Channel(PlayerID)='
};

// opcode 152 (0x0098, GALAXY_MAP_REQUEST): vtable 0xB09960, constructor 0x871350, 0x20 bytes
class GalaxyMapRequestPacketClass : public GalaxyMapPacketClass  // NAME 2: Net-7's opcode name GALAXY_MAP_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	GalaxyMapRequestPacketClass(int type, LinearBufferClass &buffer, int channel, bool b);	// 0x871220
	GalaxyMapRequestPacketClass(RawPacketClass *raw);	// 0x871350
	virtual RawPacketClass *Build();	// +0x04 0x871460
	virtual bool Process_Packet(void *client);	// +0x08 0x868C70 (clientstub.cpp l.299)
	virtual int Get_Data_Size();	// +0x0C 0x8714A0
	virtual int Get_Type() { return(152); }	// +0x28 0x871300

	bool Unknown1C;	// +0x1C
};

#endif
