// Earth & Beyond client (reconstruction): the galaxy map's focal point (notes at the end)
#include "galaxy_map_focal_point.h"
#include "..\..\ww3d2\camera.h"
#include "..\shared\g_assert.h"
#include "wwmath.h"

// 0x7A5980 clamps through this, not WWMath::Clamp: the value stays on the FPU and the bounds go to the stack (it is exact only so)
static inline float Unknown_Clamp(float val, float min, float max)	// NAME 1: neutral (D10)
{
	if (val < min) {
		val = min;
	} else if (val > max) {
		val = max;
	}
	return(val);
}

GalaxyMapFocalPointClass::GalaxyMapFocalPointClass() :
	Transform(true),
	Distance(0.0f),
	MinDistance(2.0f),
	MaxDistance(500.0f)
{
}

GalaxyMapFocalPointClass::~GalaxyMapFocalPointClass()
{
}

void GalaxyMapFocalPointClass::Unknown_7A57A0(const Matrix3D &transform, float distance)
{
	Transform = transform;
	Distance = distance;
}

void GalaxyMapFocalPointClass::Unknown_7A5820(const Matrix3D &transform)
{
	Transform = transform;
}











































void GalaxyMapFocalPointClass::Unknown_7A5890(const RefCountPtr<CameraClass> &camera)
{
	FAIL_IF(!camera) {
		return;
	}
	Matrix3D tm(Transform);
	tm.Translate_Z(Distance);
	camera->Set_Transform(tm);
}

void GalaxyMapFocalPointClass::Unknown_7A5980(const Vector2 &arg, const Vector2 &min, const Vector2 &max)
{
	float y = -(arg.Y * Distance);
	float x = arg.X * Distance;
	Transform[0][3] += x * Transform[0][0] + y * Transform[0][1];
	Transform[1][3] += x * Transform[1][0] + y * Transform[1][1];
	Transform[2][3] += x * Transform[2][0] + y * Transform[2][1];
	Transform[0][3] = Unknown_Clamp(Transform[0][3], min.X, max.Y);
	Transform[1][3] = Unknown_Clamp(Transform[1][3], min.Y, max.Y);
	Transform[2][3] = 0.0f;
}

void GalaxyMapFocalPointClass::Unknown_7A5AA0(const Vector2 &arg)
{
	float a = arg.X * 50.0f;
	Transform.In_Place_Pre_Rotate_Z(DEG_TO_RADF(a));
	float b = arg.Y * 50.0f;
	Transform.Rotate_X(DEG_TO_RADF(b));
	float x = Transform.Get_X_Rotation();
	if (x < 0.0f) {
		Transform.Rotate_X(-x);
	} else if (x > 1.0f) {
		Transform.Rotate_X(1.0f - x);
	}
	float y = Transform.Get_Y_Rotation();
	if (y < -1.0f) {
		Transform.Rotate_Y(-y - 1.0f);
	} else if (y > 1.0f) {
		Transform.Rotate_Y(1.0f - y);
	}
	float z = Transform.Get_Z_Rotation();
	if (z < -1.0f) {
		Transform.Rotate_Z(-z - 1.0f);
	} else if (z > 1.0f) {
		Transform.Rotate_Z(1.0f - z);
	}
}

void GalaxyMapFocalPointClass::Unknown_7A5E10(float arg)
{
	float m = (Distance < 1.0f) ? 1.0f : Distance;
	float d = Distance;
	Distance = d - ((1.0f / m + 1.0f) * arg) * d;
	Distance = WWMath::Clamp(Distance, MinDistance, MaxDistance);
}

void GalaxyMapFocalPointClass::Unknown_7A5ED0(const GalaxyMapFocalPointClass &a, const GalaxyMapFocalPointClass &b, float t)
{
	Transform = Lerp(a.Transform, b.Transform, t);
	Distance = (b.Distance - a.Distance) * t + a.Distance;
}

bool GalaxyMapFocalPointClass::operator == (const GalaxyMapFocalPointClass &that) const
{
	if (Distance == that.Distance) {
		return(Transform == that.Transform);
	}
	return(false);
}

/*
**	Reconstruction notes.
**
**	Placed by the client's __FILE__ 0xB9E8F4 (0x7A5890's FAIL_IF, line 85). The object runs 0x7A5730-0x7A5FF3:
**	galaxy_map_render.cpp's constructor 0x7A6020 follows, and galaxy_map_client.cpp ends before 0x7A5730.
**	0x7A5820 has no caller in the client (a public member nobody used); nor has 0x7A5F90, the object's last function.
**	0x7A5980's first clamp takes max.Y where max.X belongs: the client's, kept.
*/
