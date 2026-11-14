// Earth & Beyond client (reconstruction): gd_key_press.cpp. Notes at the end of the file.
#include "gd_key_press.h"
#include "fontmanager.h"
#include "..\input\inpkeys.h"
#include "..\shared\g_assert.h"

bool Unknown_42EC60(int button, int arg);							// 0x42EC60 (a key down: input.cpp's)  // NAME 1 [Unknown_42EC60, button, arg]: neutral (D10), as ctrngadg.cpp's

KeyPressGadget::KeyPressGadget() :
	GadgetClass(0xFFFF),
	Unknown44(0)
{
	Unknown_65FFA0(-1000);
	GadgetClass::Unknown_40(false);
}

KeyPressGadget::~KeyPressGadget()
{
}

bool KeyPressGadget::Unknown_30(float x, float y)
{
	return(Is_Enabled() ? true : false);
}

bool KeyPressGadget::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (mask & 0x100) {
		Event = *event;
		if (Event.Is_Up()) {
			Event.Flags ^= 0x1000;
			Unknown_40(false);
		} else {
			Unknown_66AB50();
		}
		event->Key = 0;
		event->Flags = 0;
		return(true);
	}
	if (Event.Key == 0) {
		Event.Flags = 0;
		if (Unknown_42EC60(0x10, 0)) Event.Flags |= 0x100;
		if (Unknown_42EC60(0x11, 0)) Event.Flags |= 0x200;
		if (Unknown_42EC60(0x12, 0)) Event.Flags |= 0x400;
	}
	if (Unknown44 && Event.Flags == 0 && Event.Key == 0) {
		Unknown_40(false);
		return(false);
	}
	Unknown_66AB50();
	Unknown44 = Event.Flags;
	return(false);
}




























































void KeyPressGadget::Unknown_40(bool on)
{
	if (on) {
		FAIL_IF(!Text && "KeyPressGadget::Set_Text() must be called first!") return;
		Event = KernelEventClass();
		Unknown44 = 0;
		Unknown_66AB50();
	} else if (Text) {
		Unknown_66AB50();
		Text->Unknown_6CE910(false);
		Text = NULL;
	}
	GadgetClass::Unknown_40(on);
}





void KeyPressGadget::Set_Text(const RefCountPtr<FontSys> &text)
{
	FAIL_IF(Is_Enabled()) return;
	Text = text;
}






void KeyPressGadget::Unknown_66AB50()
{
	FAIL_IF(!Text) return;
	Text->Unknown_6CE910(true);
	if (Event.Key == 0 && Event.Flags == 0) {
		Text->Change_Text("????", 0);
		return;
	}
	std::string name;
	Unknown_42B7E0(Event, name);
	Text->Change_Text(name.c_str(), 0);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gd_key_press.cpp (the client's own string: the FAIL_IFs
**	'!Text && "KeyPressGadget::Set_Text() must be called first!"' l.117, 'Is_Enabled()' l.135, '!Text' l.146)
**
**	The options' control keys page's key-press gadget (gd_key_press.h): 0x66A700-0x66ACD0, after
**	gd_inv_slider.cpp's deleting destructor 0x66A6D0 and before gd_pip_scene.cpp (0x66ACE0). Table 0xAFA548
**	(slots +0x2C 0x66A880, +0x30 0x66A860, +0x40 0x66A9C0 its own). The deleting destructor 0x66A7A0 sits between
**	the constructor and the destructor; the object ends with <string>'s guarded static 0x66ACB0 (guard 0xC416FC,
**	atexit 0x42B320), as every file including it. FAIL_IF 'Is_Enabled()' (l.135) is GadgetClass's slot +0x60
**	(gadget.h's Is_Enabled, named after this FAIL_IF): the call and the FAIL_IF's string agree. The asserts sit on the client's lines (D25); the blank lines keep them there.
**	0x42EC60 (input.cpp's: a key down) has no header; declared here as ctrngadg.cpp does.
*/
