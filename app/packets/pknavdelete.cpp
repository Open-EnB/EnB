// Earth & Beyond client (reconstruction): app\packets\pknavdelete.cpp. Notes at the end of the file.
#include "pknavdelete.h"
#include <stdio.h>


NavigationPacketClass::NavigationPacketClass(int game_id, float signature, bool player_has_visited, int nav_type, bool is_huge) :
	GameID(game_id),
	Signature(signature),
	PlayerHasVisited(player_has_visited),
	NavType(nav_type),
	IsHuge(is_huge),
	Channel(-1)
{
	Channel = Unknown_14(game_id, -1);
}

NavigationPacketClass::NavigationPacketClass(RawPacketClass *raw) :
	GameID(0),
	Signature(0.0f),
	PlayerHasVisited(false),
	NavType(-1),
	IsHuge(false),
	Channel(-1)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Signature, sizeof(Signature));
	raw->Get(&PlayerHasVisited, sizeof(PlayerHasVisited));
	raw->Get(&NavType, sizeof(NavType));
	raw->Get(&IsHuge, sizeof(IsHuge));
}

RawPacketClass *NavigationPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(153, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Signature, sizeof(Signature));
	raw->Add(&PlayerHasVisited, sizeof(PlayerHasVisited));
	raw->Add(&NavType, sizeof(NavType));
	raw->Add(&IsHuge, sizeof(IsHuge));
	return(raw);
}

int NavigationPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Signature) + sizeof(PlayerHasVisited) + sizeof(NavType) + sizeof(IsHuge));
}

int NavigationPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Signature=%f PlayerHasVisited=%d NavType=%d IsHuge=%d }", "NavigationPacketClass", GameID, Signature, PlayerHasVisited, NavType, IsHuge));
}

NavDeletePacketClass::NavDeletePacketClass(int game_id) : GameID(game_id), Unknown10(-1)
{
	Unknown10 = Unknown_14(game_id, -1);
}

NavDeletePacketClass::NavDeletePacketClass(RawPacketClass *raw) : GameID(0), Unknown10(-1)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *NavDeletePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(154, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int NavDeletePacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int NavDeletePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "NavDeletePacketClass", GameID));
}


/*
**	Reconstruction notes (pknavdelete.cpp)
**
**	* Client 0x89E8C0-0x89EEF0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Opcode 153 (NAVIGATION) and NavDelete's value constructor written by hand (builder P3,
**	  2026-10-26), 6 of 6 exact in appfile.py match: the value constructors set the channel from
**	  PacketClass::Unknown_14 of their game_id argument after the initialiser's -1.
*/
