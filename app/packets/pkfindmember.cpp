// Earth & Beyond client (reconstruction): app\packets\pkfindmember.cpp. Notes at the end of the file.
#include "pkfindmember.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>

FindMemberPacketClass::FindMemberPacketClass() :
	NumPlayers(0)
{
}

void FindMemberPacketClass::Add_Player(int unknown00, int unknown04, char unknown08, char unknown0c)
{
	FindMemberStruct *player = new FindMemberStruct;
	player->Unknown00 = unknown00;
	player->Unknown04 = unknown04;
	player->Unknown08 = unknown08;
	player->Unknown0C = unknown0c;
	PlayerList.push_back(player);
	NumPlayers++;
}

FindMemberPacketClass::FindMemberPacketClass(RawPacketClass *raw)
{
	raw->Get(&NumPlayers, sizeof(NumPlayers));
	for (int i = 0; i < NumPlayers; i++) {
		FindMemberStruct *player = new FindMemberStruct;
		raw->Get(player->Unknown00);
		raw->Get(player->Unknown04);
		raw->Get(player->Unknown08);
		raw->Get(player->Unknown0C);
		PlayerList.push_back(player);
	}
}

FindMemberPacketClass::~FindMemberPacketClass()
{


	G_ASSERT(NumPlayers == (int)PlayerList.size());
	for (std::list<FindMemberStruct *>::iterator it = PlayerList.begin(); it != PlayerList.end(); it++) {
		delete *it;
	}
	PlayerList.erase(PlayerList.begin(), PlayerList.end()); PlayerList.size();	// the dead size() keeps erase's inner it++ out of line, as the client (workflow.md 3.1, 6bd0611)
}

RawPacketClass *FindMemberPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(83, Get_Data_Size());
	raw->Add(&NumPlayers, sizeof(NumPlayers));
	for (std::list<FindMemberStruct *>::iterator it = PlayerList.begin(); it != PlayerList.end(); it++) {
		FindMemberStruct *player = *it;
		raw->Add(player->Unknown00);
		raw->Add(player->Unknown04);
		raw->Add(player->Unknown08);
		raw->Add(player->Unknown0C);
	}
	return(raw);
}

int FindMemberPacketClass::Get_Data_Size()
{
	int size = sizeof(NumPlayers);
	for (std::list<FindMemberStruct *>::iterator it = PlayerList.begin(); it != PlayerList.end(); it++) {
		size += sizeof(FindMemberStruct);
	}
	return(size);
}

int FindMemberPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s {NumPlayers=%d}", NumPlayers));
}


/*
**	Reconstruction notes (pkfindmember.cpp)
**
**	* The client's app\Packets\pkfindmember.cpp (its assert's __FILE__), 0x886580-0x886C40 (0x886C40 is the
**	  guarded static, 0xC416FC): FIND_MEMBER 83 written by hand (builder A-opus, 2026-10-27). The players are
**	  new'd 16-byte records in a std::list of pointers; the destructor's G_ASSERT is on line 39.
**	* Print passes NumPlayers where its format wants the name first (the client's own slip: four arguments).
**	* The object also holds std::list's out-of-line ~list body (0x886B60), erase (0x886BD0) and the
**	  iterator's post-increment (0x886C20).
*/
