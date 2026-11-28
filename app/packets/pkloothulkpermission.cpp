// Earth & Beyond client (reconstruction): app\packets\pkloothulkpermission.cpp. Notes at the end of the file.
#include "pkloothulkpermission.h"
#include <stdio.h>


LootHulkPermissionPacketClass::LootHulkPermissionPacketClass(int hulk_game_id) : HulkGameID(hulk_game_id)
{
}

LootHulkPermissionPacketClass::LootHulkPermissionPacketClass(PacketClass *raw)
{
	HulkGameID = -1;
	raw->Get(HulkGameID);
}

PacketClass *LootHulkPermissionPacketClass::Build()
{
	PacketClass *raw = new PacketClass(140, Get_Data_Size());
	raw->Add(HulkGameID);
	return(raw);
}

int LootHulkPermissionPacketClass::Get_Data_Size()
{
	return(sizeof(HulkGameID));
}

int LootHulkPermissionPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Loot Hulk Permission {HulkGameID=%d}", HulkGameID));
}


/*
**	Reconstruction notes (pkloothulkpermission.cpp)
**
**	* Client 0x8737A0-0x8739D0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
