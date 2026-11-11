// Earth & Beyond client (reconstruction): app\client\camctrl_land.cpp, the landing camera (state 5; notes at the end)
#include "camctrl_land.h"
#include "eff_o.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "random.h"
#include "hermitespline.h"
#include "..\shared\g_assert.h"

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

static float Unknown_B04D30 = 0.00002f;			// NAME 1: neutral (D10): the module's +0x38, signed at random

// @0x793920
CameraController05Class::CameraController05Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0)
{
	Unknown0C = new CameraModule79C4A0Class(Client);
}

// @0x793A10
CameraController05Class::~CameraController05Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x793A90
void CameraController05Class::Unknown_04()
{
	Unknown_793CE0();
	Unknown10 = Client->Unknown10BC;
	Unknown14 = Client->Next_Local_ID();
	EffectObjectClass *effect = Unknown_6E9490(0x16A, Unknown14, 4000, Unknown10, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);

























	FAIL_IF(!effect) return;

	UnknownB00060EffectClass *source = effect->Unknown_10();
	if (source) {
		source->Set_Source_Object_ID(Client->AvatarID);
	} else {
		G_ASSERT(0);
	}
	MainViewClass *view = Client->MainView;
	Client->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
}

// @0x793BC0
void CameraController05Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown0C->Unknown_08(pos, rot);
	if (Client->Unknown10BC - Unknown10 > 4000) {
		Client->CameraManager->Unknown_79B2B0(9);
		*mode = 2;
		return;
	}
	*mode = 2;
}

// @0x793C40
void CameraController05Class::Unknown_0C()
{
	Unknown0C->Unknown_0C();
	ClientObjectClass *ship = Client->Get_Avatar();
	FAIL_IF(!ship) {
		return;
	}
	Unknown98Class *state = ship->Unknown98;
	if (state && state->Unknown_2C()) {
		ship->Unknown98 = state->Unknown_28();
		delete state;
	}
	EffectObjectClass *effect = Client->Get_Effect(Unknown14);
	if (effect) {
		effect->Unknown_6E6E30();
	}
}

// @0x793CE0
// NOT EXACT: written from the client's code and Ghidra's (0x830 bytes; the temporaries' order and slots differ), functional
void CameraController05Class::Unknown_793CE0()
{
	ClientObjectClass *target = Client->Find_Object(Client->Unknown13B8);
	FAIL_IF(!target) {
		return;
	}
	ClientObjectClass *ship = Client->Get_Avatar();
	FAIL_IF(!ship) {
		return;
	}
	Matrix3D tm;
	Vector3 target_pos = target->Unknown_24();
	Vector3 ship_pos = ship->Unknown_24();
	float radius = target->Unknown_30();
	Vector3 dir = ship_pos - target_pos;
	dir.Normalize();
	Vector3 side = Vector3::Cross_Product(Vector3(0.0f, 0.0f, 1.0f), dir);
	Vector3 up = Vector3::Cross_Product(dir, side);
	if (Unknown_BDB1C0(0, 1)) {
		side = -side;
	}
	Vector3 offset = (side + up) * radius * 0.5f;
	Vector3 from = target_pos + dir * radius * 0.5f;
	Vector3 to = target_pos + offset;
	tm.Look_At(from, to, 0.0f);
	Unknown0C->Unknown08 = tm;
	Unknown0C->Unknown38 = Unknown_BDB1C0(0, 1) ? Unknown_B04D30 : -Unknown_B04D30;
	Unknown0C->Unknown3C = 2;
	Unknown0C->Unknown_04();

	Vector3 key0 = from + dir * 100.0f;
	offset.Normalize();
	Vector3 key1 = key0 + (offset - dir) * 1000.0f;
	Unknown718510Class *state = new Unknown718510Class(Client->Unknown10BC, Client->Unknown10BC + 4000, Vector3(0.0f, 0.0f, 1.0f), dir, ship->Unknown98);
	HermiteSpline3DClass *spline = state->Unknown_30();
	dir *= 100.0f;
	offset *= 1000.0f;
	spline->Add_Key(key0, 0.0f);
	Vector3 tangent0 = -dir;
	spline->Set_Tangents(0, tangent0, tangent0);
	spline->Add_Key(key1, 1.0f);
	Vector3 tangent1 = offset - dir * 5.0f;
	spline->Set_Tangents(1, tangent1, tangent1);
	ship->Unknown98 = state;
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x793910-0x7944FF: after camctrl_entersector.cpp's <string> initialiser 0x7938F0, ending with its own
** at 0x7944E0 (before the state-1 controller's 0x794510). Its own __FILE__ 0xB9E374
** (e:\build\beta\G\app\Client\camctrl_land.cpp, FAIL_IFs l.68 / 97 / 116 / 120, G_ASSERT l.74). The start takes
** its effect id through an inline that returns --Unknown113C (client.h's Next_Local_ID: the value stays in a
** register; '--Client->Unknown113C' reads it back). 0x793CE0 is functional: the camera half a radius off the
** target towards the ship, looking along (side + up); the ship's spline from there, out 100 then 1000 along the
** bent direction, over four seconds (Unknown718510Class).
*/
