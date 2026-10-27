/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\gameedit\pkgekv.h (the .cpp named by its asserts)
**
**	The packet classes of client 0x85E640-0x85F220 rebuilt so far (opcode 30006); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGEKV_H  // NAME 3 [pkgekv.h]: the header of the client's gameedit\pkgekv.cpp
#define PKGEKV_H

#include "..\pk.h"

// opcode 30005 (0x7535, ?): vtable 0xB08EB8, constructor 0x85E750, 0x20 bytes
class Packet_7535Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7535 (Print 'GameEditorRequestKeyValues')
{
public:
	Packet_7535Class(int game_id, char subscribe, char *s);	// 0x85E640 (Data is the caller's text, not copied; Subscribe = subscribe != 0)
	Packet_7535Class(RawPacketClass *raw);	// 0x85E750
	virtual ~Packet_7535Class();	// +0x00 0x85E860 / 0x85E720
	virtual RawPacketClass *Build();	// +0x04 0x85E8A0
	virtual bool Process_Packet(void *client);	// +0x08 0x868100 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85E990
	virtual int Print(char *buffer, int len);	// +0x10 0x85E9B0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Subscribe;	// +0x10 (0 or 1, sent as 4 bytes)  // NAME 3: the client's Print text 'Subscribe='
	int Size;	// +0x14 (Data's bytes with its 0)  // NAME 2: what it holds
	char *Data;	// +0x18 (the keys' text)  // NAME 3: the client's assert 'Data' and Print text 'Keys='
	bool Owned;	// +0x1C (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// opcode 30006 (0x7536, ?): vtable 0xB08EE8, constructor 0x85EA90, 0x10 bytes
class Packet_7536Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7536
{
public:
	Packet_7536Class(int game_id);	// 0x85EA00
	Packet_7536Class(RawPacketClass *raw);	// 0x85EA90
	virtual RawPacketClass *Build();	// +0x04 0x85EB20
	virtual bool Process_Packet(void *client);	// +0x08 0x868130 (clientstub.cpp l.119)
	virtual int Get_Data_Size();	// +0x0C 0x85EBE0
	virtual int Print(char *buffer, int len);	// +0x10 0x85EC00

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

// 0x7537 (30007, not in the packet switch: sent only): vtable 0xB08F18, constructor 0x85ECB0, 0x1C bytes
class Packet_7537Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7537 (Print 'GameEditorUpdateKeyValues')
{
public:
	Packet_7537Class(int game_id, char *data, int size);	// 0x85EC30 (Data is the caller's, not copied)
	Packet_7537Class(RawPacketClass *raw);	// 0x85ECB0
	virtual ~Packet_7537Class();	// +0x00 0x85EDA0 / 0x85EC80
	virtual RawPacketClass *Build();	// +0x04 0x85EDE0
	virtual bool Process_Packet(void *client);	// +0x08 0x85EF20
	virtual int Get_Data_Size();	// +0x0C 0x85EED0
	virtual int Print(char *buffer, int len);	// +0x10 0x85EEF0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char *Data;	// +0x10 (Size bytes: the key values)  // NAME 3: the client's assert 'Data'
	int Size;	// +0x14  // NAME 2: what it holds
	bool Owned;	// +0x18 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// opcode 30008 (0x7538, ?): vtable 0xB08F48, constructor 0x85EFC0, 0x1C bytes
class Packet_7538Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7538 (Print 'GameEditorModifyKeyValues')
{
public:
	Packet_7538Class(int game_id, char *data, int size);	// 0x85EF40 (Data is the caller's, not copied)
	Packet_7538Class(RawPacketClass *raw);	// 0x85EFC0
	virtual ~Packet_7538Class();	// +0x00 0x85F0B0 / 0x85EF90
	virtual RawPacketClass *Build();	// +0x04 0x85F0F0
	virtual bool Process_Packet(void *client);	// +0x08 0x868160 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85F1D0
	virtual int Print(char *buffer, int len);	// +0x10 0x85F1F0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char *Data;	// +0x10 (Size bytes: the key values)  // NAME 3: the client's assert 'Data'
	int Size;	// +0x14  // NAME 2: what it holds
	bool Owned;	// +0x18 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

#endif
