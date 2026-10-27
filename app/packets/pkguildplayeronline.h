/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkguildplayeronline.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x88EC70-0x894390 rebuilt so far (opcode 203, 206, 209, 214); the file's other
**	classes are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGUILDPLAYERONLINE_H  // NAME 1 [pkguildplayeronline]: provisional, pk + its first class's name
#define PKGUILDPLAYERONLINE_H

#include "pk.h"
#include <string>

char *Unknown_8B44E0(__int64 value, char *buffer);	// 0x8B44E0: _i64toa(value, buffer, 10) (not rebuilt)  // NAME 1: neutral placeholder (D10)

// GuildPermissions' second field (read and sent through an int)
enum Unknown_GuildPermissionsEnum  // NAME 1: neutral placeholder (D10)
{
	UNKNOWN_GUILDPERMISSIONS_0	// NAME 1: neutral placeholder (D10)
};

// opcode 203 (0x00CB, ?): vtable 0xB0AA94, constructor 0x88F280, 0x28 bytes
class GuildSimpleGuildSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildSimpleGuildSectorPacket {' and the PacketClass convention
{
public:
	GuildSimpleGuildSectorPacketClass(int type, __int64 player_id, const char *optional_param);	// 0x88F0E0
	GuildSimpleGuildSectorPacketClass(RawPacketClass *raw);	// 0x88F280
	virtual ~GuildSimpleGuildSectorPacketClass();	// +0x00 0x88F250 / 0x88F350
	virtual RawPacketClass *Build();	// +0x04 0x88F3C0
	virtual bool Process_Packet(void *client);	// +0x08 0x868F40 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x88F490
	virtual int Print(char *buffer, int len);	// +0x10 0x88F4B0

	int Type;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Type='
	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string OptionalParam;	// +0x18  // NAME 3: the client's Print text 'OptionalParam='
};

// opcode 206 (0x00CE, ?): vtable 0xB0AAC4, constructor 0x88F6C0, 0x28 bytes
class GuildSimpleSectorGuildPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildSimpleSectorGuildPacket {' and the PacketClass convention
{
public:
	GuildSimpleSectorGuildPacketClass(int type, __int64 player_id, const char *optional_param);	// 0x88F520
	GuildSimpleSectorGuildPacketClass(RawPacketClass *raw);	// 0x88F6C0
	virtual ~GuildSimpleSectorGuildPacketClass();	// +0x00 0x88F690 / 0x88F790
	virtual RawPacketClass *Build();	// +0x04 0x88F800
	virtual bool Process_Packet(void *client);	// +0x08 0x868FA0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x88F8D0
	virtual int Print(char *buffer, int len);	// +0x10 0x88F8F0

	int Type;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Type='
	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string OptionalParam;	// +0x18  // NAME 3: the client's Print text 'OptionalParam='
};

// opcode 209 (0x00D1, ?): vtable 0xB0AC44, constructor 0x891E90, 0x28 bytes
class GuildPermissionsPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildPermissionsPacket {' and the PacketClass convention
{
public:
	GuildPermissionsPacketClass(__int64 player_id, Unknown_GuildPermissionsEnum unknown18, int unknown1c, int unknown20, int unknown24);	// 0x891DE0
	GuildPermissionsPacketClass(RawPacketClass *raw);	// 0x891E90
	virtual RawPacketClass *Build();	// +0x04 0x891F80
	virtual bool Process_Packet(void *client);	// +0x08 0x869180 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x892070
	virtual int Print(char *buffer, int len);	// +0x10 0x892090

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	Unknown_GuildPermissionsEnum Unknown18;	// +0x18 (network order)
	int Unknown1C;	// +0x1C (network order)
	int Unknown20;	// +0x20 (network order)
	int Unknown24;	// +0x24 (network order)
};

// opcode 214 (0x00D6, ?): vtable 0xB0AC74, constructor 0x892180, 0x18 bytes
class GuildRankNamesRequestSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildRankNamesRequestSector {' and the PacketClass convention
{
public:
	GuildRankNamesRequestSectorPacketClass(__int64 player_id);	// 0x8920E0
	GuildRankNamesRequestSectorPacketClass(RawPacketClass *raw);	// 0x892180
	virtual RawPacketClass *Build();	// +0x04 0x892210
	virtual bool Process_Packet(void *client);	// +0x08 0x8691E0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8922D0
	virtual int Print(char *buffer, int len);	// +0x10 0x8922F0

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
};

#endif
