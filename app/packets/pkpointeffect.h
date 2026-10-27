/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkpointeffect.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x882710-0x885FD0 rebuilt so far (opcode 9, 10, 15); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKPOINTEFFECT_H  // NAME 1 [pkpointeffect]: provisional, pk + its first named class's name
#define PKPOINTEFFECT_H

#include "pkflag.h"
#include "..\shared\duration.h"
#include "vector3.h"

// opcode 9 (0x0009, OBJECT_EFFECT): vtable 0xB0A3A0, constructor 0x882980, 0x30 bytes
class ObjectEffectPacketClass : public FlagPacketClass
{
public:
	ObjectEffectPacketClass(int effect_id, const DurationClass &time_stamp, int game_id, const DurationClass &duration, short effect_desc_id, float scale, const Vector3 &hsv_shift);	// 0x882710
	ObjectEffectPacketClass(RawPacketClass *raw);	// 0x882980
	virtual RawPacketClass *Build();	// +0x04 0x882B40
	virtual bool Process_Packet(void *client);	// +0x08 0x734DF0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x883060
	virtual int Print(char *buffer, int len);	// +0x10 0x883250
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x8828C0
	virtual int Get_Flag_Count();	// +0x28 0x883230 (7)

	int Get_Size(unsigned int flags);	// 0x882E90 (the payload's bytes for these flags)  // NAME 2: what it returns
	unsigned int Get_Flags();	// 0x882F20 (the bit of each field that differs from its default; HSVShift's Z sets bit 5 again, so bit 6 is never sent)  // NAME 2: what it returns

	int EffectID;	// +0x0C (flag 0)  // NAME 3: the client's Print text 'EffectID='
	unsigned int TimeStamp;	// +0x10 (flag 1)  // NAME 3: the client's Print text 'TimeStamp='
	int GameID;	// +0x14  // NAME 3: the client's Print text 'GameID='
	short Duration;	// +0x18 (flag 2; -1 for a negative one)  // NAME 3: the client's Print text 'Duration='
	short EffectDescID;	// +0x1A  // NAME 3: the client's Print text 'EffectDescID='
	float Scale;	// +0x1C (flag 3; 1 by default)  // NAME 3: the client's Print text 'Scale='
	Vector3 HSVShift;	// +0x20 (flags 4, 5, 6; DefaultHSVShift by default)  // NAME 3: the client's Print text 'HSVShift='
	int Channel;	// +0x2C (PacketClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
};

// opcode 10 (0x000A, POINT_EFFECT): vtable 0xB0A408, constructor 0x8834D0, 0x34 bytes
class PointEffectPacketClass : public PacketClass
{
public:
	PointEffectPacketClass(int effect_id, const DurationClass &time_stamp, const Vector3 &location, const DurationClass &duration, short effect_desc_id, float scale, const Vector3 &hsv_shift);	// 0x8832D0
	PointEffectPacketClass(RawPacketClass *raw);	// 0x8834D0
	virtual RawPacketClass *Build();	// +0x04 0x8835C0
	virtual bool Process_Packet(void *client);	// +0x08 0x735000 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8836E0
	virtual int Print(char *buffer, int len);	// +0x10 0x883700

	int EffectID;	// +0x0C  // NAME 3: the client's Print text 'EffectID='
	int TimeStamp;	// +0x10  // NAME 3: the client's Print text 'TimeStamp='
	Vector3 Location;	// +0x14  // NAME 3: the client's Print text 'Location='
	short Duration;	// +0x20  // NAME 3: the client's Print text 'Duration='
	short EffectDescID;	// +0x22  // NAME 3: the client's Print text 'EffectDescID='
	float Scale;	// +0x24  // NAME 3: the client's Print text 'Scale='
	Vector3 HSVShift;	// +0x28  // NAME 3: the client's Print text 'HSVShift='
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
