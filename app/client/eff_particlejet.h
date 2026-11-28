// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_particlejet.h (name assumed from eff_particlejet.cpp)
**
**	eff_particlejet.cpp's effect: a particle emitter at the binary end's source, aimed at its target, its
**	particles' lifetime fitted to the distance at the emitter's forward speed. effect.ini's EffectClass
**	'ParticleJetEffect' (0xB92E30's thirteenth) names it; the factory's helper 0x6EC950 makes it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_PARTICLEJET_H  // NAME 3 [eff_particlejet.h]: header name assumed from the client's eff_particlejet.cpp
#define EFF_PARTICLEJET_H

#include "eff_o.h"

class ParticleEmitterClass;

// The particle-jet effect (vtable 0xB00138, 16 slots, 0xF0 bytes, constructor 0x6ECC50, destructor 0x6ECED0,
// scalar deleting 0x6ECEA0). Flag 0x4000 (effect.ini's 'Invert') aims it from the target at the source.
class ParticleJetEffectClass : public BinaryEffectClass	// NAME 3: effect.ini's EffectClass 'ParticleJetEffect', which the helper 0x6EC950 makes
{
public:
	ParticleJetEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *emitter, float min_lifetime);	// 0x6ECC50 (FAIL_IFs '!Emitter' l.18, 'ForwardVelocity < 0.01f' l.37)  // NAME 3 [min_lifetime]: effect.ini's 'MinLifetime'  // NAME 1 [client .. cycle_time, emitter]: the base's, the helper's
	virtual ~ParticleJetEffectClass();					// +0x00 0x6ECED0 (0x6ECEA0)
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6ECFF0 (the line's point closest to position)  // NAME 1 [position]: the base's
	virtual void Unknown_18();							// +0x18 0x6ED130 (the lifetime fitted, the emitter into the scene; FAIL_IF l.72)
	virtual void Unknown_1C();							// +0x1C 0x6ED4F0 (the emitter aimed, its speed from the distance; FAIL_IF l.140)
	virtual void Unknown_20();							// +0x20 0x6ED650 (the emitter out of the scene; FAIL_IF l.161)

	ParticleEmitterClass *Emitter;						// +0xE0  // NAME 3: the client's FAIL_IF '!Emitter'
	float Lifetime;										// +0xE4 (the emitter's at the start, seconds)  // NAME 2: ParticleEmitterClass::Get_Lifetime's
	float MinLifetime;									// +0xE8 (-1: a third of Lifetime)  // NAME 3: effect.ini's 'MinLifetime'
	float ForwardVelocity;								// +0xEC (the emitter's start velocity's X)  // NAME 3: the client's FAIL_IF 'ForwardVelocity < 0.01f'
};

#endif // EFF_PARTICLEJET_H
