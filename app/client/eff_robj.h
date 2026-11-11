// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_robj.h (name assumed from eff_robj.cpp)
**
**	eff_robj.cpp's effects: a render object played on one object or point (the unary end's) or between two
**	(the binary end's). effect.ini's EffectClass names two of them: 'RObjEffect' (the factory's helper
**	0x6EA830 makes the unary one) and 'RObjLookAtEffect' (0x6EA5E0 makes the binary one that looks along
**	its line). The other two have no name in the client.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_ROBJ_H  // NAME 3 [eff_robj.h]: header name assumed from the client's eff_robj.cpp
#define EFF_ROBJ_H

#include "eff_o.h"
#include "vector2.h"

// A render object on the unary end (vtable 0xB001C0, 12 slots, 0xDC bytes, constructor 0x6EEEA0, destructor
// 0x6EF040). Its own flag bits: 0x200 the model scaled by the source's radius, 0x400 moved from the source's
// centre to its surface towards the camera, 0x800 facing the camera, 0x1000 its texture mappers reset at the start.
class RObjEffectClass : public UnknownB00060EffectClass	// NAME 3: effect.ini's EffectClass 'RObjEffect' (0xB92F6C), which the helper 0x6EA830 makes
{
public:
	RObjEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj);	// 0x6EEEA0 (FAIL_IF '!RObj' l.40)  // NAME 1 [client .. cycle_time]: the base's; robj: the member
	virtual ~RObjEffectClass();							// +0x00 0x6EF040 (scalar deleting 0x6EF010)
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6EF120 (Transform's translation)  // NAME 1 [position]: the base's
	virtual void Unknown_18();							// +0x18 0x6EF200 (the model into the scene)
	virtual void Unknown_1C();							// +0x1C 0x6EF250 (FAIL_IF '!Camera' l.94)
	virtual void Unknown_20();							// +0x20 0x6EF4A0 (the model out of its scene)
	virtual void Set_Source_Object_ID(int game_id);		// +0x28 0x6EF160 (flag 0x200: the scale undone, then made again for the new source)  // NAME 1 [game_id]: the base's

	RenderObjClass *RObj;								// +0xD8  // NAME 3: the constructor's FAIL_IF '!RObj'
};

// The unary effect on the screen (vtable 0xB001FC, 12 slots, 0xF0 bytes, constructor 0x6EF4C0; its destructor is
// the implicit one, 0x6EF580, scalar deleting 0x6EF550): cpacket.cpp's damage text, drawn at the source's projection
class Unknown6EF4C0EffectClass : public RObjEffectClass	// NAME 1: neutral (D10)
{
public:
	Unknown6EF4C0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, const Vector2 &offset, const Vector3 &radius_offset);	// 0x6EF4C0 (flags 0x200 / 0x400 / 0x800 cleared)  // NAME 1 [client .. robj]: the base's; offset, radius_offset: the members
	virtual void Unknown_18();							// +0x18 0x6EF660
	virtual void Unknown_1C();							// +0x1C 0x6EF740 (FAIL_IF '!Camera' l.161)

	Vector2 UnknownDC;									// +0xDC (added to the projected position)
	Vector3 UnknownE4;									// +0xE4 (times the source's radius, added to the translation)
};

// A render object on the binary end (vtable 0xB00238, 16 slots, 0xE4 bytes, constructor 0x6EF910, destructor
// 0x6EFA70): the base of RObjLookAtEffectClass and of eff_seek.cpp's five effects. Its own flag bits: 0x4000 a
// unary next link gets the model's transform, 0x8000 its texture mappers reset at the start.
class Unknown6EF910EffectClass : public UnknownB0009CEffectClass	// NAME 1: neutral (D10)
{
public:
	Unknown6EF910EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj);	// 0x6EF910  // NAME 1 [client .. cycle_time]: the base's; robj: the member
	virtual ~Unknown6EF910EffectClass();				// +0x00 0x6EFA70 (scalar deleting 0x6EFA40; G_ASSERT 'RObj->Get_Scene() == NULL' l.208)
	virtual void Unknown_18();							// +0x18 0x6EFBB0 (the model into the scene)
	virtual void Unknown_20();							// +0x20 0x6EFC00 (the model out of its scene)
	virtual void Unknown_3C(UnknownB00060EffectClass *unary, int next);	// +0x3C 0x6EFC20 (flag 0x4000)  // NAME 1 [unary, next]: the base's

	RenderObjClass *RObj;								// +0xE0  // NAME 3: the destructor's G_ASSERT 'RObj->Get_Scene() == NULL'
};

// The binary effect that looks along its line (vtable 0xB00284, 16 slots, 0xE4 bytes, constructor 0x6EFC70; its
// destructor is the implicit one, 0x6EFDE0, scalar deleting 0x6EFDB0). Its own flag bits: 0x10000 the model scaled
// by its object's radius, 0x20000 at the source looking at the target (clear: at the target looking at the
// source), 0x40000 from the target's position (ClientObjectClass slot +0x24) instead of TargetPosition.
class RObjLookAtEffectClass : public Unknown6EF910EffectClass	// NAME 3: effect.ini's EffectClass 'RObjLookAtEffect' (0xB92F7C), which the helper 0x6EA5E0 makes
{
public:
	RObjLookAtEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj);	// 0x6EFC70 (flags 0x80 / 0x100 set)  // NAME 1 [client .. robj]: the base's
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6EFF20  // NAME 1 [position]: the base's
	virtual void Unknown_1C();							// +0x1C 0x6F0130
	virtual void Set_Source_Object_ID(int game_id);		// +0x28 0x6EFF90  // NAME 1 [game_id]: the base's
	virtual void Set_Target_Object_ID(int game_id);		// +0x34 0x6F0060  // NAME 1 [game_id]: the base's
};

#endif // EFF_ROBJ_H
