// Earth & Beyond client (reconstruction): app\packets\pkunknown_8a6430.cpp. Notes at the end of the file.
#include "pkunknown_8a6430.h"
#include <stdio.h>


Packet_003DClass::Packet_003DClass()
{
}

Packet_003DClass::Packet_003DClass(RawPacketClass *)
{
}

RawPacketClass *Packet_003DClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(61, Get_Data_Size());
	return(raw);
}

int Packet_003DClass::Get_Data_Size()
{
	return(0);
}

int Packet_003DClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Server_Stop"));
}


/*
**	Reconstruction notes (pkunknown_8a6430.cpp)
**
**	* Client 0x8A6430-0x8A65E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
