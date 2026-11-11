// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\eff_seek.cpp (the client's __FILE__)
**
**	The effects whose render object seeks the target (client 0x6F0580-0x6F1F08): see eff_seek.h. Notes at the end.
*/
#include "eff_seek.h"
#include "client.h"
#include "rendobj.h"
#include "random.h"
#include "wwmath.h"
#include <math.h>
#include "..\shared\g_assert.h"

Matrix3D Unknown_82ECB0(const Vector3 &direction, const Vector3 &position);	// 0x82ECB0 (cdecl; a transform at the position facing along the direction; its file is unknown, eff_o.cpp declares it the same way)  // NAME 1 [direction, position]: what the callers pass


















// 0x6F0580
Unknown6F0580EffectClass::Unknown6F0580EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj) :
	Unknown6EF910EffectClass(client, id, start_time, (duration == INFINITE_DURATION || duration == 0) ? 1 : duration, cycle_time, robj)
{
	G_ASSERT(duration != INFINITE_DURATION);
	G_ASSERT(duration != 0.0f);
	Set_Flag(0x80, false);
	Set_Flag(0x100, true);
}

// 0x6F06E0
Vector3 Unknown6F0580EffectClass::Unknown_08(const Vector3 &)
{
	return(RObj->Get_Position());
}



// 0x6F0710
void Unknown6F0580EffectClass::Unknown_0C(unsigned duration)
{
	FAIL_IF(duration == INFINITE_DURATION || duration == 0) {
		duration = 1;
	}
	Unknown6EF910EffectClass::Unknown_0C(duration);
}

// 0x6F0760
void Unknown6F0580EffectClass::Unknown_18()
{
	Unknown6EF910EffectClass::Unknown_18();
	Matrix3D tm = Unknown_82ECB0(TargetPosition - SourcePosition, SourcePosition);
	RObj->Set_Transform(tm);
}

// 0x6F07F0
StraightSeekEffectClass::StraightSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj) :
	Unknown6F0580EffectClass(client, id, start_time, duration, cycle_time, robj)
{
	InvDuration = 1.0f / Duration;
}

// 0x6F0980
void StraightSeekEffectClass::Unknown_0C(unsigned duration)
{
	Unknown6F0580EffectClass::Unknown_0C(duration);
	InvDuration = 1.0f / Duration;
}

// 0x6F0A00
void StraightSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float t = (float)(Client->Unknown10BC - StartTime) * InvDuration;
	Vector3 direction = TargetPosition - SourcePosition;
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition + direction * t);
	RObj->Set_Transform(tm);
}

// 0x6F0B10
SpiralSeekEffectClass::SpiralSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, float radius, float spin_rate) :
	Unknown6F0580EffectClass(client, id, start_time, duration, cycle_time, robj),
	InvDuration(1.0f / Duration),
	Radius(radius),
	SpinRate(spin_rate)
{
}

// 0x6F0CC0
void SpiralSeekEffectClass::Unknown_0C(unsigned duration)
{
	Unknown6F0580EffectClass::Unknown_0C(duration);
	InvDuration = 1.0f / Duration;
}

// NOT EXACT: shape (the frame 0xB0, the sine's slot and the kept product radius * s are the client's; the client has point above rate above direction where this has rate, direction, point, and it stores point.X before rate.X from the FPU stack, 0x6F0E3D); attempts.py show 0x6F0D40
// 0x6F0D40
void SpiralSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float t = (float)(Client->Unknown10BC - StartTime) * InvDuration;
	float spin = (float)Duration * SpinRate;
	float s = (float)sin(spin * t);
	float c = (float)cos(spin * t);
	float radius = (t - t * t) * Radius * 4.0f;
	float radius_rate = (1.0f - (t + t)) * Radius * 4.0f;
	Vector3 point;
	Vector3 rate;
	point.Y = radius * s;
	point.Z = radius * c;
	rate.Y = radius_rate * s + radius * c * spin;
	rate.Z = radius_rate * c - radius * s * spin;
	Vector3 direction = TargetPosition - SourcePosition;
	rate.X = direction.Length();
	point.X = rate.X * t;
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition);
	Vector3 position = tm * point;
	Vector3 facing;
	Matrix3D::Rotate_Vector(tm, rate, &facing);
	RObj->Set_Transform(Unknown_82ECB0(facing, position));
}

static Random3Class Unknown_BE6CC8;	// 0xBE6CC8 (its initialiser 0x6F0FB0)  // NAME 1: neutral (D10)





















































// 0x6F0FD0
SplineRandomSeekEffectClass::SplineRandomSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, unsigned change_time, float max_error_dist) :
	Unknown6F0580EffectClass(client, id, start_time, duration, cycle_time, robj),
	ChangeTime(change_time),
	MaxErrorDist(max_error_dist)
{



	FAIL_IF(ChangeTime < 100) {
		ChangeTime = 100;
	}

	FAIL_IF(MaxErrorDist < 0.0f) {
		MaxErrorDist = 0.0f;
	}
	Unknown_6F1590();
}

// 0x6F1320
void SplineRandomSeekEffectClass::Unknown_0C(unsigned duration)
{
	Unknown6F0580EffectClass::Unknown_0C(duration);
	((Curve3DClass &)Spline).Clear_Keys();
	Unknown_6F1590();
}

// NOT EXACT: order-only (the frame and the 141 instructions are the client's; the facing vector's first two products load the matrix element first where this loads the rate's, 0x6F1496); attempts.py show 0x6F1390
// 0x6F1390
void SplineRandomSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float time = (float)(Client->Unknown10BC - StartTime);
	Vector3 point;
	((Curve3DClass &)Spline).Evaluate(time, &point);
	Vector3 rate;
	((HermiteSpline3DClass &)Spline).Evaluate_Derivative(time, &rate);
	Vector3 direction = TargetPosition - SourcePosition;
	float length = direction.Length();
	point.X *= length;
	rate.X *= length;
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition);
	Vector3 position = tm * point;
	Vector3 facing;
	Matrix3D::Rotate_Vector(tm, rate, &facing);
	RObj->Set_Transform(Unknown_82ECB0(facing, position));
}

// 0x6F1590
void SplineRandomSeekEffectClass::Unknown_6F1590()
{
	float duration = (float)Duration;
	float distance = MaxErrorDist;
	unsigned count;
	if (Duration >= ChangeTime * 2) {
		count = Duration / ChangeTime + 1;
	} else {
		count = 3;
		distance = duration / (float)(ChangeTime * 2) * distance;
	}
	if (Get_Flag(16)) {
		count = 3;
	}
	float step = 1.0f / (float)(count - 1);
	float t = step;
	Vector3 point(0.0f, 0.0f, 0.0f);
	((Curve3DClass &)Spline).Add_Key(point, 0.0f);
	for (unsigned i = 1; i < count - 1; i++) {
		float angle = (float)(int)Unknown_BE6CC8 * (WWMATH_PI * 2.0f / 4294967296.0f);
		point.Set(t, (float)sin(angle) * distance, (float)cos(angle) * distance);
		((Curve3DClass &)Spline).Add_Key(point, t * duration);
		t += step;
	}
	point.Set(1.0f, 0.0f, 0.0f);
	((Curve3DClass &)Spline).Add_Key(point, duration);
}

static Random3Class Unknown_BE6CD0;	// 0xBE6CD0 (its initialiser 0x6F1740)  // NAME 1: neutral (D10)





























// 0x6F1760
LinearRandomSeekEffectClass::LinearRandomSeekEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, unsigned change_time, unsigned time_to_change, float max_error_dist) :
	Unknown6F0580EffectClass(client, id, start_time, duration, cycle_time, robj),
	ChangeTime(change_time),
	TimeToChange(time_to_change),
	MaxErrorDist(max_error_dist)
{



	FAIL_IF(ChangeTime < 100) {
		ChangeTime = 100;
	}

	FAIL_IF(TimeToChange < 100) {
		TimeToChange = 100;
	}

	FAIL_IF(MaxErrorDist < 0.0f) {
		MaxErrorDist = 0.0f;
	}
	Unknown_6F1C40();
}

// 0x6F1A60
void LinearRandomSeekEffectClass::Unknown_0C(unsigned duration)
{
	Unknown6F0580EffectClass::Unknown_0C(duration);
	((Curve3DClass &)Curve).Clear_Keys();
	Unknown_6F1C40();
}

// NOT EXACT: order-only (the frame and the 96 instructions are the client's; the squared length adds z, x, y where Length2 adds z, y, x, and two rows' products load the matrix element first); attempts.py show 0x6F1AD0
// 0x6F1AD0
void LinearRandomSeekEffectClass::Unknown_1C()
{
	Unknown6F0580EffectClass::Unknown_1C();
	float time = (float)(Client->Unknown10BC - StartTime);
	Vector3 point;
	((Curve3DClass &)Curve).Evaluate(time, &point);
	Vector3 direction = TargetPosition - SourcePosition;
	point.X *= direction.Length();
	Matrix3D tm = Unknown_82ECB0(direction, SourcePosition);
	tm.Set_Translation(tm * point);
	RObj->Set_Transform(tm);
}

// 0x6F1C40
void LinearRandomSeekEffectClass::Unknown_6F1C40()
{
	float duration = (float)Duration;
	float distance = MaxErrorDist;
	unsigned count;
	if (Duration >= ChangeTime * 2) {
		count = Duration / ChangeTime + 1;
	} else {
		count = 3;
		distance = duration / (float)(ChangeTime * 2) * distance;
	}
	if (Get_Flag(16)) {
		count = 3;
	}
	float change = (float)TimeToChange;
	float steps = (float)count - 1.0f;
	float hold = (duration - (float)count * change) / steps;
	if (hold >= 0.05f) {
		float scale = 1.0f / duration;
		float change_step = scale * change;
		float hold_step = scale * hold;
		float t = change_step;
		((Curve3DClass &)Curve).Add_Key(Vector3(0.0f, 0.0f, 0.0f), 0.0f);
		for (unsigned i = 1; i < count - 1; i++) {
			float angle = (float)(int)Unknown_BE6CD0 * (WWMATH_PI * 2.0f / 4294967296.0f);
			float y = (float)sin(angle);
			float z = (float)cos(angle);
			((Curve3DClass &)Curve).Add_Key(Vector3(t, y * distance, z * distance), t * duration);
			t += hold_step;
			((Curve3DClass &)Curve).Add_Key(Vector3(t, y * distance, z * distance), t * duration);
			t += change_step;
		}
		((Curve3DClass &)Curve).Add_Key(Vector3(1.0f, 0.0f, 0.0f), duration);
	} else {
		float step = 1.0f / steps;
		float t = step;
		((Curve3DClass &)Curve).Add_Key(Vector3(0.0f, 0.0f, 0.0f), 0.0f);
		for (unsigned i = 1; i < count - 1; i++) {
			float angle = (float)(int)Unknown_BE6CD0 * (WWMATH_PI * 2.0f / 4294967296.0f);
			((Curve3DClass &)Curve).Add_Key(Vector3(t, (float)sin(angle) * distance, (float)cos(angle) * distance), t * duration);
			t += step;
		}
		((Curve3DClass &)Curve).Add_Key(Vector3(1.0f, 0.0f, 0.0f), duration);
	}
}

/*
**	Reconstruction notes (L4, 2026-11-10)
**
**	The window: eff_rs's object ends with its guarded <string> static 0x6F0550, so this file's first function
**	is the base's constructor 0x6F0580 (G_ASSERT lines 39 / 40). The last function with its own code is
**	0x6F1C40 (to 0x6F1F08); the curve templates' and classes' inline members follow at 0x6F1FD0-0x6F2530
**	(the virtuals of DynamicVectorClass / VectorClass of HermiteSpline3DClass::TangentsClass, twelve
**	functions, all exact), then the object's guarded <string> static 0x6F2570; the warp
**	effect's constructor 0x6F25A0 starts the next object.
**	The two Random3Class statics sit where their initialisers are, 0x6F0FB0 before the spline effect's
**	constructor and 0x6F1740 before the linear one's. The angle's factor 2 pi / 2^32 is a folded constant
**	expression: the client has it three times (0xB0037C / 0xB00380 / 0xB00384), once per use.
**	The base's constructor and its slot +0x0C are inlined into the four derived classes' (the asserts' lines
**	39 / 40 / 56 repeat there). The destructors are all implicit. The curves are called through their vtables
**	(the casts to the base reference): the client's calls are virtual.
**	The key vectors are temporaries built in the call (0x6F1590 names its one vector and Sets it; 0x6F1C40's
**	are all temporaries, y * distance and z * distance written out for both keys: the client keeps the two
**	products in their own slots and stores them into the first key straight from the FPU stack).
*/
