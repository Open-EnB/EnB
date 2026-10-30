// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkserverready.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8A6140-0x8A6400 (opcode 59).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSERVERREADY_H  // NAME 1 [pkserverready]: provisional, pk + its Print text 'Server_Ready'
#define PKSERVERREADY_H

#include "pk.h"

// opcode 59 (0x003B, ?): vtable 0xB0BB9C, constructor 0x8A61E0, 0x18 bytes
class Packet_003BClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x003B
{
public:
	Packet_003BClass(int sector, int ip, unsigned short port);	// 0x8A6140
	Packet_003BClass(RawPacketClass *raw);	// 0x8A61E0
	virtual RawPacketClass *Build();	// +0x04 0x8A62A0
	virtual bool Process_Packet(void *client);	// +0x08 0x8683D0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A63A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A63C0
	virtual int Get_Type() { return(59); }	// +0x28 0x8A6380  // NAME 2: what it returns (the opcode Build sends), as SuperLinePacketClass's

	int Sector;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Sector='
	int IP;	// +0x10 (4 bytes as they are)  // NAME 3: the client's Print text 'IP='
	unsigned short Port;	// +0x14 (2 bytes as they are)  // NAME 3: the client's Print text 'Port='
};

#endif
