// Earth & Beyond client (reconstruction): app\packets\pkincapacitancerequest.cpp. Notes at the end of the file.
#include "pkincapacitancerequest.h"
#include <stdio.h>


IncapacitanceRequestPacketClass::IncapacitanceRequestPacketClass(int game_id) : GameID(game_id)
{
}

IncapacitanceRequestPacketClass::IncapacitanceRequestPacketClass(RawPacketClass *raw)
{
	GameID = -1;
	raw->Get(GameID);
}

RawPacketClass *IncapacitanceRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(141, Get_Data_Size());
	raw->Add(GameID);
	return(raw);
}

int IncapacitanceRequestPacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int IncapacitanceRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Incapacitance Details Request {GameID=%d}", GameID));
}

Packet_008FClass::Packet_008FClass(int game_id) : GameID(game_id)
{
}

Packet_008FClass::Packet_008FClass(RawPacketClass *raw)
{
	GameID = -1;
	raw->Get(GameID);
}

RawPacketClass *Packet_008FClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(143, Get_Data_Size());
	raw->Add(GameID);
	return(raw);
}

int Packet_008FClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_008FClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Request Rescue {GameID=%d}", GameID));
}


/*
**	Reconstruction notes (pkincapacitancerequest.cpp)
**
**	* Client 0x871F70-0x873770 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 142, 144, 145 (declared in pk_classes.h).
*/
