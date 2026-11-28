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
#include "..\shared\auth_product.h"
extern Random4Class Unknown_BDB1C0;	// NAME 1: neutral (D10): the client's 0xBDB1C0 (seed 0x1105, initialiser 0x444240; its home file is unknown)
Matrix3D Unknown_82ECB0(const Vector3 &dir, const Vector3 &pos);	// 0x82ECB0 (a frame along dir at pos; its file is unknown)  // NAME 1 [dir, pos]: what the placements pass

// 0x7979A0
CameraController02Class::CameraController02Class(SpaceClientClass *client) :
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
			if (Get_Flag(1)) {
				Client->CameraManager->Unknown_79B2B0(5);
				Clear_Flag(1);
			} else if ((unsigned)Client->Unknown10BC > (unsigned)Unknown14) {
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
		case 3:
			if (Get_Flag(1)) {
				Client->CameraManager->Unknown_79B2B0(5);
				Clear_Flag(1);
			} else if ((unsigned)Client->Unknown10BC > (unsigned)Unknown14) {
				Unknown10 = 2;
				Unknown14 = Client->Unknown10BC + 5000;
				Unknown18 = Client->Unknown10BC;
				Unknown0C->Unknown_0C();
				Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_79BF40(Client->AvatarID);
				Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_04();
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

// NOT EXACT: 0x7981B0 (every instruction is the client's once stack offsets are masked, and every row of both matrix products sums (M2Z + M1Y) + M0X as the client; row 2 of frame * one loads M22 / M21 before one.Z / one.Y where the client loads the vector first, a commutative operand order only)
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
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle);
	Vector3 one(0.0f, c * range, s * range);
	s = WWMath::Sin(back);
	c = WWMath::Cos(back);
	Vector3 two(0.0f, c * range, s * range); one.X = two.X = range + range;
	one.Y *= 1.25f;
	one.Z *= 1.25f;
	two.Scale(Vector3(1.0f, 1.25f, 1.25f));

	Matrix3D frame = Unknown_82ECB0(dir, tm.Get_Translation());
	Vector3 first = frame * one;
	Vector3 second = frame * two;
	Vector3 camera = Client->CameraManager->Position;
	if ((first - camera).Length2() < (second - camera).Length2()) {
		((CameraModule79C8E0Class *)Unknown0C)->Position = first;
	} else {
		((CameraModule79C8E0Class *)Unknown0C)->Position = second;
	}
	((CameraModule79C8E0Class *)Unknown0C)->Unknown14 = true;
	((CameraModule79C8E0Class *)Unknown0C)->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();
}


// 0x798570
void CameraController02Class::Unknown_798570()
{
	float range = CameraManagerClass::Unknown_79B6B0();
	Matrix3D tm = Client->Get_Avatar()->Unknown_28(true);
	ClientObjectClass *dest = Client->Find_Object(((NavigationListClass &)Client->MainView->Radar->Unknown_7BD960()).Get_Current());
	float dist = 1000.0f;
	float angle;
	if (dest) {
		Vector3 pos = tm.Get_Translation();
		dist = (dest->Unknown_24() - pos).Length();
	}
	angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * 0.62831855f;
	if (Unknown_BDB1C0(0, 1)) {
		angle += 3.1415927f;
	}
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle);
	Vector3 offset(dist * 0.75f, c * range, s * range);
	offset.Scale(Vector3(1.0f, 2.0f, 2.0f));

	Matrix3D frame = Unknown_82ECB0(tm.Get_X_Vector(), tm.Get_Translation());
	((CameraModule79C8E0Class *)Unknown0C)->Position = frame * offset;
	((CameraModule79C8E0Class *)Unknown0C)->Unknown14 = true;
	((CameraModule79C8E0Class *)Unknown0C)->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();
}


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
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle);
	Vector3 offset(speed * 1000.0f, c * range, s * range);
	offset.Scale(Vector3(1.0f, 1.25f, 1.25f));

	Matrix3D frame = Unknown_82ECB0(dir, tm.Get_Translation());
	((CameraModule79C8E0Class *)Unknown0C)->Position = frame * offset;
	((CameraModule79C8E0Class *)Unknown0C)->Unknown14 = true;
	((CameraModule79C8E0Class *)Unknown0C)->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();

}

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object 0x7979A0-0x798ACF: after camctrl_tractor.cpp's (its <string> initialiser 0x797970), ending with its own at
** 0x798AB0. No __FILE__ or assert: the name is PROVISIONAL (D137). In the slot +0x08 0x797B50 phase 1 has its own copy
** of phase 3's body (the client keeps one copy, after phase 2's: its jump table sends 1 and 3 to 0x797BDC / 0x797BEB). The module's
** placement 0x7981B0 remains NOT EXACT; WWMath::Sin / Cos, matrix * vector and member reloads follow the client (P2).
** 0x798570 and 0x7987F0 exact: Vector3::Scale(Vector3(1,2,2)) / (1,1.25,1.25) gives the client's matrix product order.
*/
