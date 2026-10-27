// Earth & Beyond client (reconstruction): app\packets\pkguildplayeronline.cpp. Notes at the end of the file.
#include "pkguildplayeronline.h"
#include <stdio.h>

GuildSimpleGuildSectorPacketClass::GuildSimpleGuildSectorPacketClass(int type, __int64 player_id, const char *optional_param) :
	Type(type),
	PlayerID(player_id),
	OptionalParam(optional_param)
{
}

GuildSimpleGuildSectorPacketClass::GuildSimpleGuildSectorPacketClass(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(PlayerID);
	raw->Get(&OptionalParam);
}

GuildSimpleGuildSectorPacketClass::~GuildSimpleGuildSectorPacketClass()
{
}

RawPacketClass *GuildSimpleGuildSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(203, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(PlayerID);
	raw->Add(OptionalParam);
	return(raw);
}

int GuildSimpleGuildSectorPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(OptionalParam) + sizeof(Type) + sizeof(PlayerID));
}

int GuildSimpleGuildSectorPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildSimpleGuildSectorPacket {Type=%d PlayerID=%s OptionalParam=%s}", Type, Unknown_8B44E0(PlayerID, id), OptionalParam.c_str()));
}

GuildSimpleSectorGuildPacketClass::GuildSimpleSectorGuildPacketClass(int type, __int64 player_id, const char *optional_param) :
	Type(type),
	PlayerID(player_id),
	OptionalParam(optional_param)
{
}

GuildSimpleSectorGuildPacketClass::GuildSimpleSectorGuildPacketClass(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(PlayerID);
	raw->Get(&OptionalParam);
}

GuildSimpleSectorGuildPacketClass::~GuildSimpleSectorGuildPacketClass()
{
}

RawPacketClass *GuildSimpleSectorGuildPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(206, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(PlayerID);
	raw->Add(OptionalParam);
	return(raw);
}

int GuildSimpleSectorGuildPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(OptionalParam) + sizeof(Type) + sizeof(PlayerID));
}

int GuildSimpleSectorGuildPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildSimpleSectorGuildPacket {Type=%d PlayerID=%s OptionalParam=%s}", Type, Unknown_8B44E0(PlayerID, id), OptionalParam.c_str()));
}

GuildPermissionsPacketClass::GuildPermissionsPacketClass(__int64 player_id, Unknown_GuildPermissionsEnum unknown18, int unknown1c, int unknown20, int unknown24) :
	PlayerID(player_id),
	Unknown18(unknown18),
	Unknown1C(unknown1c),
	Unknown20(unknown20),
	Unknown24(unknown24)
{
}

GuildPermissionsPacketClass::GuildPermissionsPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	int value;
	raw->Get(value);
	Unknown18 = (Unknown_GuildPermissionsEnum)value;
	raw->Get(Unknown1C);
	raw->Get(Unknown20);
	raw->Get(Unknown24);
}

RawPacketClass *GuildPermissionsPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(209, Get_Data_Size());
	raw->Add(PlayerID);
	int value = Unknown18;
	raw->Add(value);
	raw->Add(Unknown1C);
	raw->Add(Unknown20);
	raw->Add(Unknown24);
	return(raw);
}

int GuildPermissionsPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(int) + sizeof(Unknown1C) + sizeof(Unknown20) + sizeof(Unknown24));
}

int GuildPermissionsPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildPermissionsPacket {PlayerID=%s}", Unknown_8B44E0(PlayerID, id)));
}

GuildRankNamesRequestSectorPacketClass::GuildRankNamesRequestSectorPacketClass(__int64 player_id) :
	PlayerID(player_id)
{
}

GuildRankNamesRequestSectorPacketClass::GuildRankNamesRequestSectorPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
}

RawPacketClass *GuildRankNamesRequestSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(214, Get_Data_Size());
	raw->Add(PlayerID);
	return(raw);
}

int GuildRankNamesRequestSectorPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int GuildRankNamesRequestSectorPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRankNamesRequestSector {PlayerID=%s}", Unknown_8B44E0(PlayerID, id)));
}


/*
**	Reconstruction notes (pkguildplayeronline.cpp)
**
**	* Client 0x88EC70-0x894390 (0x88EC40 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x894390): the guild packets 193-220 not in pkguildranknamesrequestclient.cpp. Written by hand
**	  (builder P4, 2026-10-27); each class's value constructor comes first, then the raw one.
**	* The file name is provisional (no assert names it).
**	* Not rebuilt yet: opcode 193, 194, 195, 196, 198, 199, 202, 207, 213, 215, 217, 218, 219,
**	  220 (declared in pk_classes.h).
*/
