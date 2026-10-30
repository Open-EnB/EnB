/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkdirlock.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8816D0-0x882680 (opcodes 223, 225, 224, 226) over their common
**	base (vtable 0xB0A290).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKDIRLOCK_H  // NAME 1 [pkdirlock]: provisional, pk + its classes' common name
#define PKDIRLOCK_H

#include "pk.h"

// the 8 bytes at +0x20, read and written raw, passed by value  // NAME 1: neutral (D10)
struct Unknown_DirLockStruct
{
	int Unknown00;
	int Unknown04;
};

// the base of DirLockRequestPacketClass, DirLockResponseSectorPacketClass, DirLockResponseGlobalPacketClass,
// DirLockReleasePacketClass: vtable 0xB0A290, constructors 0x8816D0 / 0x8817C0 (inlined in the derived ones)
class DirLockPacketClass : public PacketClass  // NAME 3: by convention from its derived classes' Print texts 'DirLock...'
{
public:
	DirLockPacketClass(int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d);	// 0x8816D0
	DirLockPacketClass(RawPacketClass *raw);	// 0x8817C0
	virtual ~DirLockPacketClass();	// +0x00 0x881880 / 0x881790
	virtual RawPacketClass *Build() = 0;	// +0x04 _purecall
	virtual bool Process_Packet(void *client) = 0;	// +0x08 _purecall
	virtual int Get_Data_Size();	// +0x0C 0x881910
	virtual int Print(char *buffer, int len) = 0;	// +0x10 _purecall
	virtual void Unknown_28(RawPacketClass *raw);	// +0x28 0x8818C0: Adds the base's members  // NAME 1: neutral (D10)

	int Unknown0C;	// +0x0C
	char *Unknown10;	// +0x10 (nstrdup / Get_String; deleted by the destructor)
	__int64 Unknown18;	// +0x18
	Unknown_DirLockStruct Unknown20;	// +0x20
	int Unknown28;	// +0x28
};

// opcode 223 (0x00DF, ?): vtable 0xB0A2C4, constructor 0x881A30, 0x30 bytes
class DirLockRequestPacketClass : public DirLockPacketClass  // NAME 3: the client's Print text 'DirLockRequest'
{
public:
	DirLockRequestPacketClass(int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d);	// 0x881930
	DirLockRequestPacketClass(RawPacketClass *raw);	// 0x881A30
	virtual RawPacketClass *Build();	// +0x04 0x881B30
	virtual bool Process_Packet(void *client);	// +0x08 0x8693C0 (clientstub.cpp l.416)
	virtual int Get_Data_Size();	// +0x0C 0x881C10
	virtual int Print(char *buffer, int len);	// +0x10 0x881C30
};

// opcode 225 (0x00E1, ?): vtable 0xB0A2F8, constructor 0x881D60, 0x38 bytes
class DirLockResponseSectorPacketClass : public DirLockPacketClass  // NAME 3: the client's Print text 'DirLockResponseSector'
{
public:
	DirLockResponseSectorPacketClass(int x, int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d);	// 0x881C60
	DirLockResponseSectorPacketClass(RawPacketClass *raw);	// 0x881D60
	virtual RawPacketClass *Build();	// +0x04 0x881E80
	virtual bool Process_Packet(void *client);	// +0x08 0x869420 (clientstub.cpp l.422)
	virtual int Get_Data_Size();	// +0x0C 0x881F70
	virtual int Print(char *buffer, int len);	// +0x10 0x881F90

	int Unknown30;	// +0x30
};

// opcode 224 (0x00E0, ?): vtable 0xB0A32C, constructor 0x8820D0, 0x38 bytes
class DirLockResponseGlobalPacketClass : public DirLockPacketClass  // NAME 3: the client's Print text 'DirLockResponseGlobal'
{
public:
	DirLockResponseGlobalPacketClass(int x, int a, const char *name, __int64 b, Unknown_DirLockStruct c, int y, int d);	// 0x881FC0
	DirLockResponseGlobalPacketClass(RawPacketClass *raw);	// 0x8820D0
	virtual RawPacketClass *Build();	// +0x04 0x8821F0
	virtual bool Process_Packet(void *client);	// +0x08 0x8693F0 (clientstub.cpp l.419)
	virtual int Get_Data_Size();	// +0x0C 0x882300
	virtual int Print(char *buffer, int len);	// +0x10 0x882320

	int Unknown30;	// +0x30
	int Unknown34;	// +0x34
};

// opcode 226 (0x00E2, ?): vtable 0xB0A360, constructor 0x882450, 0x30 bytes
class DirLockReleasePacketClass : public DirLockPacketClass  // NAME 3: the client's Print text 'DirLockRelease'
{
public:
	DirLockReleasePacketClass(int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d);	// 0x882350
	DirLockReleasePacketClass(RawPacketClass *raw);	// 0x882450
	virtual RawPacketClass *Build();	// +0x04 0x882550
	virtual bool Process_Packet(void *client);	// +0x08 0x869450 (clientstub.cpp l.425)
	virtual int Get_Data_Size();	// +0x0C 0x882630
	virtual int Print(char *buffer, int len);	// +0x10 0x882650
};

#endif
