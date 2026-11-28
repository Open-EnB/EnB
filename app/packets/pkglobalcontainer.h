// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalcontainer.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x863720-0x864090 (opcodes 230, 231).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALCONTAINER_H  // NAME 3 [pkglobalcontainer.h]: by convention from its class GlobalContainerRequest / Response (thunk order) and its place in retail's object order (between pkglobalconnect and pkglobalcreatecharacter)
#define PKGLOBALCONTAINER_H

#include "pk.h"

// opcode 230 (0x00E6, ?): vtable 0xB093C8, constructor 0x863940, 0x38 bytes
class GlobalContainerRequestPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GlobalContainerRequestPacketClass(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, UnknownPacketBaseClass *packet);	// 0x863720 (Data: the packet's Build, header and payload)
	GlobalContainerRequestPacketClass(PacketClass *raw);	// 0x863940
	virtual ~GlobalContainerRequestPacketClass();	// +0x00 0x8638B0 / 0x863880
	virtual PacketClass *Encode();	// +0x04 0x863A50  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x869360 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x863B80
	virtual int Print(char *buffer, int len);	// +0x10 0x863BB0
	virtual int Get_Type();	// +0x28 0x863B60  // NAME 2: as TaskStartPacketClass's

	char *Data;	// +0x0C (DataSize bytes: a message's header and payload)  // NAME 2: what it holds
	int DataSize;	// +0x10 (network order)  // NAME 2: what it holds
	__int64 Unknown18;	// +0x18
	char *String;	// +0x20 (nstrdup'd; sent as a string)  // NAME 1: neutral (D10)
	__int64 Unknown28;	// +0x28
	int Unknown30;	// +0x30 (network order)
	UnknownPacketBaseClass *Unknown34;	// +0x34 (0 from both constructors; the destructor deletes it)
};

// opcode 231 (0x00E7, ?): vtable 0xB093FC, constructor 0x863DC0, 0x38 bytes
class GlobalContainerResponsePacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GlobalContainerResponsePacketClass(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, UnknownPacketBaseClass *packet);	// 0x863BE0 (Data: the packet's Build, header and payload)
	GlobalContainerResponsePacketClass(PacketClass *raw);	// 0x863DC0
	virtual ~GlobalContainerResponsePacketClass();	// +0x00 0x863D70 / 0x863D40
	virtual PacketClass *Encode();	// +0x04 0x863ED0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x869390 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x864000
	virtual int Print(char *buffer, int len);	// +0x10 0x864030
	virtual int Get_Type();	// +0x28 0x863FE0  // NAME 2: as TaskStartPacketClass's

	char *Data;	// +0x0C (DataSize bytes: a message's header and payload)  // NAME 2: what it holds
	int DataSize;	// +0x10 (network order)  // NAME 2: what it holds
	__int64 Unknown18;	// +0x18
	char *String;	// +0x20 (nstrdup'd; sent as a string)  // NAME 1: neutral (D10)
	__int64 Unknown28;	// +0x28
	int Unknown30;	// +0x30 (network order)
};

#endif
