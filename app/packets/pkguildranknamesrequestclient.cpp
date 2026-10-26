// Earth & Beyond client (reconstruction): app\packets\pkguildranknamesrequestclient.cpp. Notes at the end of the file.
#include "pkguildranknamesrequestclient.h"
#include <stdio.h>


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


/*
**	Reconstruction notes (pkguildranknamesrequestclient.cpp)
**
**	* Client 0x88BD40-0x88EC40 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 204, 205, 197, 200, 201, 208, 210, 211, 216 (declared in pk_classes.h).
*/
