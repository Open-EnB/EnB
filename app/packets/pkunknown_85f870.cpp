// Earth & Beyond client (reconstruction): app\packets\pkunknown_85f870.cpp. Notes at the end of the file.
#include "pkunknown_85f870.h"
#include <stdio.h>


Packet_753FClass::Packet_753FClass()
{
}

Packet_753FClass::Packet_753FClass(RawPacketClass *)
{
}

RawPacketClass *Packet_753FClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30015, Get_Data_Size());
	return(raw);
}

int Packet_753FClass::Get_Data_Size()
{
	return(0);
}

int Packet_753FClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GameEditorCreateCancel"));
}

Packet_7539Class::Packet_7539Class(int game_id) : GameID(game_id)
{
}

Packet_7539Class::Packet_7539Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *Packet_7539Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30009, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int Packet_7539Class::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_7539Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorDelete", GameID));
}

Packet_753AClass::Packet_753AClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_753AClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30010, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_753AClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Unknown10));
}

int Packet_753AClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorMove", GameID));
}


/*
**	Reconstruction notes (pkunknown_85f870.cpp)
**
**	* Client 0x85F870-0x860D00 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Not rebuilt yet: opcode 30016, 30014, 30012 (declared in pk_classes.h).
*/
