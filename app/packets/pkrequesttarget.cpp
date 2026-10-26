// Earth & Beyond client (reconstruction): app\packets\pkrequesttarget.cpp. Notes at the end of the file.
#include "pkrequesttarget.h"
#include <stdio.h>


RequestTargetPacketClass::RequestTargetPacketClass(int game_id, int target_id) : GameID(game_id), TargetID(target_id)
{
}

RequestTargetPacketClass::RequestTargetPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&TargetID, sizeof(TargetID));
}

RawPacketClass *RequestTargetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(23, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&TargetID, sizeof(TargetID));
	return(raw);
}

int RequestTargetPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(TargetID));
}

int RequestTargetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d TargetID=%d}", "Request_Target", GameID, TargetID));
}

RequestTargetsTargetPacketClass::RequestTargetsTargetPacketClass(int game_id, int target_id) : GameID(game_id), TargetID(target_id)
{
}

RequestTargetsTargetPacketClass::RequestTargetsTargetPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&TargetID, sizeof(TargetID));
}

RawPacketClass *RequestTargetsTargetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(24, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&TargetID, sizeof(TargetID));
	return(raw);
}

int RequestTargetsTargetPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(TargetID));
}

int RequestTargetsTargetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d TargetID=%d}", "Request_Target_Target", GameID, TargetID));
}

SetTargetPacketClass::SetTargetPacketClass(int target_id, int unknown10) : TargetID(target_id), Unknown10(unknown10)
{
}

SetTargetPacketClass::SetTargetPacketClass(RawPacketClass *raw)
{
	raw->Get(&TargetID, sizeof(TargetID));
	raw->Get(&Unknown10, sizeof(Unknown10));
}

RawPacketClass *SetTargetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(25, Get_Data_Size());
	raw->Add(&TargetID, sizeof(TargetID));
	raw->Add(&Unknown10, sizeof(Unknown10));
	return(raw);
}

int SetTargetPacketClass::Get_Data_Size()
{
	return(sizeof(TargetID) + sizeof(Unknown10));
}

int SetTargetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {TargetID=%d}", "Set_Target", TargetID));
}


/*
**	Reconstruction notes (pkrequesttarget.cpp)
**
**	* Client 0x8A7EA0-0x8A85E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
