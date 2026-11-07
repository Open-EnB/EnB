// Earth & Beyond client (reconstruction): app\client\camctrl_tractor.cpp, the tractor camera (state 8; notes at the end)
#include "camctrl_tractor.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "random.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

static float Unknown_B05198 = 45.0f;			// NAME 1: neutral (D10): the angle's spread (degrees)
static float Unknown_B0519C = -3.0f;			// NAME 1: neutral (D10): the offset along the line, in 0x79B6B0's distances
static float Unknown_B051A0 = 3.0f;			// NAME 1: neutral (D10): the offset across it

// @0x797320
CameraController08Class::CameraController08Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0),
	Unknown18(0),
	Unknown1C(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x797410
CameraController08Class::~CameraController08Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x797490
void CameraController08Class::Unknown_04()
{
	Unknown1C = 1;
	Unknown_797770();
	Unknown10 = Client->Unknown10BC;
	Unknown18 = Client->Unknown13B4;
}

// @0x797740
void CameraController08Class::Unknown_797740()
{
	Unknown_0C();
	Unknown_04();
}

// @0x7974D0
void CameraController08Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown_797570();
	if (Unknown1C == 1 && Unknown18 != Client->Unknown13B4) {
		Unknown_797740();
	}
	Unknown0C->Unknown_08(pos, rot);
	if (Unknown1C == 3) {
		Client->CameraManager->Unknown_79B2B0(0x10);
		Client->Unknown13B4 = 0;
		*mode = 1;
		return;
	}
	*mode = 1;
}

// @0x797570
// NOT EXACT: the register choice (edx / ebp for 3) and the duplicated !ship test differ; functional
void CameraController08Class::Unknown_797570()
{
	if (Unknown1C == 3) {
		return;
	}
	if (Unknown1C == 2) {
		if (Client->Unknown10BC > Unknown14) {
			Unknown1C = 3;
		}
		return;
	}
	if (Client->Unknown10BC - Unknown10 > 15000) {
		Unknown1C = 3;
		return;
	}
	ClientObjectClass *target = Client->Find_Object(Client->Unknown13B4);
	ClientObjectClass *ship = Client->Get_Avatar();
	if (target) {
		Vector3 target_pos = target->Unknown_24();
		Vector3 delta = target_pos - ship->Unknown_24();
		float radius = target->Unknown_30();
		float distance = CameraManagerClass::Unknown_79B6B0();
		if (!(delta.Length2() < radius * radius + distance * distance)) {
			return;
		}
	}
	if (!ship) {
		Unknown1C = 3;
		return;
	}
	Unknown1C = 2;
	Unknown14 = Client->Unknown10BC + 1000;
	Unknown0C->Unknown18 = ship->GameID;
}

// @0x797720
void CameraController08Class::Unknown_0C()
{
	Unknown0C->Unknown_0C();
}

// @0x797770
// NOT EXACT: a few operand loads and slots differ (16 rows); functional
void CameraController08Class::Unknown_797770()
{
	ClientObjectClass *target = Client->Find_Object(Client->Unknown13B4);
	if (!target) {
		Client->CameraManager->Unknown_79B2B0(0x10);
		return;
	}
	ClientObjectClass *ship = Client->Get_Avatar();



















































	FAIL_IF(!ship) {
		return;
	}
	float angle = Unknown_BDB1C0.Get_Float() * (Unknown_B05198 * (WWMATH_PI / 180.0f));
	switch (Unknown_BDB1C0(0, 1)) {
		case 1:
			angle = WWMATH_PI - angle;
			break;
	}
	float distance = CameraManagerClass::Unknown_79B6B0();
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle);
	Vector3 offset(Unknown_B0519C * distance, c * Unknown_B051A0 * distance, s * Unknown_B051A0 * distance);
	Vector3 target_pos = target->Unknown_24();
	Matrix3D tm;
	tm.Obj_Look_At(ship->Unknown_24(), target_pos, 0.0f);
	Unknown0C->Position = tm * offset;
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->Unknown13B4;
	Unknown0C->Unknown_04();
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x797310-0x79798F: after camctrl_takeoff.cpp's <string> initialiser 0x797300, ending with its own at
** 0x797970 (before the state-2 controller's object 0x7979A0). Its __FILE__ 0xB9E504
** (e:\build\beta\G\app\Client\camctrl_tractor.cpp) is used only by 0x797770 (FAIL_IF l.176: places the module on the line from the avatar to the object).
** 0x797740 (stop and start again) is an ordinary member that /Ob2 inlines into the update: its own copy stays.
*/
