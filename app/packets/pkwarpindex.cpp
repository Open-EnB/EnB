// Earth & Beyond client (reconstruction): app\packets\pkwarpindex.cpp. Notes at the end of the file.
#include "pkwarpindex.h"
#include "..\common\unknown_navlist.h"
#include <stdio.h>

WarpPacketClass::WarpPacketClass(int player_id, const NavigationListClass &list) :
	PlayerID(player_id),
	List(0)
{
	List = new NavigationListClass(list);
}

WarpPacketClass::WarpPacketClass(RawPacketClass *raw) :
	PlayerID(0),
	List(0)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
	List = new NavigationListClass(raw);
}

WarpPacketClass::~WarpPacketClass()
{
	delete List;
	List = 0;
}

RawPacketClass *WarpPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(155, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	List->Add(raw);
	return(raw);
}

int WarpPacketClass::Get_Data_Size()
{
	return(List->Get_Data_Size() + sizeof(PlayerID));
}

int WarpPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d NavigationListSize=%d }", "Warp", PlayerID, List->Get_Data_Size()));
}


WarpIndexPacketClass::WarpIndexPacketClass(int index) : Index(index)
{
}

WarpIndexPacketClass::WarpIndexPacketClass(RawPacketClass *raw)
{
	Index = -1;
	raw->Get(&Index, sizeof(Index));
}

RawPacketClass *WarpIndexPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(156, Get_Data_Size());
	raw->Add(&Index, sizeof(Index));
	return(raw);
}

int WarpIndexPacketClass::Get_Data_Size()
{
	return(sizeof(Index));
}

int WarpIndexPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Index=%d}", "Warp_Index", Index));
}


/*
**	Reconstruction notes (pkwarpindex.cpp)
**
**	* Client 0x8B1AA0-0x8B2050 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* WARP 155 by hand (builder P4, 2026-10-26): its navigation list is NavigationListClass
**	  (app\common\unknown_navlist.cpp).
*/
