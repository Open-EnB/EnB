// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown: the header of the object 0x6F25A0-0x6F27C0, after eff_seek.cpp
**
**	The warp effect: effect.ini's EffectClass 'WarpEffect' (0xB92F5C), which the factory's helper 0x6EAA40 makes
**	(effect.ini's warp models, eff_wrp01). A unary render object whose model's highlight fades in to 1 while its
**	source warps and out again (the effect then ends) once the source is back, or gone.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_WARP_H  // NAME 1 [eff_warp.h]: PLACEMENT: provisional (D137): the object has no __FILE__; its name sorts after eff_seek
#define EFF_WARP_H

#include "eff_robj.h"

// vtable 0xB005F0 (12 slots), 0xE0 bytes, constructor 0x6F25A0; its destructor is the implicit one, 0x6F2670
// (a jump to RObjEffectClass's 0x6EF040; scalar deleting 0x6F2640). Every other slot is RObjEffectClass's.
class WarpEffectClass : public RObjEffectClass	// NAME 3: effect.ini's EffectClass 'WarpEffect' (0xB92F5C), which the helper 0x6EAA40 makes
{
public:
	WarpEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj);	// 0x6F25A0 (the model's mappers made unique, 0x496AE0)  // NAME 1 [client .. robj]: the base's
	virtual void Unknown_1C();							// +0x1C 0x6F2690 (the highlight 0.05 a frame up to 1, or down below 0.1: the effect ends)

	float UnknownDC;									// +0xDC (the model's highlight, 0x496110, from 0)
};

#endif // EFF_WARP_H
