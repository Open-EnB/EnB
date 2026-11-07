// Earth & Beyond client (reconstruction): app\client\camctrl_stargate.cpp, the stargate camera (state 3; notes at the end)
#include "camctrl_stargate.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "..\shared\g_assert.h"

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

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7958F0-0x79619F (its <string> initialiser 0x796180 last): after camctrl_rescue.cpp's, before the state-9 controller 0x7961B0. Its own
** __FILE__ 0xB9E47C (e:\build\beta\G\app\Client\camctrl_stargate.cpp, FAIL_IF l.65). The static 0xB04FF8 (4.0) comes
** before the vtable, 0xB05038 (7000) and 0xB0503C (pi / 4) after it. Not rebuilt: 0x795C80 (the module for the
** phase).
*/
