// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gd_key_press.h (name assumed from gd_key_press.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_key_press.cpp'; the client holds no string of this header)
**
**	The gadget that takes one key press for the options' control keys page (ui_option_control_keys.cpp news it,
**	id 14): while it is on it holds the key pressed and shows its name in the button's text ("????" before one).
**	Table 0xAFA548 (29 slots: GadgetClass's, +0x2C / +0x30 / +0x40 its own), constructor 0x66A700, 0x48 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_KEY_PRESS_H  // NAME 3 [gd_key_press.h]: header name assumed from the client's gd_key_press.cpp
#define GD_KEY_PRESS_H

#include <string.h>
#include "ref_ptr.h"
#include "gadget.h"
#include "..\kernel\kernel.h"

class FontSys;

class KeyPressGadget : public GadgetClass  // NAME 5: the client's (FAIL_IF 'KeyPressGadget::Set_Text() must be called first!' l.117)
{
public:
	KeyPressGadget();									// 0x66A700 (mask 0xFFFF; sort -1000; off)
	virtual ~KeyPressGadget();							// +0x00 0x66A7D0 (deleting 0x66A7A0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66A880 (mask 0x100: the key taken, its release turns the gadget off; else Shift / Ctrl / Alt held)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_30(float x, float y);			// +0x30 0x66A860 (over it while it is on)  // NAME 1 [x, y]: GadgetClass's
	virtual void Unknown_40(bool on);					// +0x40 0x66A9C0 (FAIL_IF '!Text && ...' l.117: on clears the key)  // NAME 1 [on]: GadgetClass's

	void Set_Text(const RefCountPtr<FontSys> &text);	// 0x66AAD0 (FAIL_IF 'Is_Enabled()' l.135: the text the key is shown in)  // NAME 3: the client's FAIL_IF 'KeyPressGadget::Set_Text()'; NAME 2 [text]: the member it sets
	void Unknown_66AB50();								// 0x66AB50 (FAIL_IF '!Text' l.146: the key's name into the text, "????" for none)
	void Clear_Event() { memset(&Event, 0, sizeof(Event)); }	// (inline: ui_option_control_keys.cpp's Escape)  // NAME 1: what it does

	RefCountPtr<FontSys> Text;							// +0x30  // NAME 3: the client's FAIL_IF '!Text'
	KernelEventClass Event;								// +0x34 (the key taken)  // NAME 2: what it holds
	int Unknown44;										// +0x44 (the modifiers last shown)
};

#endif
