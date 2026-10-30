// Earth & Beyond client (reconstruction): app\packets\pkmessagechatstream.cpp. Notes at the end of the file.
#include "pkmessagechatstream.h"
#include <stdio.h>

MessageChatstreamPacketClass::MessageChatstreamPacketClass(unsigned char color, char *chat_string, short chatsize, int ship_id) :
	ShipID(ship_id),
	Color(color),
	Owned(false)
{
	Chatsize = chatsize;
	ChatString = chat_string;
}

MessageChatstreamPacketClass::MessageChatstreamPacketClass(RawPacketClass *raw) :
	Owned(true)
{
	raw->Get(&Chatsize, sizeof(Chatsize));
	raw->Get(&Color, sizeof(Color));
	raw->Get(&ShipID, sizeof(ShipID));
	ChatString = new char[Chatsize];
	raw->Get(ChatString, Chatsize);
}

MessageChatstreamPacketClass::~MessageChatstreamPacketClass()
{
	if (Owned && ChatString != 0) {
		delete [] ChatString;
	}
}

RawPacketClass *MessageChatstreamPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(95, Get_Data_Size());
	raw->Add(&Chatsize, sizeof(Chatsize));
	raw->Add(&Color, sizeof(Color));
	raw->Add(&ShipID, sizeof(ShipID));
	raw->Add(ChatString, Chatsize);
	return(raw);
}

int MessageChatstreamPacketClass::Get_Data_Size()
{
	return(sizeof(Chatsize) + sizeof(Color) + sizeof(ShipID) + Chatsize);
}

int MessageChatstreamPacketClass::Print(char *buffer, int)
{
	Unknown_ChatStreamClass stream(ChatString, Chatsize);
	char text[512] = "";
	stream.Unknown_4C48C0(text);
	return(sprintf(buffer, "%s {Chatsize=%hd ChatString=%s ShipID=%d Color=%d}", "Message_String", Chatsize, text, ShipID, Color));
}

/*
**	Reconstruction notes (pkmessagechatstream.cpp)
**
**	* Client 0x89D310-0x89D740 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC): opcode 95, written by hand (builder P5, 2026-10-27), the value constructor first; the
**	  shape of MessageStringPacketClass's (opcode 29) with a ShipID. Print decodes the stream through
**	  the chat-stream class 0x4C4360 (not rebuilt; udpreplay's stubs give it).
**	* The file name is provisional (no assert names it).
*/
