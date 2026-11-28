// Earth & Beyond client (reconstruction): app\packets\pkserverstop.cpp. Notes at the end of the file.
#include "pkserverstop.h"
#include <stdio.h>


ServerStopPacketClass::ServerStopPacketClass()
{
}

ServerStopPacketClass::ServerStopPacketClass(PacketClass *)
{
}

PacketClass *ServerStopPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(61, Get_Data_Size());
	return(raw);
}

int ServerStopPacketClass::Get_Data_Size()
{
	return(0);
}

int ServerStopPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Server_Stop"));
}


/*
**	Reconstruction notes (pkserverstop.cpp)
**
**	* Client 0x8A6430-0x8A65E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class ServerStopPacketClass (thunk order) and its place in retail's object order (between pkserverready and pkstart), NAME 3; no assert names it.
*/
