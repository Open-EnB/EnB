// Earth & Beyond client (reconstruction): app\packets\pkchatbasicrequest.cpp. Notes at the end of the file.
#include "pkchatbasicrequest.h"
#include <stdio.h>


ChatBasicRequestPacketClass::ChatBasicRequestPacketClass() : Unknown0C(0)
{
}

ChatBasicRequestPacketClass::ChatBasicRequestPacketClass(PacketClass *raw) : Unknown0C(0)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10);
	raw->Get(&Unknown20);
}

ChatBasicRequestPacketClass::~ChatBasicRequestPacketClass()
{
}

PacketClass *ChatBasicRequestPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(167, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(Unknown10);
	raw->Add(Unknown20);
	return(raw);
}

int ChatBasicRequestPacketClass::Get_Data_Size()
{
	int size = sizeof(Unknown0C);
	size += PacketClass::Get_Size(Unknown10);
	size += PacketClass::Get_Size(Unknown20);
	return(size);
}

int ChatBasicRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatBasicRequestPacketClass"));
}


/*
**	Reconstruction notes (pkchatbasicrequest.cpp)
**
**	* Client 0x87A940-0x87ACA0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class ChatBasicRequestPacketClass (thunk order) and its place in retail's object order (after pkclientchat), NAME 3; no assert names it.
*/
