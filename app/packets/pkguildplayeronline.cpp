// Earth & Beyond client (reconstruction): app\packets\pkguildplayeronline.cpp. Notes at the end of the file.
#include "pkguildplayeronline.h"
#include <stdio.h>
#include "..\shared\auth_product.h"
GuildPlayerOnlineSectorPacket::GuildPlayerOnlineSectorPacket(__int64 player_id, const char *player_name, __int64 account_id) :
	PlayerID(player_id),
	PlayerName(player_name),
	AccountID(account_id)
{
}

GuildPlayerOnlineSectorPacket::GuildPlayerOnlineSectorPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&PlayerName);
	raw->Get(AccountID);
}

GuildPlayerOnlineSectorPacket::~GuildPlayerOnlineSectorPacket()
{
}

RawPacketClass *GuildPlayerOnlineSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(193, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(PlayerName);
	raw->Add(AccountID);
	return(raw);
}

int GuildPlayerOnlineSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(PlayerName) + sizeof(PlayerID) + sizeof(AccountID));
}

int GuildPlayerOnlineSectorPacket::Print(char *buffer, int len)
{
	char player_id[32];
	char account_id[32];
	return(_snprintf(buffer, len, "GuildPlayerOnline { PlayerID=%s PlayerName=%s AccountID=%s }", Unknown_8B44E0(PlayerID, player_id), PlayerName.c_str(), Unknown_8B44E0(PlayerID, account_id)));
}

GuildStatusUpdatePacket::GuildStatusUpdatePacket(__int64 player_id, int unknown18, const char *rank_name, Vector3 same_guild_tag_color, Vector3 other_guild_tag_color, const char *new_guild_name) :
	PlayerID(player_id),
	Unknown18(unknown18),
	RankName(rank_name),
	SameGuildTagColor(same_guild_tag_color),
	OtherGuildTagColor(other_guild_tag_color),
	NewGuildName(new_guild_name)
{
}

GuildStatusUpdatePacket::GuildStatusUpdatePacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&Unknown18, sizeof(Unknown18));
	raw->Get(&RankName);
	raw->Get(&SameGuildTagColor, sizeof(SameGuildTagColor));
	raw->Get(&OtherGuildTagColor, sizeof(OtherGuildTagColor));
	raw->Get(&NewGuildName);
}

GuildStatusUpdatePacket::~GuildStatusUpdatePacket()
{
}

RawPacketClass *GuildStatusUpdatePacket::Build()
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

int GuildStatusUpdatePacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(NewGuildName) + RawPacketClass::Get_Size(RankName) + 36);
}

int GuildStatusUpdatePacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildStatusUpdate {PlayerID=%s RankName='%s' SameGuildTagColor=(%g,%g,%g) OtherGuildTagColor=(%g,%g,%g) NewGuildName='%s'}",
		Unknown_8B44E0(PlayerID, id), RankName.c_str(),
		SameGuildTagColor.X, SameGuildTagColor.Y, SameGuildTagColor.Z,
		OtherGuildTagColor.X, OtherGuildTagColor.Y, OtherGuildTagColor.Z, NewGuildName.c_str()));
}

GuildSimpleGuildSectorPacket::GuildSimpleGuildSectorPacket(int type, __int64 player_id, const char *optional_param) :
	Type(type),
	PlayerID(player_id),
	OptionalParam(optional_param)
{
}

GuildSimpleGuildSectorPacket::GuildSimpleGuildSectorPacket(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(PlayerID);
	raw->Get(&OptionalParam);
}

GuildSimpleGuildSectorPacket::~GuildSimpleGuildSectorPacket()
{
}

RawPacketClass *GuildSimpleGuildSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(203, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(PlayerID);
	raw->Add(OptionalParam);
	return(raw);
}

int GuildSimpleGuildSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(OptionalParam) + sizeof(Type) + sizeof(PlayerID));
}

int GuildSimpleGuildSectorPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildSimpleGuildSectorPacket {Type=%d PlayerID=%s OptionalParam=%s}", Type, Unknown_8B44E0(PlayerID, id), OptionalParam.c_str()));
}

GuildSimpleSectorGuildPacket::GuildSimpleSectorGuildPacket(int type, __int64 player_id, const char *optional_param) :
	Type(type),
	PlayerID(player_id),
	OptionalParam(optional_param)
{
}

GuildSimpleSectorGuildPacket::GuildSimpleSectorGuildPacket(RawPacketClass *raw)
{
	raw->Get(&Type, sizeof(Type));
	raw->Get(PlayerID);
	raw->Get(&OptionalParam);
}

GuildSimpleSectorGuildPacket::~GuildSimpleSectorGuildPacket()
{
}

RawPacketClass *GuildSimpleSectorGuildPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(206, Get_Data_Size());
	raw->Add(&Type, sizeof(Type));
	raw->Add(PlayerID);
	raw->Add(OptionalParam);
	return(raw);
}

int GuildSimpleSectorGuildPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(OptionalParam) + sizeof(Type) + sizeof(PlayerID));
}

int GuildSimpleSectorGuildPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildSimpleSectorGuildPacket {Type=%d PlayerID=%s OptionalParam=%s}", Type, Unknown_8B44E0(PlayerID, id), OptionalParam.c_str()));
}

GuildAdjustContribPacket::GuildAdjustContribPacket(__int64 player_id, int contrib, float amount) :
	PlayerID(player_id),
	Contrib(contrib),
	Amount(amount)
{
}

GuildAdjustContribPacket::GuildAdjustContribPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&Contrib, sizeof(Contrib));
	raw->Get(&Amount, sizeof(Amount));
}

RawPacketClass *GuildAdjustContribPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(194, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(&Contrib, sizeof(Contrib));
	raw->Add(&Amount, sizeof(Amount));
	return(raw);
}

int GuildAdjustContribPacket::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Contrib) + sizeof(Amount));
}

int GuildAdjustContribPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildAdjustContrib {PlayerID=%s Contrib=%d Amount=%g}", Unknown_8B44E0(PlayerID, id), Contrib, Amount));
}

GuildBeginCreationPacket::GuildBeginCreationPacket(__int64 leader, const std::vector<__int64> &founders, const char *guild_name) :
	Leader(leader),
	Founders(founders),
	GuildName(guild_name)
{
}

GuildBeginCreationPacket::GuildBeginCreationPacket(RawPacketClass *raw)
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

GuildBeginCreationPacket::~GuildBeginCreationPacket()
{
}

RawPacketClass *GuildBeginCreationPacket::Build()
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

int GuildBeginCreationPacket::Get_Data_Size()
{
	return(sizeof(Leader) + sizeof(int) + Founders.size() * sizeof(__int64) + RawPacketClass::Get_Size(GuildName));
}

int GuildBeginCreationPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildBeginCreation {Leader=%s Founders[%u] GuildName=%s}", Unknown_8B44E0(Leader, id), Founders.size(), GuildName.c_str()));
}

GuildLeaderAcceptSectorPacket::GuildLeaderAcceptSectorPacket(__int64 player_id, const char *guild_name, bool accept) :
	PlayerID(player_id),
	GuildName(guild_name),
	Accept(accept)
{
}

GuildLeaderAcceptSectorPacket::GuildLeaderAcceptSectorPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&GuildName);
	raw->Get(&Accept, sizeof(Accept));
}

GuildLeaderAcceptSectorPacket::~GuildLeaderAcceptSectorPacket()
{
}

RawPacketClass *GuildLeaderAcceptSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(198, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(GuildName);
	raw->Add(&Accept, sizeof(Accept));
	return(raw);
}

int GuildLeaderAcceptSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + 9);
}

int GuildLeaderAcceptSectorPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildLeaderAcceptSector {PlayerID=%s GuildName=%s Accept=%s}", Unknown_8B44E0(PlayerID, id), GuildName.c_str(), Accept ? "true" : "false"));
}

GuildRecruitConfirmGuildPacket::GuildRecruitConfirmGuildPacket(__int64 player_id, const char *inviter_name, const char *guild_name) :
	PlayerID(player_id),
	InviterName(inviter_name),
	GuildName(guild_name)
{
}

GuildRecruitConfirmGuildPacket::GuildRecruitConfirmGuildPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&InviterName);
	raw->Get(&GuildName);
}

GuildRecruitConfirmGuildPacket::~GuildRecruitConfirmGuildPacket()
{
}

RawPacketClass *GuildRecruitConfirmGuildPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(199, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(InviterName);
	raw->Add(GuildName);
	return(raw);
}

int GuildRecruitConfirmGuildPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + RawPacketClass::Get_Size(InviterName) + 8);
}

int GuildRecruitConfirmGuildPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRecruitConfirmGuild {PlayerID=%s InviterName=%s GuildName=%s}", Unknown_8B44E0(PlayerID, id), InviterName.c_str(), GuildName.c_str()));
}

GuildRecruitAcceptSectorPacket::GuildRecruitAcceptSectorPacket(__int64 player_id, const char *inviter_name, const char *guild_name, bool accept) :
	PlayerID(player_id),
	InviterName(inviter_name),
	GuildName(guild_name),
	Accept(accept)
{
}

GuildRecruitAcceptSectorPacket::GuildRecruitAcceptSectorPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&InviterName);
	raw->Get(&GuildName);
	raw->Get(&Accept, sizeof(Accept));
}

GuildRecruitAcceptSectorPacket::~GuildRecruitAcceptSectorPacket()
{
}

RawPacketClass *GuildRecruitAcceptSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(202, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(InviterName);
	raw->Add(GuildName);
	raw->Add(&Accept, sizeof(Accept));
	return(raw);
}

int GuildRecruitAcceptSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + RawPacketClass::Get_Size(InviterName) + 9);
}

int GuildRecruitAcceptSectorPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRecruitAcceptSector {PlayerID=%s InviterName=%s GuildName=%s Accept=%s}", Unknown_8B44E0(PlayerID, id), InviterName.c_str(), GuildName.c_str(), Accept ? "true" : "false"));
}

GuildMessageGuildPacket::GuildMessageGuildPacket(__int64 player_id, int message_type, const char *other_name, const char *guild_name) :
	PlayerID(player_id),
	MessageType(message_type),
	OtherName(other_name),
	GuildName(guild_name)
{
}

GuildMessageGuildPacket::GuildMessageGuildPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&MessageType, sizeof(MessageType));
	raw->Get(&OtherName);
	raw->Get(&GuildName);
}

GuildMessageGuildPacket::~GuildMessageGuildPacket()
{
}

RawPacketClass *GuildMessageGuildPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(207, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(&MessageType, sizeof(MessageType));
	raw->Add(OtherName);
	raw->Add(GuildName);
	return(raw);
}

int GuildMessageGuildPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(GuildName) + RawPacketClass::Get_Size(OtherName) + 12);
}

int GuildMessageGuildPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildMessageGuild {PlayerID=%s MessageType=%d OtherName=%s GuildName=%s}", Unknown_8B44E0(PlayerID, id), MessageType, OtherName.c_str(), GuildName.c_str()));
}

GuildPermissionsPacket::GuildPermissionsPacket(__int64 player_id, Unknown_GuildPermissionsEnum unknown18, int unknown1c, int unknown20, int unknown24) :
	PlayerID(player_id),
	Unknown18(unknown18),
	Unknown1C(unknown1c),
	Unknown20(unknown20),
	Unknown24(unknown24)
{
}

GuildPermissionsPacket::GuildPermissionsPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	int value;
	raw->Get(value);
	Unknown18 = (Unknown_GuildPermissionsEnum)value;
	raw->Get(Unknown1C);
	raw->Get(Unknown20);
	raw->Get(Unknown24);
}

RawPacketClass *GuildPermissionsPacket::Build()
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

int GuildPermissionsPacket::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(int) + sizeof(Unknown1C) + sizeof(Unknown20) + sizeof(Unknown24));
}

int GuildPermissionsPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildPermissionsPacket {PlayerID=%s}", Unknown_8B44E0(PlayerID, id)));
}

GuildRankNamesGuildPacket::GuildRankNamesGuildPacket(__int64 player_id, const std::vector<GuildRankNameStruct> &ranks) :
	PlayerID(player_id),
	Ranks(ranks)
{
}

GuildRankNamesGuildPacket::GuildRankNamesGuildPacket(RawPacketClass *raw)
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

RawPacketClass *GuildRankNamesGuildPacket::Build()
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

int GuildRankNamesGuildPacket::Get_Data_Size()
{
	int size = sizeof(PlayerID) + sizeof(int);
	for (std::vector<GuildRankNameStruct>::iterator it = Ranks.begin(); it != Ranks.end(); ++it) {
		size += sizeof(it->Rank) + RawPacketClass::Get_Size(it->Name.c_str());
	}
	return(size);
}

int GuildRankNamesGuildPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRankNamesGuild {PlayerID=%s}", Unknown_8B44E0(PlayerID, id)));
}

void GuildRankNamesGuildPacket::Add(const std::string &name, int rank)
{
	GuildRankNameStruct entry;
	entry.Name = name;
	entry.Rank = rank;
	Ranks.push_back(entry);
}

GuildRankNamesRequestSectorPacket::GuildRankNamesRequestSectorPacket(__int64 player_id) :
	PlayerID(player_id)
{
}

GuildRankNamesRequestSectorPacket::GuildRankNamesRequestSectorPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
}

RawPacketClass *GuildRankNamesRequestSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(214, Get_Data_Size());
	raw->Add(PlayerID);
	return(raw);
}

int GuildRankNamesRequestSectorPacket::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int GuildRankNamesRequestSectorPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildRankNamesRequestSector {PlayerID=%s}", Unknown_8B44E0(PlayerID, id)));
}

GuildGMRenameConfirmGuildPacket::GuildGMRenameConfirmGuildPacket(__int64 player_id, const char *old_guild_name, const char *new_guild_name) :
	PlayerID(player_id),
	OldGuildName(old_guild_name),
	NewGuildName(new_guild_name)
{
}

GuildGMRenameConfirmGuildPacket::GuildGMRenameConfirmGuildPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&OldGuildName);
	raw->Get(&NewGuildName);
}

GuildGMRenameConfirmGuildPacket::~GuildGMRenameConfirmGuildPacket()
{
}

RawPacketClass *GuildGMRenameConfirmGuildPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(215, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(OldGuildName);
	raw->Add(NewGuildName);
	return(raw);
}

int GuildGMRenameConfirmGuildPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(NewGuildName) + RawPacketClass::Get_Size(OldGuildName) + sizeof(PlayerID));
}

int GuildGMRenameConfirmGuildPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildGMRenameConfirmGuild {PlayerID=%s OldGuildName=%s NewGuildName=%s}", Unknown_8B44E0(PlayerID, id), OldGuildName.c_str(), NewGuildName.c_str()));
}

GuildGMRenameAcceptSectorPacket::GuildGMRenameAcceptSectorPacket(__int64 player_id, const char *old_guild_name, const char *new_guild_name) :
	PlayerID(player_id),
	OldGuildName(old_guild_name),
	NewGuildName(new_guild_name)
{
}

GuildGMRenameAcceptSectorPacket::GuildGMRenameAcceptSectorPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(&OldGuildName);
	raw->Get(&NewGuildName);
}

GuildGMRenameAcceptSectorPacket::~GuildGMRenameAcceptSectorPacket()
{
}

RawPacketClass *GuildGMRenameAcceptSectorPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(217, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(OldGuildName);
	raw->Add(NewGuildName);
	return(raw);
}

int GuildGMRenameAcceptSectorPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(NewGuildName) + RawPacketClass::Get_Size(OldGuildName) + sizeof(PlayerID));
}

int GuildGMRenameAcceptSectorPacket::Print(char *buffer, int len)
{
	char id[32];
	return(_snprintf(buffer, len, "GuildGMRenameAcceptSector {PlayerID=%s OldGuildName=%s NewGuildName=%s}", Unknown_8B44E0(PlayerID, id), OldGuildName.c_str(), NewGuildName.c_str()));
}

GuildDeletePlayerPacket::GuildDeletePlayerPacket(__int64 player_id, int slot, const char *account_name, __int64 account_id) :
	PlayerID(player_id),
	Slot(slot),
	AccountID(account_id)
{
	Bounded_Strcpy(AccountName, sizeof(AccountName), account_name);
}

GuildDeletePlayerPacket::GuildDeletePlayerPacket(RawPacketClass *raw)
{
	raw->Get(PlayerID);
	raw->Get(Slot);
	raw->Get(AccountName, sizeof(AccountName));
	raw->Get(AccountID);
}

RawPacketClass *GuildDeletePlayerPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(218, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(Slot);
	raw->Add(AccountName, sizeof(AccountName));
	raw->Add(AccountID);
	return(raw);
}

int GuildDeletePlayerPacket::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Slot) + sizeof(AccountName) + sizeof(AccountID));
}

int GuildDeletePlayerPacket::Print(char *buffer, int len)
{
	char player_id[32];
	char account_id[32];
	return(_snprintf(buffer, len, "GuildDeletePlayer { PlayerID=%s Slot=%d AccountName=%s AccountID=%s }", Unknown_8B44E0(PlayerID, player_id), Slot, AccountName, Unknown_8B44E0(AccountID, account_id)));
}

GuildPlayerInfoDeletedPacket::GuildPlayerInfoDeletedPacket(__int64 player_id, int slot, const char *account_name, __int64 account_id) :
	PlayerID(player_id),
	Slot(slot),
	AccountID(account_id),
	Unknown68(0),
	Unknown6C(0)
{
	Bounded_Strcpy(AccountName, sizeof(AccountName), account_name);
}

GuildPlayerInfoDeletedPacket::GuildPlayerInfoDeletedPacket(RawPacketClass *raw) :
	Unknown68(0),
	Unknown6C(0)
{
	raw->Get(PlayerID);
	raw->Get(Slot);
	raw->Get(AccountName, sizeof(AccountName));
	raw->Get(AccountID);
}

RawPacketClass *GuildPlayerInfoDeletedPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(219, Get_Data_Size());
	raw->Add(PlayerID);
	raw->Add(Slot);
	raw->Add(AccountName, sizeof(AccountName));
	raw->Add(AccountID);
	return(raw);
}

int GuildPlayerInfoDeletedPacket::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Slot) + sizeof(AccountName) + sizeof(AccountID));
}

int GuildPlayerInfoDeletedPacket::Print(char *buffer, int len)
{
	char player_id[32];
	char account_id[32];
	return(_snprintf(buffer, len, "GuildPlayerInfoDeleted { PlayerID=%s Slot=%d AccountName=%s AccountID=%s }", Unknown_8B44E0(PlayerID, player_id), Slot, AccountName, Unknown_8B44E0(AccountID, account_id)));
}

GuildDeductFeePacket::GuildDeductFeePacket(__int64 player_id, const MoneyClass &num_credits, const char *description) :
	PlayerID(player_id),
	NumCredits(num_credits),
	Description(description)
{
}

GuildDeductFeePacket::GuildDeductFeePacket(RawPacketClass *raw) :
	PlayerID(0),
	NumCredits(),
	Description("")
{
	raw->Get(PlayerID);
	MoneyPacketDataClass money(&NumCredits);
	raw->Get(money);
	raw->Get(&Description);
}

GuildDeductFeePacket::~GuildDeductFeePacket()
{
}

RawPacketClass *GuildDeductFeePacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(220, Get_Data_Size());
	raw->Add(PlayerID);
	MoneyPacketDataClass money(&NumCredits);
	raw->Add(money);
	raw->Add(Description);
	return(raw);
}

int GuildDeductFeePacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Description) + sizeof(PlayerID) + MoneyPacketDataClass(&NumCredits).Get_Size());
}

int GuildDeductFeePacket::Print(char *buffer, int len)
{
	char player_id[32];
	return(_snprintf(buffer, len, "GuildDeductFeePacket { PlayerID=%s NumCredits=%s Description=%s }", Unknown_8B44E0(PlayerID, player_id), NumCredits.Amount_As_Text(false).c_str(), Description.c_str()));
}


/*
**	Reconstruction notes (pkguildplayeronline.cpp)
**
**	* Client 0x88EC70-0x894390 (0x88EC40 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x894390): the guild packets 193-220 not in pkguildranknamesrequestclient.cpp. Written by hand
**	  (builder P4, 2026-10-27); each class's value constructor comes first, then the raw one.
**	* The file name is provisional (no assert names it).
**	* Opcode 220 written by hand (builder P5, 2026-10-27): NumCredits goes through the packet-data wrapper
**	  MoneyPacketDataClass (pkunknown_873a00.cpp, vtable 0xB0ADC4) and Print through money.cpp's 0x4F00C0.
*/
