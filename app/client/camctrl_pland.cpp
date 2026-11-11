// Earth & Beyond client (reconstruction): app\client\camctrl_pland.cpp, the planet-landing camera (state 7; notes at the end)
#include "camctrl_pland.h"
#include "eff_o.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "ini.h"
#include "random.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\land.h"
#include "..\shared\heightfield.h"
#include "..\shared\g_assert.h"
#include "hermitespline.h"
#include <stdio.h>

struct LandClassAccessor : public LandClass {
	HeightFieldClass *Get_Field() const { return Field; }
};

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file
static int Unknown_B9E3B0 = 0;					// NAME 1: neutral (D10): 0x794A90's section id

// @0x7946D0
CameraController07Class::CameraController07Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x7947C0
CameraController07Class::~CameraController07Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x794840
void CameraController07Class::Unknown_04()
{
	Unknown_794A90();
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

// @0x794970
void CameraController07Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown0C->Unknown_08(pos, rot);
	if (Client->Unknown10BC - Unknown10 > 4000) {
		Client->CameraManager->Unknown_79B2B0(0x0E);
		*mode = 2;
		return;
	}
	*mode = 2;
}

// @0x7949F0
void CameraController07Class::Unknown_0C()
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

// @0x794A90
// NOT EXACT: field-offset: a [reg + N] displacement differs; functional
void CameraController07Class::Unknown_794A90()
{
	ClientObjectClass *ship = Client->Get_Avatar();
	FAIL_IF(!ship) return;

	LandClass *land = Client->MainView->Land;
	FAIL_IF(!land) return;

	HeightFieldClass *hm = ((LandClassAccessor *)land)->Get_Field();
	FAIL_IF(!hm) return;

	INIClass ini;
	ini.Load("landingpattern.ini");
	char section[64];
	sprintf(section, "%d", Unknown_B9E3B0);
	if (!ini.Find_Section(section)) {
		sprintf(section, "Default");
	}
	float sx = ini.Get_Float(section, "SX", -1000.0f);
	float sy = ini.Get_Float(section, "SY", -1000.0f);
	float sz = ini.Get_Float(section, "SZ", 1250.0f);
	float ux = ini.Get_Float(section, "UX", 0.0f);
	float uy = ini.Get_Float(section, "UY", 0.0f);
	float uz = ini.Get_Float(section, "UZ", 1.0f);
	float speed_fwd = ini.Get_Float(section, "SpeedForward", 0.0f);
	float speed_down = ini.Get_Float(section, "SpeedDown", 0.0f);

	Matrix3D ship_tm = ship->Unknown_28(true);
	Vector3 start_pos = ship_tm * Vector3(sx, sy, sz);
	Vector3 up_dir = ship_tm.Rotate_Vector(Vector3(ux, uy, uz));
	up_dir.Normalize();

	Vector3 tangent0(speed_fwd, 0.0f, -speed_down);
	Vector3 cam_pos = ship_tm.Get_Translation();
	cam_pos.X += (Unknown_BDB1C0.Get_Float() - 0.5f) * 500.0f;
	cam_pos.Y += (Unknown_BDB1C0.Get_Float() - 0.5f) * 500.0f;
	cam_pos.Z += (Unknown_BDB1C0.Get_Float() - 0.5f) * 500.0f;

	float h = hm->Get_Height(cam_pos);
	if (cam_pos.Z < h + 100.0f) {
		cam_pos.Z = h + 100.0f;
	}

	Unknown0C->Position = cam_pos;
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();

	Unknown718510Class *state = new Unknown718510Class(Client->Unknown10BC, Client->Unknown10BC + 4000, Vector3(0.0f, 0.0f, 1.0f), up_dir, ship->Unknown98);
	HermiteSpline3DClass *spline = state->Unknown_30();
	spline->Add_Key(start_pos, 0.0f);
	spline->Set_Tangents(0, tangent0, tangent0);
	spline->Add_Key(cam_pos, 1.0f);
	Vector3 tangent1 = tangent0;
	spline->Set_Tangents(1, tangent1, tangent1);
	ship->Unknown98 = state;
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7946C0-0x79539F: after the state-1 controller's <string> initialiser 0x7946A0, ending with its own at 0x795380. Its own __FILE__
** 0xB9E3B8 (e:\build\beta\G\app\Client\camctrl_pland.cpp, FAIL_IFs l.71 / 100 / 119 / 122 / 125, G_ASSERT l.77): the
** same shape as camctrl_land.cpp with 0x79C8E0's module and request 14. 0x794A90 places the module, moves the ship along a spline.
*/
