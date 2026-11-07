// Earth & Beyond client (reconstruction): app\client\unknown_7979a0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_7979a0.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"

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
** 0x798AB0. No __FILE__ or assert: the name is PROVISIONAL (D137). Not rebuilt: the slot +0x08 0x797B50 and the
** helpers 0x7981B0 / 0x798570 / 0x7987F0.
*/
