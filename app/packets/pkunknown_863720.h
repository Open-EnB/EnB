// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_863720.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x863720-0x864090 (opcodes 230, 231).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_863720_H  // NAME 1 [pkunknown_863720]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_863720_H

#include "pk.h"

// opcode 230 (0x00E6, ?): vtable 0xB093C8, constructor 0x863940, 0x38 bytes
class Packet_00E6Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00E6 (Print 'GlobalContainerRequest')
{
public:
	Packet_00E6Class(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, PacketClass *packet);	// 0x863720 (Data: the packet's Build, header and payload)
	Packet_00E6Class(RawPacketClass *raw);	// 0x863940
	virtual ~Packet_00E6Class();	// +0x00 0x8638B0 / 0x863880
	virtual RawPacketClass *Build();	// +0x04 0x863A50
	virtual bool Process_Packet(void *client);	// +0x08 0x869360 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x863B80
	virtual int Print(char *buffer, int len);	// +0x10 0x863BB0
	virtual int Get_Type();	// +0x28 0x863B60  // NAME 2: as Packet_138AClass's

	char *Data;	// +0x0C (DataSize bytes: a message's header and payload)  // NAME 2: what it holds
	int DataSize;	// +0x10 (network order)  // NAME 2: what it holds
	__int64 Unknown18;	// +0x18
	char *String;	// +0x20 (nstrdup'd; sent as a string)  // NAME 1: neutral (D10)
	__int64 Unknown28;	// +0x28
	int Unknown30;	// +0x30 (network order)
	PacketClass *Unknown34;	// +0x34 (0 from both constructors; the destructor deletes it)
};

// opcode 231 (0x00E7, ?): vtable 0xB093FC, constructor 0x863DC0, 0x38 bytes
class Packet_00E7Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00E7 (Print 'GlobalContainerResponse')
{
public:
	Packet_00E7Class(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, PacketClass *packet);	// 0x863BE0 (Data: the packet's Build, header and payload)
	Packet_00E7Class(RawPacketClass *raw);	// 0x863DC0
	virtual ~Packet_00E7Class();	// +0x00 0x863D70 / 0x863D40
	virtual RawPacketClass *Build();	// +0x04 0x863ED0
	virtual bool Process_Packet(void *client);	// +0x08 0x869390 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x864000
	virtual int Print(char *buffer, int len);	// +0x10 0x864030
	virtual int Get_Type();	// +0x28 0x863FE0  // NAME 2: as Packet_138AClass's

	char *Data;	// +0x0C (DataSize bytes: a message's header and payload)  // NAME 2: what it holds
	int DataSize;	// +0x10 (network order)  // NAME 2: what it holds
	__int64 Unknown18;	// +0x18
	char *String;	// +0x20 (nstrdup'd; sent as a string)  // NAME 1: neutral (D10)
	__int64 Unknown28;	// +0x28
	int Unknown30;	// +0x30 (network order)
};

#endif
