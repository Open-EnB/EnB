// Earth & Beyond client (reconstruction): the ship picture-in-picture gadget (notes at the end)
#include <math.h>
#include "ship_pip.h"
#include "sclient.h"
#include "tasks\tsk_game.h"
#include "tasks\tsk_login.h"
#include "avatar.h"
#include "..\shared\unknown_debug.h"
#include "..\input\input.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\light.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\ww3d.h"
#include "matrix3d.h"
#include "quat.h"
#include "sphere.h"
#include "wwmath.h"
// The file's float constants, in the client's .rdata order (loaded from memory, not as immediates)  // NAME 1: neutral (D10), by address
static const float Unknown_B07D78 = 50000.0f;							// the PIP scene's poly budget (PIPView's 0x65B7A0)
static const float Unknown_B07D7C = -70.0f;								// type 0: the pan's x limits
static const float Unknown_B07D80 = 70.0f;
static const float Unknown_B07D84 = -50.0f;								// type 0: the pan's y limits
static const float Unknown_B07D88 = 50.0f;
static const float Unknown_B07D8C = -2.0f;								// type 1: the pan's x limits
static const float Unknown_B07D90 = 2.0f;
static const float Unknown_B07D94 = -4.75f;								// type 1: the pan's y limits
static const float Unknown_B07D98 = 4.75f;
static const float Unknown_B07D9C = 0.0f;								// type 0: the pan's start
static const float Unknown_B07DA0 = -3.0f;
static const float Unknown_B07DA4 = 0.0f;								// type 1: the pan's start
static const float Unknown_B07DA8 = 2.5f;
static const float Unknown_B07DAC = 0.0f;								// type 1, +0xDC: the pan's start
static const float Unknown_B07DB0 = 4.0f;
static const float Unknown_B07DB4 = 2.1f;								// the farthest distance in radii (type 0, 1)
static const float Unknown_B07DB8 = 3.0f;
static const float Unknown_B07DBC = 0.5f;								// the nearest distance in radii (type 0, 1)
static const float Unknown_B07DC0 = 0.75f;
static const float Unknown_B07DC4 = 60.0f;								// type 0: the pan / zoom speed (0x80E7A0)
static const float Unknown_B07DC8 = 0.05f;								// type 0: the float parent's turn (radians per second)



































































// @0x80B2E0
ShipPipClass::ShipPipClass(int type) :
	GadgetClass(0x77),
	Type(type),
	PIPView(NULL),
	Unknown048(0),
	Unknown058(0.0f, 0.0f, 0.0f),
	Unknown064(0.0f, 0.0f),
	Unknown06C(true),
	Unknown09C(0), Unknown0A0(0), Unknown0A4(0), Unknown0A8(0),
	Unknown0AC(0.0f), Unknown0B0(0.0f), Unknown0B4(0.0f),
	Unknown0B8(0.0f), Unknown0BC(0.0f), Unknown0C0(0.0f), Unknown0C4(0.0f), Unknown0C8(0.0f),
	Unknown0CC(0.0f), Unknown0D0(0.0f),
	Unknown0DC(false),
	Unknown0E0(0.0f),
	Unknown0E4(0.0f),
	Unknown0E8(true)
{
	switch (type) {
		case 0:	Unknown0EC = Unknown_B07DBC; Unknown0F0 = Unknown_B07DB4; break;
		case 1:	Unknown0EC = Unknown_B07DC0; Unknown0F0 = Unknown_B07DB8; break;
		default:
			G_ASSERT(false);
			break;
	}
}

// @0x80B520
ShipPipClass::~ShipPipClass()
{
	Unknown_80();
}

// The lights' colours and directions (dynamic initializers 0x80B6F0-0x80BA30, in this order)
static Vector3 Unknown_BE8C60(1.0f, -1.0f, -1.0f);						// type 0: the first light's direction
static Vector3 Unknown_BE8C28(-1.0f, 1.0f, 1.0f);						// type 0: the second light's direction
static Vector3 Unknown_BE8CA8[3] = { Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f) };	// type 0: the first light's diffuse by race
static Vector3 Unknown_BE8CD0[3] = { Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f) };	// type 0: the first light's ambient by race
static Vector3 Unknown_BE8C38[3] = { Vector3(0.06f, 0.42f, 0.83f), Vector3(0.06f, 0.42f, 0.83f), Vector3(0.06f, 0.42f, 0.83f) };	// type 0: the second light's diffuse by race
static Vector3 Unknown_BE8C18(0.5f, -1.0f, -2.0f);						// type 1: the first light's direction
static Vector3 Unknown_BE8C98(-0.75f, 1.0f, 1.0f);						// type 1: the second light's direction
static Vector3 Unknown_BE8BF0[3] = { Vector3(0.9f, 0.9f, 0.75f), Vector3(0.9f, 0.8f, 0.85f), Vector3(0.9f, 0.8f, 0.85f) };	// type 1: the first light's diffuse by race
static Vector3 Unknown_BE8BC8[3] = { Vector3(0.15f, 0.1f, 0.09f), Vector3(0.15f, 0.1f, 0.1f), Vector3(0.15f, 0.1f, 0.1f) };	// type 1: the first light's ambient by race
static Vector3 Unknown_BE8C70[3] = { Vector3(0.5f, 0.0f, 0.0f), Vector3(0.5f, 0.4f, 0.1f), Vector3(0.1f, 0.45f, 0.45f) };	// type 1: the second light's diffuse by race

// @0x80BAB0
void ShipPipClass::Unknown_74(int race)
{
	if (Type == 0) {
		Unknown04C = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
		Unknown04C->Set_Intensity(3.0f);
		Unknown04C->Set_Force_Visible(true);
		Unknown04C->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown04C->Set_Ambient(Unknown_BE8CD0[race]);
		Unknown04C->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown04C->Set_Diffuse(Unknown_BE8CA8[race]);
		Unknown04C->Set_Transform(Matrix3D::Identity);
		Unknown04C->Set_Spot_Direction(Unknown_BE8C60);
		Unknown050 = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
		Unknown050->Set_Intensity(1.3f);
		Unknown050->Set_Force_Visible(true);
		Unknown050->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown050->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown050->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown050->Set_Diffuse(Unknown_BE8C38[race]);
		Unknown050->Set_Transform(Matrix3D::Identity);
		Unknown050->Set_Spot_Direction(Unknown_BE8C28);
	} else if (Type == 1) {
		Unknown04C = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
		Unknown04C->Set_Intensity(1.0f);
		Unknown04C->Set_Force_Visible(true);
		Unknown04C->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown04C->Set_Ambient(Unknown_BE8BC8[race]);
		Unknown04C->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown04C->Set_Diffuse(Unknown_BE8BF0[race]);
		Unknown04C->Set_Transform(Matrix3D::Identity);
		Unknown04C->Set_Spot_Direction(Unknown_BE8C18);
		Unknown050 = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
		Unknown050->Set_Intensity(1.0f);
		Unknown050->Set_Force_Visible(true);
		Unknown050->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown050->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown050->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown050->Set_Diffuse(Unknown_BE8C70[race]);
		Unknown050->Set_Transform(Matrix3D::Identity);
		Unknown050->Set_Spot_Direction(Unknown_BE8C98);
	}
}






































// @0x80C180
void ShipPipClass::Unknown_80C180()
{
	Unknown_80();
	Unknown0D8 = NULL;
	FloatParent = NULL;
	Scene = NULL;
	Camera = NULL;
	Unknown040 = 0.0f;
	Unknown044 = 0.0f;
}

// @0x80C220
bool ShipPipClass::Unknown_80C220(ViewClass *view)
{
	Unknown_80C180();
	PIPView = view->Unknown_1C();
	FAIL_IF(!PIPView) return(false);
	Scene = Create_NEW(new SimpleSceneClass);
	Scene->Set_Fog_Enable(false);
	Scene->Set_Ambient_Light(Vector3(0.0f, 0.0f, 0.0f));
	Camera = Create_NEW(new CameraClass);
	FloatParent = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("BONE_HOV01A"));








	G_ASSERT(FloatParent);
	return(true);
}

// @0x80C540
void ShipPipClass::Unknown_80C540(const Vector2 &size, const Vector3 &position, unsigned int sort_level)
{
	Unknown048 = sort_level;
	Unknown040 = size.X;
	Unknown044 = size.Y;
	Unknown064.Set(position.X - size.X * 0.5f, size.Y * 0.5f + position.Y);
	Unknown064.X += 320.0f;
	Unknown064.Y = 240.0f - Unknown064.Y;
}

// @0x80C5C0
void ShipPipClass::Unknown_80()
{
	if (Unknown054) {
		Unknown04C->Remove();
		Unknown050->Remove();
		FloatParent->Remove_Sub_Objects_From_Bone("NewShipScale");
		PIPView->Unknown_65B7E0(Scene);
		Unknown054 = NULL;
	}
}

// NOT EXACT: ShipPipClass
// @0x80C640
void ShipPipClass::Unknown_7C(const RefCountPtr<RenderObjClass> &model)
{
	if (Unknown054) {
		Unknown_80();
	}
	Unknown054 = model;
	const SphereClass &sphere = Unknown054->Get_Bounding_Sphere();
	Unknown058 = sphere.Center;
	float radius = sphere.Radius;
	Matrix3D identity(true);
	Unknown054->Set_Transform(identity);
	float distance = radius * Unknown0F0;
	Unknown0AC = distance;
	Unknown0B0 = radius * Unknown0EC;
	Unknown0B4 = distance;
	Unknown06C.Make_Identity();
	Unknown06C.Set_Z_Translation(distance);
	Unknown04C->Add(Scene.Peek());
	Unknown050->Add(Scene.Peek());
	FloatParent->Add(Scene.Peek());
	int bone = FloatParent->Get_Bone_Index("NewShipScale");
	FloatParent->Capture_Bone(bone);
	Matrix3D tm = Unknown054->Get_Transform();
	tm.Translate(-Unknown058);
	FloatParent->Control_Bone(bone, tm, false);
	FloatParent->Add_Sub_Object_To_Bone(Unknown054.Peek(), "NewShipScale");
	PIPView->Unknown_65B7A0(Scene, Camera, Unknown_B07D78, Unknown048);
	Unknown_88();
	Unknown_8C();
	Unknown_90();
}

// NOT EXACT: ShipPipClass
// @0x80CA30
void ShipPipClass::Unknown_84(const RefCountPtr<RenderObjClass> &model)
{
	if (Unknown054) {
		FloatParent->Remove_Sub_Objects_From_Bone("NewShipScale");
		Unknown054 = NULL;
	}
	Unknown054 = model;
	SphereClass sphere = Unknown054->Get_Bounding_Sphere();
	Unknown058 = sphere.Center;
	float radius = sphere.Radius;
	Unknown0AC = radius * Unknown0F0;
	float nearest = radius * Unknown0EC;
	if (Type == 0) {
		Unknown0B0 = nearest;
	} else {
		Unknown0B0 = nearest;
	}
	if (Unknown0B4 < Unknown0B0) {
		float distance = Unknown0B0 - Unknown0B4;
		if (distance != 0.0f) {
			float clamped = WWMath::Clamp(distance + Unknown0B4, Unknown0B0, Unknown0AC);
			Unknown06C.Translate(0.0f, 0.0f, clamped - Unknown0B4);
			Unknown0B4 = clamped;
		}
	}
	if (Unknown0AC < Unknown0B4) {
		float distance = Unknown0AC - Unknown0B4;
		if (distance != 0.0f) {
			float clamped = WWMath::Clamp(distance + Unknown0B4, Unknown0B0, Unknown0AC);
			Unknown06C.Translate(0.0f, 0.0f, clamped - Unknown0B4);
			Unknown0B4 = clamped;
		}
	}
	Matrix3D identity(true);
	Unknown054->Set_Transform(identity);
	FloatParent->Add_Sub_Object_To_Bone(Unknown054.Peek(), "NewShipScale");
	Unknown_90();
}

// NOT EXACT: ShipPipClass
// @0x80CE10
void ShipPipClass::Unknown_78()
{
	if (!Unknown0E8) {
		return;
	}
	float time = WW3D::Get_Frame_Time() * 0.001f;
	float x = 0.0f;
	float y = 0.0f;
	if (Type == 0 && FloatParent) {
		Matrix3D tm = FloatParent->Get_Transform();
		tm.Rotate_Z(time * Unknown_B07DC8);
		FloatParent->Set_Transform(tm);
	}
	if (Unknown0B8 == 0.0f && Unknown0BC == 0.0f && Unknown0C0 == 0.0f && Unknown0C4 == 0.0f && Unknown0C8 == 0.0f) {
		Unknown0E0 *= 0.99f;
		Unknown0E4 *= 0.99f;
	} else {
		if (Unknown0B8 != 0.0f) {
			Unknown0E0 = 0.0f;
		}
		if (Unknown0BC != 0.0f) {
			Unknown0E4 = 0.0f;
		}
	}
	if (Unknown0B8 != 0.0f) {
		x = time * Unknown0B8;
	} else if (Unknown0E0 != 0.0f) {
		x = time * Unknown0E0;
	}
	if (Unknown0BC != 0.0f) {
		y = time * Unknown0BC;
	} else if (Unknown0E4 != 0.0f) {
		y = time * Unknown0E4;
	}
	if (Unknown0C0 == 0.0f && Unknown0C4 == 0.0f) {
		if (Unknown0C8 != 0.0f) {
			float distance = Unknown_80E7A0() * time * Unknown0C8;
			if (distance != 0.0f) {
				float clamped = WWMath::Clamp(distance + Unknown0B4, Unknown0B0, Unknown0AC);
				Unknown06C.Translate(0.0f, 0.0f, clamped - Unknown0B4);
				Unknown0B4 = clamped;
			}
			Unknown_90();
			return;
		}
		if (fabs(x) >= 0.0001f || fabs(y) >= 0.0001f) {
			Quaternion q;
			if (Type == 0) {
				q = Trackball(0.0f, 0.0f, x, y, 0.8f);
			} else {
				q = Trackball(0.0f, 0.0f, x, 0.0f, 0.8f);
			}
			Matrix3D inverse;
			Unknown06C.Get_Orthogonal_Inverse(inverse);
			Vector3 center = inverse.Get_Translation();
			Unknown06C.Translate(center);
			Matrix3D::Multiply(Unknown06C, Build_Matrix3D(q), &Unknown06C);
			Unknown06C.Translate(-center);
			Unknown_90();
			return;
		}
	} else {
		float speed = Unknown_80E7A0();
		Unknown_80E020(speed * time * Unknown0C0, speed * Unknown0C4 * time);
		Unknown_90();
	}
}

// @0x80D540
bool ShipPipClass::Unknown_30(float x, float y)
{
	if (Unknown_60()) {
		int px = (int)(x * 640.0f - Unknown064.X);
		Unknown0A4 = px;
		int py = (int)(y * 480.0f - Unknown064.Y);
		Unknown0A8 = py;
		if (px >= 0 && px <= Unknown040 && py >= 0 && py <= Unknown044) {
			return(true);
		}
	}
	return(false);
}

// NOT EXACT: ShipPipClass
// @0x80D5E0
bool ShipPipClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!Unknown_60()) {
		return(false);
	}
	GadgetClass::Unknown_2C(event, x, y, mask);
	if (mask & 0x11) {
		Unknown_68();
		Unknown0A4 = (int)(x * 640.0f - Unknown064.X);
		Unknown0A8 = (int)(y * 480.0f - Unknown064.Y);
		Unknown09C = Unknown0A4;
		Unknown0A0 = Unknown0A8;
		return(true);
	}
	if ((mask & 4) && Unknown_BE5F24 == this) {
		if (Unknown_BE5F28 == this) {
			LoginUnknown18Class *mouse = Unknown_80E7D0();
			if (mouse) {
				mouse->Unknown_515120(2);
			}
		}
	} else if ((mask & 0x40) && Unknown_BE5F28 == this) {
		LoginUnknown18Class *mouse = Unknown_80E7D0();
		if (mouse) {
			mouse->Unknown_515120(2);
		}
	} else if ((mask & 2) && Unknown_BE5F24 == this && !(mask & 0x20) && Unknown_BE5F28 != this && Unknown0B8 == 0.0f && Unknown0BC == 0.0f) {

		// the left drag: the camera turns round the model, with momentum
		Unknown0A4 = (int)(x * 640.0f - Unknown064.X);
		Unknown0A8 = (int)(y * 480.0f - Unknown064.Y);
		float half_w = Unknown040 * 0.5f;
		float half_h = Unknown044 * 0.5f;
		float time = WW3D::Get_Frame_Time() * 0.001f;
		float x0 = (Unknown09C - half_w) / half_w;
		float y0 = (half_h - Unknown0A0) / half_h;
		float x1 = (Unknown0A4 - half_w) / half_w;
		float y1 = (half_h - Unknown0A8) / half_h;
		if (time > 0.0f) {
			float dy = y1 - y0;
			if (x1 - x0 == 0.0f) {
				Unknown0E0 = Unknown0E0 * 0.66f;
			} else {
				Unknown0E0 = (x1 - x0) / time;
			}
			if (dy == 0.0f) {
				Unknown0E4 = Unknown0E4 * 0.66f;
			} else {
				Unknown0E4 = dy / time;
			}
			if (fabs(Unknown0E0) + fabs(Unknown0E4) < 1.0f) {
				Unknown0E0 = 0.0f;
				Unknown0E4 = 0.0f;
			}
		}
		Unknown0E8 = false;
		Quaternion q(true);
		if (Type == 0) {
			q = Trackball(x0, y0, x1, y1, 0.8f);
		} else if (Type == 1) {
			q = Trackball(x0, 0.0f, x1, 0.0f, 0.8f);
		}
		Matrix3D inverse;
		Unknown06C.Get_Orthogonal_Inverse(inverse);
		Vector3 center = inverse.Get_Translation();
		Unknown06C.Translate(center);
		Matrix3D::Multiply(Unknown06C, Build_Matrix3D(q), &Unknown06C);
		Unknown06C.Translate(-center);
		Unknown_90();
		Unknown09C = Unknown0A4;
		Unknown0A0 = Unknown0A8;
		LoginUnknown18Class *mouse = Unknown_80E7D0();
		if (mouse) {
			mouse->Unknown_515120(0x18);
		}
		return(true);
	} else if ((mask & 0x20) && Unknown_BE5F28 == this && (mask & 2) && Unknown_BE5F24 == this) {

		// both buttons: the pan
		Unknown0A4 = (int)(x * 640.0f - Unknown064.X);
		Unknown0A8 = (int)(y * 480.0f - Unknown064.Y);
		float speed = Unknown_80E7A0();
		Unknown_80E020(speed * ((Unknown09C - Unknown0A4) / Unknown040), speed * ((Unknown0A0 - Unknown0A8) / Unknown044) * -1.0f);
		Unknown_90();
		LoginUnknown18Class *mouse = Unknown_80E7D0();
		if (mouse) {
			mouse->Unknown_515120(0x18);
		}
		Unknown_42ED80();
		float px;
		float py;
		Unknown_42ECF0(&px, &py);
		Unknown_30(px, py);
		Unknown09C = Unknown0A4;
		Unknown0A0 = Unknown0A8;
		return(true);
	} else if ((mask & 0x20) && Unknown_BE5F28 == this) {

		// the right drag: the zoom
		Unknown0A4 = (int)(x * 640.0f - Unknown064.X);
		Unknown0A8 = (int)(y * 480.0f - Unknown064.Y);
		float distance = ((Unknown0A0 - Unknown0A8) / Unknown044) * Unknown0B4 * 3.0f;
		Unknown_80E190(distance);
		Unknown_90();
		LoginUnknown18Class *mouse = Unknown_80E7D0();
		if (mouse) {
			if (distance > 0.0f) {
				mouse->Unknown_515120(0x17);
			} else if (distance < 0.0f) {
				mouse->Unknown_515120(0x16);
			}
		}
		Unknown_42ED80();
		float px;
		float py;
		Unknown_42ECF0(&px, &py);
		Unknown_30(px, py);
		Unknown0A0 = Unknown0A8;
		return(true);
	} else {
		return(false);
	}
	if (Unknown_BE5F24 == this) {
		LoginUnknown18Class *mouse = Unknown_80E7D0();
		if (mouse) {
			mouse->Unknown_515120(2);
		}
		Unknown0E8 = true;
	}
	Unknown_6C();
	return(true);
}

// @0x80E020
void ShipPipClass::Unknown_80E020(float x, float y)
{
	Unknown0CC += x;
	Unknown0D0 += y;
	if (Type == 0) {
		if (Unknown0CC < Unknown_B07D7C) Unknown0CC = Unknown_B07D7C;
		if (Unknown0D0 < Unknown_B07D84) Unknown0D0 = Unknown_B07D84;
		if (Unknown0CC > Unknown_B07D80) Unknown0CC = Unknown_B07D80;
		if (Unknown0D0 > Unknown_B07D88) Unknown0D0 = Unknown_B07D88;
	} else {
		if (Unknown0CC < Unknown_B07D8C) Unknown0CC = Unknown_B07D8C;
		if (Unknown0D0 < Unknown_B07D94) Unknown0D0 = Unknown_B07D94;
		if (Unknown0CC > Unknown_B07D90) Unknown0CC = Unknown_B07D90;
		if (Unknown0D0 > Unknown_B07D98) Unknown0D0 = Unknown_B07D98;
	}
}

// @0x80E190
void ShipPipClass::Unknown_80E190(float distance)
{
	if (distance != 0.0f) {
		float clamped = WWMath::Clamp(distance + Unknown0B4, Unknown0B0, Unknown0AC);
		Unknown06C.Translate_Z(clamped - Unknown0B4);
		Unknown0B4 = clamped;
	}
}

// @0x80E260
void ShipPipClass::Unknown_88()
{
	if (Type == 0) {
		Unknown0CC = Unknown_B07D9C;
		Unknown0D0 = Unknown_B07DA0;
	} else if (Type == 1) {
		if (Unknown0DC) {
			Unknown0CC = Unknown_B07DAC;
			if (Unknown054) {
				float h = ((CharacterViewAvatarClass *)Unknown054.Peek())->Unknown_77D9F0(false) - 7.3f;
				Unknown0D0 = Unknown_B07DB0 + h;
			} else {
				Unknown0D0 = Unknown_B07DB0;
			}
		} else {
			Unknown0CC = Unknown_B07DA4;
			Unknown0D0 = Unknown_B07DA8;
		}
	}
	Unknown_90();
}

// @0x80E350
void ShipPipClass::Unknown_8C()
{
	Matrix3D tm(true);
	if (Type == 0) {
		tm.Look_At(Vector3(128.0f, -100.0f, Unknown0AC), Vector3(0.0f, 0.0f, 0.0f), 0.0f);
		Unknown0B4 = Unknown0AC;
	} else if (Unknown0DC) {
		tm.Look_At(Vector3(Unknown0B0, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), 0.0f);
		Unknown0B4 = Unknown0B0;
	} else {
		tm.Look_At(Vector3(Unknown0AC, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), 0.0f);
		Unknown0B4 = Unknown0AC;
	}
	Unknown06C = tm;
	Unknown_90();
}

// @0x80E530
void ShipPipClass::Unknown_90()
{
	Vector3 xv = Unknown06C.Get_X_Vector() * Unknown0CC;
	Vector3 yv = Unknown06C.Get_Y_Vector() * Unknown0D0;
	Matrix3D tm(Unknown06C);
	tm.Set_Translation(Unknown06C.Get_Translation() + xv + yv);
	Camera->Set_Transform(tm);
	if (Type == 0) {
		Unknown04C->Set_Spot_Direction(tm.Rotate_Vector(Unknown_BE8C60));
		Unknown050->Set_Spot_Direction(tm.Rotate_Vector(Unknown_BE8C28));
	} else {
		Unknown04C->Set_Spot_Direction(tm.Rotate_Vector(Unknown_BE8C18));
		Unknown050->Set_Spot_Direction(tm.Rotate_Vector(Unknown_BE8C98));
	}
}

// @0x80E7A0
float ShipPipClass::Unknown_80E7A0()
{
	if (Type == 0) {
		return(Unknown_B07DC4);
	}
	return(Unknown0B4 * 0.6f);
}

// @0x80E7D0
LoginUnknown18Class *ShipPipClass::Unknown_80E7D0()
{
	if (PIPView->Unknown14->Unknown_08()) {
		return(((LoginTaskClass *)PIPView->Unknown14->Unknown_08())->Unknown0018);
	}
	if (PIPView->Unknown_30()) {
		return(*(LoginUnknown18Class **)(*(char **)(PIPView->Unknown_30() + 0x4C) + 0x1274));	// the view's +0x4C (its client) +0x1274
	}
	return(NULL);
}

/*
**	Reconstruction notes (ship_pip.cpp, 0x80B2E0-0x80E7D0)
**
**	The asserts sit on the client's lines: G_ASSERT 'false' 132 (0x80B2E0), FAIL_IF '!PIPView' 253 and
**	G_ASSERT 'FloatParent' 267 (0x80C220). The window's first three functions (0x80B0D0, 0x80B100, 0x80B160: an
**	8-byte and a 16-byte copy, and an std::sort partition of pointers with a predicate) are out-of-line template
**	and inline copies, not this file's code; 0x80CD90 is Matrix3D::Translate(float, float, float) out of line.
**	The destructor inlines slot +0x80 (0x80C5C0). The pointer's position after a pan or zoom drag: the client
**	reads it through 0x42EC90 / 0x42ECC0 (x and y, the input device's slot +0x10), not declared in input.h yet;
**	0x42ECF0 gives both.
*/
