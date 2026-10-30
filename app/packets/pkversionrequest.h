// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkversionrequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8B15F0-0x8B1A70 rebuilt so far (opcode 0, 1); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKVERSIONREQUEST_H  // NAME 1 [pkversionrequest]: provisional, pk + its first named class's name
#define PKVERSIONREQUEST_H

#include "pk.h"

// opcode 0 (0x0000, VERSION_REQUEST): vtable 0xB0C2D0, constructor 0x8B1670, 0x14 bytes
class VersionRequestPacketClass : public PacketClass  // NAME 3: the client's Print text 'VersionRequest {' and the PacketClass convention
{
public:
	VersionRequestPacketClass(int major, int minor);	// 0x8B15F0
	VersionRequestPacketClass(RawPacketClass *raw);	// 0x8B1670
	virtual RawPacketClass *Build();	// +0x04 0x8B1710
	virtual bool Process_Packet(void *client);	// +0x08 0x868AF0 (clientstub.cpp l.275)
	virtual int Get_Data_Size();	// +0x0C 0x8B17E0
	virtual int Print(char *buffer, int len);	// +0x10 0x8B1800

	int Major;	// +0x0C  // NAME 3: the client's Print text 'Major='
	int Minor;	// +0x10  // NAME 3: the client's Print text 'Minor='
};

// opcode 1 (0x0001, VERSION_RESPONSE): vtable 0xB0C300, constructor 0x8B18D0, 0x10 bytes
class VersionResponsePacketClass : public PacketClass  // NAME 3: the client's Print text 'VersionResponse {' and the PacketClass convention
{
public:
	VersionResponsePacketClass(int status);	// 0x8B1840
	VersionResponsePacketClass(RawPacketClass *raw);	// 0x8B18D0
	virtual RawPacketClass *Build();	// +0x04 0x8B1960
	virtual bool Process_Packet(void *client);	// +0x08 0x76B8A0 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8B1A20
	virtual int Print(char *buffer, int len);	// +0x10 0x8B1A40

	int Status;	// +0x0C  // NAME 3: the client's Print text 'Status='
};

#endif
