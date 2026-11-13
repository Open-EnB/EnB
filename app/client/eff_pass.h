// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_pass.h (name assumed from eff_pass.cpp)
**
**	eff_pass.cpp's effect: effect.ini's EffectClass 'PassEffect' (the factory's helper 0x6EB9B0 makes it). It
**	puts a material pass on its source's model through the model's render hook while it runs, and can
**	project the pass's two textures onto the model (Projection0Axis / Projection1Axis).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_PASS_H  // NAME 3 [eff_pass.h]: header name assumed from the client's eff_pass.cpp
#define EFF_PASS_H

#include "eff_o.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "matrix4.h"

class MaterialPassClass;
class CompositeMatrixMapperClass;

// The pass effect (vtable 0xB00184, 12 slots, 0x174 bytes, constructor 0x6ED6C0; its destructor is the implicit
// one, 0x6EDA60, scalar deleting 0x6EDA30: no vtable store of its own). Its own
// flag bits: 0x200 the source's particle emitters hidden while it runs (HideBaseMesh), 0x400 the material's
// mappers reset at the start (ResetMappers); the projections, stage 0 then stage 1: 0x800 / 0x8000 along Y,
// 0x1000 / 0x10000 along Z (clear: X), 0x2000 / 0x20000 a world scale, 0x4000 / 0x40000 the model's box (clear: a
// cylinder round the axis).
class PassEffectClass : public UnknownB00060EffectClass	// NAME 3: effect.ini's EffectClass 'PassEffect' (0xB92E30's list), and the helper 0x6EB9B0's "for PassEffect %d"
{
public:
	PassEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, const RefCountPtr<MaterialPassClass> &pass, bool projection0, bool projection1);	// 0x6ED6C0 (flag 0x80 set)  // NAME 1 [client .. cycle_time]: the base's; pass: the member; projection0, projection1: effect.ini's Projection0Axis / Projection1Axis present
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6EDB70 (Transform's translation)  // NAME 1 [position]: the base's
	virtual void Unknown_18();							// +0x18 0x6EDC00 (the pass onto the model's render hook, the projections sized; FAIL_IF '!rend_hook' l.89)
	virtual void Unknown_1C();							// +0x1C 0x6EE260 (the texture transforms from the camera; FAIL_IF '!Camera' l.196, '!texture' l.210 / l.238)
	virtual void Unknown_20();							// +0x20 0x6EEB90 (the pass off the render hook; FAIL_IF '!rend_hook' l.277)

	void Unknown_6EDBB0(const Vector2 &scale, unsigned stage);	// 0x6EDBB0 (FAIL_IF 'stage > 1' l.69)  // NAME 1 [scale]: what it sets; stage: the FAIL_IF's
	void Unknown_6EECD0(const RefCountPtr<RenderObjClass> &robj, bool invisible);	// 0x6EECD0 (the particle emitters in the model and its sub-objects hidden or shown; FAIL_IF '!robj' l.288)  // NAME 1 [invisible]: ParticleEmitterClass::Set_Invisible's; robj: the FAIL_IF's

	RefCountPtr<MaterialPassClass> Pass;				// +0xD8  // NAME 2: what it holds
	Vector2 Scale[2];									// +0xDC (per stage: effect.ini's Projection<n>WorldScale, or the inverse box or cylinder scale)  // NAME 2: what it holds
	RefCountPtr<CompositeMatrixMapperClass> Mapper[2];	// +0xEC (per stage, made when the projection is present)  // NAME 2: what it holds
	Matrix4 Projection[2];								// +0xF4 (per stage: the orthographic projection)  // NAME 2: what it holds
};

#endif // EFF_PASS_H
