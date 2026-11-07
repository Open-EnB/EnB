// Earth & Beyond client (reconstruction): app\client\unknown_79c4a0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "camctrl_land.h"
#include "sclient.h"
#include "client.h"
#include "quat.h"

// 0x79C4A0
CameraModule79C4A0Class::CameraModule79C4A0Class(SClient *client) :
	CameraModuleClass(client),
	Unknown08(true),
	Unknown38(0.0f),
	Unknown3C(0),
	Unknown40(0)
{
}

// 0x79C560
void CameraModule79C4A0Class::Unknown_04()
{
	Unknown40 = Client->Unknown10BC;
	CameraModuleClass::Unknown_04();
}

// 0x79C590
void CameraModule79C4A0Class::Unknown_08(Vector3 *des_pos, Quaternion *des_quat)
{
	float angle = (float)(unsigned)(Client->Unknown10BC - Unknown40) * Unknown38;
	Matrix3D tm = Unknown08;
	switch (Unknown3C) {
		case 0:
			tm.Rotate_Y(angle);
			break;
		case 1:
			tm.Rotate_X(angle);
			break;
		case 2:
			tm.Rotate_Z(-angle);
			break;
	}
	*des_pos = tm.Get_Translation();
	*des_quat = Build_Quaternion(tm);
}

/*
** Reconstruction notes (WE3, 2026-11-07)
**
** The object: no __FILE__ or assert; client 0x79C4A0-0x79C8CF, after cammod_chase.cpp's <string> initialiser
** 0x79C470 and ending with its own at 0x79C8B0. The module (table 0xB05594, 0x44 bytes) turns the transform the
** landing and rescue controllers set (+0x08) about one axis (+0x3C: 0 Y, 1 X, 2 Z) by the client clock since
** slot +0x04 started it (+0x40) times the rate +0x38. Its destructor 0x79C540 and deleting destructor 0x79C510.
*/
