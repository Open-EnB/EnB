// Earth & Beyond client (reconstruction): app\client\unknown_794510.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_794510.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"

// @0x794510
CameraController01Class::CameraController01Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(true)
{
}

// @0x794570
CameraController01Class::~CameraController01Class()
{
}

// @0x794590
void CameraController01Class::Unknown_04()
{
	Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_79BF40(Client->AvatarID);
	Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_04();
	Unknown0C = true;
}

// @0x7945E0
void CameraController01Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_08(pos, rot);
	if (!Unknown0C && Client->CameraMode) {
		*mode = 1;
	} else {
		*mode = 2;
		Unknown0C = false;
	}
	*target = Client->AvatarID;
}

// @0x794670
void CameraController01Class::Unknown_0C()
{
	Client->CameraManager->Get_Player_Chase_Camera_Module()->Unknown_0C();
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x794510-0x7946BF: after camctrl_land.cpp's (0x793910-0x7944FF), ending with <string>'s initialiser
** 0x7946A0, before the state-7 controller's 0x7946D0. No __FILE__, assert or string: the name is PROVISIONAL (D137).
** The update stores the client's AvatarID through its fourth argument (camctrl.h's 'int *target').
*/
