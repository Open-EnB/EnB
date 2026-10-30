// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\gameedit\pkgespwn.h (the .cpp named by its asserts)
**
**	The packet classes of client 0x862350-0x862970 (opcodes 30017, 30018).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGESPWN_H  // NAME 3 [pkgespwn.h]: the header of the client's gameedit\pkgespwn.cpp
#define PKGESPWN_H

#include "..\pk.h"

// opcode 30017 (0x7541, ?): vtable 0xB09308, constructor 0x862440, 0x18 bytes
class Packet_7541Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7541 (Print 'GameEditorRequestSpawnDefaults')
{
public:
	Packet_7541Class(char *s);	// 0x862350 (Data is the caller's text, not copied)
	Packet_7541Class(RawPacketClass *raw);	// 0x862440
	virtual ~Packet_7541Class();	// +0x00 0x862520 / 0x862410
	virtual RawPacketClass *Build();	// +0x04 0x862560
	virtual bool Process_Packet(void *client);	// +0x08 0x868370 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x862630
	virtual int Print(char *buffer, int len);	// +0x10 0x862650

	int Size;	// +0x0C (4 bytes as they are; the text's with its 0)  // NAME 2: what it holds
	char *Data;	// +0x10  // NAME 3: the client's assert 'Data' and Print text 'SpawnLocationTemplateName='
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// 0x7542 (30018, not in the packet switch: sent only): vtable 0xB09338, constructor 0x862700, 0x18 bytes
class Packet_7542Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7542 (Print 'GameEditorSpawnDefaults')
{
public:
	Packet_7542Class(char *data, int size);	// 0x862680 (Data is the caller's, not copied)
	Packet_7542Class(RawPacketClass *raw);	// 0x862700
	virtual ~Packet_7542Class();	// +0x00 0x8627E0 / 0x8626D0
	virtual RawPacketClass *Build();	// +0x04 0x862820
	virtual bool Process_Packet(void *client);	// +0x08 0x862950
	virtual int Get_Data_Size();	// +0x0C 0x862900
	virtual int Print(char *buffer, int len);	// +0x10 0x862920

	char *Data;	// +0x0C (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data' and Print text 'DefaultValues='
	int Size;	// +0x10 (4 bytes as they are)  // NAME 2: what it holds
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

#endif
