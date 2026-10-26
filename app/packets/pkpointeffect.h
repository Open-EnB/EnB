/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkpointeffect.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x882710-0x885FD0 rebuilt so far (opcode 10, 15); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKPOINTEFFECT_H  // NAME 1 [pkpointeffect]: provisional, pk + its first named class's name
#define PKPOINTEFFECT_H

#include "pk.h"

// opcode 10 (0x000A, POINT_EFFECT): vtable 0xB0A408, constructor 0x8834D0, 0x34 bytes
class PointEffectPacketClass : public PacketClass
{
public:
	PointEffectPacketClass(RawPacketClass *raw);	// 0x8834D0
	virtual RawPacketClass *Build();	// +0x04 0x8835C0
	virtual bool Process_Packet(void *client);	// +0x08 0x735000 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8836E0
	virtual int Print(char *buffer, int len);	// +0x10 0x883700

	int EffectID;	// +0x0C  // NAME 3: the client's Print text 'EffectID='
	int TimeStamp;	// +0x10  // NAME 3: the client's Print text 'TimeStamp='
	float Location[3];	// +0x14  // NAME 3: the client's Print text 'Location='
	short Duration;	// +0x20  // NAME 3: the client's Print text 'Duration='
	short EffectDescID;	// +0x22  // NAME 3: the client's Print text 'EffectDescID='
	float Scale;	// +0x24  // NAME 3: the client's Print text 'Scale='
	float HSVShift[3];	// +0x28  // NAME 3: the client's Print text 'HSVShift='
};

// opcode 15 (0x000F, REMOVE_EFFECT): vtable 0xB0A4FC, constructor 0x885E30, 0x14 bytes
class RemoveEffectPacketClass : public PacketClass
{
public:
	RemoveEffectPacketClass(RawPacketClass *raw);	// 0x885E30
	virtual RawPacketClass *Build();	// +0x04 0x885EC0
	virtual bool Process_Packet(void *client);	// +0x08 0x735D50 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x885F80
	virtual int Print(char *buffer, int len);	// +0x10 0x885FA0
	virtual int Get_Channel() { return(Unknown10); }	// +0x18 0x885DC0

	int EffectID;	// +0x0C  // NAME 3: the client's Print text 'EffectID='
	int Unknown10;	// +0x10
};

#endif
