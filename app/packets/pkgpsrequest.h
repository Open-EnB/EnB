/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgpsrequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x887F20-0x888670 (opcodes 221 and 222).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGPSREQUEST_H  // NAME 1 [pkgpsrequest]: provisional, pk + its first class's name
#define PKGPSREQUEST_H

#include "pk.h"

// opcode 221 (0x00DD, GPS_REQUEST): vtable 0xB0A6D4, constructor 0x888020, 0x20 bytes
class GpsRequestPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name GPS_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	GpsRequestPacketClass(char *unknown0c, char *unknown10, char *unknown14, char *unknown18, int unknown1c);	// 0x887F20
	GpsRequestPacketClass(RawPacketClass *raw);	// 0x888020
	virtual ~GpsRequestPacketClass();	// +0x00 (its scalar deleting destructor first)
	virtual RawPacketClass *Build();	// +0x04 0x888150
	virtual bool Process_Packet(void *client);	// +0x08 0x869480 (clientstub.cpp l.428)
	virtual int Get_Data_Size();	// +0x0C 0x888240
	virtual int Print(char *buffer, int len);	// +0x10 0x888290

	char *Unknown0C;	// +0x0C (new[]'d)
	char *Unknown10;	// +0x10 (new[]'d)
	char *Unknown14;	// +0x14 (new[]'d)
	char *Unknown18;	// +0x18 (new[]'d)
	int Unknown1C;	// +0x1C (network order)
};

// opcode 222 (0x00DE, ?): vtable 0xB0A704, constructor 0x8883C0, 0x24 bytes
class GpsResponsePacketClass : public PacketClass  // NAME 3: the client's Print text 'GPSResponse'
{
public:
	GpsResponsePacketClass(char *unknown0c, char *unknown10, char *unknown14, char *unknown18, int unknown1c, int unknown20);	// 0x8882C0
	GpsResponsePacketClass(RawPacketClass *raw);	// 0x8883C0
	virtual ~GpsResponsePacketClass();	// +0x00 (its scalar deleting destructor first)
	virtual RawPacketClass *Build();	// +0x04 0x888500
	virtual bool Process_Packet(void *client);	// +0x08 0x8694B0 (clientstub.cpp l.431)
	virtual int Get_Data_Size();	// +0x0C 0x8885F0
	virtual int Print(char *buffer, int len);	// +0x10 0x888640

	char *Unknown0C;	// +0x0C (new[]'d)
	char *Unknown10;	// +0x10 (new[]'d)
	char *Unknown14;	// +0x14 (new[]'d)
	char *Unknown18;	// +0x18 (new[]'d)
	int Unknown1C;	// +0x1C (network order)
	int Unknown20;	// +0x20 (network order)
};

#endif
