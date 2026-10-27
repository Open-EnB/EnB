/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85f870.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x85F870-0x860D00 rebuilt so far (opcode 30016, 30014, 30015, 30009, 30010, 30012; 30011 and 30013, not in the switch); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85F870_H  // NAME 1 [pkunknown_85f870]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85F870_H

#include "pk.h"

// opcode 30016 (0x7540, ?): vtable 0xB09008, constructor 0x85F8F0, 0x18 bytes
class Packet_7540Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7540 (Print 'GameEditorCreateFinal')
{
public:
	Packet_7540Class(char *data, int size);	// 0x85F870 (Data is the caller's, not copied)
	Packet_7540Class(RawPacketClass *raw);	// 0x85F8F0
	virtual ~Packet_7540Class();	// +0x00 0x85F9D0 / 0x85F8C0
	virtual RawPacketClass *Build();	// +0x04 0x85FA10
	virtual bool Process_Packet(void *client);	// +0x08 0x8681C0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85FAE0
	virtual int Print(char *buffer, int len);	// +0x10 0x85FB00

	int Size;	// +0x0C (4 bytes as they are)  // NAME 2: what it holds
	char *Data;	// +0x10 (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data'
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// opcode 30014 (0x753E, ?): vtable 0xB09038, constructor 0x85FBB0, 0x18 bytes
class Packet_753EClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753E (Print 'GameEditorCreateEmpty')
{
public:
	Packet_753EClass(char *data, int size);	// 0x85FB30 (Data is the caller's, not copied)
	Packet_753EClass(RawPacketClass *raw);	// 0x85FBB0
	virtual ~Packet_753EClass();	// +0x00 0x85FC90 / 0x85FB80
	virtual RawPacketClass *Build();	// +0x04 0x85FCD0
	virtual bool Process_Packet(void *client);	// +0x08 0x8681F0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85FDA0
	virtual int Print(char *buffer, int len);	// +0x10 0x85FDC0

	int Size;	// +0x0C (4 bytes as they are)  // NAME 2: what it holds
	char *Data;	// +0x10 (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data'
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// opcode 30015 (0x753F, ?): vtable 0xB09068, constructor 0x85FE60, 0xC bytes
class Packet_753FClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753F
{
public:
	Packet_753FClass();	// 0x85FDF0
	Packet_753FClass(RawPacketClass *raw);	// 0x85FE60
	virtual RawPacketClass *Build();	// +0x04 0x85FEC0
	virtual bool Process_Packet(void *client);	// +0x08 0x868220 (clientstub.cpp l.134)
	virtual int Get_Data_Size();	// +0x0C 0x85FF50
	virtual int Print(char *buffer, int len);	// +0x10 0x85FF70

};

// opcode 30009 (0x7539, ?): vtable 0xB09098, constructor 0x860030, 0x10 bytes
class Packet_7539Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7539
{
public:
	Packet_7539Class(int game_id);	// 0x85FFA0
	Packet_7539Class(RawPacketClass *raw);	// 0x860030
	virtual RawPacketClass *Build();	// +0x04 0x8600C0
	virtual bool Process_Packet(void *client);	// +0x08 0x868250 (clientstub.cpp l.137)
	virtual int Get_Data_Size();	// +0x0C 0x860180
	virtual int Print(char *buffer, int len);	// +0x10 0x8601A0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

// opcode 30010 (0x753A, ?): vtable 0xB090C8, constructor 0x8602C0, 0x40 bytes
class Packet_753AClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753A
{
public:
	Packet_753AClass(RawPacketClass *raw);	// 0x8602C0
	virtual RawPacketClass *Build();	// +0x04 0x860360
	virtual bool Process_Packet(void *client);	// +0x08 0x868280 (clientstub.cpp l.140)
	virtual int Get_Data_Size();	// +0x0C 0x860430
	virtual int Print(char *buffer, int len);	// +0x10 0x860450

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Unknown10[48];	// +0x10
};

// 0x753B (30011, not in the packet switch: sent only): vtable 0xB090F8, constructor 0x860500, 0x18 bytes
class Packet_753BClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753B (Print 'GameEditorTemplateCategories')
{
public:
	Packet_753BClass(char *data, int size);	// 0x860480 (Data is the caller's, not copied)
	Packet_753BClass(RawPacketClass *raw);	// 0x860500
	virtual ~Packet_753BClass();	// +0x00 0x8605E0 / 0x8604D0
	virtual RawPacketClass *Build();	// +0x04 0x860620
	virtual bool Process_Packet(void *client);	// +0x08 0x860740
	virtual int Get_Data_Size();	// +0x0C 0x8606F0
	virtual int Print(char *buffer, int len);	// +0x10 0x860710

	char *Data;	// +0x0C (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data'
	int Size;	// +0x10 (4 bytes as they are)  // NAME 2: what it holds
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// opcode 30012 (0x753C, ?): vtable 0xB09128, constructor 0x8607E0, 0x18 bytes
class Packet_753CClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753C (Print 'GameEditorRequestTemplateList')
{
public:
	Packet_753CClass(char *data, int size);	// 0x860760 (Data is the caller's, not copied)
	Packet_753CClass(RawPacketClass *raw);	// 0x8607E0
	virtual ~Packet_753CClass();	// +0x00 0x8608C0 / 0x8607B0
	virtual RawPacketClass *Build();	// +0x04 0x860900
	virtual bool Process_Packet(void *client);	// +0x08 0x8682B0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8609D0
	virtual int Print(char *buffer, int len);	// +0x10 0x8609F0

	int Size;	// +0x0C (4 bytes as they are)  // NAME 2: what it holds
	char *Data;	// +0x10 (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data'
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

// 0x753D (30013, not in the packet switch: sent only): vtable 0xB09158, constructor 0x860AA0, 0x18 bytes
class Packet_753DClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x753D (Print 'GameEditorTemplateList')
{
public:
	Packet_753DClass(char *data, int size);	// 0x860A20 (Data is the caller's, not copied)
	Packet_753DClass(RawPacketClass *raw);	// 0x860AA0
	virtual ~Packet_753DClass();	// +0x00 0x860B80 / 0x860A70
	virtual RawPacketClass *Build();	// +0x04 0x860BC0
	virtual bool Process_Packet(void *client);	// +0x08 0x860CE0
	virtual int Get_Data_Size();	// +0x0C 0x860C90
	virtual int Print(char *buffer, int len);	// +0x10 0x860CB0

	char *Data;	// +0x0C (Size bytes; printed as a string)  // NAME 3: the client's assert 'Data'
	int Size;	// +0x10 (4 bytes as they are)  // NAME 2: what it holds
	bool Owned;	// +0x14 (Data new[]'d by the packet: the destructor deletes it)  // NAME 2: what it means, as MessageStringPacketClass's
};

#endif
