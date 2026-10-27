/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pknotelist.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87EA50-0x87F2F0 rebuilt so far (opcode 184); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKNOTELIST_H  // NAME 1 [pknotelist]: provisional, pk + its first named class's name
#define PKNOTELIST_H

#include "pk.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

// opcode 184 (0x00B8, ?): vtable 0xB0A0E0, constructor 0x87F020, 0x58 bytes
class NoteContentPacketClass : public PacketClass
{
public:
	NoteContentPacketClass(int unknown0c, const char *unknown10, const char *unknown54);	// 0x87EF50
	NoteContentPacketClass(RawPacketClass *raw);	// 0x87F020
	virtual ~NoteContentPacketClass();	// +0x00 0x87EFF0 / 0x87F0E0
	virtual RawPacketClass *Build();	// +0x04 0x87F110
	virtual bool Process_Packet(void *client);	// +0x08 0x73AF80 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x87F1E0
	virtual int Print(char *buffer, int len);	// +0x10 0x87F200 ('NoteContent:')

	int Unknown0C;	// +0x0C (network order)
	char Unknown10[0x41];	// +0x10 (65 bytes as they are)
	char *Unknown54;	// +0x54 (a string, new[]'d)
};

#endif
