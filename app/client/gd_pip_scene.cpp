
// Earth & Beyond client (reconstruction): gd_pip_scene.cpp. Notes at the end of the file.
#include "gd_pip_scene.h"
#include "vector2.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "..\input\input.h"
#include "..\shared\g_assert.h"

// Where the pointer goes when it is wrapped: just inside the opposite edge of the window (0..1)
#define WRAP_NEAR 0.01f	// NAME 1: neutral (D10)
#define WRAP_FAR 0.99f	// NAME 1: neutral (D10)





























GadgetPipSceneClass::GadgetPipSceneClass(PipSceneClass *pip) :
	GadgetClass(0x1077),
	PIPScene(pip),
	Position(0.0f, 0.0f),
	WrapPosition(0.0f, 0.0f),
	WrapPointer(false),
	PointerInside(false)
{
	G_ASSERT(PIPScene);
}


GadgetPipSceneClass::~GadgetPipSceneClass()
{
	PIPScene = NULL;
}




bool GadgetPipSceneClass::Unknown_30(float x, float y)
{
	FAIL_IF(!PIPScene) return(false);
	if (!Unknown_60()) {
		return(false);
	}
	bool wrapped = false;
	PipSceneClass *pip = PIPScene;
	if (WrapPointer) {
		if (Wrap_Pointer()) {
			Unknown_42ECF0(&x, &y);
			wrapped = true;
		}
	}

	// The pointer on the 640 x 480 screen the scenes' rectangles are in, y up
	x = x * 640.0f - 320.0f;
	y = 240.0f - y * 480.0f;
	PointerInside = (x >= pip->Rect.Left && x <= pip->Rect.Right && y >= pip->Rect.Top && y <= pip->Rect.Bottom);
	if (PointerInside || WrapPointer) {
		x -= pip->Rect.Left;
		x /= pip->ViewportSize.X;
		y = pip->Rect.Bottom - y;
		Position.Set(x, y / pip->ViewportSize.Y);
		if (wrapped) {
			WrapPosition = Position;
		}
	}
	return(PointerInside);
}


bool GadgetPipSceneClass::Wrap_Pointer()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	if (x <= 0.0f) {
		Unknown_42ED20(WRAP_FAR, y);
		return(true);
	}
	if (x >= 1.0f) {
		Unknown_42ED20(WRAP_NEAR, y);
		return(true);
	}
	if (y <= 0.0f) {
		Unknown_42ED20(x, WRAP_FAR);
		return(true);
	}
	if (y >= 1.0f) {
		Unknown_42ED20(x, WRAP_NEAR);
		return(true);
	}
	return(false);
}




























void GadgetPipSceneClass::Set_Cursor(int cursor)
{
	FAIL_IF(!PIPScene) return;
	FAIL_IF(!PIPScene->Peek_Client()) return;

	LoginUnknown18Class *mouse = PIPScene->Peek_Client()->Mouse;
	FAIL_IF(!mouse) return;
	mouse->Unknown_515120(cursor);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_pip_scene.cpp (the client's own string: the asserts 'PIPScene'
**	l.50, '!PIPScene' l.64 / l.147, '!PIPScene->Peek_Client()' l.148, '!mouse' l.151)
**
**	The gadget over a picture-in-picture scene (gd_pip_scene.h): 0x66ACE0-0x66B230, table 0xAFA65C. The next
**	function, 0x66B240, is gd_proxy.cpp's first.
*/
