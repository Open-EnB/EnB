// Earth & Beyond client (reconstruction): app\shared\fp_tt.cpp. Notes at the end of the file.
#include "fp_tt.h"
#include "px_wrapper.h"
#include "dynamic_obj.h"
#include "wwmath.h"
#include <math.h>
#include <float.h>
#include "..\shared\g_assert.h"

extern int Unknown_BE350C;	// NAME 1: neutral (D10): SClient::Do_Frame's frame toggle (sclient.cpp)

// 0x449360
FpTTClass::FpTTClass(float max_yaw, float max_pitch, float max_slope, bool unknown38) :
	Unknown28(0),
	Unknown2C(0),
	Unknown30(1.0f, 0.0f),
	Unknown38(unknown38),
	Unknown39(false),
	Unknown3C(0.0f, 0.0f),
	Unknown44(0.0f)
{
	MaxYaw = (max_yaw > 0.0f) ? max_yaw : 0.0f;
	MaxYaw = (MaxYaw < WWMATH_PI) ? MaxYaw : WWMATH_PI;
	MaxYawCos = cos(MaxYaw);
	MaxYawSin = sin(MaxYaw);
	MaxYawInv = (MaxYaw > 0.0f) ? 1.0f / MaxYaw : -1.0f;
	MaxYawHalfCos = sqrt((MaxYawCos + 1.0f) * 0.5f);
	MaxPitch = (max_pitch > 0.0f) ? max_pitch : 0.0f;
	MaxPitch = (MaxPitch < WWMATH_PI) ? MaxPitch : WWMATH_PI;
	MaxPitchCos = cos(MaxPitch);
	MaxPitchSin = sin(MaxPitch);
	MaxSlope = tan(WWMath::Min(WWMath::Max(max_slope, 0.0f), WWMATH_PI - 0.0001f));
}

// 0x449520
FpTTClass::~FpTTClass()
{
}

// 0x449540
// NOT EXACT: 0.44, frame (sub esp 0x48 against 0x44): the client shares stack slots between the target delta, the climb vector and the new axes
int FpTTClass::Unknown_04(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch)
{
	Unknown2C = NULL;
	float xy_len = 1.0f;
	Vector3 x_axis = transforms[Unknown_BE350C ^ 1].Get_X_Vector();
	Vector2 heading;
	heading.X = x_axis.X;
	heading.Y = x_axis.Y;
	double abs_z = fabs(x_axis.Z);
	if (abs_z > 0.000001) {
		float len2 = x_axis.X * x_axis.X + x_axis.Y * x_axis.Y;
		float inv = WWMath::Inv_Sqrt(len2);
		heading.X = inv * x_axis.X;
		heading.Y = inv * x_axis.Y;
		xy_len = inv * len2;
	}
	Vector3 pos = transforms[Unknown_BE350C ^ 1].Get_Translation();
	Vector2 dir;
	float slope;
	{
		Vector3 delta = target - pos;
		float len2 = delta.X * delta.X + delta.Y * delta.Y;
		if (len2 < 0.000000000001f) {
			dir.X = 1.0f;
			dir.Y = 0.0f;
			slope = (delta.Z >= 0.0f) ? MaxSlope : -MaxSlope;
		} else {
			float inv = WWMath::Inv_Sqrt(len2);
			dir.X = inv * delta.X;
			dir.Y = inv * delta.Y;
			slope = inv * delta.Z;
			slope = WWMath::Clamp(slope, -MaxSlope, MaxSlope);
		}
	}

	bool aligned = false;
	bool positive;
	switch (Unknown28) {
		case 0:
			aligned = heading.Rotate_Towards_Vector(dir, MaxYawSin, MaxYawCos, positive);
			if (aligned) {
				Unknown39 = false;
				yaw = 0.0f;
			} else if (positive) {
				yaw = MaxYaw;
			} else {
				yaw = -MaxYaw;
			}
			Unknown3C = dir;
			break;
		case 1:
			if (Vector2::Dot_Product(heading, Unknown30) < MaxYawCos || Vector2::Perp_Dot_Product(Unknown30, heading) > 0.0f) {
				heading.Rotate(-MaxYawSin, MaxYawCos);
				yaw = -MaxYaw;
				Unknown3C = Unknown30;
			} else {
				Unknown28 = 0;
				Unknown2C = NULL;
				heading.Rotate(-MaxYawSin, MaxYawCos);
				if (Vector2::Perp_Dot_Product(dir, heading) > 0.0f) {
					yaw = MaxYaw;
				} else {
					yaw = -MaxYaw;
				}
				Unknown3C = dir;
			}
			break;
		case 2:
			if (Vector2::Dot_Product(heading, Unknown30) < MaxYawCos || Vector2::Perp_Dot_Product(Unknown30, heading) < 0.0f) {
				heading.Rotate(MaxYawSin, MaxYawCos);
				yaw = MaxYaw;
				Unknown3C = Unknown30;
			} else {
				Unknown28 = 0;
				Unknown2C = NULL;
				heading.Rotate(MaxYawSin, MaxYawCos);
				if (Vector2::Perp_Dot_Product(dir, heading) > 0.0f) {
					yaw = MaxYaw;
				} else {
					yaw = -MaxYaw;
				}
				Unknown3C = dir;
			}
			break;
	}

	float climb_c;
	float climb_s;
	if (MaxSlope <= 0.0f || (fabs(slope) < 0.0001 && abs_z < 0.0001)) {
		climb_s = 0.0f;
		climb_c = 1.0f;
		pitch = 0.0f;
		Unknown44 = 0.0f;
	} else {
		climb_c = WWMath::Inv_Sqrt(slope * slope + 1.0f);
		climb_s = climb_c * slope;
		if (climb_c * xy_len + climb_s * x_axis.Z < MaxPitchCos) {
			if (climb_s * xy_len - climb_c * x_axis.Z <= 0.0f) {
				climb_s = MaxPitchCos * x_axis.Z + -MaxPitchSin * xy_len;
				climb_c = MaxPitchCos * xy_len - -MaxPitchSin * x_axis.Z;
				pitch = -MaxPitch;
				Unknown44 = slope;
			} else {
				climb_s = MaxPitchSin * xy_len + MaxPitchCos * x_axis.Z;
				pitch = MaxPitch;
				climb_c = MaxPitchCos * xy_len - MaxPitchSin * x_axis.Z;
				Unknown44 = slope;
			}
		} else {
			pitch = 0.0f;
			Unknown44 = slope;
		}
	}

	Vector3 x(climb_c * heading.X, climb_c * heading.Y, climb_s);
	pos += x * dist;
	Vector3 y(-heading.Y, heading.X, 0.0f);
	Vector3 z(-(climb_s * heading.X), y.X * climb_s, heading.X * x.X - y.X * x.Y);
	transforms[Unknown_BE350C].Set(x, y, z, pos);

	if (Unknown38 && !aligned) {
		float dx = target.X - pos.X;
		float dy = target.Y - pos.Y;
		float inv = WWMath::Inv_Sqrt(dx * dx + dy * dy);
		Unknown39 = (inv * dx) * dir.X + (inv * dy) * dir.Y < MaxYawHalfCos;
	}
	return(Unknown39 ? 0 : 2);
}





















































































































































// 0x449EA0
// NOT EXACT: written from the client's code (the obstacle pass, then 0x449540's steer around or away from the nearest)
int FpTTClass::Unknown_08(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch, PxWrapper &proxim, float radius, DynObj *self, Vector3 *aims)
{
	G_ASSERT(proxim.Get_Avoidance());
	Vector3 pos = transforms[Unknown_BE350C ^ 1].Get_Translation();
	Vector3 delta = target - pos;
	std::vector<DynObj *> &list = proxim.Unknown_4568E0();
	DynObj *nearest = NULL;
	int count = list.size();
	bool inside = false;
	Vector3 away;
	if (count != 0) {
		float inv_len = WWMath::Inv_Sqrt(delta.Length2());
		float best_ahead = FLT_MAX;
		float best_inside = FLT_MAX;
		for (int i = 0; i < count; i++) {
			DynObj *obj = list[i];
			float reach = obj->Unknown04 + radius;
			Vector3 d = obj->Unknown_Dyn48() - pos;
			float d2 = d.Length2();
			float near_reach = radius + radius + reach;
			if (near_reach * near_reach <= d2) {
				if (!inside) {
					float ahead = Vector3::Dot_Product(d, delta) * inv_len;
					if (ahead >= 0.0f && d2 - ahead * ahead <= reach * reach && ahead - reach < best_ahead) {
						best_ahead = ahead - reach;
						nearest = obj;
						away = d;
					}
				}
			} else {
				inside = true;
				float gap = sqrt(d2) - reach;
				if (gap < best_inside) {
					best_inside = gap;
					nearest = obj;
					away = d;
				}
			}
		}
	}

	float xy_len = 1.0f;
	Vector3 x_axis = transforms[Unknown_BE350C ^ 1].Get_X_Vector();
	double abs_z = fabs(x_axis.Z);
	Vector2 heading;
	heading.X = x_axis.X;
	heading.Y = x_axis.Y;
	if (abs_z > 0.000001) {
		float len2 = x_axis.X * x_axis.X + x_axis.Y * x_axis.Y;
		float inv = WWMath::Inv_Sqrt(len2);
		heading.X = inv * x_axis.X;
		heading.Y = inv * x_axis.Y;
		xy_len = inv * len2;
	}
	float slope = 0.0f;
	int result = 2;
	if (nearest == NULL) {
		result = Unknown_04(target, transforms, dist, yaw, pitch);
	} else {
		if (Unknown2C != nearest) {
			Unknown28 = 0;
			Unknown2C = NULL;
		}
		Vector2 dir;
		if (inside) {
			float fx = -away.X;
			float fy = -away.Y;
			float len2 = fx * fx + fy * fy;
			if (len2 >= 0.000000000001f) {
				float inv = WWMath::Inv_Sqrt(len2);
				dir.X = inv * fx;
				dir.Y = inv * fy;
				slope = WWMath::Clamp(-(inv * away.Z), -MaxSlope, MaxSlope);
			} else {
				dir.X = 1.0f;
				dir.Y = 0.0f;
				slope = (away.Z >= 0.0f) ? -MaxSlope : MaxSlope;
			}
		} else {
			float sx = -away.Y;
			float len2 = sx * sx + away.X * away.X;
			if (len2 >= 0.000000000001f) {
				float inv = WWMath::Inv_Sqrt(len2);
				dir.X = inv * sx;
				dir.Y = inv * away.X;
			} else {
				dir = heading;
			}
			if (Vector2::Dot_Product(dir, heading) < 0.0f) {
				dir.X = -dir.X;
				dir.Y = -dir.Y;
			}
			Vector3 fwd = nearest->Unknown_Dyn4C();
			if (fwd.X * x_axis.X + fwd.Y * x_axis.Y + fwd.Z * x_axis.Z >= 0.70710677f && dist != 0.0f) {
				float speed = nearest->Unknown_Dyn58();
				if (speed / dist < 2.0f && speed / dist > 0.5f && (!nearest->Unknown_44AFD0() || self < nearest)) {
					if (Vector2::Perp_Dot_Product(dir, heading) <= 0.0f) {
						Unknown28 = 1;
					} else {
						Unknown28 = 2;
					}
					Unknown2C = nearest;
					Unknown30 = heading;
				}
			}
		}

		float c = MaxYawCos;
		float s = MaxYawSin;
		if (Vector2::Dot_Product(dir, heading) < c) {
			if (Vector2::Perp_Dot_Product(dir, heading) <= 0.0f) {
				heading.Rotate(-s, c);
				yaw = -MaxYaw;
			} else {
				yaw = MaxYaw;
				heading.Rotate(s, c);
			}
		} else {
			heading = dir;
			yaw = 0.0f;
		}
		Unknown3C = dir;

		float climb_c;
		float climb_s;
		if (MaxSlope <= 0.0f || (fabs(slope) < 0.0001 && abs_z < 0.0001f)) {
			climb_s = 0.0f;
			climb_c = 1.0f;
			pitch = 0.0f;
			Unknown44 = 0.0f;
		} else {
			climb_c = WWMath::Inv_Sqrt(slope * slope + 1.0f);
			climb_s = climb_c * slope;
			if (climb_c * xy_len + climb_s * x_axis.Z < MaxPitchCos) {
				if (climb_s * xy_len - climb_c * x_axis.Z <= 0.0f) {
					climb_s = MaxPitchCos * x_axis.Z + -MaxPitchSin * xy_len;
					climb_c = MaxPitchCos * xy_len - -MaxPitchSin * x_axis.Z;
					pitch = -MaxPitch;
					Unknown44 = slope;
				} else {
					climb_s = MaxPitchSin * xy_len + MaxPitchCos * x_axis.Z;
					pitch = MaxPitch;
					climb_c = MaxPitchCos * xy_len - MaxPitchSin * x_axis.Z;
					Unknown44 = slope;
				}
			} else {
				pitch = 0.0f;
				Unknown44 = slope;
			}
		}

		Vector3 x(climb_c * heading.X, climb_c * heading.Y, climb_s);
		pos += x * dist;
		Vector3 y(-heading.Y, heading.X, 0.0f);
		Vector3 z(-(climb_s * heading.X), y.X * climb_s, heading.X * x.X - y.X * x.Y);
		transforms[Unknown_BE350C].Set(x, y, z, pos);
	}

	Vector3 now = transforms[Unknown_BE350C].Get_Translation();
	aims[Unknown_BE350C] = target - now;
	aims[Unknown_BE350C].Normalize();
	float margin = radius * 3.0f;
	Vector3 box[2];
	box[1].Set(now.X + margin, now.Y + margin, now.Z + margin);
	box[0].Set(now.X - margin, now.Y - margin, now.Z - margin);
	float reach = dist * MaxYawInv * 1.15f;
	for (int axis = 0; axis < 3; axis++) {
		float over = target[axis] - box[1][axis];
		if (over > 0.0f) {
			if (reach < over) {
				over = reach;
			}
			box[1][axis] += over;
		} else {
			over = box[0][axis] - target[axis];
			if (over > 0.0f) {
				if (reach < over) {
					over = reach;
				}
				box[0][axis] -= over;
			}
		}
	}
	proxim.Unknown_456900(box[0], box[1]);
	return(result);
}

// 0x44AC40
void FpTTClass::Unknown_0C(float &yaw, float &pitch)
{
	yaw = atan2(Unknown3C.Y, Unknown3C.X);
	pitch = atan(Unknown44);
}

/*
**	Reconstruction notes (fp_tt.cpp, client 0x449360-0x44AC60; __FILE__ 0xB70600, FAIL_IF 'proxim.Get_Avoidance()'
**	0xB705E4 on the client's line 323 in 0x449EA0).
**
**	- The object follows fp_ita.cpp (its __FILE__ 0xB705B4 precedes this one; its last function 0x449330 ends at
**	  0x44934F, then an int3 run) and precedes dynamic_obj.cpp (0x44AC70).
**	- FpTTClass's table 0xAE7934: the deleting destructor 0x4494F0, 0x449540, 0x449EA0, 0x44AC40; the base table
**	  0xAE7910 is a deleting destructor (0x4492D0, fp_ita.obj's copy) and three pure slots.
**	- The constructor clamps through WWMath::Max / Min (the tan argument's double load is Min over Max's result);
**	  0x44AC40's pitch is atan (fld1; fpatan), not atan2.
**	- 0x449540 steers the heading with Vector2::Rotate_Towards_Vector (state 0) or a forced turn toward +0x30 (states 1
**	  and 2), the climb the same way against the clamped slope, and writes this frame's transform (Unknown_BE350C) from
**	  the other one.
**	- 0x449EA0 (2778 bytes: the obstacle pass over the proximity list 0x4568E0, then 0x449540's steer, or its own around
**	  or away from the nearest obstacle, then the proximity box toward the target) is written NOT EXACT.
**	- 0x449CC0 (the out-of-line destructor of unknown_7cef30.h's RegistrationUnknown568Struct) and 0x44A980
**	  (vector<SubsCouponStruct>::operator=) lie inside this object's span: COMDAT copies of a header this file
**	  reached (or the linker's identical-code folding); no source form here produces them yet.
*/
