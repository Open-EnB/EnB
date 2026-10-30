// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_877640.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x877640-0x877C50 (opcode 97).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_877640_H  // NAME 1 [pkunknown_877640]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_877640_H

#include "pk.h"
#include "..\shared\avatar_desc.h"

// opcode 97 (0x0061, AVATAR_DESCRIPTION): vtable 0xB09D4C, constructor 0x877A60, 0x120 bytes
class AvatarDescPacketClass : public PacketClass
{
public:
	AvatarDescPacketClass(int game_id, const AvatarDescClass *desc);	// 0x877640 (Desc a copy, when given)
	AvatarDescPacketClass(RawPacketClass *raw);	// 0x877A60
	virtual RawPacketClass *Build();	// +0x04 0x877B20
	virtual bool Process_Packet(void *client);	// +0x08 0x733B10 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x877BF0
	virtual int Print(char *buffer, int len);	// +0x10 0x877C10
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x8779A0

	int GameID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'GameID='
	AvatarDescClass Desc;	// +0x10 (through RawPacketClass's Add / Get of its wire interface)  // NAME 2: what it holds
	int Channel;	// +0x11C (PacketClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
};

#endif
