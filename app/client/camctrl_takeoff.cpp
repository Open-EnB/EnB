// Earth & Beyond client (reconstruction): app\client\camctrl_takeoff.cpp, the take-off camera (state 9; notes at the end)
#include "camctrl_takeoff.h"
#include "eff_o.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "..\shared\g_assert.h"

// @0x7961B0
CameraController09Class::CameraController09Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x796280
CameraController09Class::~CameraController09Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x796300
void CameraController09Class::Unknown_04()
{
	Unknown_796920();
	Unknown5C = false;
	Unknown10 = Client->Unknown10BC;
	Unknown14 = Client->Next_Local_ID();
	EffectObjectClass *effect = Unknown_6E9490(0x170, Unknown14, 5000, Unknown10, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);




































































	FAIL_IF(!effect) return;

	UnknownB00060EffectClass *source = effect->Unknown_10();
	if (source) {
		source->Unknown_28(Client->AvatarID);
	} else {
		G_ASSERT(0);
	}
	MainViewClass *view = Client->MainView;
	Client->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
}

// @0x796850
void CameraController09Class::Unknown_0C()
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
	EffectObjectClass *effect = Client->Get_Effect(Unknown14);
	if (effect) {
		effect->Unknown_6E6E30();
	}
}

// @0x7968F0
bool CameraController09Class::Unknown_14()
{
	return(Client->Unknown10BC - Unknown10 > 2000);
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7961A0-0x79730F: after camctrl_stargate.cpp's <string> initialiser 0x796180, ending with its own at
** 0x797300. Its own __FILE__ 0xB9E4BC (e:\build\beta\G\app\Client\camctrl_takeoff.cpp, FAIL_IFs l.106 / 190, G_ASSERT
** l.112): the start and the stop have land's shape (effect 0x170 for five seconds). Not rebuilt: the update 0x796440
** and 0x796920 (the module, the ship's path).
*/
