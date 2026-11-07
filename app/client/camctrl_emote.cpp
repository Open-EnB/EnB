// Earth & Beyond client (reconstruction): app\client\camctrl_emote.cpp, the emote camera (state 10; notes at the end)
#include "camctrl_emote.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "..\shared\g_assert.h"

// @0x7930E0
CameraController10Class::CameraController10Class(SClient *client) :
	CameraControllerClass(client)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x7931B0
CameraController10Class::~CameraController10Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x793230
void CameraController10Class::Unknown_04()
{
	Unknown_7932A0();
}

// @0x793250
void CameraController10Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown0C->Unknown_08(pos, rot);
	*mode = 1;
}

// @0x793280
void CameraController10Class::Unknown_0C()
{
	Unknown0C->Unknown_0C();
}

// @0x7932A0
void CameraController10Class::Unknown_7932A0()
{
	ClientObjectClass *ship = Client->Get_Avatar();














	FAIL_IF(!ship) {
		return;
	}
	float distance = CameraManagerClass::Unknown_79B6B0() * 4.0f;
	Vector3 pos = Client->CameraManager->Position;
	Vector3 ship_pos = ship->Unknown_24();
	Vector3 dir = ship_pos - pos;
	if (dir.Length2() < distance * distance) {
		dir.Normalize();
		pos = ship_pos - dir * distance;
	}
	Unknown0C->Position = pos;
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7930D0-0x7934DF: after camctrl_dock.cpp's <string> initialiser 0x7930B0, ending with its own at
** 0x7934C0. Its own __FILE__ 0xB9E2D8 (e:\build\beta\G\app\Client\camctrl_emote.cpp, FAIL_IF l.62). The client's
** function table runs 0x7932A0 to 0x79350F (623 bytes, over the two initialisers): appfile.py shows it as PREFIX,
** ours being the whole of the client's 0x7932A0-0x793445.
*/
