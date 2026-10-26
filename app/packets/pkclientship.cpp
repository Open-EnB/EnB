// Earth & Beyond client (reconstruction): app\packets\pkclientship.cpp. Notes at the end of the file.
#include "pkclientship.h"
#include <stdio.h>


ClientShipPacketClass::ClientShipPacketClass(int ship_id) : ShipID(ship_id)
{
}

ClientShipPacketClass::ClientShipPacketClass(RawPacketClass *raw)
{
	raw->Get(&ShipID, sizeof(ShipID));
}

RawPacketClass *ClientShipPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(71, Get_Data_Size());
	raw->Add(&ShipID, sizeof(ShipID));
	return(raw);
}

int ClientShipPacketClass::Get_Data_Size()
{
	return(sizeof(ShipID));
}

int ClientShipPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ShipID=%d}", "Client_Ship", ShipID));
}


/*
**	Reconstruction notes (pkclientship.cpp)
**
**	* Client 0x8A4790-0x8A49C0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
