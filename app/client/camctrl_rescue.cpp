// Earth & Beyond client (reconstruction): app\client\camctrl_rescue.cpp, the rescue camera (state 11; notes at the end)
#include "camctrl_rescue.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "random.h"
#include "..\shared\g_assert.h"

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

static float Unknown_B04F3C = 1.75f;			// NAME 1: neutral (D10): the side offset, in 0x79B6B0's distances
static float Unknown_B04F40 = 0.5f;				// NAME 1: neutral (D10): the forward offset

// @0x7953B0
CameraController11Class::CameraController11Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL)
{
	Unknown0C = new CameraModule79C4A0Class(Client);
}

// @0x795480
CameraController11Class::~CameraController11Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x795500
void CameraController11Class::Unknown_04()
{
	Unknown_795570();
}

// @0x795520
void CameraController11Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown0C->Unknown_08(pos, rot);
	*mode = 1;
}

// @0x795550
void CameraController11Class::Unknown_0C()
{
	Unknown0C->Unknown_0C();
}

// @0x795570
// NOT EXACT: the offset's temporaries and slots differ (the expression right to left, as the client's); functional
void CameraController11Class::Unknown_795570()
{
	ClientObjectClass *ship = Client->Get_Avatar();



















	FAIL_IF(!ship) {
		return;
	}
	Matrix3D cam(true);
	Matrix3D tm = ship->Unknown_28(true);
	float distance = CameraManagerClass::Unknown_79B6B0();
	Vector3 y = tm.Get_Y_Vector();
	Vector3 x = tm.Get_X_Vector();
	Vector3 pos;
	tm.Get_Translation(&pos);
	Vector3 from = (x * Unknown_B04F40 + y * Unknown_B04F3C * (float)(Unknown_BDB1C0(0, 1) ? -1 : 1)) * distance + pos;
	cam.Look_At(from, pos, 0.0f);
	Unknown0C->Unknown08 = cam;
	static float Unknown_B04F80 = -0.000005f;	// NAME 1: neutral (D10)
	static float Unknown_B04F84 = 0.000005f;	// NAME 1: neutral (D10)
	Unknown0C->Unknown38 = Unknown_BDB1C0(0, 1) ? Unknown_B04F84 : Unknown_B04F80;
	Unknown0C->Unknown3C = Unknown_BDB1C0(0, 2);
	Unknown0C->Unknown_04();
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7953A0-0x7958EF: after camctrl_pland.cpp's <string> initialiser 0x795380, ending with its own at 0x7958D0
** (before the state-3 controller 0x795900). Its own __FILE__ 0xB9E440 (e:\build\beta\G\app\Client\camctrl_rescue.cpp,
** FAIL_IF l.75). The statics 0xB04F3C / 0xB04F40 (1.75, 0.5) come before the vtable, 0xB04F80 / 0xB04F84 (-+5e-6)
** after it: function-local here. 0x795570 puts the camera at (X 0.5 +- Y 1.75) distances from the avatar, looking
** at it.
*/
