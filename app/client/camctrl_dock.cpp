// Earth & Beyond client (reconstruction): app\client\camctrl_dock.cpp, the docking camera (state 6; notes at the end)
#include "camctrl_dock.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "rendobj.h"
#include "ini.h"
#include "random.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"
#include <string.h>
#include <ctype.h>

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1 [Unknown_4A1590]: neutral placeholder (D10)
extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

static float Unknown_B04AF0 = 0.4f;				// NAME 1: neutral (D10): dock.ini's Speed default
static float Unknown_B04AF4 = 1000.0f;			// NAME 1: neutral (D10): EnterLead / ExitLead default
static float Unknown_B04AF8 = 90.0f;			// NAME 1: neutral (D10): Angle default (degrees)
static float Unknown_B04AFC = 1.5f;				// NAME 1: neutral (D10): Radius default

// The 3x3 part of tm applied to v (0x792750's form: inline, rows built right to left, no translation)
inline Vector3 Unknown_Rotate(const Matrix3D &tm, const Vector3 &v)	// NAME 1: neutral (D10)
{
	return(Vector3(tm[0][0] * v.X + tm[0][1] * v.Y + tm[0][2] * v.Z, tm[1][0] * v.X + tm[1][1] * v.Y + tm[1][2] * v.Z, tm[2][0] * v.X + tm[2][1] * v.Y + tm[2][2] * v.Z));
}

// @0x792320
CameraController06Class::CameraController06Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(false),
	Unknown18(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x7924E0
CameraController06Class::~CameraController06Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x792560
void CameraController06Class::Unknown_04()
{
	Unknown_792750();
	Unknown10 = Client->Unknown10BC;
}

// @0x792590
void CameraController06Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	ClientObjectClass *ship = Client->Get_Avatar();
	bool done = true;
	if (Unknown14) {
		Unknown0C->Unknown_08(pos, rot);
		done = (float)(Client->Unknown10BC - Unknown10) > Unknown18;
	}
	if (done) {
		if (Get_Flag(0)) {
			Client->CameraManager->Unknown_79B2B0(0x0B);
			if (ship) {
				ship->Unknown100 = 3;
			}
			*mode = 2;
			return;
		}
		if (Get_Flag(1)) {
			Client->CameraManager->Unknown_79B2B0(0x13);
			*mode = 2;
			return;
		}
		*mode = 2;
		return;
	}
	*mode = 2;
}

// @0x7926B0
void CameraController06Class::Unknown_0C()
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
	Clear_Flag(0);
	Clear_Flag(1);
}

// @0x792750
// NOT EXACT: stack slots and register choices differ (same length, 0x760 bytes); functional
void CameraController06Class::Unknown_792750()
{
	ClientObjectClass *target = Client->Find_Object(Client->Unknown13AC);
	bool failed = false;



	FAIL_IF(!target) {
		failed = true;
	}
	ClientObjectClass *ship = Client->Get_Avatar();


	FAIL_IF(!ship) {
		failed = true;
	}
	if (failed) {
		if (Get_Flag(0)) {
			Client->CameraManager->Unknown_79B2B0(0x0B);
			if (ship) {
				ship->Unknown100 = 3;
			}
		} else if (Get_Flag(1)) {
			Client->CameraManager->Unknown_79B2B0(0x13);
		}
		return;
	}

	if (Get_Flag(0)) {
		ship->Unknown_7009A0(false, false);
		ship->Unknown100 = 3;
	} else if (Get_Flag(1)) {
		ship->Unknown_7009A0(true, true);
	}

	Vector3 end = target->Unknown_24();
	Vector3 start = ship->Unknown_24();
	float speed = Unknown_B04AF0;
	float scale = CameraManagerClass::Unknown_79B6B0();
	float lead = scale * 3.0f;
	RenderObjClass *robj = target->RenderObject;
	float angle = Unknown_B04AF8;
	float radius = Unknown_B04AFC;
	Unknown14 = false;
	if (robj) {
		const char *name = robj->Get_Name();
		static INIClass ini("dock.ini");
		char section[100];
		Unknown_4A1590(section, 100, name);
		for (int i = 0; i < (int)strlen(section); i++) {
			section[i] = toupper(section[i]);
		}
		Vector3 start_offset;
		start_offset.X = ini.Get_Float(section, "SX", 0.0f);
		start_offset.Y = ini.Get_Float(section, "SY", 0.0f);
		start_offset.Z = ini.Get_Float(section, "SZ", 0.0f);
		Vector3 end_offset;
		end_offset.X = ini.Get_Float(section, "EX", 0.0f);
		end_offset.Y = ini.Get_Float(section, "EY", 0.0f);
		end_offset.Z = ini.Get_Float(section, "EZ", 0.0f);
		angle = ini.Get_Float(section, "Angle", Unknown_B04AF8) * (WWMATH_PI / 180.0f);
		radius = ini.Get_Float(section, "Radius", Unknown_B04AFC);
		char bone[100];
		if (ini.Get_String(section, "Bone", "", bone, 100)) {
			Matrix3D tm = robj->Get_Bone_Transform(bone);
			char sub_name[100];
			if (ini.Get_String(section, "SubObject", "", sub_name, 100)) {
				RenderObjClass *sub = robj->Get_Sub_Object_By_Name(sub_name);
				if (sub) {
					tm = sub->Get_Bone_Transform(bone);
					sub->Release_Ref();
				}
			}
			Unknown14 = true;
			start_offset = Unknown_Rotate(tm, start_offset);
			end = tm.Get_Translation() + Unknown_Rotate(tm, end_offset);
		} else {
			Unknown14 = false;
		}
		start = end + start_offset;
		speed = ini.Get_Float(section, "Speed", Unknown_B04AF0);
		Unknown18 = ini.Get_Float(section, "Duration", 4.0f) * 1000.0f;
		if (Get_Flag(0)) {
			lead = ini.Get_Float(section, "EnterLead", Unknown_B04AF4);
		} else if (Get_Flag(1)) {
			Vector3 temp = start;
			start = end;
			end = temp;
			lead = ini.Get_Float(section, "ExitLead", Unknown_B04AF4);
		} else {













































			G_ASSERT(0);
		}
	}

	angle = Unknown_BDB1C0.Get_Float() * angle;
	if (Unknown_BDB1C0(0, 1) == 1) {
		angle = WWMATH_PI - angle;
	}
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle);
	Vector3 offset(lead, c * radius * scale, s * radius * scale);
	Matrix3D tm;
	tm.Obj_Look_At(start, end, 0.0f);
	Unknown0C->Position = tm * offset;
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();
	ship->Unknown98 = new Unknown718160Class(start, end, speed, Client->Unknown10BC, ship->Unknown98);
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x792310-0x7930CF: after the object ending with the atexit thunk 0x792304, ending with <string>'s
** initialiser 0x7930B0 (before camctrl_*.cpp's state-10 controller 0x7930E0). Its own __FILE__ 0xB9E248
** (e:\build\beta\G\app\Client\camctrl_dock.cpp, FAIL_IFs l.109 / 130 / 136, G_ASSERT l.258). The base's inline
** virtuals 0x7923C0-0x792460 are this object's COMDATs. The statics 0xB04AF0-0xB04AFC are dock.ini's defaults;
** the function-local INIClass 0xBE8670 (guard 0xBE8680, destructor thunk 0x793090).
** Flags are read afresh at each test ('test byte ptr [esi+8], 1', then 'mov; shr; test dl, 1'): only a volatile
** read gives that (camctrl.h's Get_Flag); a plain inline or a bitfield is CSE'd into one load.
** 0x792750 reads the rotation inline with rows built right to left (Unknown_Rotate; the library's Rotate_Vector
** member is a call) and adds the bone's translation to the end point (Get_Translation() + rotated).
*/
