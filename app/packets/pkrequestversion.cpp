// Earth & Beyond client (reconstruction): app\packets\pkrequestversion.cpp. Notes at the end of the file.
#include "pkrequestversion.h"
#include <stdio.h>


RequestVersionPacketClass::RequestVersionPacketClass()
{
}

RequestVersionPacketClass::RequestVersionPacketClass(PacketClass *)
{
}

PacketClass *RequestVersionPacketClass::Build()
{
	PacketClass *raw = new PacketClass(69, Get_Data_Size());
	return(raw);
}

int RequestVersionPacketClass::Get_Data_Size()
{
	return(0);
}

int RequestVersionPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Request_Version"));
}


/*
**	Reconstruction notes (pkrequestversion.cpp)
**
**	* Client 0x8A2B70-0x8A2D40 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class RequestVersionPacketClass (thunk order) and its place in retail's object order (after pkrequesttransformchange), NAME 3; no assert names it.
*/
