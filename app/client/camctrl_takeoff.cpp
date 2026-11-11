// Earth & Beyond client (reconstruction): app\client\camctrl_takeoff.cpp, the take-off camera (state 9; notes at the end)
#include "camctrl_takeoff.h"
#include "eff_o.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "random.h"
#include "obbox.h"
#include "castres.h"
#include "scene.h"
#include "camera.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

class MainViewSceneClass : public SimpleSceneClass
{
public:
	void Unknown_75FB00(const OBBoxClass &to, const OBBoxClass &from, const Vector3 &move, CastResultStruct *result, RenderObjClass *ignore, bool ship);
};

// @0x7961B0
CameraController09Class::CameraController09Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x796280
CameraController09Class::~CameraController09Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x796300
void CameraController09Class::Unknown_04()
{
	Unknown_796920();
	Unknown5C = false;
	Unknown10 = Client->Unknown10BC;
	Unknown14 = Client->Next_Local_ID();
	EffectObjectClass *effect = Unknown_6E9490(0x170, Unknown14, 5000, Unknown10, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);





















































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

// @0x796440
// NOT EXACT: shape: the same callees, 16 calls in ours and 17 in the client; functional
void CameraController09Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	ClientObjectClass *ship = Client->Get_Avatar();
	float distance = CameraManagerClass::Unknown_79B6B0();
	FAIL_IF(!ship) {
		return;
	}
	Unknown0C->Unknown_08(pos, rot);
	unsigned int elapsed = Client->Unknown10BC - Unknown10;
	if (elapsed < 2000) {
		float t = (float)elapsed * 0.0005f;
		Quaternion q;
		Slerp(q, Unknown3C, Unknown4C, t * t);
		Matrix3D tm = Build_Matrix3D(q);
		Vector3 p = (Unknown30 - Unknown24) * t + Unknown24;
		tm.Set_Translation(p);
		if (ship->Unknown98 && ship->Unknown98->Unknown_04() == 0x0E) {
			((Unknown718D70Class *)ship->Unknown98)->Unknown_30(Client->Unknown10BC, tm);
			*mode = 2;
			return;
		}
	} else if (elapsed < 5000) {
		if (!Unknown5C) {
			Unknown5C = true;
			Matrix3D lookat;
			lookat.Obj_Look_At(Unknown30, Unknown18, 0.0f);
			float angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * 30.0f * (WWMATH_PI / 180.0f);
			if (Unknown_BDB1C0(0, 1) == 1) {
				angle = WWMATH_PI - angle;
			}
			float dist = (Unknown_BDB1C0.Get_Float() * 7.0f + 10.0f) * distance;
			float s = sin(angle);
			float c = cos(angle);
			Unknown0C->Position = lookat * Vector3(dist, c * 1.5f * distance, s * 1.5f * distance);
			Unknown0C->Unknown_04();
		}
		float t = (float)(elapsed - 2000) * 0.00033333333f;
		Matrix3D tm = Build_Matrix3D(Unknown4C);
		Vector3 p = (Unknown18 - Unknown30) * t + Unknown30;
		tm.Set_Translation(p);
		if (ship->Unknown98 && ship->Unknown98->Unknown_04() == 0x0E) {
			((Unknown718D70Class *)ship->Unknown98)->Unknown_30(Client->Unknown10BC, tm);
			*mode = 2;
			return;
		}
	} else {
		Client->CameraManager->Unknown_79B2B0(0x12);
	}
	*mode = 2;
}















// @0x796850
void CameraController09Class::Unknown_0C()
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

// @0x7968F0
bool CameraController09Class::Unknown_14()
{
	return(Client->Unknown10BC - Unknown10 > 2000);
}
// @0x796920
// NOT EXACT: field-offset: a [reg + N] displacement differs; functional
void CameraController09Class::Unknown_796920()
{
	ClientObjectClass *ship = Client->Get_Avatar();
	FAIL_IF(!ship) {
		return;
	}
	RenderObjClass *ship_ro = ship->RenderObject;
	FAIL_IF(!ship_ro) {
		return;
	}

	Unknown24 = ship->Unknown_24();
	Unknown30 = Unknown24;
	Unknown30.Z += Unknown_BDB1C0.Get_Float() * 500.0f + 100.0f;
	Unknown3C = Build_Quaternion(ship->Unknown_28(true));

	for (int i = 0; i <= 10; ++i) {
		if (i >= 10) {
			Unknown18 = Unknown30;
			Unknown18.Z += 5000.0f;
			break;
		}
		float pitch = (Unknown_BDB1C0.Get_Float() * 30.0f + 30.0f) * (WWMATH_PI / 180.0f);
		float yaw = (Unknown_BDB1C0.Get_Float() * 315.0f + 45.0f) * (WWMATH_PI / 180.0f);
		Matrix3D ship_tm = ship->Unknown_28(true);
		float zrot = ship_tm.Get_Z_Rotation();
		float c_p = cos(pitch);
		float s_p = sin(pitch);
		float c_y = cos(zrot + yaw);
		float s_y = sin(zrot + yaw);
		Vector3 dir(c_y * c_p, s_y * c_p, s_p);
		Vector3 offset = dir * 5000.0f;
		Unknown18 = Unknown30 + offset;

		CameraClass *cam = (CameraClass *)Client->MainView->Unknown_7C();
		OBBoxClass box = cam->Get_Near_Clip_Bounding_Box();
		OBBoxClass moved;
		Vector3 delta = Unknown18 - Unknown30;
		Matrix3D shift(true);
		shift.Set_Translation(delta);
		OBBoxClass::Transform(shift, box, &moved);
		CastResultStruct result;
		MainViewSceneClass *scene = (MainViewSceneClass *)Client->MainView->Unknown_64();
		scene->Unknown_75FB00(moved, box, delta, &result, ship_ro, false);
		if (result.Fraction >= 1.0f) {
			break;
		}
	}

	Matrix3D lookat;
	lookat.Obj_Look_At(Unknown30, Unknown18, 0.0f);
	Unknown4C = Build_Quaternion(lookat);

	float distance = CameraManagerClass::Unknown_79B6B0();
	float angle = Unknown_BDB1C0.Get_Float() * 360.0f * (WWMATH_PI / 180.0f);
	Vector3 mid = (Unknown24 + Unknown30) * 0.5f;
	Matrix3D mid_lookat;
	mid_lookat.Obj_Look_At(mid, Unknown30, 0.0f);
	float s = sin(angle);
	float c = cos(angle);
	Unknown0C->Position = mid_lookat * Vector3(0.0f, c * 1.5f * distance, s * 1.5f * distance);
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();

	ship->Unknown98 = new Unknown718D70Class(Client->Unknown10BC, ship->Unknown_28(true), ship->Unknown98);
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7961A0-0x79730F: after camctrl_stargate.cpp's <string> initialiser 0x796180, ending with its own at
** 0x797300. Its own __FILE__ 0xB9E4BC (e:\build\beta\G\app\Client\camctrl_takeoff.cpp, FAIL_IFs l.106 / 190, G_ASSERT
** l.112; 0x796440 l.124, 0x796920 l.214, 218): the start and the stop have land's shape (effect 0x170 for five seconds).
*/
