// Earth & Beyond client (reconstruction): app\packets\pkguildplayeronline.cpp. Notes at the end of the file.
#include "pkguildplayeronline.h"
#include <stdio.h>

GuildPlayerOnlinePacketClass::GuildPlayerOnlinePacketClass(__int64 player_id, const char *player_name, __int64 account_id) :
	PlayerID(player_id),
	PlayerName(player_name),
	AccountID(account_id)
{
}

GuildPlayerOnlinePacketClass::GuildPlayerOnlinePacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&PlayerName);
	raw->Get(AccountID);
}

GuildPlayerOnlinePacketClass::~GuildPlayerOnlinePacketClass()
{
}

RawPacketClass *GuildPlayerOnlinePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(193, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(PlayerName);
	raw->Add(AccountID);
	return(raw);
}

int GuildPlayerOnlinePacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(PlayerName) + sizeof(PlayerID) + sizeof(AccountID));
}

int GuildPlayerOnlinePacketClass::Print(char *buffer, int len)
{
	char player_id[32];
	char account_id[32];
	return(_snprintf(buffer, len, "GuildPlayerOnline { PlayerID=%s PlayerName=%s AccountID=%s }", Unknown_8B44E0(PlayerID, player_id), PlayerName.c_str(), Unknown_8B44E0(PlayerID, account_id)));
}

GuildStatusUpdatePacketClass::GuildStatusUpdatePacketClass(__int64 player_id, int unknown18, const char *rank_name, Vector3 same_guild_tag_color, Vector3 other_guild_tag_color, const char *new_guild_name) :
	PlayerID(player_id),
	Unknown18(unknown18),
	RankName(rank_name),
	SameGuildTagColor(same_guild_tag_color),
	OtherGuildTagColor(other_guild_tag_color),
	NewGuildName(new_guild_name)
{
}

GuildStatusUpdatePacketClass::GuildStatusUpdatePacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&Unknown18, sizeof(Unknown18));
	raw->Get(&RankName);
	raw->Get(&SameGuildTagColor, sizeof(SameGuildTagColor));
	raw->Get(&OtherGuildTagColor, sizeof(OtherGuildTagColor));
	raw->Get(&NewGuildName);
}

GuildStatusUpdatePacketClass::~GuildStatusUpdatePacketClass()
{
}

RawPacketClass *GuildStatusUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(195, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(&Unknown18, sizeof(Unknown18));
	raw->Add(RankName);
	raw->Add(&SameGuildTagColor, sizeof(SameGuildTagColor));
	raw->Add(&OtherGuildTagColor, sizeof(OtherGuildTagColor));
	raw->Add(NewGuildName);
	return(raw);
}

int GuildStatusUpdatePacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(NewGuildName) + RawPacketClass::Get_Size(RankName) + 36);
}

int GuildStatusUpdatePacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildStatusUpdate {PlayerID=%s RankName='%s' SameGuildTagColor=(%g,%g,%g) OtherGuildTagColor=(%g,%g,%g) NewGuildName='%s'}",
		Unknown_8B44E0(PlayerID, id), RankName.c_str(),
		SameGuildTagColor.X, SameGuildTagColor.Y, SameGuildTagColor.Z,
		OtherGuildTagColor.X, OtherGuildTagColor.Y, OtherGuildTagColor.Z, NewGuildName.c_str()));
}

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

GuildAdjustContribPacketClass::GuildAdjustContribPacketClass(__int64 player_id, int contrib, float amount) :
	PlayerID(player_id),
	Contrib(contrib),
	Amount(amount)
{
}

GuildAdjustContribPacketClass::GuildAdjustContribPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&Contrib, sizeof(Contrib));
	raw->Get(&Amount, sizeof(Amount));
}

RawPacketClass *GuildAdjustContribPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(194, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(&Contrib, sizeof(Contrib));
	raw->Add(&Amount, sizeof(Amount));
	return(raw);
}

int GuildAdjustContribPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Contrib) + sizeof(Amount));
}

int GuildAdjustContribPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildAdjustContrib {PlayerID=%s Contrib=%d Amount=%g}", Unknown_8B44E0(PlayerID, id), Contrib, Amount));
}

GuildBeginCreationPacketClass::GuildBeginCreationPacketClass(__int64 leader, const std::vector<__int64> &founders, const char *guild_name) :
	Leader(leader),
	Founders(founders),
	GuildName(guild_name)
{
}

GuildBeginCreationPacketClass::GuildBeginCreationPacketClass(RawPacketClass *raw)
{
	raw->Get(Leader);
	int count;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		__int64 founder;
		raw->Get(founder);
		Founders.push_back(founder);
	}
	raw->Get(&GuildName);
}

GuildBeginCreationPacketClass::~GuildBeginCreationPacketClass()
{
}

RawPacketClass *GuildBeginCreationPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(196, Get_Data_Size());
	raw->Add(Leader);
	int count = Founders.size();
	raw->Add(count);
	for (int i = 0; i < count; i++) {
		raw->Add(Founders[i]);
	}
	raw->Add(GuildName);
	return(raw);
}

int GuildBeginCreationPacketClass::Get_Data_Size()
{
	return(sizeof(Leader) + sizeof(int) + Founders.size() * sizeof(__int64) + RawPacketClass::Get_Size(GuildName));
}

int GuildBeginCreationPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildBeginCreation {Leader=%s Founders[%u] GuildName=%s}", Unknown_8B44E0(Leader, id), Founders.size(), GuildName.c_str()));
}

GuildLeaderAcceptSectorPacketClass::GuildLeaderAcceptSectorPacketClass(__int64 player_id, const char *guild_name, bool accept) :
	PlayerID(player_id),
	GuildName(guild_name),
	Accept(accept)
{
}

GuildLeaderAcceptSectorPacketClass::GuildLeaderAcceptSectorPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&GuildName);
	raw->Get(&Accept, sizeof(Accept));
}

GuildLeaderAcceptSectorPacketClass::~GuildLeaderAcceptSectorPacketClass()
{
}

RawPacketClass *GuildLeaderAcceptSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(198, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(GuildName);
	raw->Add(&Accept, sizeof(Accept));
	return(raw);
}

int GuildLeaderAcceptSectorPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + 9);
}

int GuildLeaderAcceptSectorPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildLeaderAcceptSector {PlayerID=%s GuildName=%s Accept=%s}", Unknown_8B44E0(PlayerID, id), GuildName.c_str(), Accept ? "true" : "false"));
}

GuildRecruitConfirmGuildPacketClass::GuildRecruitConfirmGuildPacketClass(__int64 player_id, const char *inviter_name, const char *guild_name) :
	PlayerID(player_id),
	InviterName(inviter_name),
	GuildName(guild_name)
{
}

GuildRecruitConfirmGuildPacketClass::GuildRecruitConfirmGuildPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&InviterName);
	raw->Get(&GuildName);
}

GuildRecruitConfirmGuildPacketClass::~GuildRecruitConfirmGuildPacketClass()
{
}

RawPacketClass *GuildRecruitConfirmGuildPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(199, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(InviterName);
	raw->Add(GuildName);
	return(raw);
}

int GuildRecruitConfirmGuildPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + RawPacketClass::Get_Size(InviterName) + 8);
}

int GuildRecruitConfirmGuildPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRecruitConfirmGuild {PlayerID=%s InviterName=%s GuildName=%s}", Unknown_8B44E0(PlayerID, id), InviterName.c_str(), GuildName.c_str()));
}

GuildRecruitAcceptSectorPacketClass::GuildRecruitAcceptSectorPacketClass(__int64 player_id, const char *inviter_name, const char *guild_name, bool accept) :
	PlayerID(player_id),
	InviterName(inviter_name),
	GuildName(guild_name),
	Accept(accept)
{
}

GuildRecruitAcceptSectorPacketClass::GuildRecruitAcceptSectorPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&InviterName);
	raw->Get(&GuildName);
	raw->Get(&Accept, sizeof(Accept));
}

GuildRecruitAcceptSectorPacketClass::~GuildRecruitAcceptSectorPacketClass()
{
}

RawPacketClass *GuildRecruitAcceptSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(202, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(InviterName);
	raw->Add(GuildName);
	raw->Add(&Accept, sizeof(Accept));
	return(raw);
}

int GuildRecruitAcceptSectorPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + RawPacketClass::Get_Size(InviterName) + 9);
}

int GuildRecruitAcceptSectorPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRecruitAcceptSector {PlayerID=%s InviterName=%s GuildName=%s Accept=%s}", Unknown_8B44E0(PlayerID, id), InviterName.c_str(), GuildName.c_str(), Accept ? "true" : "false"));
}

GuildMessageGuildPacketClass::GuildMessageGuildPacketClass(__int64 player_id, int message_type, const char *other_name, const char *guild_name) :
	PlayerID(player_id),
	MessageType(message_type),
	OtherName(other_name),
	GuildName(guild_name)
{
}

GuildMessageGuildPacketClass::GuildMessageGuildPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&MessageType, sizeof(MessageType));
	raw->Get(&OtherName);
	raw->Get(&GuildName);
}

GuildMessageGuildPacketClass::~GuildMessageGuildPacketClass()
{
}

RawPacketClass *GuildMessageGuildPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(207, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(&MessageType, sizeof(MessageType));
	raw->Add(OtherName);
	raw->Add(GuildName);
	return(raw);
}

int GuildMessageGuildPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + RawPacketClass::Get_Size(OtherName) + 12);
}

int GuildMessageGuildPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildMessageGuild {PlayerID=%s MessageType=%d OtherName=%s GuildName=%s}", Unknown_8B44E0(PlayerID, id), MessageType, OtherName.c_str(), GuildName.c_str()));
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

GuildRankNamesGuildPacketClass::GuildRankNamesGuildPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
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

RawPacketClass *GuildRankNamesGuildPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(213, Get_Data_Size());
	raw->Add(PlayerID);
	int count = Ranks.size();
	raw->Add(count);
	for (std::vector<GuildRankNameStruct>::iterator it = Ranks.begin(); it != Ranks.end(); ++it) {
		raw->Add_String(it->Name.c_str());
		raw->Add(it->Rank);
	}
	return(raw);
}

int GuildRankNamesGuildPacketClass::Get_Data_Size()
{
	int size = sizeof(PlayerID) + sizeof(int);
	for (std::vector<GuildRankNameStruct>::iterator it = Ranks.begin(); it != Ranks.end(); ++it) {
		size += sizeof(it->Rank) + RawPacketClass::Get_Size(it->Name.c_str());
	}
	return(size);
}

int GuildRankNamesGuildPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRankNamesGuild {PlayerID=%s}", Unknown_8B44E0(PlayerID, id)));
}

void GuildRankNamesGuildPacketClass::Add(const std::string &name, int rank)
{
	GuildRankNameStruct entry;
	entry.Name = name;
	entry.Rank = rank;
	Ranks.push_back(entry);
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

GuildGMRenameConfirmGuildPacketClass::GuildGMRenameConfirmGuildPacketClass(__int64 player_id, const char *old_guild_name, const char *new_guild_name) :
	PlayerID(player_id),
	OldGuildName(old_guild_name),
	NewGuildName(new_guild_name)
{
}

GuildGMRenameConfirmGuildPacketClass::GuildGMRenameConfirmGuildPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&OldGuildName);
	raw->Get(&NewGuildName);
}

GuildGMRenameConfirmGuildPacketClass::~GuildGMRenameConfirmGuildPacketClass()
{
}

RawPacketClass *GuildGMRenameConfirmGuildPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(215, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(OldGuildName);
	raw->Add(NewGuildName);
	return(raw);
}

int GuildGMRenameConfirmGuildPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(NewGuildName) + RawPacketClass::Get_Size(OldGuildName) + sizeof(PlayerID));
}

int GuildGMRenameConfirmGuildPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildGMRenameConfirmGuild {PlayerID=%s OldGuildName=%s NewGuildName=%s}", Unknown_8B44E0(PlayerID, id), OldGuildName.c_str(), NewGuildName.c_str()));
}

GuildGMRenameAcceptSectorPacketClass::GuildGMRenameAcceptSectorPacketClass(__int64 player_id, const char *old_guild_name, const char *new_guild_name) :
	PlayerID(player_id),
	OldGuildName(old_guild_name),
	NewGuildName(new_guild_name)
{
}

GuildGMRenameAcceptSectorPacketClass::GuildGMRenameAcceptSectorPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&OldGuildName);
	raw->Get(&NewGuildName);
}

GuildGMRenameAcceptSectorPacketClass::~GuildGMRenameAcceptSectorPacketClass()
{
}

RawPacketClass *GuildGMRenameAcceptSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(217, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(OldGuildName);
	raw->Add(NewGuildName);
	return(raw);
}

int GuildGMRenameAcceptSectorPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(NewGuildName) + RawPacketClass::Get_Size(OldGuildName) + sizeof(PlayerID));
}

int GuildGMRenameAcceptSectorPacketClass::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildGMRenameAcceptSector {PlayerID=%s OldGuildName=%s NewGuildName=%s}", Unknown_8B44E0(PlayerID, id), OldGuildName.c_str(), NewGuildName.c_str()));
}

GuildDeletePlayerPacketClass::GuildDeletePlayerPacketClass(__int64 player_id, int slot, const char *account_name, __int64 account_id) :
	PlayerID(player_id),
	Slot(slot),
	AccountID(account_id)
{
	Unknown_4A1590(AccountName, sizeof(AccountName), account_name);
}

GuildDeletePlayerPacketClass::GuildDeletePlayerPacketClass(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(Slot);
	raw->Get(AccountName, sizeof(AccountName));
	raw->Get(AccountID);
}

RawPacketClass *GuildDeletePlayerPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(218, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(Slot);
	raw->Add(AccountName, sizeof(AccountName));
	raw->Add(AccountID);
	return(raw);
}

int GuildDeletePlayerPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Slot) + sizeof(AccountName) + sizeof(AccountID));
}

int GuildDeletePlayerPacketClass::Print(char *buffer, int len)
{
	char player_id[32];
	char account_id[32];
	return(_snprintf(buffer, len, "GuildDeletePlayer { PlayerID=%s Slot=%d AccountName=%s AccountID=%s }", Unknown_8B44E0(PlayerID, player_id), Slot, AccountName, Unknown_8B44E0(AccountID, account_id)));
}

// NOT EXACT: scheduling (the client zeroes Unknown68 after its EH state store; attempts.csv)
GuildPlayerInfoDeletedPacketClass::GuildPlayerInfoDeletedPacketClass(__int64 player_id, int slot, const char *account_name, __int64 account_id) :
	PlayerID(player_id),
	Slot(slot),
	AccountID(account_id),
	Unknown68(0)
{
	Unknown_4A1590(AccountName, sizeof(AccountName), account_name);
}

// NOT EXACT: scheduling (as the value constructor; attempts.csv)
GuildPlayerInfoDeletedPacketClass::GuildPlayerInfoDeletedPacketClass(RawPacketClass *raw) :
	Unknown68(0)
{
	raw->Get(PlayerID);
	raw->Get(Slot);
	raw->Get(AccountName, sizeof(AccountName));
	raw->Get(AccountID);
}

RawPacketClass *GuildPlayerInfoDeletedPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(219, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(Slot);
	raw->Add(AccountName, sizeof(AccountName));
	raw->Add(AccountID);
	return(raw);
}

int GuildPlayerInfoDeletedPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Slot) + sizeof(AccountName) + sizeof(AccountID));
}

int GuildPlayerInfoDeletedPacketClass::Print(char *buffer, int len)
{
	char player_id[32];
	char account_id[32];
	return(_snprintf(buffer, len, "GuildPlayerInfoDeleted { PlayerID=%s Slot=%d AccountName=%s AccountID=%s }", Unknown_8B44E0(PlayerID, player_id), Slot, AccountName, Unknown_8B44E0(AccountID, account_id)));
}


/*
**	Reconstruction notes (pkguildplayeronline.cpp)
**
**	* Client 0x88EC70-0x894390 (0x88EC40 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x894390): the guild packets 193-220 not in pkguildranknamesrequestclient.cpp. Written by hand
**	  (builder P4, 2026-10-27); each class's value constructor comes first, then the raw one.
**	* The file name is provisional (no assert names it).
**	* Not rebuilt yet: opcode 220 (declared in pk_classes.h: its MoneyClass NumCredits goes through money.cpp
**	  0x4F00C0 and a packet-data wrapper, vtable 0xB0ADC4).
*/
