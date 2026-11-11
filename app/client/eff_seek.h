// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_seek.h (name assumed from eff_seek.cpp)
**
**	eff_seek.cpp's effects: a render object that travels from the effect's source to its target over the
**	effect's duration (the binary end's, through eff_robj.h's render-object effect). effect.ini's EffectClass
**	names the four the factory makes: 'StraightSeekEffect' (the helper 0x6EAFA0), 'SpiralSeekEffect'
**	(0x6EB150), 'SplineRandomSeekEffect' (0x6EB330) and 'LinearRandomSeekEffect' (0x6EB590). Their base has
**	no name in the client.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_SEEK_H  // NAME 3 [eff_seek.h]: header name assumed from the client's eff_seek.cpp
#define EFF_SEEK_H

#include "eff_robj.h"
#include "catmullromspline.h"

#define INFINITE_DURATION	0xFFFFFFFF	// NAME 3: the asserts' 'duration != INFINITE_DURATION' (EffectObjectClass::Duration's -1; its own header is not known, eff_o.h compares with (unsigned)-1)

// The seekers' base (vtable 0xB003A0, 16 slots, 0xE4 bytes, constructor 0x6F0580; its destructor is the implicit
// one, 0x6F06C0, scalar deleting 0x6F0690): a duration that is neither infinite nor zero, the model placed at
// the source facing the target at the start. Flag 0x80 cleared, 0x100 (TrackTarget) set.
class Unknown6F0580EffectClass : public Unknown6EF910EffectClass	// NAME 1: neutral (D10)
{
public:
	Unknown6F0580EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj);	// 0x6F0580 (G_ASSERT 'duration != INFINITE_DURATION' l.39, 'duration != 0.0f' l.40)  // NAME 1 [client .. robj]: the base's
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6F06E0 (the model's position)  // NAME 1 [position]: the base's
	virtual void Unknown_0C(unsigned duration);			// +0x0C 0x6F0710 (FAIL_IF 'duration == INFINITE_DURATION || duration == 0' l.56: then 1)  // NAME 1 [duration]: the base's
	virtual void Unknown_18();							// +0x18 0x6F0760 (the model at the source, facing the target)
};

// The seeker on the straight line (vtable 0xB003EC, 0xE8 bytes, constructor 0x6F07F0; implicit destructor
// 0x6F0960, scalar deleting 0x6F0930)
class StraightSeekEffectClass : public Unknown6F0580EffectClass	// NAME 3: effect.ini's EffectClass 'StraightSeekEffect' (0xB92E30's fifth), which the helper 0x6EAFA0 makes
{
public:
	StraightSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj);	// 0x6F07F0  // NAME 1 [client .. robj]: the base's
	virtual void Unknown_0C(unsigned duration);			// +0x0C 0x6F0980  // NAME 1 [duration]: the base's
	virtual void Unknown_1C();							// +0x1C 0x6F0A00

	float InvDuration;									// +0xE4 (1 / Duration)  // NAME 2: what it holds
};

// The seeker that spirals around the line (vtable 0xB00438, 0xF0 bytes, constructor 0x6F0B10; implicit
// destructor 0x6F0CA0, scalar deleting 0x6F0C70): the radius 4 t (1 - t) Radius, the angle Duration SpinRate t
class SpiralSeekEffectClass : public Unknown6F0580EffectClass	// NAME 3: effect.ini's EffectClass 'SpiralSeekEffect', which the helper 0x6EB150 makes
{
public:
	SpiralSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, float radius, float spin_rate);	// 0x6F0B10  // NAME 1 [client .. robj]: the base's; radius, spin_rate: the members
	virtual void Unknown_0C(unsigned duration);			// +0x0C 0x6F0CC0  // NAME 1 [duration]: the base's
	virtual void Unknown_1C();							// +0x1C 0x6F0D40

	float InvDuration;									// +0xE4 (1 / Duration)  // NAME 2: what it holds
	float Radius;										// +0xE8  // NAME 3: effect.ini's 'SpiralRadius'
	float SpinRate;										// +0xEC (radians per ms)  // NAME 3: effect.ini's 'SpiralSpinRate'
};

// The seeker on a Catmull-Rom spline through random points around the line (vtable 0xB00484, 0x12C bytes,
// constructor 0x6F0FD0; implicit destructor 0x6F1250, scalar deleting 0x6F11D0). Flag 0x10000: one change only.
class SplineRandomSeekEffectClass : public Unknown6F0580EffectClass	// NAME 3: effect.ini's EffectClass 'SplineRandomSeekEffect', which the helper 0x6EB330 makes
{
public:
	SplineRandomSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, unsigned change_time, float max_error_dist);	// 0x6F0FD0 (FAIL_IF 'ChangeTime < 100' l.200, 'MaxErrorDist < 0.0f' l.204)  // NAME 1 [client .. robj]: the base's; change_time, max_error_dist: the members
	virtual void Unknown_0C(unsigned duration);			// +0x0C 0x6F1320  // NAME 1 [duration]: the base's
	virtual void Unknown_1C();							// +0x1C 0x6F1390

	void Unknown_6F1590();								// 0x6F1590 (the spline's keys: the origin, a random point on a circle of MaxErrorDist every ChangeTime, the end)

	unsigned ChangeTime;								// +0xE4  // NAME 3: the constructor's FAIL_IF 'ChangeTime < 100'
	float MaxErrorDist;									// +0xE8  // NAME 3: the constructor's FAIL_IF 'MaxErrorDist < 0.0f'
	CatmullRomSpline3DClass Spline;						// +0xEC (x: the part of the line, 0..1; y, z: off the line)  // NAME 2: what it is
};

// The seeker on straight pieces between random points around the line (vtable 0xB00560, 0x114 bytes,
// constructor 0x6F1760; implicit destructor 0x6F19C0, scalar deleting 0x6F1970). Flag 0x10000: one change only.
class LinearRandomSeekEffectClass : public Unknown6F0580EffectClass	// NAME 3: effect.ini's EffectClass 'LinearRandomSeekEffect', which the helper 0x6EB590 makes
{
public:
	LinearRandomSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, unsigned change_time, unsigned time_to_change, float max_error_dist);	// 0x6F1760 (FAIL_IF 'ChangeTime < 100' l.308, 'TimeToChange < 100' l.312, 'MaxErrorDist < 0.0f' l.316)  // NAME 1 [client .. robj]: the base's; change_time .. max_error_dist: the members
	virtual void Unknown_0C(unsigned duration);			// +0x0C 0x6F1A60  // NAME 1 [duration]: the base's
	virtual void Unknown_1C();							// +0x1C 0x6F1AD0

	void Unknown_6F1C40();								// 0x6F1C40 (the curve's keys: with room between the changes, each point held and then left over TimeToChange)

	unsigned ChangeTime;								// +0xE4  // NAME 3: the constructor's FAIL_IF 'ChangeTime < 100'
	unsigned TimeToChange;								// +0xE8  // NAME 3: the constructor's FAIL_IF 'TimeToChange < 100'
	float MaxErrorDist;									// +0xEC  // NAME 3: the constructor's FAIL_IF 'MaxErrorDist < 0.0f'
	LinearCurve3DClass Curve;							// +0xF0 (as the spline effect's)  // NAME 2: what it is
};

#endif // EFF_SEEK_H
