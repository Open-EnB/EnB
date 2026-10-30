// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkcta_request.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x86F070-0x86F600 (opcodes 188 and 189).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCTA_REQUEST_H  // NAME 1 [pkcta_request]: provisional, pk + its first named class's name
#define PKCTA_REQUEST_H

#include "pk.h"

// opcode 188 (0x00BC, CTA_REQUEST): vtable 0xB097DC, constructor 0x86F0F0, 0x18 bytes
class CTA_Request_PacketClass : public PacketClass  // NAME 3: the client's Print text 'CTA_Request_Packet {' and the PacketClass convention
{
public:
	CTA_Request_PacketClass(int avatar_id, int target_id, int request_type);	// 0x86F070
	CTA_Request_PacketClass(RawPacketClass *raw);	// 0x86F0F0
	virtual RawPacketClass *Build();	// +0x04 0x86F1E0
	virtual bool Process_Packet(void *client);	// +0x08 0x868DF0 (clientstub.cpp l.323)
	virtual int Get_Data_Size();	// +0x0C 0x86F2C0
	virtual int Print(char *buffer, int len);	// +0x10 0x86F2E0

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	int TargetID;	// +0x10  // NAME 3: the client's Print text 'TargetID='
	int RequestType;	// +0x14  // NAME 3: the client's Print text 'RequestType='
};

// opcode 189 (0x00BD, CTA_RESPONSE): vtable 0xB0980C, constructor 0x86F3C0, 0x18 bytes
class CTAResponsePacketClass : public PacketClass
{
public:
	CTAResponsePacketClass(int avatar_id, int request_type, bool success);	// 0x86F320
	CTAResponsePacketClass(RawPacketClass *raw);	// 0x86F3C0
	virtual RawPacketClass *Build();	// +0x04 0x86F4B0
	virtual bool Process_Packet(void *client);	// +0x08 0x73DAA0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x86F590
	virtual int Print(char *buffer, int len);	// +0x10 0x86F5B0
	virtual int Get_Channel() { return(AvatarID); }	// +0x18 0x86F370

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	int RequestType;	// +0x10  // NAME 3: the client's Print text 'RequestType='
	bool Success;	// +0x14  // NAME 3: the client's Print text 'Success='
};

#endif
