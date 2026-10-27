/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkguildplayeronline.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x88EC70-0x894390 rebuilt so far (opcode 193, 203, 206, 194, 198, 199, 202, 207, 209, 214, 215, 217); the file's other
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

// opcode 193 (0x00C1, ?): vtable 0xB0AA64, constructor 0x88EE20, 0x30 bytes
class GuildPlayerOnlinePacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildPlayerOnline {' and the PacketClass convention
{
public:
	GuildPlayerOnlinePacketClass(__int64 player_id, const char *player_name, __int64 account_id);	// 0x88EC70
	GuildPlayerOnlinePacketClass(RawPacketClass *raw);	// 0x88EE20
	virtual ~GuildPlayerOnlinePacketClass();	// +0x00 0x88EDF0 / 0x88EEF0
	virtual RawPacketClass *Build();	// +0x04 0x88EF60
	virtual bool Process_Packet(void *client);	// +0x08 0x8692A0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x88F030
	virtual int Print(char *buffer, int len);	// +0x10 0x88F050 (its AccountID= shows PlayerID, as the client's)

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string PlayerName;	// +0x18  // NAME 3: the client's Print text 'PlayerName='
	__int64 AccountID;	// +0x28  // NAME 3: the client's Print text 'AccountID='
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

// opcode 194 (0x00C2, ?): vtable 0xB0AAF4, constructor 0x88FA00, 0x20 bytes
class GuildAdjustContribPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildAdjustContrib {' and the PacketClass convention
{
public:
	GuildAdjustContribPacketClass(__int64 player_id, int contrib, float amount);	// 0x88F960
	GuildAdjustContribPacketClass(RawPacketClass *raw);	// 0x88FA00
	virtual RawPacketClass *Build();	// +0x04 0x88FAE0
	virtual bool Process_Packet(void *client);	// +0x08 0x868FD0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x88FBC0
	virtual int Print(char *buffer, int len);	// +0x10 0x88FBE0

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	int Contrib;	// +0x18 (4 bytes as they are)  // NAME 3: the client's Print text 'Contrib='
	float Amount;	// +0x1C (4 bytes as they are)  // NAME 3: the client's Print text 'Amount='
};

// opcode 198 (0x00C6, ?): vtable 0xB0AB54, constructor 0x890540, 0x30 bytes
class GuildLeaderAcceptSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildLeaderAcceptSector {' and the PacketClass convention
{
public:
	GuildLeaderAcceptSectorPacketClass(__int64 player_id, const char *guild_name, bool accept);	// 0x8903A0
	GuildLeaderAcceptSectorPacketClass(RawPacketClass *raw);	// 0x890540
	virtual ~GuildLeaderAcceptSectorPacketClass();	// +0x00 0x890510 / 0x890610
	virtual RawPacketClass *Build();	// +0x04 0x890680
	virtual bool Process_Packet(void *client);	// +0x08 0x869060 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x890750
	virtual int Print(char *buffer, int len);	// +0x10 0x890770

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string GuildName;	// +0x18  // NAME 3: the client's Print text 'GuildName='
	bool Accept;	// +0x28 (1 byte as it is)  // NAME 3: the client's Print text 'Accept='
};

// opcode 199 (0x00C7, ?): vtable 0xB0AB84, constructor 0x890A00, 0x38 bytes
class GuildRecruitConfirmGuildPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildRecruitConfirmGuild {' and the PacketClass convention
{
public:
	GuildRecruitConfirmGuildPacketClass(__int64 player_id, const char *inviter_name, const char *guild_name);	// 0x8907F0
	GuildRecruitConfirmGuildPacketClass(RawPacketClass *raw);	// 0x890A00
	virtual ~GuildRecruitConfirmGuildPacketClass();	// +0x00 0x8909D0 / 0x890AE0
	virtual RawPacketClass *Build();	// +0x04 0x890B80
	virtual bool Process_Packet(void *client);	// +0x08 0x869090 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x890C50
	virtual int Print(char *buffer, int len);	// +0x10 0x890C90

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string InviterName;	// +0x18  // NAME 3: the client's Print text 'InviterName='
	std::string GuildName;	// +0x28  // NAME 3: the client's Print text 'GuildName='
};

// opcode 202 (0x00CA, ?): vtable 0xB0ABB4, constructor 0x890F30, 0x40 bytes
class GuildRecruitAcceptSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildRecruitAcceptSector {' and the PacketClass convention
{
public:
	GuildRecruitAcceptSectorPacketClass(__int64 player_id, const char *inviter_name, const char *guild_name, bool accept);	// 0x890D00
	GuildRecruitAcceptSectorPacketClass(RawPacketClass *raw);	// 0x890F30
	virtual ~GuildRecruitAcceptSectorPacketClass();	// +0x00 0x890F00 / 0x891020
	virtual RawPacketClass *Build();	// +0x04 0x8910C0
	virtual bool Process_Packet(void *client);	// +0x08 0x8690F0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8911A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8911E0

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string InviterName;	// +0x18  // NAME 3: the client's Print text 'InviterName='
	std::string GuildName;	// +0x28  // NAME 3: the client's Print text 'GuildName='
	bool Accept;	// +0x38 (1 byte as it is)  // NAME 3: the client's Print text 'Accept='
};

// opcode 207 (0x00CF, ?): vtable 0xB0ABE4, constructor 0x8914A0, 0x40 bytes
class GuildMessageGuildPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildMessageGuild {' and the PacketClass convention
{
public:
	GuildMessageGuildPacketClass(__int64 player_id, int message_type, const char *other_name, const char *guild_name);	// 0x891270
	GuildMessageGuildPacketClass(RawPacketClass *raw);	// 0x8914A0
	virtual ~GuildMessageGuildPacketClass();	// +0x00 0x891470 / 0x891590
	virtual RawPacketClass *Build();	// +0x04 0x891630
	virtual bool Process_Packet(void *client);	// +0x08 0x869120 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x891710
	virtual int Print(char *buffer, int len);	// +0x10 0x891750

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	int MessageType;	// +0x18 (4 bytes as they are)  // NAME 3: the client's Print text 'MessageType='
	std::string OtherName;	// +0x1C  // NAME 3: the client's Print text 'OtherName='
	std::string GuildName;	// +0x2C  // NAME 3: the client's Print text 'GuildName='
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

// opcode 215 (0x00D7, ?): vtable 0xB0ACD4, constructor 0x892E10, 0x38 bytes
class GuildGMRenameConfirmGuildPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildGMRenameConfirmGuild {' and the PacketClass convention
{
public:
	GuildGMRenameConfirmGuildPacketClass(__int64 player_id, const char *old_guild_name, const char *new_guild_name);	// 0x892C00
	GuildGMRenameConfirmGuildPacketClass(RawPacketClass *raw);	// 0x892E10
	virtual ~GuildGMRenameConfirmGuildPacketClass();	// +0x00 0x892DE0 / 0x892EF0
	virtual RawPacketClass *Build();	// +0x04 0x892F90
	virtual bool Process_Packet(void *client);	// +0x08 0x869240 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x893060
	virtual int Print(char *buffer, int len);	// +0x10 0x8930A0

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string OldGuildName;	// +0x18  // NAME 3: the client's Print text 'OldGuildName='
	std::string NewGuildName;	// +0x28  // NAME 3: the client's Print text 'NewGuildName='
};

// opcode 217 (0x00D9, ?): vtable 0xB0AD04, constructor 0x893320, 0x38 bytes
class GuildGMRenameAcceptSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GuildGMRenameAcceptSector {' and the PacketClass convention
{
public:
	GuildGMRenameAcceptSectorPacketClass(__int64 player_id, const char *old_guild_name, const char *new_guild_name);	// 0x893110
	GuildGMRenameAcceptSectorPacketClass(RawPacketClass *raw);	// 0x893320
	virtual ~GuildGMRenameAcceptSectorPacketClass();	// +0x00 0x8932F0 / 0x893400
	virtual RawPacketClass *Build();	// +0x04 0x8934A0
	virtual bool Process_Packet(void *client);	// +0x08 0x869270 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x893570
	virtual int Print(char *buffer, int len);	// +0x10 0x8935B0

	__int64 PlayerID;	// +0x10  // NAME 3: the client's Print text 'PlayerID='
	std::string OldGuildName;	// +0x18  // NAME 3: the client's Print text 'OldGuildName='
	std::string NewGuildName;	// +0x28  // NAME 3: the client's Print text 'NewGuildName='
};

#endif
