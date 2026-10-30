/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_888b60.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x888B60-0x88BD10 (opcodes 173-177).
**
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_888B60_H  // NAME 1 [pkunknown_888b60]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_888B60_H

#include "pk.h"
#include <string>

#include <vector>

// 0x20 bytes: a string with a 2-byte length and two __int64 in network order. GroupServerLogin 173 holds one
// at +0x10, GroupServerUpdate 177 at +0x18, GroupServerCreate 176 two vectors of them; the packets' base
// reads and writes them (0x888E10-0x8891A8)
struct GroupServerEntryStruct  // NAME 1: neutral (D10): what it holds
{
	GroupServerEntryStruct();	// 0x888B60
	GroupServerEntryStruct(const std::string &unknown00, __int64 unknown10, __int64 unknown18);	// 0x888C90
	~GroupServerEntryStruct();	// 0x889230

	std::string Unknown00;	// +0x00  // NAME 1: neutral (D10)
	__int64 Unknown10;	// +0x10  // NAME 1: neutral (D10)
	__int64 Unknown18;	// +0x18  // NAME 1: neutral (D10)
};

// The group-server packets' base (vtable 0xB0A794: PacketClass's defaults; its implicit destructor 0x889520 /
// 0x8895B0 out of line here): opcodes 173, 176 and 177. Its helpers read and write the entries (they don't
// use this; 176's constructor calls Read through its packet)
class Unknown_B0A794PacketClass : public PacketClass  // NAME 1: neutral (D10): its vtable
{
public:
	int Get_Size(const GroupServerEntryStruct &entry);	// 0x888E10  // NAME 2: what it returns
	bool Write(const GroupServerEntryStruct &entry, RawPacketClass *raw);	// 0x888E40  // NAME 2: what it does
	bool Read(GroupServerEntryStruct &entry, RawPacketClass *raw);	// 0x888E80  // NAME 2: what it does
	int Get_Size(const std::vector<GroupServerEntryStruct> &list);	// 0x888EC0 (a 1-byte count, then the entries)
	bool Write(const std::vector<GroupServerEntryStruct> &list, RawPacketClass *raw);	// 0x888F10
	bool Read(std::vector<GroupServerEntryStruct> &list, RawPacketClass *raw);	// 0x888FB0
};

// opcode 173 (0x00AD, ?): vtable 0xB0A764, constructor 0x8895E0, 0x30 bytes
class Packet_00ADClass : public Unknown_B0A794PacketClass  // NAME 1: neutral (D10): opcode 0x00AD (its Print says 'GroupServerLoginPacketClass')
{
public:
	Packet_00ADClass(int unknown0c, const std::string &unknown10, __int64 unknown20, __int64 unknown28);	// 0x889280
	Packet_00ADClass(RawPacketClass *raw);	// 0x8895E0
	virtual RawPacketClass *Build();	// +0x04 0x889740
	virtual bool Process_Packet(void *client);	// +0x08 0x868E50 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x889820
	virtual int Print(char *buffer, int len);	// +0x10 0x889840

	int Unknown0C;	// +0x0C (network order)
	GroupServerEntryStruct Unknown10;	// +0x10
};

// opcode 174 (0x00AE, ?): vtable 0xB0A7C4, constructor 0x889940, 0x28 bytes
class Packet_00AEClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AE (its Print says 'GroupServerRequestPacketClass')
{
public:
	Packet_00AEClass(int unknown0c, int unknown10, __int64 unknown18, __int64 unknown20);	// 0x889870
	Packet_00AEClass(RawPacketClass *raw);	// 0x889940
	virtual RawPacketClass *Build();	// +0x04 0x889A10
	virtual bool Process_Packet(void *client);	// +0x08 0x868E80 (clientstub.cpp l.332)
	virtual int Get_Data_Size();	// +0x0C 0x889AF0
	virtual int Print(char *buffer, int len);	// +0x10 0x889B10

	int Unknown0C;	// +0x0C (in network order)
	int Unknown10;	// +0x10
	__int64 Unknown18;	// +0x18 (in network order)
	__int64 Unknown20;	// +0x20 (in network order)
};

// opcode 175 (0x00AF, ?): vtable 0xB0A7F4, constructor 0x889BE0, 0x14 bytes
class Packet_00AFClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AF
{
public:
	Packet_00AFClass(int unknown0_c, int unknown10);	// 0x889B40
	Packet_00AFClass(RawPacketClass *raw);	// 0x889BE0
	virtual RawPacketClass *Build();	// +0x04 0x889C80
	virtual bool Process_Packet(void *client);	// +0x08 0x868EB0 (clientstub.cpp l.335)
	virtual int Get_Data_Size();	// +0x0C 0x889D50
	virtual int Print(char *buffer, int len);	// +0x10 0x889D70

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

// opcode 176 (0x00B0, ?): vtable 0xB0A824, constructor 0x889F30, 0x40 bytes
class Packet_00B0Class : public Unknown_B0A794PacketClass  // NAME 1: neutral (D10): opcode 0x00B0 (its Print says 'GroupServerCreatePacketClass')
{
public:
	Packet_00B0Class(RawPacketClass *raw);	// 0x889F30
	virtual RawPacketClass *Build();	// +0x04 0x88A2F0
	virtual bool Process_Packet(void *client);	// +0x08 0x868EE0 (clientstub.cpp l.338)
	virtual int Get_Data_Size();	// +0x0C 0x88A4B0
	virtual int Print(char *buffer, int len);	// +0x10 0x88A530

	void Add_Unknown18(const GroupServerEntryStruct &entry);	// 0x88A560 (push_back)  // NAME 1: neutral (D10)
	void Add_Unknown28(const GroupServerEntryStruct &entry);	// 0x88A780 (push_back)  // NAME 1: neutral (D10)

	__int64 Unknown10;	// +0x10 (network order)
	std::vector<GroupServerEntryStruct> Unknown18;	// +0x18
	std::vector<GroupServerEntryStruct> Unknown28;	// +0x28
	bool Unknown38;	// +0x38 (true by default; 1 byte as it is)
	int Unknown3C;	// +0x3C (network order)
};

// opcode 177 (0x00B1, ?): vtable 0xB0A854, constructor 0x88ABC0, 0x40 bytes
class Packet_00B1Class : public Unknown_B0A794PacketClass  // NAME 1: neutral (D10): opcode 0x00B1 (its Print says 'GroupServerUpdatePacketClass')
{
public:
	Packet_00B1Class(__int64 unknown10, int unknown38);	// 0x88A9A0
	Packet_00B1Class(RawPacketClass *raw);	// 0x88ABC0
	virtual RawPacketClass *Build();	// +0x04 0x88AD40
	virtual bool Process_Packet(void *client);	// +0x08 0x868F10 (clientstub.cpp l.341)
	virtual int Get_Data_Size();	// +0x0C 0x88AE50
	virtual int Print(char *buffer, int len);	// +0x10 0x88AE70

	__int64 Unknown10;	// +0x10 (network order)
	GroupServerEntryStruct Unknown18;	// +0x18
	int Unknown38;	// +0x38 (4 bytes as they are)
	int Unknown3C;	// +0x3C (network order)
};

#endif
