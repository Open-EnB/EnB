// Earth & Beyond client (reconstruction): app\client\unknown_7979a0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7979a0.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "..\common\unknown_navlist.h"

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
		case 1:
		case 3:
		case 4:
			Unknown0C->Unknown_08(pos, rot);
			*mode = 2;
			break;
		case 2:
			Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_08(pos, rot);
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

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object 0x7979A0-0x798ACF: after camctrl_tractor.cpp's (its <string> initialiser 0x797970), ending with its own at
** 0x798AB0. No __FILE__ or assert: the name is PROVISIONAL (D137). The slot +0x08 0x797B50 NOT EXACT (CS1); not rebuilt: the
** helpers 0x7981B0 / 0x798570 / 0x7987F0 (the module's placements by Random4Class and 0x82ECB0).
*/
