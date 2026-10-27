/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkpointeffect.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x882710-0x885FD0 rebuilt so far (opcode 9, 10, 11, 15); the file's other classes
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
#include "..\common\lego_identifier.h"

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

// opcode 11 (0x000B, OBJECT_TO_OBJECT_EFFECT): vtable 0xB0A438, constructor 0x883A60, 0x60 bytes
class ObjectToObjectEffectPacketClass : public FlagPacketClass
{
public:
	ObjectToObjectEffectPacketClass(int effect_id, const DurationClass &time_stamp, int source_game_id, const LegoIdentifierClass &source_lego, int target_game_id, const DurationClass &duration, short effect_desc_id, const Vector3 &target_offset, short unknown34, short unknown36, bool outside_target_radius, float scale, const Vector3 &hsv_shift, float speedup);	// 0x883790
	ObjectToObjectEffectPacketClass(RawPacketClass *raw);	// 0x883A60
	virtual RawPacketClass *Build();	// +0x04 0x883DD0
	virtual bool Process_Packet(void *client);	// +0x08 0x735280 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x884020
	virtual int Print(char *buffer, int len);	// +0x10 0x884410
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x8839A0
	virtual int Get_Flag_Count();	// +0x28 0x8843F0 (12)

	int Get_Size(unsigned int flags);	// 0x884050 (the payload's bytes for these flags)  // NAME 2: what it returns
	unsigned int Get_Flags();	// 0x8841D0 (the bit of each field that differs from its default)  // NAME 2: what it returns

	int EffectID;	// +0x0C (flag 0)  // NAME 3: the client's Print text 'EffectID='
	unsigned int TimeStamp;	// +0x10 (flag 1)  // NAME 3: the client's Print text 'TimeStamp='
	int SourceGameID;	// +0x14  // NAME 3: the client's Print text 'SourceGameID='
	LegoIdentifierClass SourceLego;	// +0x18 (sent as its name)  // NAME 2: what it holds
	int TargetGameID;	// +0x2C  // NAME 3: the client's Print text 'TargetGameID='
	short Duration;	// +0x30 (flag 2)  // NAME 3: the client's Print text 'Duration='
	short EffectDescID;	// +0x32  // NAME 3: the client's Print text 'EffectDescID='
	short Unknown34;	// +0x34 (flag 3; not printed)
	short Unknown36;	// +0x36 (flag 4; not printed)
	bool OutsideTargetRadius;	// +0x38 (flag 5, no data)  // NAME 3: the client's Print text 'OutsideTargetRadius='
	Vector3 TargetOffset;	// +0x3C (flag 6; DefaultTargetOffset by default)  // NAME 3: the client's Print text 'TargetOffset='
	float Scale;	// +0x48 (flag 7; 1 by default)  // NAME 3: the client's Print text 'Scale='
	Vector3 HSVShift;	// +0x4C (flags 8, 9, 10; DefaultHSVShift by default)  // NAME 3: the client's Print text 'HSVShift='
	float Speedup;	// +0x58 (flag 11; 1 by default)  // NAME 3: the client's Print text 'Speedup='
	int Channel;	// +0x5C (PacketClass::Unknown_14 of TargetGameID; not read from the message)  // NAME 2: Get_Channel returns it
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
