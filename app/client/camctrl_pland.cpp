// Earth & Beyond client (reconstruction): app\client\camctrl_pland.cpp, the planet-landing camera (state 7; notes at the end)
#include "camctrl_pland.h"
#include "eff_o.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "..\shared\g_assert.h"

// @0x7946D0
CameraController07Class::CameraController07Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x7947C0
CameraController07Class::~CameraController07Class()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x794840
void CameraController07Class::Unknown_04()
{
	Unknown_794A90();
	Unknown10 = Client->Unknown10BC;
	Unknown14 = Client->Next_Local_ID();
	EffectObjectClass *effect = Unknown_6E9490(0x16A, Unknown14, 4000, Unknown10, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);


































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

// @0x794970
void CameraController07Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	Unknown0C->Unknown_08(pos, rot);
	if (Client->Unknown10BC - Unknown10 > 4000) {
		Client->CameraManager->Unknown_79B2B0(0x0E);
		*mode = 2;
		return;
	}
	*mode = 2;
}

// @0x7949F0
void CameraController07Class::Unknown_0C()
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

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7946C0-0x79539F: after the state-1 controller's <string> initialiser 0x7946A0, ending with its own at 0x795380. Its own __FILE__
** 0xB9E3B8 (e:\build\beta\G\app\Client\camctrl_pland.cpp, FAIL_IFs l.71 / 100 / 119 / 122 / 125, G_ASSERT l.77): the
** same shape as camctrl_land.cpp with 0x79C8E0's module and request 14. Not rebuilt: 0x794A90 (landingpattern.ini's
** section by the planet, SX / SY / SZ, EX / EY / EZ, SpeedForward, SpeedDown; the land's height field; the ship's
** spline).
*/
