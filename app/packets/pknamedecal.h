// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pknamedecal.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8805C0-0x8812D0 rebuilt so far (opcodes 16, 178); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKNAMEDECAL_H  // NAME 1 [pknamedecal]: provisional, pk + its first named class's name
#define PKNAMEDECAL_H

#include "pk.h"
#include <vector>
#include "vector4.h"
#include "vector3.h"

// One decal of a DECAL packet: 24 bytes, read and written as 4 + 4 + 16 bytes (its vector's insert
// helpers are this object's, 0x8810F0-0x881250). The 16 bytes are a Vector4: the client copies a
// decal member by member (Vector4's copy constructor), not as a block (rep movsd)
struct DecalStruct  // NAME 1: neutral (D10), the packet's 'NumDecals'
{
	int Unknown00;	// +0x00
	int Unknown04;	// +0x04
	Vector4 Unknown08;	// +0x08 (the inline copies in 0x8805C0 / 0x8807B0 copy it as Vector4's copy constructor does)
};

// opcode 16 (0x0010, DECAL): vtable 0xB0A200, constructor 0x8807B0, 0x24 bytes
class DecalPacketClass : public UnknownPacketBaseClass
{
public:
	DecalPacketClass(int game_id, const std::vector<DecalStruct> &decals);	// 0x8805C0
	DecalPacketClass(PacketClass *raw);	// 0x8807B0
	virtual PacketClass *Build();	// +0x04 0x880BC0
	virtual bool Process_Packet(void *client);	// +0x08 0x735DF0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x880D00
	virtual int Print(char *buffer, int len);	// +0x10 0x880D50
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x880720

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	std::vector<DecalStruct> Decals;	// +0x10 (a short count, then the decals)  // NAME 3: the client's Print text 'NumDecals='
	int Channel;	// +0x20 (UnknownPacketBaseClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
};

// opcode 178 (0x00B2, NAME_DECAL): vtable 0xB0A230, constructor 0x880EE0, 0x40 bytes
class NameDecalPacketClass : public UnknownPacketBaseClass
{
public:
	NameDecalPacketClass(int game_id, const char *name, const Vector3 &rgb);	// 0x880DA0  // NAME 2: the stores (no caller left)
	NameDecalPacketClass(PacketClass *raw);	// 0x880EE0
	virtual PacketClass *Build();	// +0x04 0x880FA0
	virtual bool Process_Packet(void *client);	// +0x08 0x735FC0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x881080
	virtual int Print(char *buffer, int len);	// +0x10 0x8810A0
	virtual int Get_Channel() { return(Unknown3C); }	// +0x18 0x880E70

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Name[32];	// +0x10  // NAME 3: the client's Print text 'Name='
	Vector3 ColorRGB;	// +0x30  // NAME 2: the client's Print text 'RGB=' (RGB itself is windows.h's macro: udpreplay's one-TU build)
	int Unknown3C;	// +0x3C
};

#endif
