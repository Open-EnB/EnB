// Earth & Beyond client (reconstruction): app\packets\pkstarbasetow.cpp. Notes at the end of the file.
#include "pkstarbasetow.h"
#include <stdio.h>


StarbaseRequestPacketClass::StarbaseRequestPacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
	raw->Get(&StarbaseID, sizeof(StarbaseID));
	raw->Get(&Action, sizeof(Action));
}

RawPacketClass *StarbaseRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(78, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	raw->Add(&StarbaseID, sizeof(StarbaseID));
	raw->Add(&Action, sizeof(Action));
	return(raw);
}

int StarbaseRequestPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(StarbaseID) + sizeof(Action));
}

int StarbaseRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d StarbaseID=%d Action=%d}", "Starbase_Request", PlayerID, StarbaseID, Action));
}

StarbaseSetPacketClass::StarbaseSetPacketClass(unsigned char action, unsigned char exit_mode, int starbase_id) :
	StarbaseID(starbase_id),
	Action(action),
	ExitMode(exit_mode)
{
}

StarbaseSetPacketClass::StarbaseSetPacketClass(RawPacketClass *raw)
{
	raw->Get(&StarbaseID, sizeof(StarbaseID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&ExitMode, sizeof(ExitMode));
}

RawPacketClass *StarbaseSetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(79, Get_Data_Size());
	raw->Add(&StarbaseID, sizeof(StarbaseID));
	raw->Add(&Action, sizeof(Action));
	raw->Add(&ExitMode, sizeof(ExitMode));
	return(raw);
}

int StarbaseSetPacketClass::Get_Data_Size()
{
	return(sizeof(StarbaseID) + sizeof(Action) + sizeof(ExitMode));
}

int StarbaseSetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {StarbaseID=%d Action=%d ExitMode=%s}", "Starbase_Set", StarbaseID, Action, Sector_Type_Name(ExitMode)));
}

StarbaseTowPacketClass::StarbaseTowPacketClass(int starbase_id1, int tow_cost1, int starbase_id2, int tow_cost2, int starbase_id3, int tow_cost3) : StarbaseID1(starbase_id1), StarbaseID2(starbase_id2), StarbaseID3(starbase_id3), TowCost1(tow_cost1), TowCost2(tow_cost2), TowCost3(tow_cost3)
{
}

StarbaseTowPacketClass::StarbaseTowPacketClass(RawPacketClass *raw)
{
	raw->Get(&StarbaseID1, sizeof(StarbaseID1));
	raw->Get(&StarbaseID2, sizeof(StarbaseID2));
	raw->Get(&StarbaseID3, sizeof(StarbaseID3));
	raw->Get(&TowCost1, sizeof(TowCost1));
	raw->Get(&TowCost2, sizeof(TowCost2));
	raw->Get(&TowCost3, sizeof(TowCost3));
}

RawPacketClass *StarbaseTowPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(80, Get_Data_Size());
	raw->Add(&StarbaseID1, sizeof(StarbaseID1));
	raw->Add(&StarbaseID2, sizeof(StarbaseID2));
	raw->Add(&StarbaseID3, sizeof(StarbaseID3));
	raw->Add(&TowCost1, sizeof(TowCost1));
	raw->Add(&TowCost2, sizeof(TowCost2));
	raw->Add(&TowCost3, sizeof(TowCost3));
	return(raw);
}

int StarbaseTowPacketClass::Get_Data_Size()
{
	return(sizeof(StarbaseID1) + sizeof(StarbaseID2) + sizeof(StarbaseID3) + sizeof(TowCost1) + sizeof(TowCost2) + sizeof(TowCost3));
}

int StarbaseTowPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {StarbaseID1=%d) {StarbaseID2=%d) {StarbaseID3=%d) (TowCost1=%d) (TowCost2=%d) (TowCost3=%d)", "Starbase Tow", StarbaseID1, StarbaseID2, StarbaseID3, TowCost1, TowCost2, TowCost3));
}


/*
**	Reconstruction notes (pkstarbasetow.cpp)
**
**	* Client 0x8A2D70-0x8A35D0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 79 (declared in pk_classes.h).
*/
