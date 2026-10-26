// Earth & Beyond client (reconstruction): app\packets\pkauxdata.cpp. Notes at the end of the file.
#include "pkauxdata.h"
#include <stdio.h>
#include <string.h>

AuxDataPacketClass::AuxDataPacketClass(int game_id, const void *data, int size) : GameID(game_id)
{
	Data = new char[size];
	Size = size;
	memcpy(Data, data, Size);
	Channel = Unknown_14(GameID, -1);
}

AuxDataPacketClass::AuxDataPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Size, sizeof(Size));
	Data = new char[Size];
	raw->Get(Data, Size);
}

AuxDataPacketClass::~AuxDataPacketClass()
{
	if (Data != 0) {
		delete [] Data;
	}
}

RawPacketClass *AuxDataPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Size, sizeof(Size));
	raw->Add(Data, Size);
	return(raw);
}

int AuxDataPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Size) + Size);
}

// NOT EXACT: the client reads Size before the first dword of Data (register order; 2 forms, workflow.md 3.1)
int AuxDataPacketClass::Print(char *buffer, int)
{
	const char *name = Get_Name();
	return(sprintf(buffer, "%s {GameID=%d Size=%d Flags=%x}", name, GameID, Size, *(int *)Data));
}

AuxDataPlayerVarPacketClass::AuxDataPlayerVarPacketClass(int game_id, const void *data, int size) : AuxDataPacketClass(game_id, data, size)
{
}

AuxDataPlayerVarPacketClass::AuxDataPlayerVarPacketClass(RawPacketClass *raw) : AuxDataPacketClass(raw)
{
}


/*
**	Reconstruction notes (pkauxdata.cpp)
**
**	* Client 0x876D30-0x877340 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand from the client (builder P, 2026-10-26): the aux data
**	  packet (opcode 27) and the player-variable one derived from it (28: its own Get_Type, Get_Name
**	  and Process_Packet, cpacket.cpp's). Get_Channel, Get_Type and Get_Name are inline (emitted
**	  before each class's scalar deleting destructor, where the vtable is first used).
**	* Build takes its opcode from Get_Type. The constructor from fields sets Channel through
**	  PacketClass::Unknown_14(GameID, -1); the one from the message leaves it unset.
**	* Print is not byte-exact: the client loads Size before the first dword of Data; Get_Name in a local
**	  puts its call first as the client's (attempts.py show 0x8770A0).
*/
