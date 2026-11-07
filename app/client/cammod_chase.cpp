// Earth & Beyond client (reconstruction): app\client\cammod_chase.cpp (notes at the end)
#include "cammod_chase.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "..\shared\g_assert.h"

// 0x79BB80
ChaseCameraModuleClass::ChaseCameraModuleClass(SClient *client) :
	CameraModuleClass(client),
	Heading(0.0f),
	Distance(1.0f),
	Tilt(0.0f),
	Unknown14(true),
	ElevationOffset(0.0f),
	ObjectID(-1),
	Unknown34(false)
{
	Unknown24.Set(0.0f, 0.0f, 0.001f);
}




// 0x79BCE0
void ChaseCameraModuleClass::Unknown_04()
{
	Unknown34 = true;
	ClientObjectClass *obj = Client->Find_Object(ObjectID);
	FAIL_IF(!obj) {return;}
	if (Unknown1C) {
		obj->Unknown_6F3BF0(true);
	}
	CameraModuleClass::Unknown_04();
}



// 0x79BD50
void ChaseCameraModuleClass::Unknown_08(Vector3 *des_pos, Quaternion *des_quat)
{
	ClientObjectClass *obj = Client->Find_Object(ObjectID);
	FAIL_IF(!obj) {return;}
	Unknown_79BFC0(obj, des_quat, des_pos);
}

// 0x79BDC0
void ChaseCameraModuleClass::Unknown_0C()
{
	Unknown34 = false;
	if (Unknown1C) {
		ClientObjectClass *obj = Client->Find_Object(ObjectID);
		if (obj) {
			obj->Unknown_6F3BF0(false);
		}
	}
	CameraModuleClass::Unknown_0C();
}

// 0x79BE00
void ChaseCameraModuleClass::Set_Heading(float heading)
{
	if (Heading != heading) {
		Heading = heading;
	}
}

// 0x79BE30
void ChaseCameraModuleClass::Set_Distance(float distance)
{
	if (Distance != distance) {
		Distance = distance;
	}
}

// 0x79BE60
void ChaseCameraModuleClass::Set_Tilt(float tilt)
{
	if (Tilt != tilt) {
		Tilt = tilt;
	}
}

// 0x79BE90
void ChaseCameraModuleClass::Unknown_79BE90(bool arg)
{
	if (Unknown14 != arg) {
		Unknown14 = arg;
	}
}

// 0x79BEC0
void ChaseCameraModuleClass::Set_Elevation_Offset(float offset)
{
	if (ElevationOffset != offset) {
		ElevationOffset = offset;
	}
}

// 0x79BEF0
void ChaseCameraModuleClass::Unknown_79BEF0(bool hidden)
{
	if (Unknown1C != hidden) {
		Unknown1C = hidden;
		if (Unknown34) {
			ClientObjectClass *obj = Client->Find_Object(ObjectID);
			if (obj) {
				obj->Unknown_6F3BF0(Unknown1C);
			}
		}
	}
}

// 0x79BF40
void ChaseCameraModuleClass::Unknown_79BF40(int id)
{
	if (ObjectID != id) {
		if (Unknown1C && Unknown34) {
			ClientObjectClass *obj = Client->Find_Object(ObjectID);
			if (obj) {
				obj->Unknown_6F3BF0(false);
			}
		}
		ObjectID = id;
		if (Unknown1C && Unknown34) {
			ClientObjectClass *obj = Client->Find_Object(id);
			if (obj) {
				obj->Unknown_6F3BF0(true);
			}
		}
	}
}





#include "camera_manager.h"
#include "ww3d.h"
#include "wwmath.h"
#include "quat.h"
#include "vector2.h"

// 0x79BFC0
// NOT EXACT: inline budget (0.42: the client inlines Translate_Z and computes Normalize's Length2 twice; ours calls Translate_Z)
void ChaseCameraModuleClass::Unknown_79BFC0(ClientObjectClass *obj, Quaternion *des_quat, Vector3 *des_pos)
{
	FAIL_IF(!des_quat) {return;}
	FAIL_IF(!des_pos) {return;}
	FAIL_IF(!obj) {return;}
	Matrix3D tm = obj->Unknown_28(false);
	float target = obj->Unknown_6F3ED0() * 0.25f;
	Unknown24.Set_Target(target);
	Unknown24.Get_Value((float)WW3D::Get_Frame_Time());
	tm.Rotate_X(Unknown24.Get_Current());
	if (!Unknown14) {
		Vector2 dir(tm[0][0], tm[1][0]);
		dir.Normalize();
		Vector3 pos = tm.Get_Translation();
		tm.Set(dir.X, -dir.Y, 0.0f, pos.X, dir.Y, dir.X, 0.0f, pos.Y, 0.0f, 0.0f, 1.0f, pos.Z);
	}
	tm.Rotate_Y(-1.5707963705062866f);
	tm.Rotate_Z(-1.5707963705062866f);
	tm.Rotate_Y(Heading);
	tm.Rotate_X(-Tilt);
	tm.Translate_Z(CameraManagerClass::Unknown_79B6B0() * Distance);
	tm.Rotate_X(ElevationOffset);
	*des_pos = tm.Get_Translation();
	*des_quat = Build_Quaternion(tm);
	des_quat->Normalize();
}

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object: cammod_chase.cpp's __FILE__ 0xB9E658 (FAIL_IFs on lines 30 / 43 / 148-150). It starts at the constructor
** 0x79BB80, after cammod.cpp's <string> initialiser 0x79BB50, and ends with its own at 0x79C470. The base's
** destructor 0x79BC40 and deleting destructor 0x79BC60 (table 0xB054F8) are emitted here, before ours 0x79BC90 /
** 0x79BCC0. 0x79BFC0 (the chase placement, lines 148-150; WE3): the object's transform turned about X by the
** steered angle (Matrix3D::Rotate_X(float) and Rotate_Y(float) inlined in the client: sin before cos, one load of the
** angle), flattened onto XY when +0x14 is clear, then Rotate_Y(-pi/2), Rotate_Z(-pi/2), heading, tilt, the distance
** along Z and the elevation; CM1's draft read the first turn as Rotate_Z.
*/
