// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown: the header of the object 0x6F0200-0x6F0580, between eff_robj.cpp and eff_seek.cpp
**
**	The render-state effect: effect.ini's EffectClass 'RenderStateEffect' (0xB92EA0), which the factory's helper
**	0x6EC7D0 makes. When it starts it activates, deactivates, fades or steps a render state of its source's model.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_RS_H  // NAME 1 [eff_rs.h]: PLACEMENT: provisional (D137): the object has no __FILE__; its name sorts between eff_robj and eff_seek
#define EFF_RS_H

#include "eff_o.h"

// vtable 0xB002D0 (12 slots), 0xE4 bytes, constructor 0x6F0200; its destructor is the implicit one (the unary end's,
// 0x6F02C0; scalar deleting 0x6F0290). Its own flag bit: 0x200, effect.ini's ForceReactivate.
class RenderStateEffectClass : public UnknownB00060EffectClass	// NAME 3: effect.ini's EffectClass 'RenderStateEffect' (0xB92EA0), which the helper 0x6EC7D0 makes
{
public:
	RenderStateEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, int render_state, int action, unsigned action_time);	// 0x6F0200  // NAME 1 [client .. cycle_time]: the base's; render_state .. action_time: effect.ini's RenderStateID, ActionType, ActionTime (ms)
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6F0380 (Transform's translation)  // NAME 1 [position]: the base's
	virtual void Unknown_18();							// +0x18 0x6F03C0 (the action on the source's model, an RsTrackClass)

	int RenderStateID;									// +0xD8 (negative: none)  // NAME 3: effect.ini's 'RenderStateID'
	int ActionType;										// +0xDC (0 Activate, 1 Deactivate, 2 Linger, 3 Ramp, 4 ActivateNextStateOnce)  // NAME 3: effect.ini's 'ActionType'
	float ActionTime;									// +0xE0 (ms, from an unsigned)  // NAME 3: effect.ini's 'ActionTime'
};

#endif // EFF_RS_H
