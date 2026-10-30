/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\gameedit\pkgeplan.h (the .cpp named by its asserts)
**
**	The packet classes of client 0x860D30-0x861730 rebuilt so far (opcode 30025); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGEPLAN_H  // NAME 3 [pkgeplan.h]: the header of the client's gameedit\pkgeplan.cpp
#define PKGEPLAN_H

#include "..\pk.h"

// opcode 30023 (0x7547, ?): vtable 0xB09188, constructor 0x860DA0, 0xC bytes
class GameEditorRequestPlanetUnitTypesPacketClass : public PacketClass  // NAME 3: the client's Print text 'GameEditorRequestPlanetUnitTypes {' and the PacketClass convention
{
public:
	GameEditorRequestPlanetUnitTypesPacketClass();	// 0x860D30
	GameEditorRequestPlanetUnitTypesPacketClass(RawPacketClass *raw);	// 0x860DA0
	virtual RawPacketClass *Build();	// +0x04 0x860E00
	virtual bool Process_Packet(void *client);	// +0x08 0x8682E0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x860E90
	virtual int Print(char *buffer, int len);	// +0x10 0x860EB0
};

// 0x7548 (30024, not in the packet switch: sent only): vtable 0xB091B8, constructor 0x860F80, 0x18 bytes
class Packet_7548Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7548 (Print 'GameEditorPlanetUnitTypes')
{
public:
	Packet_7548Class(char *data, int size);	// 0x860F00 (Data is the caller's, not copied)
	Packet_7548Class(RawPacketClass *raw);	// 0x860F80
	virtual ~Packet_7548Class();	// +0x00 0x861060 / 0x860F50
	virtual RawPacketClass *Build();	// +0x04 0x8610A0
	virtual bool Process_Packet(void *client);	// +0x08 0x8611D0
	virtual int Get_Data_Size();	// +0x0C 0x861180
	virtual int Print(char *buffer, int len);	// +0x10 0x8611A0

	char *Data;	// +0x0C (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data' and Print text 'Types='
	int Size;	// +0x10  // NAME 2: what it holds
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// opcode 30025 (0x7549, ?): vtable 0xB091E8, constructor 0x861260, 0x10 bytes
class Packet_7549Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7549
{
public:
	Packet_7549Class(int unit_type);	// 0x8611F0
	Packet_7549Class(RawPacketClass *raw);	// 0x861260
	virtual RawPacketClass *Build();	// +0x04 0x861310
	virtual bool Process_Packet(void *client);	// +0x08 0x868310 (clientstub.cpp l.149)
	virtual int Get_Data_Size();	// +0x0C 0x8613D0
	virtual int Print(char *buffer, int len);	// +0x10 0x8613F0

	int UnitType;	// +0x0C  // NAME 3: the client's Print text 'UnitType='
};

// 0x754A (30026, not in the packet switch: sent only): vtable 0xB09218, constructor 0x8614A0, 0x18 bytes
class Packet_754AClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x754A (Print 'GameEditorPlanetUnitDetails')
{
public:
	Packet_754AClass(char *data, int size);	// 0x861420 (Data is the caller's, not copied)
	Packet_754AClass(RawPacketClass *raw);	// 0x8614A0
	virtual ~Packet_754AClass();	// +0x00 0x861590 / 0x861470
	virtual RawPacketClass *Build();	// +0x04 0x8615D0
	virtual bool Process_Packet(void *client);	// +0x08 0x861710
	virtual int Get_Data_Size();	// +0x0C 0x8616C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8616E0

	char *Data;	// +0x0C (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data' and Print text 'Details='
	int Size;	// +0x10  // NAME 2: what it holds
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

#endif
