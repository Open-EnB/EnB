// Earth & Beyond client (reconstruction): gd_galaxy_map.cpp. Notes at the end of the file.
#include "gd_galaxy_map.h"
#include "galaxy_map_pip.h"
#include "..\shared\g_assert.h"
#include "..\kernel\kernel.h"

bool Unknown_42EC60(int button, int arg);							// 0x42EC60 (a key down: input.cpp's, as ctrngadg.cpp declares it)  // NAME 1 [button, arg]: neutral

















































GadgetGalaxyMapClass::GadgetGalaxyMapClass(GalaxyMapPipClass *pip) :
	GadgetPipSceneClass(pip),
	GalaxyMap(pip)
{
	G_ASSERT(GalaxyMap);
}


GadgetGalaxyMapClass::~GadgetGalaxyMapClass()
{
	GalaxyMap = NULL;
}





bool GadgetGalaxyMapClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	FAIL_IF(!GalaxyMap) return(false);
	if (Unknown_60()) {
		if (mask & 0x11) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			Unknown_68();
			if (!GalaxyMap->Unknown_53FE10()) {
				Set_Cursor((mask & 1) ? 0x18 : 0x16);
			}
			WrapPosition = Position;
			return(true);
		}
		if (((mask & 4) && Unknown_BE5F24 == this) || ((mask & 0x40) && Unknown_BE5F28 == this)) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			Unknown_6C();
			if (!WrapPointer) {
				GalaxyMap->Unknown_53FDA0((mask & 0x40) == 0x40);
			} else {
				WrapPointer = false;
			}
			Set_Cursor(2);
			return(true);
		}
		if (((mask & 2) && Unknown_BE5F24 == this) || ((mask & 0x20) && Unknown_BE5F28 == this)) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			if (Unknown_42EC60(0x11, 0)) {
				Unknown_6692E0();
			} else if (mask & 2) {
				Unknown_6693A0();
			} else {
				Unknown_669460();
			}
			return(true);
		}
		if ((mask & 8) && Unknown_BE5F24) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			return(true);
		}
	}
	return(false);
}


bool GadgetGalaxyMapClass::Unknown_30(float x, float y)
{
	return(GadgetPipSceneClass::Unknown_30(x, y));
}


































void GadgetGalaxyMapClass::Unknown_6692E0()
{
	FAIL_IF(!GalaxyMap) return;
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			WrapPosition = Position;
			GalaxyMap->Unknown_53FD60(delta);
		} else {
			WrapPointer = true;
			Set_Cursor(0x18);
		}
	}
}






void GadgetGalaxyMapClass::Unknown_6693A0()
{
	FAIL_IF(!GalaxyMap) return;
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			WrapPosition = Position;
			GalaxyMap->Unknown_53FD10(delta);
		} else {
			WrapPointer = true;
			Set_Cursor(0x18);
		}
	}
}






void GadgetGalaxyMapClass::Unknown_669460()
{
	FAIL_IF(!GalaxyMap) return;
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			WrapPosition = Position;
			GalaxyMap->Unknown_53FCA0(delta.Y);
			if (delta.Y > 0.0f) {
				Set_Cursor(0x16);
			} else {
				Set_Cursor(0x17);
			}
		} else {
			WrapPointer = true;
		}
	}
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gd_galaxy_map.cpp (the client's own string: the G_ASSERT
**	'GalaxyMap' l.61, the FAIL_IFs '!GalaxyMap' l.76 / 158 / 178 / 198)
**
**	The galaxy map PIP's gadget (gd_galaxy_map.h): 0x668E30-0x66955F, table 0xAFA120 (slots +0x2C and +0x30 its
**	own; +0x30 only forwards to GadgetPipSceneClass's). Slot +0x2C inlines 0x6692E0 (the drag with Ctrl held:
**	0x53FD60) and 0x6693A0 (the first button's drag: 0x53FD10) and calls 0x669460 (the second button's drag:
**	the zoom). The asserts sit on the client's lines (D25); the blank lines keep them there.
*/
