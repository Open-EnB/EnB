// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: the client holds no string of this file; notes in gd_scut_proxy.cpp)
**
**	Two gadget classes of the run 0x66C8F0-0x66CBF0, between gd_robj.cpp and gd_selct.cpp:
**	  FullScreenGadgetClass   table 0xAFAB64, constructor 0x66C8F0, 0x40 bytes: a rectangle gadget over the whole
**	                          640 x 480 screen in the last sort group (no caller in the client)
**	  UIScutProxyGadgetClass  table 0xAFAC78, constructor 0x66C9C0, 0x80 bytes: the shortcut bar's proxy button
**	                          (ui_scut.cpp makes one a button, 'bttn_%d_%02d'), over gd_proxy.h's RObjProxyGadgetClass:
**	                          its table has that class's 0x66B510 / 0x66B5F0 / 0x66B690.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_SCUT_PROXY_H  // NAME 2 [gd_scut_proxy.h]: the link order puts the file between gd_robj.cpp and gd_selct.cpp; what it holds
#define GD_SCUT_PROXY_H

#include "gd_proxy.h"

class UIScutBarClass;

// A gadget over the whole screen, sorted last (0x40 bytes, table 0xAFAB64)
class FullScreenGadgetClass : public RectangleGadgetClass  // NAME 2: what it covers
{
public:
	FullScreenGadgetClass(int arg);										// 0x66C8F0 (0, 0, 640, 480 and the argument to the base 0x660D30, then Set_Last_Sort_Group)  // NAME 1 [arg]: the base's fifth
	virtual ~FullScreenGadgetClass();									// +0x00 0x66C9A0 (deleting 0x66C970)
};

// The shortcut bar's proxy button: shows the shortcut (an RObjGadgetClass) at its place on the bar (0x80 bytes, table 0xAFAC78)
class UIScutProxyGadgetClass : public RObjProxyGadgetClass  // NAME 1: neutral (D10): after ui_scut.cpp, its maker
{
public:
	UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *robj, const LayerClass &layer, unsigned int mask, int bttn_idx);	// 0x66C9C0 (ui_scut.cpp's 0x610F80: the bar, its 'bttn_%d_%02d' model, the layer, 0x55, the button)  // NAME 3 [owner]: the assert's 'Owner'; NAME 1 [robj, layer, mask]: the base's; NAME 3 [bttn_idx]: 0x611BD0's asserts
	virtual ~UIScutProxyGadgetClass();									// +0x00 0x66CA60 (deleting 0x66CA30)
	virtual UIScutProxyGadgetClass *As_UIScutProxyGadget() { return(this); }						// +0x14 0x66CA10 (inline; itself: 'are you a shortcut button')
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66CA80 (the base's; bit 0 makes this the bar's active shortcut, bit 2 clears it)  // NAME 1 [event, x, y, mask]: the base's

	void Set_Shortcut(GadgetClass *gadget);								// 0x66CB20 (ui_scut_gadgets.cpp's Create_Shortcut 0x614360: the gadget onto the bar at this button, 0x611BD0)  // NAME 2: what it does; NAME 1 [gadget]: what 0x614360 passes
	void Unknown_66CB50();												// 0x66CB50 (ui_scut.cpp's 0x612430, every frame: the target's slot +0x84 with this button)
	static bool Can_Be_Shortcut(GadgetClass *gadget);					// 0x66CB80 (no caller: a model gadget with a model that is no shortcut button)  // NAME 2: what it answers; NAME 1 [gadget]: neutral

	UIScutBarClass *Owner;												// +0x78 (G_ASSERT 'ActiveShortcut->Owner == this' l.904 of ui_scut.cpp)  // NAME 3: the assert's
	int BttnIdx;														// +0x7C (the button on the bar: 0x66CB20 passes it as 0x611BD0's bttn_idx)  // NAME 3: 0x611BD0's asserts' 'bttn_idx'
};

#endif
