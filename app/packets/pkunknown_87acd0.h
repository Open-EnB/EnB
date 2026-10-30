// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_87acd0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87ACD0-0x87DE90 rebuilt so far (opcode 170); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_87ACD0_H  // NAME 1 [pkunknown_87acd0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_87ACD0_H

#include "pk.h"
#include <string>
#include <vector>

// opcode 168 (0x00A8, ?): vtable 0xB09F30, constructor 0x87AE80, 0x68 bytes
class Packet_00A8Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00A8 (its Print says 'ChatProxyLoginPacketClass')
{
public:
	Packet_00A8Class();	// 0x87ACD0
	Packet_00A8Class(RawPacketClass *raw);	// 0x87AE80
	virtual ~Packet_00A8Class();	// +0x00 0x87AE50 / 0x87B280
	virtual RawPacketClass *Build();	// +0x04 0x87B3E0
	virtual bool Process_Packet(void *client);	// +0x08 0x868DC0 (clientstub.cpp l.320)
	virtual int Get_Data_Size();	// +0x0C 0x87B5A0
	virtual int Print(char *buffer, int len);	// +0x10 0x87B640

	int Unknown0C;	// +0x0C (4 bytes as they are)
	std::string Unknown10;	// +0x10 (a 2-byte length, then the bytes)
	std::string Unknown20;	// +0x20
	int Unknown30;	// +0x30 (network order)
	char Unknown34;	// +0x34 (1 byte)
	__int64 Unknown38;	// +0x38 (network order)
	__int64 Unknown40;	// +0x40 (network order)
	std::vector<std::string> Unknown48;	// +0x48 (a 4-byte count, then the strings)
	std::vector<std::string> Unknown58;	// +0x58 (a 4-byte count, then the strings)
};

// opcode 169 (0x00A9, ?): vtable 0xB09F60, constructor 0x87B7C0, 0x80 bytes
class Packet_00A9Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00A9 (its Print says 'ChatProxyRequestPacketClass')
{
public:
	Packet_00A9Class();	// 0x87B670
	Packet_00A9Class(RawPacketClass *raw);	// 0x87B7C0
	virtual ~Packet_00A9Class();	// +0x00 0x87B790 / 0x87BB60
	virtual RawPacketClass *Build();	// +0x04 0x87BD20
	virtual bool Process_Packet(void *client);	// +0x08 0x868D00 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87BEB0
	virtual int Print(char *buffer, int len);	// +0x10 0x87C160

	void Add_Unknown64(const std::string &string);	// 0x87BF40 (push_back)  // NAME 1: neutral (D10)

	int Unknown0C;	// +0x0C (4 bytes as they are)
	int Unknown10;	// +0x10 (4 bytes as they are; -1 by default)
	std::string Unknown14;	// +0x14 (a 2-byte length, then the bytes)
	std::string Unknown24;	// +0x24
	std::string Unknown34;	// +0x34
	std::string Unknown44;	// +0x44
	std::string Unknown54;	// +0x54
	std::vector<std::string> Unknown64;	// +0x64 (a 4-byte count, then the strings)
	char *Data;	// +0x74 (Size bytes, new'd when read)  // NAME 2: what it holds
	int Size;	// +0x78 (4 bytes as they are, before the data)  // NAME 2: what it holds
	bool Owned;	// +0x7C (the destructor deletes Data)  // NAME 2: what it means
};

// opcode 170 (0x00AA, ?): vtable 0xB09F90, constructor 0x87C230, 0x14 bytes
class Packet_00AAClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AA
{
public:
	Packet_00AAClass(int unknown0_c, int unknown10);	// 0x87C190
	Packet_00AAClass(RawPacketClass *raw);	// 0x87C230
	virtual RawPacketClass *Build();	// +0x04 0x87C2D0
	virtual bool Process_Packet(void *client);	// +0x08 0x868D30 (clientstub.cpp l.311)
	virtual int Get_Data_Size();	// +0x0C 0x87C3A0
	virtual int Print(char *buffer, int len);	// +0x10 0x87C3C0

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

// opcode 171 (0x00AB, ?): vtable 0xB09FC0, constructor 0x87C6C0, 0x50 bytes
class Packet_00ABClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AB (its Print says 'ChatProxyResponseListPacketClass')
{
public:
	Packet_00ABClass(const std::string &unknown0c);	// 0x87C3F0
	Packet_00ABClass(RawPacketClass *raw);	// 0x87C6C0
	virtual RawPacketClass *Build();	// +0x04 0x87CB00
	virtual bool Process_Packet(void *client);	// +0x08 0x868D60 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87CC80
	virtual int Print(char *buffer, int len);	// +0x10 0x87D150

	void Add_Unknown30(const std::string &string);	// 0x87CD10 (push_back)  // NAME 1: neutral (D10)
	void Add_Unknown40(const std::string &string);	// 0x87CF30 (push_back)  // NAME 1: neutral (D10)

	std::string Unknown0C;	// +0x0C (a 2-byte length, then the bytes)
	std::string Unknown1C;	// +0x1C
	int Unknown2C;	// +0x2C (4 bytes as they are; -1 by default)
	std::vector<std::string> Unknown30;	// +0x30 (a 4-byte count, then the strings)
	std::vector<std::string> Unknown40;	// +0x40 (a 4-byte count, then the strings)
};

// opcode 172 (0x00AC, ?): vtable 0xB09FF0, constructor 0x87D2E0, 0x8C bytes
class Packet_00ACClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AC (its Print says 'ChatProxyEventPacketClass')
{
public:
	Packet_00ACClass();	// 0x87D180
	Packet_00ACClass(RawPacketClass *raw);	// 0x87D2E0
	virtual ~Packet_00ACClass();	// +0x00 0x87D2B0 / 0x87D690
	virtual RawPacketClass *Build();	// +0x04 0x87D860
	virtual bool Process_Packet(void *client);	// +0x08 0x868D90 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x87D9F0
	virtual int Print(char *buffer, int len);	// +0x10 0x87DA90

	void Add_Unknown0C(const std::string &string);	// 0x87DAC0 (push_back unless already there)  // NAME 1: neutral (D10)

	std::vector<std::string> Unknown0C;	// +0x0C (a 4-byte count, then the strings)
	int Unknown1C;	// +0x1C (4 bytes as they are)
	std::string Unknown20;	// +0x20 (a 2-byte length, then the bytes)
	std::string Unknown30;	// +0x30
	std::string Unknown40;	// +0x40
	std::string Unknown50;	// +0x50
	std::string Unknown60;	// +0x60
	std::string Unknown70;	// +0x70
	char *Data;	// +0x80 (Size bytes, new'd when read)  // NAME 2: what it holds
	int Size;	// +0x84 (4 bytes as they are, before the data)  // NAME 2: what it holds
	bool Owned;	// +0x88 (the destructor deletes Data)  // NAME 2: what it means
};

#endif
