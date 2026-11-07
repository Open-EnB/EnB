// Earth & Beyond client (reconstruction): app\client\unknown_798ae0.cpp, PROVISIONAL file name (D137; notes at the end)
#include "unknown_798ae0.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"

// 0x798AE0
CameraController00Class::CameraController00Class(SClient *client) :
	CameraControllerClass(client),
	Unknown0C(0),
	Unknown10(false)
{
}

// 0x798B40
CameraController00Class::~CameraController00Class()
{
}

// 0x798B60
void CameraController00Class::Unknown_04()
{
	Unknown0C = Client->Unknown10BC;
	Unknown10 = Client->Get_Avatar() != NULL;
	Unknown11 = (Flags & 1) != 0;
	Clear_Flag(0);
}

// 0x798BA0
void CameraController00Class::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	if (!Unknown11 && (Flags & 1)) {
		CameraController00Class::Unknown_04();
	}
	if (Unknown10) {
		if (!Client->Get_Avatar()) {
			Unknown10 = false;
			*mode = 0;
			return;
		}
		if ((unsigned)(Client->Unknown10BC - Unknown0C) > 15000 || Unknown11) {
			Client->CameraManager->Unknown_79B2B0(0x16);
		}
		*mode = 0;
		return;
	}
	if (Client->Get_Avatar()) {
		if (Unknown11) {
			Client->CameraManager->Unknown_79B2B0(0x16);
			*mode = 0;
			return;
		}
		int *state = mode;
		Unknown0C = Client->Unknown10BC;
		Unknown10 = true;
		*state = 0;
		return;
	}
	*mode = 0;
}

// 0x798CD0
void CameraController00Class::Unknown_0C()
{
}

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object 0x798AE0-0x798D0F: after the object ending with <string>'s initialiser 0x798AB0, ending with its own at
** 0x798CF0, before camera_manager.cpp's constructor 0x798D20. No __FILE__, assert or string: the name is PROVISIONAL
** (D137); by the link order it is one of the camctrl_*.cpp files after camctrl_tractor.cpp (0x797770), like the
** object 0x797990-0x798ACF before it. Its constructor is the camera manager's first new (state 0). 0x798BA0's
** last arms keep their own *mode stores only with the pointer in a local (VC6 otherwise tail-merges them).
*/
