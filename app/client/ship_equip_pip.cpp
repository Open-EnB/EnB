// Earth & Beyond client (reconstruction): app\client\ship_equip_pip.cpp, the ship's PIP on the PDA's equipment window (the
// client's __FILE__ 'e:\build\beta\G\app\Client\ship_equip_pip.cpp', 0x541D60-0x543550). Notes at the end.
#include "ship_equip_pip.h"
#include "camera.h"
#include "light.h"
#include "scene.h"
#include "quat.h"
#include "ww3d.h"
#include "wwmath.h"
#include "camera_manager.h"
#include "..\shared\ro_proxy.h"
#include "..\shared\g_assert.h"





























































// 0x541DE0
ShipEquipPipClass::ShipEquipPipClass(SClient *client) :
	PipSceneClass(client),
	LastMouse(0, 0),
	Distance(0),
	Unknown48(0),
	Unknown4C(0),
	Unknown50(true),
	Unknown80(true)
{
	Proxy = Create_NEW(new ProxyRObj(false));
	Proxy->Add(Scene.Peek());
	Light = Create_NEW(new LightClass(LightClass::POINT));
	Light->Set_Intensity(4.0f);
	Light->Set_Force_Visible(1);
	Light->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Light->Set_Ambient(Vector3(0, 0, 0));
	Light->Set_Specular(Vector3(0, 0, 0));
	Light->Set_Diffuse(Vector3(0.8f, 0.8f, 0.8f));
	Light->Set_Transform(Matrix3D::Identity);
	Light->Add(Scene.Peek());
	SimpleSceneClass *scene = Scene.Peek();
	FAIL_IF(!scene) return;
	Vector3 ambient(0.2f, 0.2f, 0.2f);
	scene->Set_Ambient_Light(ambient);
}

// 0x542180 (deleting 0x542150)
ShipEquipPipClass::~ShipEquipPipClass()
{
	Light->Remove();
	Proxy->Remove();
}














































// NOT EXACT: functional from the pseudocode (D64); its locals sit in other frame slots
// 0x542250
void ShipEquipPipClass::Unknown_542250(const RefCountPtr<RenderObjClass> &robj)
{
	if (Proxy->Proxied != robj.Peek()) {
		Proxy->Unknown_47FF70(robj.Peek());
		Proxy->Set_Transform(Matrix3D::Identity);
		if (robj) {
			float unit = CameraManagerClass::Unknown_79B6B0();
			Unknown48 = unit;
			Vector3 target(0, 0, 0);
			Vector3 pos(unit * 1.1, unit * 1.1, unit * 1.1);
			Proxy->Set_Position(Vector3(0, 0, 0));
			Matrix3D tm(true);
			tm.Look_At(pos, target, 0);
			Unknown_543270(tm);
			Distance = (pos - target).Length();
		}
	} else if (robj) {
		float unit = CameraManagerClass::Unknown_79B6B0();
		Vector3 target(0, 0, 0);
		Vector3 pos;
		if (unit != Unknown48) {
			CameraClass *camera = Camera.Peek();
			FAIL_IF(!camera) return;
			Unknown50 = camera->Get_Transform();
			Unknown80 = Unknown50;
			Unknown80.Translate_Z(unit - Unknown48);
			pos = Unknown80.Get_Translation();
			Unknown80.Look_At(pos, target, 0);
			Unknown4C = 1.0f;
			Unknown48 = unit;
		}
		if (Unknown4C > 0.0f) {
			Unknown4C -= WW3D::Get_Frame_Time() * 0.001f;
			Unknown4C = WWMath::Clamp(Unknown4C, 0.0f, 1.0f);
			Matrix3D tm = Lerp(Unknown50, Unknown80, 1.0f - Unknown4C);
			Unknown_543270(tm);
			pos = tm.Get_Translation();
			pos -= target;
			Distance = pos.Length();
			if (Unknown4C == 0.0f) {
				Unknown_542EE0(0);
			}
		}
	}
}























// NOT EXACT: functional from the pseudocode (D64); the translations' products are summed in another order
// 0x542A10
void ShipEquipPipClass::Unknown_542A10(const Vector2 &point, bool on)
{
	if (Unknown4C <= 0.0f) {
		Vector2 mouse;
		Unknown_543200(&mouse, point);
		Vector2 last = LastMouse;
		LastMouse = mouse;
		if (!on) {
			CameraClass *camera = Camera.Peek();
			FAIL_IF(!camera) return;
			Matrix3D tm = camera->Get_Transform();
			Quaternion rot = Trackball(last.X, last.Y, mouse.X, mouse.Y, 0.8f);
			Matrix3D inv;
			tm.Get_Orthogonal_Inverse(inv);
			CameraManagerClass::Unknown_79B6B0();
			Vector3 center = inv.Get_Translation();
			tm.Translate(center.X, center.Y, center.Z);
			Matrix3D::Multiply(tm, Build_Matrix3D(rot), &tm);
			tm.Translate(-center.X, -center.Y, -center.Z);
			Unknown_543270(tm);
		}
	}
}




























#pragma auto_inline(off)
// NOT EXACT: functional from the pseudocode (D64); the inlined Unknown_543270 reads Camera earlier (scheduling)
// 0x542EE0 (out of line in all three callers, 0x542250's included)
void ShipEquipPipClass::Unknown_542EE0(float zoom)
{
	CameraClass *camera = Camera.Peek();
	FAIL_IF(!camera) return;
	Matrix3D tm = camera->Get_Transform();
	float unit = CameraManagerClass::Unknown_79B6B0();
	float distance = WWMath::Clamp(unit * zoom + Distance, unit * 0.5f, unit * 2);
	tm.Translate_Z(distance - Distance);
	Distance = distance;
	Unknown_543270(tm);
}

#pragma auto_inline(on)























// 0x543200
void ShipEquipPipClass::Unknown_543200(Vector2 *dest_mouse, Vector2 point)
{
	FAIL_IF(!dest_mouse) return;
	dest_mouse->X = point.X * 2 - 1.0f;
	dest_mouse->Y = (1.0f - point.Y) * 2 - 1.0f;
}














// 0x543270 (no callers: the functions above inline it)
void ShipEquipPipClass::Unknown_543270(const Matrix3D &tm)
{
	CameraClass *camera = Camera.Peek();
	FAIL_IF(!camera) return;
	camera->Set_Transform(tm);
	Matrix3D light_tm = tm;
	light_tm.Pre_Rotate_X(DEG_TO_RADF(30));
	light_tm.Pre_Rotate_Y(DEG_TO_RADF(30));
	Light->Set_Transform(light_tm);
}


/*
**	Reconstruction notes (O18, 2026-11-13)
**
**	The object: the previous object ends with its guarded <string> static 0x541D30; this one holds the deleting
**	destructors of two MemberCallback copies (0x541D60, 0x541D90) and the this-adjusting thunks 0x541DC0 / 0x541DD0
**	(InterfaceClass's deleting destructor), then the class from the constructor 0x541DE0 to Unknown_543270 0x543270,
**	RefCountPtr<ProxyRObj>'s Create_NEW 0x543520 and its Clear 0x543550 (also in the unwinds of 0xAD34A0); the next
**	object opens with permstr.h's PermStringInitClass static 0x543580. No <string> static: this file includes none.
**	Unknown_542EE0 is out of line in its three callers (0x542250 here, gd_ship_equip_pip.cpp's two): auto_inline(off)
**	at its definition keeps 0x542250 from inlining it. Unknown_543270 has no callers: 0x542250 (twice), 0x542A10 and
**	0x542EE0 inline it. Unknown_543200 takes the point by value (the client copies it onto the stack).
**	Not exact (one round each, attempts.csv): 0x542250's locals sit in other frame slots (the client keeps the unit in
**	the frame and orders the target, the proxy's position and the camera's position differently); 0x542A10 sums each
**	translation's products in another order; 0x542EE0 loads Camera for the inlined Unknown_543270 before the
**	translation (the client after).
*/
