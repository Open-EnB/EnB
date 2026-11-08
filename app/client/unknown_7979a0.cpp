// Earth & Beyond client (reconstruction): app\client\unknown_7979a0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7979a0.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "..\common\unknown_navlist.h"
#include "random.h"
#include "matrix3d.h"
#include <math.h>

extern Random4Class Unknown_BDB1C0;	// NAME 1: neutral (D10): the client's 0xBDB1C0 (seed 0x1105, initialiser 0x444240; its home file is unknown)
Matrix3D Unknown_82ECB0(const Vector3 &dir, const Vector3 &pos);	// 0x82ECB0 (a frame along dir at pos; its file is unknown)  // NAME 1 [dir, pos]: what the placements pass

// 0x7979A0
CameraController02Class::CameraController02Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(-1),
	Unknown18(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// 0x797A90
CameraController02Class::~CameraController02Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// 0x797B10
void CameraController02Class::Unknown_04()
{
	Unknown10 = 0;
	Unknown_7981B0();
	Unknown14 = Client->Unknown10BC + 2000;
}

// NOT EXACT: 0x797B50 (case 0 tests Flags bit 0 from the bit-1 test's register where the client reloads it (test byte ptr), and the switch's blocks for cases 3 / 4 come in another order)
// 0x797B50
void CameraController02Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	float range = CameraManagerClass::Unknown_79B6B0();
	switch (Unknown10) {
		case 0:
			if (Get_Flag(1)) {
				Client->CameraManager->Unknown_79B2B0(5);
				Clear_Flag(1);
			} else if ((Flags & 1) || (unsigned)Client->Unknown10BC > (unsigned)Unknown14) {
				Unknown10 = 1;
				Unknown14 = Client->Unknown10BC + 6000;
				Clear_Flag(0);
			}
			break;
		case 1:
			if ((Flags & 1)) {
				Clear_Flag(0);
			}
		case 3:
			if (Get_Flag(1)) {
				Client->CameraManager->Unknown_79B2B0(5);
				Clear_Flag(1);
				break;
			}
			if ((unsigned)Client->Unknown10BC > (unsigned)Unknown14) {
				Unknown10 = 2;
				Unknown14 = Client->Unknown10BC + 5000;
				Unknown18 = Client->Unknown10BC;
				Unknown0C->Unknown_0C();
				Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_79BF40(Client->AvatarID);
				Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_04();
			}
			break;
		case 2:
			if (Get_Flag(1)) {
				Client->CameraManager->Unknown_79B2B0(5);
				Clear_Flag(1);
			} else if ((unsigned)Client->Unknown10BC > (unsigned)Unknown14) {
				NavigationListClass &path = (NavigationListClass &)Client->MainView->Radar->Unknown_7BD960();
				if (path.Is_Last()) {
					ClientObjectClass *dest = Client->Find_Object(path.Get_Current());
					if (dest) {
						Vector3 dest_pos = dest->Unknown_24();
						float dist = (dest_pos - Client->Get_Avatar()->Unknown_24()).Length();
						dist = dist - (dest->Unknown_30() + range);
						float speed = Client->Get_Avatar()->Unknown_2C().Length();
						if (speed <= 0.001f) {
							speed = 0.001f;
						}
						if ((dist - 1000.0f) / speed < 2000.0f && (unsigned)(Client->Unknown10BC - Unknown18) > 3000) {
							Unknown10 = 4;
							Unknown14 = -1;
							Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_0C();
							Unknown_7987F0();
						}
					}
				} else {
					ClientObjectClass *dest = Client->Find_Object(path.Get_Current());
					if (dest) {
						Vector3 dest_pos = dest->Unknown_24();
						float dist = (dest_pos - Client->Get_Avatar()->Unknown_24()).Length();
						float speed = Client->Get_Avatar()->Unknown_2C().Length();
						if (speed <= 0.001f) {
							speed = 0.001f;
						}
						if (dist / speed < 2000.0f && (unsigned)(Client->Unknown10BC - Unknown18) > 3000) {
							Unknown10 = 3;
							Unknown14 = Client->Unknown10BC + 4000;
							Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_0C();
							Unknown_798570();
						}
					}
				}
			}
			break;
		case 4:
			if (Get_Flag(1)) {
				Unknown14 = Client->Unknown10BC + 2000;
				Clear_Flag(1);
			} else if ((unsigned)Client->Unknown10BC > (unsigned)Unknown14) {
				Client->CameraManager->Unknown_79B2B0(5);
			}
			break;
	}
	switch (Unknown10) {
		case 0:
			Unknown0C->Unknown_08(pos, rot);
			*mode = 1;
			break;
		case 2:
			Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_08(pos, rot);
			*mode = 2;
			break;
		case 1:
		case 3:
		case 4:
			Unknown0C->Unknown_08(pos, rot);
			*mode = 2;
			break;
	}
}

// 0x798130
void CameraController02Class::Unknown_0C()
{
	switch (Unknown10) {
		case 0:
			Unknown0C->Unknown_0C();
			break;
		case 1:
			Unknown0C->Unknown_0C();
			break;
		case 2:
			Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_0C();
			break;
		case 3:
			Unknown0C->Unknown_0C();
			break;
		case 4:
			Unknown0C->Unknown_0C();
			break;
	}
}

// NOT EXACT: 0x7981B0 (written from the pseudocode, functional; not matched form by form)
// 0x7981B0
void CameraController02Class::Unknown_7981B0()
{
	Matrix3D tm = Client->Get_Avatar()->Unknown_28(true);
	ClientObjectClass *dest = Client->Find_Object(((NavigationListClass &)Client->MainView->Radar->Unknown_7BD960()).Get_Current());
	Vector3 dir;
	if (dest) {
		Vector3 target = dest->Unknown_24();
		dir = target - tm.Get_Translation();
	} else {
		dir = tm.Get_X_Vector();
	}
	float angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * 0.62831855f;
	float back = angle + 3.1415927f;
	float range = CameraManagerClass::Unknown_79B6B0();
	float s1 = (float)sin(angle);
	float c1 = (float)cos(angle);
	float s2 = (float)sin(back);
	float c2 = (float)cos(back);
	Vector3 one(range + range, c1 * range, s1 * range);
	one.Y *= 1.25f;
	one.Z *= 1.25f;
	Vector3 two(range + range, c2 * range, s2 * range);
	two.Y *= 1.25f;
	two.Z *= 1.25f;
	Matrix3D frame = Unknown_82ECB0(dir, tm.Get_Translation());
	Vector3 first, second;
	frame.mulVector3(one, first);
	frame.mulVector3(two, second);
	const Vector3 &camera = Client->CameraManager->Position;
	CameraModule79C8E0Class *module = (CameraModule79C8E0Class *)Unknown0C;
	if ((first - camera).Length2() <= (second - camera).Length2()) {
		module->Position = first;
	} else {
		module->Position = second;
	}
	module->Unknown14 = true;
	module->Unknown18 = Client->AvatarID;
	module->Unknown_04();
}

// NOT EXACT: 0x798570 (written from the pseudocode, functional; not matched form by form)
// 0x798570
void CameraController02Class::Unknown_798570()
{
	float range = CameraManagerClass::Unknown_79B6B0();
	Matrix3D tm = Client->Get_Avatar()->Unknown_28(true);
	ClientObjectClass *dest = Client->Find_Object(((NavigationListClass &)Client->MainView->Radar->Unknown_7BD960()).Get_Current());
	float dist = 1000.0f;
	if (dest) {
		Vector3 target = dest->Unknown_24();
		dist = (target - tm.Get_Translation()).Length();
	}
	float angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * 0.62831855f;
	if (Unknown_BDB1C0(0, 1)) {
		angle += 3.1415927f;
	}
	float s = (float)sin(angle);
	float c = (float)cos(angle);
	Vector3 offset(dist * 0.75f, c * range, s * range);
	offset.Y += offset.Y;
	offset.Z += offset.Z;
	Matrix3D frame = Unknown_82ECB0(tm.Get_X_Vector(), tm.Get_Translation());
	CameraModule79C8E0Class *module = (CameraModule79C8E0Class *)Unknown0C;
	frame.mulVector3(offset, module->Position);
	module->Unknown14 = true;
	module->Unknown18 = Client->AvatarID;
	module->Unknown_04();
}

// NOT EXACT: 0x7987F0 (written from the pseudocode, functional; not matched form by form)
// 0x7987F0
void CameraController02Class::Unknown_7987F0()
{
	Matrix3D tm = Client->Get_Avatar()->Unknown_28(true);
	ClientObjectClass *dest = Client->Find_Object(((NavigationListClass &)Client->MainView->Radar->Unknown_7BD960()).Get_Current());
	Vector3 dir;
	if (dest) {
		Vector3 target = dest->Unknown_24();
		dir = target - tm.Get_Translation();
	} else {
		dir = tm.Get_X_Vector();
	}
	float angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * 0.62831855f;
	if (Unknown_BDB1C0(0, 1)) {
		angle += 3.1415927f;
	}
	float speed = Client->Get_Avatar()->Unknown_2C().Length();
	float range = CameraManagerClass::Unknown_79B6B0();
	float s = (float)sin(angle);
	float c = (float)cos(angle);
	Vector3 offset(speed * 1000.0f, c * range, s * range);
	offset.Y *= 1.25f;
	offset.Z *= 1.25f;
	Matrix3D frame = Unknown_82ECB0(dir, tm.Get_Translation());
	CameraModule79C8E0Class *module = (CameraModule79C8E0Class *)Unknown0C;
	frame.mulVector3(offset, module->Position);
	module->Unknown14 = true;
	module->Unknown18 = Client->AvatarID;
	module->Unknown_04();
}

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object 0x7979A0-0x798ACF: after camctrl_tractor.cpp's (its <string> initialiser 0x797970), ending with its own at
** 0x798AB0. No __FILE__ or assert: the name is PROVISIONAL (D137). The slot +0x08 0x797B50 NOT EXACT (CS1); the module's placements
** 0x7981B0 / 0x798570 / 0x7987F0 (Random4Class 0xBDB1C0 and 0x82ECB0) NOT EXACT from the pseudocode (CS1).
*/
