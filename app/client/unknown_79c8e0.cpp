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
// NOT EXACT: shape (0.52: the client's Normalize of the direction stores the difference first, the cross products load in another order)
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
			Vector3::Cross_Product(dir, x, &up);
			if (up.Z < 0.0f) {
				up = -up;
			}
			up.Normalize();
			if (WWMath::Fabs(Vector3::Dot_Product(up, dir)) == 1.0f) {
				if (WWMath::Fabs(up.Z) < 0.001f) {
					up.Z += 1.0f;
				} else {
					up.X += 1.0f;
				}
				up.Normalize();
			}
		}
		Vector3 right;
		Vector3::Cross_Product(dir, up, &right);
		right.Normalize();
		Vector3 cam_up;
		Vector3::Cross_Product(right, dir, &cam_up);
		cam_up.Normalize();
		Matrix3D cam(
			right.X, cam_up.X, -dir.X, pos.X,
			right.Y, cam_up.Y, -dir.Y, pos.Y,
			right.Z, cam_up.Z, -dir.Z, pos.Z);
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
*/
