// Earth & Beyond client (reconstruction): app\packets\pkunknown_8a2b70.cpp. Notes at the end of the file.
#include "pkunknown_8a2b70.h"
#include <stdio.h>


Packet_0045Class::Packet_0045Class()
{
}

Packet_0045Class::Packet_0045Class(RawPacketClass *)
{
}

RawPacketClass *Packet_0045Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(69, Get_Data_Size());
	return(raw);
}

int Packet_0045Class::Get_Data_Size()
{
	return(0);
}

int Packet_0045Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Request_Version"));
}


/*
**	Reconstruction notes (pkunknown_8a2b70.cpp)
**
**	* Client 0x8A2B70-0x8A2D40 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
