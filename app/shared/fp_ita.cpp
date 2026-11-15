// Earth & Beyond client (reconstruction): app\shared\fp_ita.cpp. Notes at the end of the file.
#include "fp_ita.h"
#include "wwmath.h"
#include <math.h>
#include "..\shared\g_assert.h"

extern int Unknown_BE350C;	// NAME 1: neutral (D10): SClient::Do_Frame's frame toggle (sclient.cpp)

// 0x448C30
FpItaClass::FpItaClass(float max_turn)
{
	float turn = (max_turn > 0.0f) ? max_turn : 0.0f;
	turn = (turn < WWMATH_PI) ? turn : WWMATH_PI;
	MaxTurnCos = cos(turn);
	MaxTurnSin = sin(turn);
}

// 0x448CE0
FpItaClass::~FpItaClass()
{
}































































































// NOT EXACT (0.93, attempts.py 0x448D00): order: the client multiplies inv * dir.X (the factor loaded first) and pushes edx after Inv_Sqrt
// 0x448D00
int FpItaClass::Unknown_04(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch)
{
	Vector3 pos;
	transforms[Unknown_BE350C ^ 1].Get_Translation(&pos);
	Vector3 dir = target - pos;
	float len2 = dir.Length2();
	if (len2 >= 0.000001f) {
		dir *= WWMath::Inv_Sqrt(len2);
		Vector3 x_axis = transforms[Unknown_BE350C ^ 1].Get_X_Vector();
		Vector3 axis;
		Vector3::Cross_Product(x_axis, dir, &axis);
		float sin2 = axis.Length2();
		if (sin2 < 3.046171e-06f) {
			transforms[Unknown_BE350C] = transforms[Unknown_BE350C ^ 1];
			transforms[Unknown_BE350C].Translate_X(dist);
			return(2);
		}
		Vector3 x;
		if (sin2 < MaxTurnSin * MaxTurnSin) {
			x = dir;
		} else {
			axis *= WWMath::Inv_Sqrt(sin2);
			Vector3 perp;
			Vector3::Cross_Product(axis, x_axis, &perp);
			x = x_axis * MaxTurnCos + perp * MaxTurnSin;
		}
		Vector3 y_axis;
		if (fabs(x.X) > 0.0001f || fabs(x.Y) > 0.0001f) {
			y_axis.X = -x.Y; y_axis.Y = x.X; y_axis.Z = 0.0f;
			y_axis.Normalize();
		} else {
			y_axis = transforms[Unknown_BE350C ^ 1].Get_Y_Vector();
			G_ASSERT(y_axis.Z < 0.0001f);
		}
		Vector3 z_axis;
		Vector3::Cross_Product(x, y_axis, &z_axis);
		pos += x * dist;
		transforms[Unknown_BE350C].Set(x, y_axis, z_axis, pos);
		return(2);
	}
	transforms[Unknown_BE350C] = transforms[Unknown_BE350C ^ 1];
	transforms[Unknown_BE350C].Translate_X(dist);
	return(2);
}



// 0x449300
void FpItaClass::Unknown_0C(float &yaw, float &pitch) { yaw = 0.0f; pitch = 0.0f; }
// 0x449330
int FpItaClass::Unknown_08(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch, PxWrapper &proxim, float radius, DynObj *self, Vector3 *aims)
{
	G_ASSERT(0);
	return(1);
}

/*
**	Reconstruction notes (fp_ita.cpp, client 0x448C30-0x44934F; __FILE__ 0xB705B4, G_ASSERT 'y_axis.Z < 0.0001f' on the
**	client's line 151 in 0x448D00, G_ASSERT '0' on line 171 in 0x449330).
**
**	- The object follows unknown_445740.cpp (its ctype initialiser 0x448BB0) and precedes fp_tt.cpp (0x449360); it holds
**	  the fp_*.cpp base's deleting destructor 0x4492D0 (fp_tt.h's FpBaseClass, the first object to need it).
**	- FpItaClass's table 0xAE78FC: the deleting destructor 0x448CB0, 0x448D00, 0x449330, 0x449300; its destructor
**	  0x448CE0 is the base's inline one (the table 0xAE7910 stored).
**	- 0x448D00 turns the previous transform's X axis toward the target by at most the constructor's angle (the full
**	  turn when the sine of the angle between them is within it), keeps Y level (the old Y axis when X is vertical,
**	  G_ASSERT 'y_axis.Z < 0.0001f'), moves dist along the new X and writes this frame's transform (Unknown_BE350C);
**	  a target on the position or straight ahead copies the old transform and moves it along X. Unknown_08 asserts and
**	  returns 1; Unknown_0C clears both angles (one line: the client's line 171 leaves no room).
**	- 0x448D00 is NOT EXACT (order: the first normalising multiply loads the factor first, inv * dir.X, and the push
**	  after Inv_Sqrt takes edx where ours takes ecx).
*/
