// Earth & Beyond client (reconstruction): app\shared\unknown_499180.cpp (PROVISIONAL file name, D137). Notes at the end.
#include "vector3.h"
#include "vector2.h"
#include "intersec.h"
#include "intersec.inl"
#include "wwmath.h"
#include <math.h>

// @0x499180
bool Unknown_499180(Vector3 a, float slope, const Vector3 *b)	// NAME 1: neutral (D10)
{
	a.X = b->X - a.X;
	a.Y = b->Y - a.Y;
	a.Z = b->Z - a.Z;
	float dist = sqrtf(a.Y * a.Y + a.X * a.X);
	return ((a.Z / dist) < slope);
}

// NOT EXACT: x87 operand order (masked residue 2, registers only: the divisor's first product loads (*RayDirection)[0] and multiplies by PlaneNormal[0] at 0x4992D3, the client the other way round; equal values); every sum's association and every other load is the client's; attempts.csv 0x4991F0
// @0x4991F0
bool Unknown_4991F0(Vector3 *a, const Vector3 *b, float *out_val, float dist_sq, Vector3 *normal, Vector3 *p1, Vector3 *p2)	// NAME 1: neutral (D10)
{
	Vector3 dir = *p2 - *p1;
	float len_sq = dir.Length2();
	dir.Normalize();
	IntersectionClass intersection(p1, &dir, NULL);
	IntersectionResultClass *result = &intersection.Result;
	if (!intersection.Intersect_Plane_Quick(result, *normal, *a)) {
		return true;
	}
	float range = result->Range;
	if (range < 0.0f) {
		return true;
	}
	if (range * range > len_sq) {
		return true;
	}
	intersection.Calculate_Intersection(result);
	Vector3 delta;
	delta = result->Intersection - *a;
	float d1_xy_sq = delta.Y * delta.Y + delta.X * delta.X;
	if (d1_xy_sq == 0.0f) {
		return true;
	}
	if (d1_xy_sq > dist_sq) {
		return true;
	}

	float d2_xy_sq = Vector2(result->Intersection.X - b->X, result->Intersection.Y - b->Y).Length2();
	if (d2_xy_sq == 0.0f) {
		return true;
	}
	if (d2_xy_sq > dist_sq) {
		return true;
	}

	float dist = sqrtf(d1_xy_sq);
	float slope = delta.Z / dist;
	if (slope < *out_val) {
		return true;
	}
	*out_val = slope;
	return false;
}

// @0x4995C0
bool Unknown_4995C0(Vector3 *a, const Vector3 *b, float *slope, float dist_sq, Vector3 *normal, Vector3 *v0, Vector3 *v1, Vector3 *v2, bool flag)	// NAME 1: neutral (D10)
{
	if (flag) {
		if (Unknown_499180(*a, *slope, v0) &&
			Unknown_499180(*a, *slope, v1) &&
			Unknown_499180(*a, *slope, v2)) {
			return true;
		}
	}
	if (!Unknown_4991F0(a, b, slope, dist_sq, normal, v0, v1) && flag) {
		return false;
	}
	if (!Unknown_4991F0(a, b, slope, dist_sq, normal, v1, v2) && flag) {
		return false;
	}
	if (!Unknown_4991F0(a, b, slope, dist_sq, normal, v2, v0) && flag) {
		return false;
	}
	return true;
}

/*
**	Reconstruction notes. The client holds no __FILE__, assert or <string> guard for this object: it sits
**	between accel.cpp (ends at 0x499148) and boolpars.cpp (starts at 0x499750) in app\shared, and holds
**	Unknown_499180 (0x499180, thunk 0x4052E0), Unknown_4991F0 (0x4991F0, thunk 0x403EB8), COMDATs
**	IntersectionClass::Set (0x499560) and IntersectionResultClass::IntersectionResultClass (0x4995A0),
**	Unknown_4995C0 (0x4995C0, thunk 0x4084E5), and Vector3::Vector3(float, float, float) (0x499720).
**	Hence the interim file name (D137, PROVISIONAL) between accel and boolpars.
**
**	Unknown_4991F0 uses Vector2(...).Length2() to push IntersectionResultClass::IntersectionResultClass
**	(0x4995A0) out of line while keeping the exact 0x98 frame; its locals (delta declared then assigned, range,
**	result, dist) set VC6's x87 operand order: all but one commuted product (0x4992D3) is the client's.
*/
