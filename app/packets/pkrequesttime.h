/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkrequesttime.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AAD50-0x8AB27F (opcodes 68 and 52): the time requests.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREQUESTTIME_H  // NAME 1 [pkrequesttime]: provisional, pk + its first class's name
#define PKREQUESTTIME_H

#include "pk.h"

// opcode 68 (0x0044, REQUEST_TIME): vtable 0xB0BFB8, constructor 0x8AADE0, 0x14 bytes
class RequestTimePacketClass : public PacketClass  // NAME 2: Net-7's opcode name REQUEST_TIME (the Net-7 opcode table; no structure check)
{
public:
	RequestTimePacketClass();	// 0x8AAD50
	RequestTimePacketClass(RawPacketClass *raw);	// 0x8AADE0
	virtual RawPacketClass *Build();	// +0x04 0x8AAE70
	virtual bool Process_Packet(void *client);	// +0x08 0x867AD0 (clientstub.cpp l.17)
	virtual int Get_Data_Size();	// +0x0C 0x8AAF40
	virtual int Print(char *buffer, int len);	// +0x10 0x8AAF60
	virtual int Get_Priority() { return(63); }	// +0x1C 0x8AAD90

	unsigned int TimeSent;	// +0x0C (Build stamps it: Unknown_Get_Ticks)  // NAME 3: the client's Print text 'TimeSent='
	unsigned int TimeReceived;	// +0x10 (the raw constructor stamps it)  // NAME 3: the client's Print text 'TimeReceived='
};

// opcode 52 (0x0034, CLIENT_SET_TIME): vtable 0xB0BFE8, constructor 0x8AB060, 0x1C bytes
class SetClientTimePacketClass : public PacketClass
{
public:
	SetClientTimePacketClass(unsigned int client_sent, unsigned int server_received);	// 0x8AAFA0
	SetClientTimePacketClass(RawPacketClass *raw);	// 0x8AB060
	virtual RawPacketClass *Build();	// +0x04 0x8AB120
	virtual bool Process_Packet(void *client);	// +0x08 0x737960 (cpacket.cpp, shape residue)
	virtual int Get_Data_Size();	// +0x0C 0x8AB200
	virtual int Print(char *buffer, int len);	// +0x10 0x8AB220
	virtual int Get_Priority() { return(63); }	// +0x1C 0x8AAFF0

	unsigned int ClientSent;	// +0x0C  // NAME 3: the client's Print text 'ClientSent='
	unsigned int ServerReceived;	// +0x10  // NAME 3: the client's Print text 'ServerReceived='
	unsigned int ServerSent;	// +0x14 (Build stamps it: Unknown_Get_Ticks)  // NAME 3: the client's Print text 'ServerSent='
	unsigned int ClientReceived;	// +0x18 (the raw constructor stamps it)  // NAME 3: the client's Print text 'ClientReceived='
};

#endif
