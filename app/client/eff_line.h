// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_line.h (name assumed from eff_line.cpp)
**
**	eff_line.cpp's effect: a segmented line from the binary end's source to its target, offset from
**	both ends, grown at the start and shrunk at the end, kept off the camera's near plane. effect.ini's
**	EffectClass 'LineEffect' (0xB92E30's fourth) names it; the factory's helper 0x6EAC50 makes it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_LINE_H  // NAME 3 [eff_line.h]: header name assumed from the client's eff_line.cpp
#define EFF_LINE_H

#include "eff_o.h"

class SegmentedLineClass;

// The line effect (vtable 0xAFFF4C, 16 slots, 0x114 bytes, constructor 0x6E5040, destructor 0x6E5600,
// scalar deleting 0x6E54E0). Flags 0x4000 / 0x8000 (InvertGrow, InvertShrink) grow and shrink from the
// other end; 0x10000 / 0x20000 mark the source / the target as the player's object (the near plane test).
class LineEffectClass : public UnknownB0009CEffectClass	// NAME 3: effect.ini's EffectClass 'LineEffect', which the helper 0x6EAC50 makes
{
public:
	LineEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, SegmentedLineClass *line, float start_offset, float end_offset, int grow_time, int shrink_time, float grow_accel, float shrink_accel, float tiling_min_length);	// 0x6E5040 (FAIL_IF '!SegLineRObj' l.51)  // NAME 3 [start_offset .. tiling_min_length]: effect.ini's keys  // NAME 1 [client .. cycle_time, line]: the base's
	virtual ~LineEffectClass();							// +0x00 0x6E5600 (0x6E54E0)
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6E5720 (the line's point closest to position)  // NAME 1 [position]: the base's
	virtual void Unknown_18();							// +0x18 0x6E5960 (the line into the scene; FAIL_IF l.138)
	virtual void Unknown_1C();							// +0x1C 0x6E59B0 (the line's two points; FAIL_IF l.146)
	virtual void Unknown_20();							// +0x20 0x6E5B70 (the line out of the scene; FAIL_IF l.180)
	virtual void Set_Source_Object_ID(int game_id);		// +0x28 0x6E5860 (flags 0x10000 / 0x20000)  // NAME 1 [game_id]: the base's
	virtual void Set_Target_Object_ID(int game_id);		// +0x34 0x6E58E0 (flags 0x20000 / 0x10000)  // NAME 1 [game_id]: the base's

	void Unknown_6E5BB0(Vector3 *start, Vector3 *end);	// 0x6E5BB0 (StartOffset / EndOffset times the objects' radius; FAIL_IF '!start || !end' l.188)  // NAME 3 [start, end]: the client's FAIL_IF
	void Unknown_6E5E00(Vector3 *start, Vector3 *end);	// 0x6E5E00 (growing and shrinking; FAIL_IF l.215)  // NAME 3 [start, end]: the client's FAIL_IF
	void Unknown_6E6030(Vector3 *start, Vector3 *end);	// 0x6E6030 (the player's end moved off the near plane; FAIL_IFs l.261, '!Camera' l.262)  // NAME 3 [start, end]: the client's FAIL_IF

	float StartOffset;									// +0xE0  // NAME 3: effect.ini's 'StartOffset'
	float EndOffset;									// +0xE4  // NAME 3: effect.ini's 'EndOffset'
	unsigned GrowTime;									// +0xE8 (ms, -1 none)  // NAME 3: effect.ini's 'GrowTime'
	unsigned ShrinkTime;								// +0xEC (ms, -1 none)  // NAME 3: effect.ini's 'ShrinkTime'
	float GrowA;										// +0xF0 (GrowAccelFactor / GrowTime^2)  // NAME 2: the quadratic's
	float ShrinkA;										// +0xF4  // NAME 2: the quadratic's
	float GrowB;										// +0xF8 ((1 - GrowAccelFactor) / GrowTime)  // NAME 2: the quadratic's
	float ShrinkB;										// +0xFC  // NAME 2: the quadratic's
	float TilingMinLength;								// +0x100  // NAME 3: effect.ini's 'TilingMinLength'
	float InvTilingMinLength;							// +0x104  // NAME 2: what it holds
	float TextureTileFactor;							// +0x108 (the line's at the start)  // NAME 2: SegmentedLineClass's
	unsigned SubdivisionLevels;							// +0x10C (the line's at the start)  // NAME 2: SegmentedLineClass's
	SegmentedLineClass *SegLineRObj;					// +0x110  // NAME 3: the client's FAIL_IF '!SegLineRObj'
};

#endif // EFF_LINE_H
