// Earth & Beyond client (reconstruction): app\packets\pkstart.cpp. Notes at the end of the file.
#include "pkstart.h"
#include <stdio.h>


StartPacketClass::StartPacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
}

RawPacketClass *StartPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(5, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	return(raw);
}

int StartPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int StartPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d}", "Start", PlayerID));
}

StartAckPacketClass::StartAckPacketClass(int player_id) : PlayerID(player_id)
{
}

StartAckPacketClass::StartAckPacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
}

RawPacketClass *StartAckPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(6, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	return(raw);
}

int StartAckPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int StartAckPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d}", "StartAck", PlayerID));
}


/*
**	Reconstruction notes (pkstart.cpp)
**
**	* Client 0x8A6610-0x8A6A50 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
