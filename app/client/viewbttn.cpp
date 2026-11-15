// Earth & Beyond client (reconstruction): app\client\viewbttn.cpp (__FILE__ 0xB91D5C). Notes at the end of the file.
#include <stddef.h>
#include "viewbttn.h"
#include "radar_system.h"
#include "sclient.h"
#include "ui_cpit.h"
#include "targeting.h"
#include "option.h"
#include "tasks\tsk_login.h"
#include "..\packets\pkturn.h"
#include "..\packets\pktilt.h"
#include "..\packets\pkmove.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"
bool Unknown_42EC60(int button, int arg);							// 0x42EC60 (a key down: input.cpp's)  // NAME 1 [button, arg]: neutral
inline bool Unknown_Cpit_Moving(MainViewClass *view) { UICpitClass *cpit = view->Cpit; if (cpit != NULL) return(cpit->Unknown144 != 0); return(false); }	// (inline: the bool the client tests with setne)  // NAME 1: neutral (D10)
// NOT EXACT: the client keeps an EH state around the new UnknownAFE0B8Class (a byte state store before Get_Bool, attempts.csv); ours has none
ViewButtonClass::ViewButtonClass(int x, int y, int width, int height, MainViewClass *view) :
	RectangleGadgetClass(x, y, width, height, 0xFF),
	View(view),
	ThrottleRightMouse(true),
	Unknown49(false),
	Unknown4A(false),
	Unknown4C(NULL),
	Unknown50(0),
	Unknown54(0),
	Unknown58(1.0f)
{
	Unknown4C = new UnknownAFE0B8Class;
	ThrottleRightMouse = OptionClass::Instance->Get_Bool("Cpit_ThrottleRightMouse");
}

ViewButtonClass::~ViewButtonClass()
{
	BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Cpit_ThrottleRightMouse");
	if (option) option->Set_Value(ThrottleRightMouse);
	OptionClass::Instance->Save();
	delete Unknown4C;
}
























































































































// NOT EXACT: register allocation only (explain.py; attempts.csv)
bool ViewButtonClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!(Mask & mask)) return(false);
	SClient *sclient = View->Client;
	LoginUnknown18Class *mouse = sclient->Mouse;
	int shift = Unknown_42EC60(0x10, 0);				// VK_SHIFT
	unsigned avatar = sclient->AvatarID;
	if (mouse->DragObject) return(false);
	GadgetClass::Unknown_2C(event, x, y, mask);
	if ((mask & 1) || (mask & 0x10)) {
		if (!Unknown_68()) return(false);
	} else if (mask & 0x44) {
		Unknown_6C();
	}
	ClientObjectClass *hover = View->Radar->Hover;
	bool turn = (!shift && ((mask & 0x20) || ((mask & 0x10) && hover == NULL)));
	bool lock_speed = false;
	bool lock_orient = false;
	Unknown_57D6C0(sclient, &lock_speed, &lock_orient);
	if (!turn || (lock_orient && lock_speed)) {
		mouse->Unknown_515120(2);
	} else {
		Unknown4A = true;
		if (!lock_orient && Unknown4C->Unknown_00(sclient->Unknown10BC, x, y)) {
			float turn_intensity = (x - 0.5f) + (x - 0.5f);
			float tilt_intensity = (y - 0.5f) * -2.0f;
			sclient->Send_Packet(&TurnPacketClass(avatar, turn_intensity));
			sclient->Send_Packet(&TiltPacketClass(avatar, tilt_intensity));
			sclient->Unknown_745D50(turn_intensity, tilt_intensity);
		}
		if (!lock_speed) {
			if (!ThrottleRightMouse || Unknown_Cpit_Moving(View)) {
				Unknown49 = false;
			} else if (x > 0.25f && x < 0.75 && y > 0.2f && y < 0.8) {
				if (!Unknown49) {
					sclient->Send_Packet(&MovePacketClass(sclient->AvatarID, 0));
					sclient->Unknown_745E40(1, false);
					Unknown49 = true;
				}
			} else if (Unknown49) {
				sclient->Send_Packet(&MovePacketClass(sclient->AvatarID, 4));
				sclient->Unknown_745E40(0, false);
				Unknown49 = false;
			}
		}
		int cursor = (int)(x * 7.0f);
		if (cursor < 0) {
			cursor = 0;
		} else if (cursor > 6) {
			cursor = 6;
		}
		if (Unknown49) {
			mouse->Unknown_515120(cursor + 15);
		} else {
			mouse->Unknown_515120(cursor + 8);
		}
	}
	if (!turn && Unknown4A) {
		Unknown4C->Unknown_04();
		sclient->Send_Packet(&TurnPacketClass(avatar, 0.0f));
		sclient->Send_Packet(&TiltPacketClass(avatar, 0.0f));
		sclient->Unknown_745DD0();
		Unknown4A = false;
		if (Unknown49) {
			if (ThrottleRightMouse && !Unknown_Cpit_Moving(View) && !lock_speed) {
				sclient->Send_Packet(&MovePacketClass(sclient->AvatarID, 4));
				sclient->Unknown_745E40(0, false);
			}
			Unknown49 = false;
		}
	}
	if (hover != NULL) {
		if (hover->GameID != sclient->AvatarID) {
			mouse->Unknown_515120(2);
			if (mask & 1) {
				TargetingClass *targeting = sclient->Targeting;
				FAIL_IF(!targeting) return(true);
				targeting->Select_And_Set_Filter(hover);
				SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
			}
		} else {
			mouse->Unknown_515120(2);
			if (mask & 1) {
				sclient->Unknown_744D80();
			}
		}
	}
	return(true);
}

bool ViewButtonClass::Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask)
{
	bool result = GadgetClass::Unknown_34(event, x, y, mask);
	if (!result && (mask & 0x44)) {
		Unknown_2C(event, x, y, mask & 0x44);
		return(false);
	}
	return(result);
}

bool ViewButtonClass::Unknown_6B5E90()
{
	if (ThrottleRightMouse) {
		UICpitClass *cpit = View->Cpit;
		if (cpit != NULL) return(cpit->Unknown144 != 0);
		return(false);
	}
	return(true);
}





















































































































bool UnknownAFE0B8Class::Unknown_00(unsigned time, float x, float y)
{
	G_ASSERT(x >= 0.0f && x <= 1.0f);
	G_ASSERT(y >= 0.0f && y <= 1.0f);
	int column = (int)(x / Unknown0C);
	int row = (int)(y / Unknown10);
	if (column == Unknown04 && row == Unknown08) return(false);
	Unknown04 = column;
	Unknown08 = row;
	return(true);
}

void UnknownAFE0B8Class::Unknown_04()
{
	Unknown04 = -1;
	Unknown08 = -1;
}
// NOT EXACT: the client's EH frame around the new (attempts.csv); ours has none
UnknownAFE0B8Class *Unknown_6B5FD0()
{
	return(new UnknownAFE0B8Class);
}


/*
**	Notes (reconstruction)
**
**	The client's viewbttn.obj from 0x6B56A0 to 0x6B6050 at least (the rectangle gadget 0x6B6050, table 0xAFE174, follows:
**	not placed here). __FILE__ is 'e:\build\beta\G\app\client\viewbttn.cpp' (0xB91D5C):
**	FAIL_IF '!targeting' on line 237, G_ASSERTs 'x >= 0.0f && x <= 1.0f' / 'y >= 0.0f && y <= 1.0f' on 389 / 390.
**	The option 'Cpit_ThrottleRightMouse' (0xB91D40) is read by the constructor and saved by the destructor.
**	Emitted here as well: GadgetClass's one-line slots and Unknown_18 0x6B57B0 (inline), the deleting destructor
**	0x6B57D0. 0x6B5E90 and 0x6B5FD0 have no caller in the client.
**
**	Forms: slot +0x2C compares x with 0.75 and y with 0.8 as doubles (qword constants) and 0.25 / 0.2 as floats; the
**	cockpit test is an inline with two returns (the client's setne / test); the hover branch tests 'not the avatar'
**	first; the cursor calls are one per branch (the client pushes each and shares the call); shift is an int
**	(and eax, 0xFF). The new UnknownAFE0B8Class carries an EH state in the client (the constructor's byte state
**	store, 0x6B5FD0's frame) that ours lacks: tried the constructor inline in the class, inline after its callers,
**	out of line after them, assignments in its body, Vector2i / Vector2 members, a pure base with and without a
**	constructor, a class operator new / delete and a template factory; none gives the frame.
**	The client's object ends at 0x6B602D with no guarded std::ctype<unsigned short>::id initialiser: the original
**	file never saw <string>. Ours does (sclient.h through client.h, option.h, soundlib.h), and its initialiser
**	makes view.cpp's tail (0x6B55D0, 0x6B5620) ambiguous in the verify.
*/
