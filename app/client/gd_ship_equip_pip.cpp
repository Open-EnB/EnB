// Earth & Beyond client (reconstruction): gd_ship_equip_pip.cpp. Notes at the end of the file.
#include "gd_ship_equip_pip.h"
#include "ship_equip_pip.h"
#include "..\shared\g_assert.h"
#include "..\kernel\kernel.h"

GadgetShipEquipPipClass::GadgetShipEquipPipClass(ShipEquipPipClass *pip) :
	GadgetPipSceneClass(pip),
	ShipPIP(pip),
	Point(0.0f, 0.0f)
{
}


GadgetShipEquipPipClass::~GadgetShipEquipPipClass()
{
}


bool GadgetShipEquipPipClass::Unknown_30(float x, float y)
{
	Point.X = x;
	Point.Y = y;
	return(GadgetPipSceneClass::Unknown_30(x, y));
}

































bool GadgetShipEquipPipClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	FAIL_IF(!ShipPIP) return(false);
	if (Unknown_60()) {
		if (mask & 0x11) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			Unknown_68();
			if (mask & 1) {
				Set_Cursor(0x18);
			} else {
				Set_Cursor(0x16);
			}
			WrapPosition = Position;
			return(true);
		}
		if (((mask & 4) && Unknown_BE5F24 == this) || ((mask & 0x40) && Unknown_BE5F28 == this)) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			Unknown_6C();
			Set_Cursor(2);
			WrapPointer = false;
			return(true);
		}
		if (((mask & 2) && Unknown_BE5F24 == this) || ((mask & 0x20) && Unknown_BE5F28 == this)) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			if (mask & 2) {
				Unknown_66DE10();
			} else {
				Unknown_66DED0();
			}
			return(true);
		}
		if ((mask & 8) && Unknown_BE5F24) {
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			return(true);
		}




















		if (mask & 0x1000) {
			switch (event->Key) {
				case 0x101:
					ShipPIP->Unknown_542EE0(0.05f);
					break;
				case 0x102:
					ShipPIP->Unknown_542EE0(-0.05f);
					break;
				default:
					G_ASSERT(false);
					return(false);
			}
			GadgetPipSceneClass::Unknown_2C(event, x, y, mask);
			return(true);
		}
	}
	return(false);
}








void GadgetShipEquipPipClass::Unknown_66DE10()
{
	FAIL_IF(!ShipPIP) return;
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			ShipPIP->Unknown_542A10(Point, false);
			WrapPosition = Position;
		} else {
			ShipPIP->Unknown_542A10(Point, true);
			WrapPointer = true;
		}
	}
}





void GadgetShipEquipPipClass::Unknown_66DED0()
{
	FAIL_IF(!ShipPIP) return;
	Vector2 delta = WrapPosition - Position;
	if (delta.X != 0.0f || delta.Y != 0.0f) {
		if (WrapPointer) {
			WrapPosition = Position;
			ShipPIP->Unknown_542EE0(-delta.Y);
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
**	Original location: e:\build\beta\G\app\Client\gd_ship_equip_pip.cpp (the client's own string: the FAIL_IFs
**	'!ShipPIP' l.61 / 142 / 161 and the G_ASSERT 'false' l.123)
**
**	The ship PIP's gadget (gd_ship_equip_pip.h): 0x66DA50-0x66DF82, table 0xAFAE64; gd_slider.cpp's GadgetSlider
**	follows at 0x66DFC0. 0x66DE10 (the first button's drag) has no caller: slot +0x2C inlines it. The FAIL_IFs and the
**	assert sit on the client's lines (D25); the blank lines keep them there. The pointer's move is a Vector2
**	(WrapPosition - Position) tested member by member; the cursor and the zoom are passed per branch (the client
**	pushes each constant in its own branch before one shared call).
*/
