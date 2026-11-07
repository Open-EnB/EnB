// Earth & Beyond client (reconstruction): app\client\camctrl_entersector.cpp, the sector-entry camera (state 4; notes at the end)
#include "camctrl_entersector.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "random.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

static Vector3 Unknown_BE8688(50.0f, 1.25f, 1.25f);	// NAME 1: neutral (D10): the offset's scale (initialiser 0x7934F0)
static float Unknown_B04CBC = 0.3f * WWMATH_PI;		// NAME 1: neutral (D10): the angle's spread (54 degrees)

// @0x793520
CameraController04Class::CameraController04Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x7935F0
CameraController04Class::~CameraController04Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x793670
void CameraController04Class::Unknown_04()
{
	Unknown_793740();
	Unknown10 = Client->Unknown10BC;
}

// @0x7936A0
void CameraController04Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown0C->Unknown_08(pos, rot);
	if (Client->Unknown10BC - Unknown10 > 5000) {
		Client->CameraManager->Unknown_79B2B0(7);
		*mode = 2;
		return;
	}
	*mode = 2;
}

// @0x793720
void CameraController04Class::Unknown_0C()
{
	Unknown0C->Unknown_0C();
}

// @0x793740
// NOT EXACT: one operand order (offset.Z *= scale.Z: ours loads scale.Z first); functional
void CameraController04Class::Unknown_793740()
{









	FAIL_IF(!Client->Get_Avatar()) {
		return;
	}
	Matrix3D tm = Client->Get_Avatar()->Unknown_28(true);
	float angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * Unknown_B04CBC;
	if (Unknown_BDB1C0.Get_Float() > 0.5f) {
		angle += WWMATH_PI;
	}
	float distance = CameraManagerClass::Unknown_79B6B0();
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle);
	Vector3 offset(distance, c * distance, s * distance);
	offset.Scale(Unknown_BE8688);
	Unknown0C->Position = tm * offset;
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7934E0-0x79390F: after camctrl_emote.cpp's <string> initialiser 0x7934C0, starting with this file's
** initialiser of Unknown_BE8688 0x7934F0 (a file-scope Vector3: its $E is emitted before the functions), ending with
** <string>'s initialiser 0x7938F0. Its own __FILE__ 0xB9E330 (e:\build\beta\G\app\Client\camctrl_entersector.cpp,
** FAIL_IF l.73). 0xB04CBC is the angle's spread (0.3 pi). 0x793740 builds the offset (distance, c d, s d) and
** scales it by Unknown_BE8688 (Vector3::Scale; the separate-component forms are further off).
*/
