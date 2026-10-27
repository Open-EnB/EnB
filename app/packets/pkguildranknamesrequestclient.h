/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkguildranknamesrequestclient.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x88BD40-0x88EC40 rebuilt so far (opcode 204, 205, 197, 200, 201, 208, 210, 212, 216); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGUILDRANKNAMESREQUESTCLIENT_H  // NAME 1 [pkguildranknamesrequestclient]: provisional, pk + its first named class's name
#define PKGUILDRANKNAMESREQUESTCLIENT_H

#include "pk.h"

// A guild permission kind (GUILD_PLAYER_PERMISSIONS reads it through an int: a 4-byte enum)
enum GuildPermissionEnum  // NAME 1: neutral placeholder (D10)
{
	GUILD_PERMISSION_NONE = 0	// NAME 1: neutral placeholder (D10)
};

// opcode 204 (0x00CC, GUILD_SIMPLE_SECTOR_CLIENT): vtable 0xB0A884, constructor 0x88BED0, 0x20 bytes
class GuildSimpleSectorClientPacket : public PacketClass
{
public:
	GuildSimpleSectorClientPacket(RawPacketClass *raw);	// 0x88BED0
	virtual RawPacketClass *Build();	// +0x04 0x88C000
	virtual bool Process_Packet(void *client);	// +0x08 0x73DD00 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x88C0D0
	virtual int Print(char *buffer, int len);	// +0x10 0x88C0F0

	int Type;	// +0x0C  // NAME 3: the client's Print text 'Type='
	std::string OptionalParam;	// +0x10  // NAME 3: the client's Print text 'OptionalParam='
};

// opcode 205 (0x00CD, GUILD_SIMPLE_CLIENT_SECTOR): vtable 0xB0A8B4, constructor 0x88C2D0, 0x24 bytes
class GuildSimpleClientSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildSimpleClientSectorPacket {' and the PacketClass convention
{
public:
	GuildSimpleClientSectorPacketClass(RawPacketClass *raw);	// 0x88C2D0
	virtual RawPacketClass *Build();	// +0x04 0x88C410
	virtual bool Process_Packet(void *client);	// +0x08 0x868F70 (clientstub.cpp l.347)
	virtual int Get_Data_Size();	// +0x0C 0x88C4E0
	virtual int Print(char *buffer, int len);	// +0x10 0x88C500

	int Type;	// +0x0C  // NAME 3: the client's Print text 'Type='
	int PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string OptionalParam;	// +0x14  // NAME 3: the client's Print text 'OptionalParam='
};

// opcode 197 (0x00C5, GUILD_LEADER_ACCEPT_CLIENT): vtable 0xB0A8E4, constructor 0x88C6E0, 0x24 bytes
class GuildLeaderAcceptClientPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildLeaderAcceptClient {' and the PacketClass convention
{
public:
	GuildLeaderAcceptClientPacketClass(int player_id, const char *guild_name, bool accept);	// 0x88C540
	GuildLeaderAcceptClientPacketClass(RawPacketClass *raw);	// 0x88C6E0
	virtual ~GuildLeaderAcceptClientPacketClass();	// +0x00 0x88C6B0 / 0x88C7B0
	virtual RawPacketClass *Build();	// +0x04 0x88C820
	virtual bool Process_Packet(void *client);	// +0x08 0x869030 (clientstub.cpp l.359)
	virtual int Get_Data_Size();	// +0x0C 0x88C8F0
	virtual int Print(char *buffer, int len);	// +0x10 0x88C910

	int PlayerID;	// +0x0C (network order)  // NAME 3: the client's Print text 'PlayerID='
	std::string GuildName;	// +0x10  // NAME 3: the client's Print text 'GuildName='
	bool Accept;	// +0x20  // NAME 3: the client's Print text 'Accept='
};

// opcode 200 (0x00C8, GUILD_RECRUIT_CONFIRM_SECTOR): vtable 0xB0A914, constructor 0x88CB70, 0x2C bytes
class GuildRecruitConfirmSectorPacket : public PacketClass
{
public:
	GuildRecruitConfirmSectorPacket(RawPacketClass *raw);	// 0x88CB70
	virtual RawPacketClass *Build();	// +0x04 0x88CCE0
	virtual bool Process_Packet(void *client);	// +0x08 0x73E720 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x88CDB0
	virtual int Print(char *buffer, int len);	// +0x10 0x88CDE0

	std::string InviterName;	// +0x0C  // NAME 3: the client's Print text 'InviterName='
	std::string GuildName;	// +0x1C  // NAME 3: the client's Print text 'GuildName='
};

// opcode 201 (0x00C9, GUILD_RECRUIT_ACCEPT_CLIENT): vtable 0xB0A944, constructor 0x88CEB0, 0x14 bytes
class GuildRecruitAcceptClientPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildRecruitAcceptClient {' and the PacketClass convention
{
public:
	GuildRecruitAcceptClientPacketClass(int player_id, bool accept);	// 0x88CE30
	GuildRecruitAcceptClientPacketClass(RawPacketClass *raw);	// 0x88CEB0
	virtual RawPacketClass *Build();	// +0x04 0x88CF70
	virtual bool Process_Packet(void *client);	// +0x08 0x8690C0 (clientstub.cpp l.368)
	virtual int Get_Data_Size();	// +0x0C 0x88D040
	virtual int Print(char *buffer, int len);	// +0x10 0x88D060

	int PlayerID;	// +0x0C (network order)  // NAME 3: the client's Print text 'PlayerID='
	bool Accept;	// +0x10  // NAME 3: the client's Print text 'Accept='
};

// opcode 208 (0x00D0, GUILD_MESSAGE_SECTOR): vtable 0xB0A974, constructor 0x88D2B0, 0x30 bytes
class GuildMessageSectorPacket : public PacketClass
{
public:
	GuildMessageSectorPacket(RawPacketClass *raw);	// 0x88D2B0
	virtual RawPacketClass *Build();	// +0x04 0x88D430
	virtual bool Process_Packet(void *client);	// +0x08 0x73E9E0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x88D500
	virtual int Print(char *buffer, int len);	// +0x10 0x88D540

	int MessageType;	// +0x0C  // NAME 3: the client's Print text 'MessageType='
	std::string OtherName;	// +0x10  // NAME 3: the client's Print text 'OtherName='
	std::string GuildName;	// +0x20  // NAME 3: the client's Print text 'GuildName='
};

// opcode 210 (0x00D2, GUILD_PLAYER_PERMISSIONS): vtable 0xB0A9A4, constructor 0x88D630, 0x1C bytes
class GuildPlayerPermissionsPacket : public PacketClass
{
public:
	GuildPlayerPermissionsPacket(GuildPermissionEnum unknown0c, int unknown10, int unknown14, int unknown18);	// 0x88D590
	GuildPlayerPermissionsPacket(RawPacketClass *raw);	// 0x88D630
	virtual RawPacketClass *Build();	// +0x04 0x88D710
	virtual bool Process_Packet(void *client);	// +0x08 0x73ED40 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x88D800
	virtual int Print(char *buffer, int len);	// +0x10 0x88D820 ('GuildPlayerPermissionsPacket {Permissions}')

	GuildPermissionEnum Unknown0C;	// +0x0C (network order)
	int Unknown10;	// +0x10 (network order)
	int Unknown14;	// +0x14 (network order)
	int Unknown18;	// +0x18 (network order)
};

// opcode 212 (0x00D4, GUILD_RANK_NAMES_REQUEST_CLIENT): vtable 0xB0A9D4, constructor 0x88D8C0, 0x10 bytes
class GuildRankNamesRequestClientPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildRankNamesRequestClient {' and the PacketClass convention
{
public:
	GuildRankNamesRequestClientPacketClass(int player_id);	// 0x88D850
	GuildRankNamesRequestClientPacketClass(RawPacketClass *raw);	// 0x88D8C0
	virtual RawPacketClass *Build();	// +0x04 0x88D950
	virtual bool Process_Packet(void *client);	// +0x08 0x8691B0 (clientstub.cpp l.383)
	virtual int Get_Data_Size();	// +0x0C 0x88DA10
	virtual int Print(char *buffer, int len);	// +0x10 0x88DA30

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
};

// opcode 216 (0x00D8, ?): vtable 0xB0AA34, constructor 0x88E580, 0x2C bytes
class GuildGMRenameConfirmSectorPacket : public PacketClass
{
public:
	GuildGMRenameConfirmSectorPacket(RawPacketClass *raw);	// 0x88E580
	virtual RawPacketClass *Build();	// +0x04 0x88E6F0
	virtual bool Process_Packet(void *client);	// +0x08 0x73EEF0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x88E7C0
	virtual int Print(char *buffer, int len);	// +0x10 0x88E7F0

	std::string OldGuildName;	// +0x0C  // NAME 3: the client's Print text 'OldGuildName='
	std::string NewGuildName;	// +0x1C  // NAME 3: the client's Print text 'NewGuildName='
};

#endif
