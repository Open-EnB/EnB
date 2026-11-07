// Earth & Beyond client (reconstruction): app\client\camctrl_stargate.cpp, the stargate camera (state 3; notes at the end)
#include "camctrl_stargate.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "random.h"
#include "..\shared\g_assert.h"

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file
static float Unknown_B04FF8 = 4.0f;				// NAME 1: neutral (D10): 0x795C80's

// @0x795900
CameraController03Class::CameraController03Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown18(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x7959D0
CameraController03Class::~CameraController03Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x795A50
void CameraController03Class::Unknown_04()
{
	Unknown_795C80(0);
	Unknown10 = Client->Unknown10BC;
	Unknown14 = 0;
}

// @0x795A80
// NOT EXACT: stack slots and one copy of the distance differ; functional
void CameraController03Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	float distance = CameraManagerClass::Unknown_79B6B0();



















	FAIL_IF(!Client->Get_Avatar()) {
		return;
	}
	switch (Unknown14) {
		case 0: {
			Unknown0C->Unknown_08(pos, rot);
			bool close = false;
			ClientObjectClass *gate = Client->Find_Object(Client->Unknown13BC);
			if (gate && gate->Unknown_6F5B70()) {
				float dist = (gate->Unknown_24() - Client->Get_Avatar()->Unknown_24()).Length();
				if (dist < (gate->Unknown_30() + distance) * 0.5f) {
					close = true;
				}
			}
			static float Unknown_B05038 = 7000.0f;	// NAME 1: neutral (D10): the phase's length (ms)
			if ((float)(Client->Unknown10BC - Unknown10) > Unknown_B05038 || close) {
				Unknown14 = 1;
				Unknown0C->Unknown_0C();
				Unknown_795C80(1);
				Client->CameraManager->Unknown_79B2B0(0x18);
			}
			break;
		}
		case 1:
			Unknown0C->Unknown_08(pos, rot);
			*mode = 2;
			return;
	}
	*mode = 2;
}

// @0x795C60
void CameraController03Class::Unknown_0C()
{
	Unknown0C->Unknown_0C();
}

// @0x795C80
// NOT EXACT: field-offset: a [reg + N] displacement differs; functional
void CameraController03Class::Unknown_795C80(bool phase)
{
	FAIL_IF(!Client->Get_Avatar()) {
		return;
	}

	float distance = CameraManagerClass::Unknown_79B6B0();
	ClientObjectClass *gate = Client->Find_Object(Client->Unknown13BC);
	if (gate && gate->Unknown_6F5B70()) {
		Vector3 pos;
		if (phase) {
			Matrix3D tm = gate->Unknown_28(true);
			float angle = Unknown_BDB1C0.Get_Float() * (WWMATH_PI / 4.0f);
			if (Unknown18 == 0) {
				angle = WWMATH_PI - angle;
			} else if (Unknown18 == 2) {
				angle = -angle;
			} else if (Unknown18 == 3) {
				angle = angle + WWMATH_PI;
			}
			float s = sin(angle);
			float c = cos(angle);
			pos = tm * Vector3(5.0f * distance, 3.0f * distance * c, 3.0f * distance * s);
		} else {
			Matrix3D avatar_tm = Client->Get_Avatar()->Unknown_28(false);
			Vector3 avatar_pos = Client->Get_Avatar()->Unknown_24();
			float angle = Unknown_BDB1C0.Get_Float() * (WWMATH_PI / 4.0f);
			Unknown18 = Unknown_BDB1C0() % 4;
			if (Unknown18 == 1) {
				angle = WWMATH_PI - angle;
			} else if (Unknown18 == 2) {
				angle = angle + WWMATH_PI;
			} else if (Unknown18 == 3) {
				angle = -angle;
			}
			float s = sin(angle);
			float c = cos(angle);
			Matrix3D lookat;
			lookat.Obj_Look_At(avatar_pos, gate->Unknown_24(), 0.0f);
			pos = lookat * Vector3(3.0f * distance, 1.5f * distance * c, 1.5f * distance * s);
		}
		Unknown0C->Position = pos;
		Unknown0C->Unknown14 = true;
		Unknown0C->Unknown18 = Client->AvatarID;
		Unknown0C->Unknown_04();

		ClientObjectClass *ship = Client->Get_Avatar();
































		FAIL_IF(!ship) {
			return;
		}
		Vector3 gate_pos = gate->Unknown_24();
		Vector3 ship_pos = ship->Unknown_24();
		ship->Unknown98 = new Unknown718160Class(ship_pos, gate_pos, Unknown_B04FF8, Client->Unknown10BC, ship->Unknown98);
	}
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7958F0-0x79619F (its <string> initialiser 0x796180 last): after camctrl_rescue.cpp's, before the state-9 controller 0x7961B0. Its own
** __FILE__ 0xB9E47C (e:\build\beta\G\app\Client\camctrl_stargate.cpp, FAIL_IF l.65, 106, 183). The static 0xB04FF8 (4.0) comes
** before the vtable, 0xB05038 (7000) and 0xB0503C (pi / 4) after it. 0x795C80 places the module for the phase.
*/
