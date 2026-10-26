/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgloballock.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8653F0-0x865D30 (opcodes 227, 228, 229) over their common base
**	(vtable 0xB094F4).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALLOCK_H  // NAME 1 [pkgloballock]: provisional, pk + its classes' common name
#define PKGLOBALLOCK_H

#include "pk.h"

// the base of GlobalLockRequestPacketClass, GlobalLockResponsePacketClass, GlobalLockReleasePacketClass:
// vtable 0xB094F4, constructors 0x8653F0 / 0x865470 (inlined in the derived ones)
class GlobalLockPacketClass : public PacketClass  // NAME 3: by convention from its derived classes' Print texts 'GlobalLock...'
{
public:
	GlobalLockPacketClass(__int64 a, int b);	// 0x8653F0
	GlobalLockPacketClass(RawPacketClass *raw);	// 0x865470
	virtual RawPacketClass *Build() = 0;	// +0x04 _purecall
	virtual bool Process_Packet(void *client) = 0;	// +0x08 _purecall
	virtual int Get_Data_Size();	// +0x0C 0x865570
	virtual int Print(char *buffer, int len) = 0;	// +0x10 _purecall
	virtual void Unknown_28(RawPacketClass *raw);	// +0x28 0x865530: Adds the base's members  // NAME 1: neutral (D10)

	__int64 Unknown10;	// +0x10
	int Unknown18;	// +0x18
};

// opcode 227 (0x00E3, ?): vtable 0xB09528, constructor 0x865630, 0x28 bytes
class GlobalLockRequestPacketClass : public GlobalLockPacketClass  // NAME 3: the client's Print text 'GlobalLockRequest'
{
public:
	GlobalLockRequestPacketClass(__int64 a, int b, int x);	// 0x865590
	GlobalLockRequestPacketClass(RawPacketClass *raw);	// 0x865630
	virtual RawPacketClass *Build();	// +0x04 0x865710
	virtual bool Process_Packet(void *client);	// +0x08 0x8694E0 (clientstub.cpp l.434)
	virtual int Get_Data_Size();	// +0x0C 0x8657E0
	virtual int Print(char *buffer, int len);	// +0x10 0x865800

	int Unknown20;	// +0x20
};

// opcode 228 (0x00E4, ?): vtable 0xB0955C, constructor 0x8658D0, 0x28 bytes
class GlobalLockResponsePacketClass : public GlobalLockPacketClass  // NAME 3: the client's Print text 'GlobalLockResponse'
{
public:
	GlobalLockResponsePacketClass(int x, __int64 a, int b);	// 0x865830
	GlobalLockResponsePacketClass(RawPacketClass *raw);	// 0x8658D0
	virtual RawPacketClass *Build();	// +0x04 0x8659B0
	virtual bool Process_Packet(void *client);	// +0x08 0x869510 (clientstub.cpp l.437)
	virtual int Get_Data_Size();	// +0x0C 0x865A80
	virtual int Print(char *buffer, int len);	// +0x10 0x865AA0

	int Unknown20;	// +0x20
};

// opcode 229 (0x00E5, ?): vtable 0xB09590, constructor 0x865B50, 0x20 bytes
class GlobalLockReleasePacketClass : public GlobalLockPacketClass  // NAME 3: the client's Print text 'GlobalLockRelease'
{
public:
	GlobalLockReleasePacketClass(__int64 a);	// 0x865AD0
	GlobalLockReleasePacketClass(RawPacketClass *raw);	// 0x865B50
	virtual RawPacketClass *Build();	// +0x04 0x865C10
	virtual bool Process_Packet(void *client);	// +0x08 0x869540 (clientstub.cpp l.440)
	virtual int Get_Data_Size();	// +0x0C 0x865CE0
	virtual int Print(char *buffer, int len);	// +0x10 0x865D00
};

#endif
