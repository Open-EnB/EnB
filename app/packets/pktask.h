/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pktask.h (the .cpp named by an assert)
**
**	The packet classes of client 0x8A8610-0x8AAA80 rebuilt so far (opcode 5000, 5001, 5002, 5003, 5004, 5005, 5006, 5009, 5010, 5011); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKTASK_H
#define PKTASK_H

#include "pk.h"
#include <list>

// The data of opcode 5000 (Print 'Task Hello'): 0x210 bytes, sent as they are
struct Unknown1388Struct  // NAME 1: neutral (D10): opcode 0x1388's data
{
	Unknown1388Struct();	// 0x8A8610
	Unknown1388Struct(int unknown00, int unknown04, short unknown08, const char *unknown0a, const char *unknown10a, int unknown20c);	// 0x8A8640
	Unknown1388Struct(const Unknown1388Struct &that);	// 0x8A86D0
	Unknown1388Struct &operator=(const Unknown1388Struct &that);	// 0x8A8750

	int Unknown00;	// +0x00
	int Unknown04;	// +0x04
	short Unknown08;	// +0x08
	char Unknown0A[0x100];	// +0x0A (strncpy'd)
	char Unknown10A[0x100];	// +0x10A (strncpy'd)
	int Unknown20C;	// +0x20C
};

// The data of opcode 5001 (Print 'Sector Hello') and of 5010's list entries: 0x218 bytes, sent as they are
struct Unknown1389Struct : public Unknown1388Struct  // NAME 1: neutral (D10): opcode 0x1389's data
{
	Unknown1389Struct();	// 0x8A87D0
	Unknown1389Struct(int unknown00, int unknown04, short unknown08, const char *unknown0a, const char *unknown10a, int unknown210, int unknown214);	// 0x8A8810 (unknown214 also goes to Unknown20C)
	Unknown1389Struct(const Unknown1389Struct &that);	// 0x8A88B0
	Unknown1389Struct &operator=(const Unknown1389Struct &that);	// 0x8A8970

	int Unknown210;	// +0x210
	int Unknown214;	// +0x214
};

// opcode 5000 (0x1388, ?): vtable 0xB0BD80, constructor 0x8A8B20, 0x21C bytes
class Packet_1388Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x1388 (Print 'Task Hello')
{
public:
	Packet_1388Class(const Unknown1388Struct &data);	// 0x8A8A10
	Packet_1388Class(RawPacketClass *raw);	// 0x8A8B20
	virtual RawPacketClass *Build();	// +0x04 0x8A8BC0
	virtual bool Process_Packet(void *client);	// +0x08 0x868400 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A8CB0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A8CD0
	virtual int Get_Type();	// +0x28 0x8A8C90  // NAME 2: as Packet_138AClass's

	Unknown1388Struct Data;	// +0x0C  // NAME 2: what it holds
};

// opcode 5001 (0x1389, ?): vtable 0xB0BDB4, constructor 0x8A8E30, 0x224 bytes
class Packet_1389Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x1389 (Print 'Sector Hello')
{
public:
	Packet_1389Class(const Unknown1389Struct &data);	// 0x8A8D00
	Packet_1389Class(RawPacketClass *raw);	// 0x8A8E30
	virtual RawPacketClass *Build();	// +0x04 0x8A8EF0
	virtual bool Process_Packet(void *client);	// +0x08 0x868430 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A8FE0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A9000
	virtual int Get_Type();	// +0x28 0x8A8FC0  // NAME 2: as Packet_138AClass's

	Unknown1389Struct Data;	// +0x0C  // NAME 2: what it holds
};

// opcode 5002 (0x138A, ?): vtable 0xB0BDE8, constructor 0x8A90D0, 0x14 bytes
class Packet_138AClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x138A
{
public:
	Packet_138AClass(int type, int num);	// 0x8A9030
	Packet_138AClass(RawPacketClass *raw);	// 0x8A90D0
	virtual RawPacketClass *Build();	// +0x04 0x8A9170
	virtual bool Process_Packet(void *client);	// +0x08 0x868460 (clientstub.cpp l.170)
	virtual int Get_Data_Size();	// +0x0C 0x8A9260
	virtual int Print(char *buffer, int len);	// +0x10 0x8A9280
	virtual int Get_Type();	// +0x28 0x8A9240  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Type;	// +0x0C  // NAME 3: the client's Print text 'Type='
	int Num;	// +0x10  // NAME 3: the client's Print text 'Num='
};

// opcode 5003 (0x138B, ?): vtable 0xB0BE1C, constructor 0x8A9360, 0x14 bytes
class Packet_138BClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x138B
{
public:
	Packet_138BClass(int type, int num);	// 0x8A92C0
	Packet_138BClass(RawPacketClass *raw);	// 0x8A9360
	virtual RawPacketClass *Build();	// +0x04 0x8A9400
	virtual bool Process_Packet(void *client);	// +0x08 0x868490 (clientstub.cpp l.173)
	virtual int Get_Data_Size();	// +0x0C 0x8A94F0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A9510
	virtual int Get_Type();	// +0x28 0x8A94D0  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Type;	// +0x0C  // NAME 3: the client's Print text 'Type='
	int Num;	// +0x10  // NAME 3: the client's Print text 'Num='
};

// opcode 5004 (0x138C, ?): vtable 0xB0BE50, constructor 0x8A95F0, 0x18 bytes
class Packet_138CClass : public Packet_138AClass  // NAME 1: neutral (D10): opcode 0x138C (Print 'Task Request')
{
public:
	Packet_138CClass(int type, int num);	// 0x8A9550 (Packet_138AClass's inlined)
	Packet_138CClass(RawPacketClass *raw);	// 0x8A95F0 (Packet_138AClass's inlined)
	virtual bool Process_Packet(void *client);	// +0x08 0x8684C0 (clientstub.cpp)
	virtual int Print(char *buffer, int len);	// +0x10 0x8A96D0
	virtual int Get_Type();	// +0x28 0x8A96B0  // NAME 2: as Packet_138AClass's

	int Unknown14;	// +0x14 (zeroed; not sent)
};

// opcode 5005 (0x138D, ?): vtable 0xB0BE84, constructor 0x8A9800, 0x1C bytes
class Packet_138DClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x138D (Print 'Task Ping')
{
public:
	Packet_138DClass(int ip, unsigned short port, char *string);	// 0x8A9710
	Packet_138DClass(RawPacketClass *raw);	// 0x8A9800
	virtual ~Packet_138DClass();	// +0x00 0x8A97D0 / 0x8A98D0
	virtual RawPacketClass *Build();	// +0x04 0x8A9910
	virtual bool Process_Packet(void *client);	// +0x08 0x8684F0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A9A20
	virtual int Print(char *buffer, int len);	// +0x10 0x8A9A40
	virtual int Get_Type();	// +0x28 0x8A9A00  // NAME 2: as Packet_138AClass's

	int Ip;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Ip='
	unsigned short Port;	// +0x10 (2 bytes as they are)  // NAME 3: the client's Print text 'Port='
	char *String;	// +0x14 (Size bytes: nstrdup'd or new[]'d)  // NAME 2: what it holds
	short Size;	// +0x18 (2 bytes as they are: the string's length with its 0)  // NAME 2: what it holds
};

// opcode 5006 (0x138E, ?): vtable 0xB0BEB8, constructor 0x8A9BB0, 0x1C bytes
class Packet_138EClass : public Packet_138DClass  // NAME 1: neutral (D10): opcode 0x138E (Print 'Task Pong')
{
public:
	Packet_138EClass(int ip, unsigned short port);	// 0x8A9A80 (Packet_138DClass's with "" inlined)
	Packet_138EClass(RawPacketClass *raw);	// 0x8A9BB0 (Packet_138DClass's inlined)
	virtual bool Process_Packet(void *client);	// +0x08 0x868520 (clientstub.cpp)
	virtual int Print(char *buffer, int len);	// +0x10 0x8A9CB0
	virtual int Get_Type();	// +0x28 0x8A9C90  // NAME 2: as Packet_138AClass's
};

// opcode 5009 (0x1391, ?): vtable 0xB0BEEC, constructor 0x8A9E80, 0x40 bytes
class Packet_1391Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x1391 (Print 'PlayerStatus')
{
public:
	Packet_1391Class(char *name, __int64 character_object_id, char *account, __int64 account_id, int ip, unsigned short port, int sector, int status, __int64 unknown38);	// 0x8A9CF0
	Packet_1391Class(RawPacketClass *raw);	// 0x8A9E80
	virtual ~Packet_1391Class();	// +0x00 0x8A9E50 / 0x8A9F90
	virtual RawPacketClass *Build();	// +0x04 0x8AA050
	virtual bool Process_Packet(void *client);	// +0x08 0x868550 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8AA1B0
	virtual int Print(char *buffer, int len);	// +0x10 0x8AA1F0
	virtual int Get_Type();	// +0x28 0x8AA190  // NAME 2: as Packet_138AClass's

	char *Name;	// +0x0C (nstrdup'd; sent as a string)  // NAME 3: the client's Print text 'Name=' and assert 'name && account'
	__int64 CharacterObjectID;	// +0x10 (8 bytes as they are)  // NAME 3: the client's Print text 'CharacterObjectID='
	char *Account;	// +0x18 (nstrdup'd; sent as a string)  // NAME 3: the client's Print text 'Account='
	__int64 AccountID;	// +0x20  // NAME 3: the client's Print text 'AccountID='
	int Ip;	// +0x28 (4 bytes as they are)  // NAME 3: the client's Print text 'Ip='
	unsigned short Port;	// +0x2C (2 bytes as they are)  // NAME 3: the client's Print text 'Port='
	int Sector;	// +0x30 (network order)  // NAME 3: the client's Print text 'Sector='
	int Status;	// +0x34 (4 bytes as they are)  // NAME 3: the client's Print text 'Status='
	__int64 Unknown38;	// +0x38 (8 bytes as they are; not printed)
};

// opcode 5010 (0x1392, ?): vtable 0xB0BF20, constructor 0x8AA490, 0x18 bytes
class Packet_1392Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x1392 (Print 'SectorList')
{
public:
	Packet_1392Class(const std::list<Unknown1389Struct> &list);	// 0x8AA280
	Packet_1392Class(RawPacketClass *raw);	// 0x8AA490
	virtual RawPacketClass *Build();	// +0x04 0x8AA670
	virtual bool Process_Packet(void *client);	// +0x08 0x868580 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8AA780
	virtual int Print(char *buffer, int len);	// +0x10 0x8AA7B0
	virtual int Get_Type();	// +0x28 0x8AA760  // NAME 2: as Packet_138AClass's

	std::list<Unknown1389Struct> List;	// +0x0C (a 4-byte count, then the entries as they are)  // NAME 2: what it holds
};

// opcode 5011 (0x1393, ?): vtable 0xB0BF54, constructor 0x8AA870, 0xC bytes
class Packet_1393Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x1393
{
public:
	Packet_1393Class();	// 0x8AA7E0
	Packet_1393Class(RawPacketClass *raw);	// 0x8AA870
	virtual RawPacketClass *Build();	// +0x04 0x8AA8B0
	virtual bool Process_Packet(void *client);	// +0x08 0x8685B0 (clientstub.cpp l.191)
	virtual int Get_Data_Size();	// +0x0C 0x8AA970
	virtual int Print(char *buffer, int len);	// +0x10 0x8AA990
	virtual int Get_Type();	// +0x28 0x8AA950  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

};

#endif
