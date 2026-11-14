// Earth & Beyond client (reconstruction): gd_starmap.cpp. Notes at the end of the file.
#include "gd_starmap.h"
#include "starmap_pip.h"
#include "..\shared\g_assert.h"
#include "..\kernel\kernel.h"


















































GadgetStarmapClass::GadgetStarmapClass(StarmapPipClass *pip) :
	GadgetPipSceneClass(pip),
	StarmapPIP(pip)
{
	G_ASSERT(StarmapPIP);
}


GadgetStarmapClass::~GadgetStarmapClass()
{
	StarmapPIP = NULL;
}





bool GadgetStarmapClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	FAIL_IF(!StarmapPIP) return(false);
	if (Unknown_60()) {
		if (mask & 0x11) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			Unknown_68();
			if (!StarmapPIP->Unknown_7C1060()) {
				Set_Cursor((mask & 1) ? 0x18 : 0x16);
			}
			WrapPosition = Position;
			return(true);
		}
		if (((mask & 4) && Unknown_BE5F24 == this) || ((mask & 0x40) && Unknown_BE5F28 == this)) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			Unknown_6C();
			if (!WrapPointer) {
				if (mask & 4) {
					StarmapPIP->Unknown_7C0FA0();
				} else if (!StarmapPIP->Unknown_7C10D0()) {
					Unknown_7B4BD0();
				}
			} else {
				WrapPointer = false;
			}
			Set_Cursor(2);
			return(true);
		}
		if (((mask & 2) && Unknown_BE5F24 == this) || ((mask & 0x20) && Unknown_BE5F28 == this)) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			if (mask & 2) {
				Unknown_7B4C50();
			} else {
				Unknown_7B4D80();
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


































void GadgetStarmapClass::Unknown_7B4BD0()
{
	FAIL_IF(!StarmapPIP) return;
	Vector2 pos = StarmapPIP->Unknown_7C1200(Position);
	StarmapPIP->Unknown_7C0EA0(false);
	StarmapPIP->Unknown_7C1770(pos.X, pos.Y);
}


void GadgetStarmapClass::Unknown_7B4C50()
{
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			WrapPosition = Position;
			Position.X = delta.X + 0.5f;
			Position.Y = delta.Y + 0.5f;
			Unknown_7B4BD0();
			Position = WrapPosition;
		} else {
			WrapPointer = true;
			Set_Cursor(0x18);
		}
	}
}















void GadgetStarmapClass::Unknown_7B4D80()
{
	FAIL_IF(!StarmapPIP) return;
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			WrapPosition = Position;
			StarmapPIP->Unknown_7C1120(delta.Y);
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
**	Original location: e:\build\beta\G\app\client\gd_starmap.cpp (the client's own string: the G_ASSERT
**	'StarmapPIP' l.60, the FAIL_IFs '!StarmapPIP' l.75 / 153 / 193)
**
**	The star map PIP's gadget (gd_starmap.h): 0x7B4700-0x7B4E2C, table 0xB05F54 (slot +0x2C its own, +0x30
**	GadgetPipSceneClass's). 0x7B4BD0 (l.153: the map centred on the pointer) is inlined into slot +0x2C and
**	0x7B4C50; 0x7B4C50 (the first button's drag: the map panned by the pointer's move) has a copy inline in
**	slot +0x2C too. The asserts sit on the client's lines (D25); the blank lines keep them there. The statics
**	after the code (0x7B4E60: three ids from the gadget counter 0xBE3B44) are a header's, not this file's.
*/
