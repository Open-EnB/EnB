// Earth & Beyond client (reconstruction): app\packets\pkunknown_85e640.cpp. Notes at the end of the file.
#include "pkunknown_85e640.h"
#include <stdio.h>


Packet_7536Class::Packet_7536Class(int game_id) : GameID(game_id)
{
}

Packet_7536Class::Packet_7536Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *Packet_7536Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30006, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int Packet_7536Class::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_7536Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorCancelKeyValues", GameID));
}


/*
**	Reconstruction notes (pkunknown_85e640.cpp)
**
**	* Client 0x85E640-0x85F220 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Not rebuilt yet: opcode 30005, 30008 (declared in pk_classes.h).
*/
