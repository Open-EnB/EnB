// Earth & Beyond client (reconstruction): app\client\unknown_79c8e0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7979a0.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "sphere.h"
#include "matrix3d.h"
#include "quat.h"

// 0x79C8E0
CameraModule79C8E0Class::CameraModule79C8E0Class(SClient *client) :
	CameraModuleClass(client),
	Position(0.0f, 0.0f, 0.0f),
	Unknown14(true),
	Unknown18(0)
{
}

// 0x79C970
// NOT EXACT: shape (18 masked rows, was 184: the first product after each Inv_Sqrt comes out fld st(0); fmul mem where the client has fld mem; fmul st(1), and two stores schedule apart; attempts.csv)
void CameraModule79C8E0Class::Unknown_08(Vector3 *des_pos, Quaternion *des_quat)
{
	ClientObjectClass *obj = Client->Find_Object(Unknown18);
	if (obj) {
		Vector3 pos = Position;
		SphereClass sphere;
		obj->Unknown_6FEC40(&sphere);
		Vector3 dir = Normalize(sphere.Center - pos);
		Vector3 up;
		if (Unknown14) {
			up.Set(0.0f, 0.0f, 1.0f);
		} else {
			Matrix3D tm = obj->Unknown_28(true);
			tm.Set_Translation(Vector3(0.0f, 0.0f, 0.0f));
			Vector3 x(tm[0][0], tm[1][0], tm[2][0]);
			x.Normalize();
			up = Vector3::Cross_Product(dir, x);
			if (up.Z < 0.0f) {
				up = -up;
			}
			up.Normalize();
			if (WWMath::Fabs(Vector3::Dot_Product(up, dir)) == 1.0) {
				if (WWMath::Fabs(up.Z) < 0.001f) {
					up.Z += 1.0f;
				} else {
					up.X += 1.0f;
				}
				up.Normalize();
			}
		}
		Vector3 right = Normalize(Vector3::Cross_Product(dir, up));
		Vector3 cam_up = Normalize(Vector3::Cross_Product(right, dir));
		Matrix3D cam(right, cam_up, -dir, Position);
		*des_pos = cam.Get_Translation();
		*des_quat = Build_Quaternion(cam);
	}
}

/*
** Reconstruction notes (WE3, 2026-11-07)
**
** The object: no __FILE__ or assert; client 0x79C8E0-0x79D0DF, after unknown_79c4a0.cpp's <string> initialiser
** 0x79C8B0 and ending with its own at 0x79D0C0. The module (table 0xB05630, 0x1C bytes) looks from +0x08 at the bounding-sphere centre of the object
** +0x18: up is world Z when +0x14 is set, else the side of the object's X axis facing up. Its destructor 0x79C950
** and deleting destructor 0x79C920.
** NF (2026-11-15): right and cam_up are the free Normalize of a by-value Cross_Product (the client pushes the stored len2
** and writes new slots), up the by-value Cross_Product (a temporary, then copied), the matrix the four-vector constructor
** with -dir as a temporary and Position read again from the member; the angle test compares with the double 1.0.
*/
