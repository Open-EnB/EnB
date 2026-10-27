/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkrecustomize.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89FB50-0x8A14D0 (opcodes 129-133; 130-133 rebuilt so far).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKRECUSTOMIZE_H  // NAME 1 [pkrecustomize]: provisional, pk + what its classes share
#define PKRECUSTOMIZE_H

#include "pk.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\shipcust_desc.h"

// The 14 ints of recustomization_cost.cpp's wire object (0x38 bytes; the constructor 0x849320 copies one)
struct RecustomizationCostDataStruct  // NAME 1: neutral (D10)
{
	int Unknown00[14];	// +0x00  // NAME 1: neutral (D10)
};

// recustomization_cost.cpp's wire object (client 0x848100-0x84A270, not rebuilt): vtable 0xB08C6C,
// 0x3C bytes; Write 0x849920 / Read 0x849960 move its 14 ints, Get_Size 0x8499A0 returns 0x38
class RecustomizationCostClass : public Unknown_PacketDataClass  // NAME 1: neutral, after the client's file recustomization_cost.cpp (D10)
{
public:
	RecustomizationCostClass();	// 0x8492F0 (the 14 ints 0)
	RecustomizationCostClass(const RecustomizationCostDataStruct &data);	// 0x849320
	virtual bool Write(LinearBufferClass *buffer);	// +0x00 0x849920
	virtual bool Read(LinearBufferClass *buffer);	// +0x04 0x849960
	virtual int Get_Size();	// +0x08 0x8499A0

	RecustomizationCostDataStruct Data;	// +0x04  // NAME 2: what it holds
};

// opcode 130 (0x0082, RECUSTOMIZE_SHIP_DONE): vtable 0xB0B5F4, constructor 0x8A01D0, 0x170 bytes
class RecustomizeShipDonePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name RECUSTOMIZE_SHIP_DONE (the Net-7 opcode table), the client's Print text 'Recustomize Ship Done'
{
public:
	RecustomizeShipDonePacketClass(RawPacketClass *raw);	// 0x8A01D0
	RecustomizeShipDonePacketClass(int unknown16C, const ShipCustDescClass &desc);	// 0x8A0380
	// the destructor is the implicit one: +0x00 0x8A0350 / 0x61F720 (the linker folded its body with another; a declared one stores the class's own vtable first, the client's doesn't)
	virtual RawPacketClass *Build();	// +0x04 0x8A05F0
	virtual bool Process_Packet(void *client);	// +0x08 0x869630 (clientstub.cpp l.455)
	virtual int Get_Data_Size();	// +0x0C 0x8A06C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A06E0

	ShipCustDescClass Desc;	// +0x0C  // NAME 2: what it holds
	int Unknown16C;	// +0x16C  // NAME 1: neutral (D10)
};

// opcode 131 (0x0083, RECUSTOMIZE_AVATAR_START): vtable 0xB0B624, constructor 0x8A0710, 0x4C bytes
class RecustomizeAvatarStartPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name RECUSTOMIZE_AVATAR_START (the Net-7 opcode table), the client's Print text 'Recustomize Avatar Start'
{
public:
	RecustomizeAvatarStartPacketClass(RawPacketClass *raw);	// 0x8A0710
	RecustomizeAvatarStartPacketClass(int unknown48, const RecustomizationCostClass &cost);	// 0x8A0820
	virtual ~RecustomizeAvatarStartPacketClass();	// +0x00 0x8A07D0 / 0x8A0800
	virtual RawPacketClass *Build();	// +0x04 0x8A08C0
	virtual bool Process_Packet(void *client);	// +0x08 0x73F570 (cpacket.cpp, not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A0990
	virtual int Print(char *buffer, int len);	// +0x10 0x8A09B0

	RecustomizationCostClass Cost;	// +0x0C  // NAME 2: what it holds (its class's file)
	int Unknown48;	// +0x48  // NAME 1: neutral (D10)
};

// opcode 132 (0x0084, RECUSTOMIZE_AVATAR_DONE): vtable 0xB0B654, constructor 0x8A09E0, 0x11C bytes
class RecustomizeAvatarDonePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name RECUSTOMIZE_AVATAR_DONE (the Net-7 opcode table), the client's Print text 'Recustomize Avatar Done'
{
public:
	RecustomizeAvatarDonePacketClass(RawPacketClass *raw);	// 0x8A09E0
	RecustomizeAvatarDonePacketClass(int unknown118, const AvatarDescClass &desc);	// 0x8A0AE0
	// the destructor is the implicit one: +0x00 0x8A0AB0 / 0x54EDD0 (the linker folded its body with another) (a declared one stores the class's own vtable first; the client's doesn't)
	virtual RawPacketClass *Build();	// +0x04 0x8A0E00
	virtual bool Process_Packet(void *client);	// +0x08 0x869660 (clientstub.cpp l.458)
	virtual int Get_Data_Size();	// +0x0C 0x8A0ED0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A0EF0

	AvatarDescClass Desc;	// +0x0C  // NAME 2: what it holds
	int Unknown118;	// +0x118  // NAME 1: neutral (D10)
};

// opcode 133 (0x0085, RECUSTOMIZE_AVATAR_UPDATE): vtable 0xB0B684, constructor 0x8A0F20, 0x11C bytes
class RecustomizeAvatarUpdatePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name RECUSTOMIZE_AVATAR_UPDATE (the Net-7 opcode table), the client's Print text 'Recustomize Avatar Update'
{
public:
	RecustomizeAvatarUpdatePacketClass(RawPacketClass *raw);	// 0x8A0F20
	RecustomizeAvatarUpdatePacketClass(int unknown118, const AvatarDescClass &desc);	// 0x8A1090
	// the destructor is the implicit one: +0x00 0x8A0FF0 / 0x8A1020 (a declared one stores the class's own vtable first; the client's doesn't)
	virtual RawPacketClass *Build();	// +0x04 0x8A13B0
	virtual bool Process_Packet(void *client);	// +0x08 0x73F5F0 (cpacket.cpp, not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8A1480
	virtual int Print(char *buffer, int len);	// +0x10 0x8A14A0

	AvatarDescClass Desc;	// +0x0C  // NAME 2: what it holds
	int Unknown118;	// +0x118  // NAME 1: neutral (D10)
};

#endif
