// Earth & Beyond client (reconstruction): app\packets\pkguildranknamesrequestclient.cpp. Notes at the end of the file.
#include "pkguildranknamesrequestclient.h"
#include <stdio.h>


GuildSimpleSectorClientPacket::GuildSimpleSectorClientPacket(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(&OptionalParam);
}

RawPacketClass *GuildSimpleSectorClientPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(204, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(OptionalParam);
	return(raw);
}

int GuildSimpleSectorClientPacket::Get_Data_Size()
{
	return(sizeof(Type) + RawPacketClass::Get_Size(OptionalParam));
}

int GuildSimpleSectorClientPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildSimpleSectorClientPacket {Type=%d OptionalParam=%s}", Type, OptionalParam.c_str()));
}

GuildSimpleClientSectorPacketClass::GuildSimpleClientSectorPacketClass(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(PlayerID);
	raw->Get(&OptionalParam);
}

RawPacketClass *GuildSimpleClientSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(205, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(PlayerID);
	raw->Add(OptionalParam);
	return(raw);
}

int GuildSimpleClientSectorPacketClass::Get_Data_Size()
{
	return(sizeof(Type) + sizeof(PlayerID) + RawPacketClass::Get_Size(OptionalParam));
}

int GuildSimpleClientSectorPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildSimpleClientSectorPacket {Type=%d PlayerID=%d OptionalParam=%s}", Type, PlayerID, OptionalParam.c_str()));
}

GuildLeaderAcceptClientPacketClass::GuildLeaderAcceptClientPacketClass(int player_id, const char *guild_name, bool accept) : PlayerID(player_id), GuildName(guild_name)
{
	Accept = accept;
}

GuildLeaderAcceptClientPacketClass::GuildLeaderAcceptClientPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&GuildName);
	raw->Get(&Accept, sizeof(Accept));
}

GuildLeaderAcceptClientPacketClass::~GuildLeaderAcceptClientPacketClass()
{
}

RawPacketClass *GuildLeaderAcceptClientPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(197, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(GuildName);
	raw->Add(&Accept, sizeof(Accept));
	return(raw);
}

int GuildLeaderAcceptClientPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + sizeof(PlayerID) + sizeof(Accept));
}

int GuildLeaderAcceptClientPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildLeaderAcceptClient {PlayerID=%d GuildName=%s Accept=%s}", PlayerID, GuildName.c_str(), Accept ? "true" : "false"));
}

GuildRecruitConfirmSectorPacket::GuildRecruitConfirmSectorPacket(RawPacketClass *raw)
{
	raw->Get(&InviterName);
	raw->Get(&GuildName);
}

RawPacketClass *GuildRecruitConfirmSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(200, Get_Data_Size());
	raw->Add(InviterName);
	raw->Add(GuildName);
	return(raw);
}

int GuildRecruitConfirmSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(InviterName) + RawPacketClass::Get_Size(GuildName));
}

int GuildRecruitConfirmSectorPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildRecruitConfirmSector {InviterName=%s GuildName=%s}", InviterName.c_str(), GuildName.c_str()));
}

GuildRecruitAcceptClientPacketClass::GuildRecruitAcceptClientPacketClass(int player_id, bool accept) : PlayerID(player_id), Accept(accept)
{
}

GuildRecruitAcceptClientPacketClass::GuildRecruitAcceptClientPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&Accept, sizeof(Accept));
}

RawPacketClass *GuildRecruitAcceptClientPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(201, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(&Accept, sizeof(Accept));
	return(raw);
}

int GuildRecruitAcceptClientPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Accept));
}

int GuildRecruitAcceptClientPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildRecruitAcceptClient {PlayerID=%d Accept=%s}", PlayerID, Accept ? "true" : "false"));
}

GuildMessageSectorPacket::GuildMessageSectorPacket(RawPacketClass *raw)
{
	raw->Get(&MessageType, sizeof(MessageType));
	raw->Get(&OtherName);
	raw->Get(&GuildName);
}

RawPacketClass *GuildMessageSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(208, Get_Data_Size());
	raw->Add(&MessageType, sizeof(MessageType));
	raw->Add(OtherName);
	raw->Add(GuildName);
	return(raw);
}

int GuildMessageSectorPacket::Get_Data_Size()
{
	return(sizeof(MessageType) + RawPacketClass::Get_Size(OtherName) + RawPacketClass::Get_Size(GuildName));
}

int GuildMessageSectorPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildMessageSector {MessageType=%d OtherName=%s GuildName=%s}", MessageType, OtherName.c_str(), GuildName.c_str()));
}

GuildPlayerPermissionsPacket::GuildPlayerPermissionsPacket(GuildPermissionEnum unknown0c, int unknown10, int unknown14, int unknown18) : Unknown0C(unknown0c), Unknown10(unknown10), Unknown14(unknown14), Unknown18(unknown18)
{
}

GuildPlayerPermissionsPacket::GuildPlayerPermissionsPacket(RawPacketClass *raw)
{
	int value;
	raw->Get(value);
	Unknown0C = (GuildPermissionEnum)value;
	raw->Get(Unknown10);
	raw->Get(Unknown14);
	raw->Get(Unknown18);
}

RawPacketClass *GuildPlayerPermissionsPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(210, Get_Data_Size());
	int value = Unknown0C;
	raw->Add(value);
	raw->Add(Unknown10);
	raw->Add(Unknown14);
	raw->Add(Unknown18);
	return(raw);
}

int GuildPlayerPermissionsPacket::Get_Data_Size()
{
	return(sizeof(int) + sizeof(Unknown10) + sizeof(Unknown14) + sizeof(Unknown18));
}

int GuildPlayerPermissionsPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildPlayerPermissionsPacket {Permissions}"));
}

GuildRankNamesRequestClientPacketClass::GuildRankNamesRequestClientPacketClass(int player_id) : PlayerID(player_id)
{
}

GuildRankNamesRequestClientPacketClass::GuildRankNamesRequestClientPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
}

RawPacketClass *GuildRankNamesRequestClientPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(212, Get_Data_Size());
	raw->Add(PlayerID);
	return(raw);
}

int GuildRankNamesRequestClientPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int GuildRankNamesRequestClientPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildRankNamesRequestClient {PlayerID=%d}", PlayerID));
}

GuildRankNamesSectorPacket::GuildRankNamesSectorPacket(RawPacketClass *raw)
{
	int count = 0;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		char *name = raw->Get_String();
		int rank = 0;
		raw->Get(rank);
		Add(name, rank);
		delete [] name;
	}
}

RawPacketClass *GuildRankNamesSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(211, Get_Data_Size());
	int count = Ranks.size();
	raw->Add(count);
	for (std::vector<GuildRankNameStruct>::iterator it = Ranks.begin(); it != Ranks.end(); ++it) {
		raw->Add_String(it->Name.c_str());
		raw->Add(it->Rank);
	}
	return(raw);
}

int GuildRankNamesSectorPacket::Get_Data_Size()
{
	int size = sizeof(int);
	for (std::vector<GuildRankNameStruct>::iterator it = Ranks.begin(); it != Ranks.end(); ++it) {
		size += sizeof(it->Rank) + RawPacketClass::Get_Size(it->Name.c_str());
	}
	return(size);
}

int GuildRankNamesSectorPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildRankNamesSector {}"));
}

void GuildRankNamesSectorPacket::Add(const std::string &name, int rank)
{
	GuildRankNameStruct entry;
	entry.Name = name;
	entry.Rank = rank;
	Ranks.push_back(entry);
}

GuildGMRenameConfirmSectorPacket::GuildGMRenameConfirmSectorPacket(RawPacketClass *raw)
{
	raw->Get(&OldGuildName);
	raw->Get(&NewGuildName);
}

RawPacketClass *GuildGMRenameConfirmSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(216, Get_Data_Size());
	raw->Add(OldGuildName);
	raw->Add(NewGuildName);
	return(raw);
}

int GuildGMRenameConfirmSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(OldGuildName) + RawPacketClass::Get_Size(NewGuildName));
}

int GuildGMRenameConfirmSectorPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "GuildGMRenameConfirmSector {OldGuildName=%s NewGuildName=%s}", OldGuildName.c_str(), NewGuildName.c_str()));
}


/*
**	Reconstruction notes (pkguildranknamesrequestclient.cpp)
**
**	* Client 0x88BD40-0x88EC40 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 197, 201, 210, 211 (declared in pk_classes.h).
*/
