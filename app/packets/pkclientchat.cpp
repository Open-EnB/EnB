// Earth & Beyond client (reconstruction): app\packets\pkclientchat.cpp. Notes at the end of the file.
#include "pkclientchat.h"
#include <stdio.h>

ClientChatEventPacketClass::ClientChatEventPacketClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(0),
	Unknown74(0),
	Unknown78(0),
	Unknown7C(false)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(&Unknown14);
	raw->Get(&Unknown24);
	raw->Get(&Unknown34);
	raw->Get(&Unknown44);
	raw->Get(&Unknown54);
	raw->Get(&Unknown64);
	raw->Get(&Unknown78, sizeof(Unknown78));
	if (Unknown78 != 0) {
		Unknown7C = true;
		Unknown74 = new char[Unknown78];
		raw->Get(Unknown74, Unknown78);
	}
}

ClientChatEventPacketClass::~ClientChatEventPacketClass()
{
	if (Unknown7C && Unknown74 != 0) {
		delete [] Unknown74;
		Unknown74 = 0;
	}
}

RawPacketClass *ClientChatEventPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(165, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown14);
	raw->Add(Unknown24);
	raw->Add(Unknown34);
	raw->Add(Unknown44);
	raw->Add(Unknown54);
	raw->Add(Unknown64);
	raw->Add(&Unknown78, sizeof(Unknown78));
	if (Unknown74 != 0) {
		raw->Add(Unknown74, Unknown78);
	}
	return(raw);
}

int ClientChatEventPacketClass::Get_Data_Size()
{
	int size = sizeof(Unknown0C) + sizeof(Unknown10);
	size += RawPacketClass::Get_Size(Unknown14);
	size += RawPacketClass::Get_Size(Unknown24);
	size += RawPacketClass::Get_Size(Unknown34);
	size += RawPacketClass::Get_Size(Unknown44);
	size += RawPacketClass::Get_Size(Unknown54);
	size += RawPacketClass::Get_Size(Unknown64);
	return(size + sizeof(Unknown78) + Unknown78);
}

int ClientChatEventPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ClientChatEventPacketClass"));
}


/*
**	Reconstruction notes (pkclientchat.cpp)
**
**	* Client 0x8785B0-0x87A910 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320): the chat packets 51, 163, 165, 164, 166 in that order. Opcode 165
**	  (CLIENT_CHAT_EVENT, the class named by its Print) written by hand (builder P3, 2026-10-26), 5 of 5
**	  exact in appfile.py match: six strings and a length-counted buffer the packet owns once it has
**	  read one. Get_Data_Size adds one string per statement and the buffer's count and bytes last.
**	* The file name is provisional (no assert names it). Not rebuilt yet: opcode 51, 163, 164, 166
**	  (declared in pk_classes.h).
*/
