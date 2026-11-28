// Earth & Beyond client (reconstruction): app\packets\pkclickindicator.cpp. Notes at the end of the file.
#include "pkclickindicator.h"
#include <stdio.h>


ClickIndicatorPacketClass::ClickIndicatorPacketClass(bool turn_on, int obj_id) : TurnOn(turn_on), ObjID(obj_id)
{
}

ClickIndicatorPacketClass::ClickIndicatorPacketClass(PacketClass *raw)
{
	raw->Get(&TurnOn, sizeof(TurnOn));
	raw->Get(&ObjID, sizeof(ObjID));
}

PacketClass *ClickIndicatorPacketClass::Build()
{
	PacketClass *raw = new PacketClass(105, Get_Data_Size());
	raw->Add(&TurnOn, sizeof(TurnOn));
	raw->Add(&ObjID, sizeof(ObjID));
	return(raw);
}

int ClickIndicatorPacketClass::Get_Data_Size()
{
	return(sizeof(TurnOn) + sizeof(ObjID));
}

int ClickIndicatorPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Click Indicator Packet {TurnOn=%d ObjID=%d}", TurnOn, ObjID));
}


/*
**	Reconstruction notes (pkclickindicator.cpp)
**
**	* Client 0x87E7D0-0x87EA30 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
