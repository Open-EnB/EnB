// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_proxy.h (name assumed from gd_proxy.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_proxy.cpp'; the client holds no string of this header)
**
**	The proxy buttons: a button gadget (unknown_675120.h's ButtonGadgetClass) that stands for another model gadget,
**	its target: the pointer over it marks the target, its events go to the target. Two classes in gd_proxy.cpp
**	(0x66B240-0x66B770), both with 44 slots (RObjGadgetClass's 41 and three of their own):
**	  ProxyGadgetClass      table 0xAFA6E8, constructor 0x66B240, 0x74 bytes (ui_cpit.cpp's 0x650A40 makes one)
**	  RObjProxyGadgetClass  table 0xAFA7BC, constructor 0x66B4C0, 0x78 bytes: shows the target's model on itself.
**	The shortcut bar's proxy button (gd_scut_proxy.h's UIScutProxyGadgetClass, table 0xAFAC78, constructor
**	0x66C9C0) derives from the second: its table has that class's 0x66B510 / 0x66B5F0 / 0x66B690.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_PROXY_H  // NAME 3 [gd_proxy.h]: header name assumed from the client's gd_proxy.cpp
#define GD_PROXY_H

#include "unknown_675120.h"

class ProxyGadgetClass : public ButtonGadgetClass  // NAME 3: class-from-file (D24) for app\Client\gd_proxy.cpp
{
public:
	ProxyGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask);	// 0x66B240 (ButtonGadgetClass 0x675240 with flags 3)  // NAME 1 [robj, layer, mask]: the base's
	virtual ~ProxyGadgetClass();						// +0x00 0x66B2E0 (deleting 0x66B2B0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66B390 (the target's slot +0x2C, or the base's without one)  // NAME 1 [event, x, y, mask]: the base's
	virtual bool Unknown_30(float x, float y);			// +0x30 0x66B320 (the base's, the answer into the target's bit 2; false without a target while the model is hidden)  // NAME 1 [x, y]: the base's
	virtual void Set_Target(RObjGadgetClass *target);	// +0xA4 0x66B300  // NAME 2: the member it sets; NAME 1 [target]: neutral
	virtual ProxyGadgetClass *Unknown_A8() { return(this); }	// +0xA8 0x66B290 (itself; inline: emitted after the constructor)
	virtual int Get_Proxy_Id();							// +0xAC 0x66B400 (its own id; acticon.cpp's 0x6762B0 overrides the same slot of unknown_671300.h's class)  // NAME 2: what it returns

	void Save_Sticky(bool on);							// 0x66B420 (G_ASSERT '!StickySave' l.107: the gadget the first button holds kept; on: the target, or this one without a target, put there, else none; ui_scut.cpp's 0x6120E0)  // NAME 2: after the assert's StickySave; NAME 1 [on]: neutral
	void Restore_Sticky();								// 0x66B4A0 (the kept one back)  // NAME 2: after the assert's StickySave

	RObjGadgetClass *Target;							// +0x64 (the gadget it stands for: the shortcut a bar's button shows)  // NAME 1: neutral (D10): what the slots do with it
	unsigned int ProxyMask;								// +0x68 (the constructor's mask, back into GadgetClass's when the target goes)  // NAME 1: what 0x66B690 does with it
	bool Pressed;										// +0x6C (ui_scut.cpp's 0x6120E0: a key holds the button down)  // NAME 1: what ui_scut.cpp does with it
	GadgetClass *StickySave;							// +0x70 (G_ASSERT '!StickySave')  // NAME 3: the assert's
};

class RObjProxyGadgetClass : public ProxyGadgetClass  // NAME 1: neutral (D10): the proxy that shows its target's model
{
public:
	RObjProxyGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask);	// 0x66B4C0 (the base's constructor inline)  // NAME 1 [robj, layer, mask]: the base's
	virtual ~RObjProxyGadgetClass();					// +0x00 0x66B560 (deleting 0x66B530)
	virtual RObjProxyGadgetClass *As_RObjProxyGadget() { return(this); }		// +0x0C 0x66B510 (inline; itself: GadgetClass's slots +0x08 to +0x1C answer 'are you a ...' with the gadget or NULL)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x66B5F0 (the target's, at this model's position)  // NAME 1 [position, frame, indicator]: the base's
	virtual void Set_Target(RObjGadgetClass *target);	// +0xA4 0x66B690 (the old target's model off this one, the new target's top model onto it, its mask taken)  // NAME 1 [target]: the base's

	RenderObjClass *TargetModel;						// +0x74 (the target's model's topmost container, referenced, shown on this model)  // NAME 1: what 0x66B690 stores
};

#endif
