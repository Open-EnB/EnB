// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmessagechatstream.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x89D310-0x89D740 (opcode 95).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMESSAGECHATSTREAM_H  // NAME 1 [pkmessagechatstream]: provisional, pk + its class's name
#define PKMESSAGECHATSTREAM_H

#include "pk.h"
#include "..\shared\chat_edit_stream.h"	// the chat stream (0x4C4360-0x4C4962)

// opcode 95 (0x005F, AVATAR_EMOTE_RESPONSE): vtable 0xB0B374, constructor 0x89D3F0, 0x1C bytes
class MessageChatstreamPacketClass : public PacketClass
{
public:
	MessageChatstreamPacketClass(unsigned char color, char *chat_string, short chatsize, int ship_id);	// 0x89D310 (ChatString is the caller's, not copied)
	MessageChatstreamPacketClass(RawPacketClass *raw);	// 0x89D3F0
	virtual ~MessageChatstreamPacketClass();	// +0x00 0x89D4D0 / 0x89D3C0
	virtual RawPacketClass *Build();	// +0x04 0x89D510
	virtual bool Process_Packet(void *client);	// +0x08 0x736F10 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89D600
	virtual int Print(char *buffer, int len);	// +0x10 0x89D620
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89D3A0
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89D380

	char *ChatString;	// +0x0C (Chatsize bytes)  // NAME 3: the client's Print text 'ChatString='
	short Chatsize;	// +0x10  // NAME 3: the client's Print text 'Chatsize='
	int ShipID;	// +0x14  // NAME 3: the client's Print text 'ShipID='
	unsigned char Color;	// +0x18  // NAME 3: the client's Print text 'Color='
	bool Owned;	// +0x19 (ChatString new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

#endif
