// Earth & Beyond client (reconstruction): gd_scut_proxy.cpp. Notes at the end of the file.
#include "gd_scut_proxy.h"
#include "intrface.h"
#include "ui_scut.h"
#include "sclient.h"


FullScreenGadgetClass::FullScreenGadgetClass(int arg) :
	InterfaceDeactivationGadgetClass(0, 0, 640, 480, arg)
{
	Set_Last_Sort_Group(true);
}


FullScreenGadgetClass::~FullScreenGadgetClass()
{
}


UIScutProxyGadgetClass::UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *robj, const LayerClass &layer, unsigned int mask, int bttn_idx) :
	RObjProxyGadgetClass(robj, layer, mask),
	Owner(owner),
	BttnIdx(bttn_idx)
{
}


UIScutProxyGadgetClass::~UIScutProxyGadgetClass()
{
}


bool UIScutProxyGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (RObjProxyGadgetClass::Unknown_2C(event, x, y, mask)) {
		if (mask & 1) {
			Owner->Owner->View->Unknown_44();
			Owner->Unknown_6124F0(this);
			return(true);
		}
		if (mask & 4) {
			Owner->Owner->View->Unknown_44();
			Owner->Unknown_6124F0(NULL);
		}
		return(true);
	}
	return(false);
}


void UIScutProxyGadgetClass::Set_Shortcut(GadgetClass *gadget)
{
	Owner->Unknown_611BD0(gadget, BttnIdx, CURRENT_GROUP, true);
}


void UIScutProxyGadgetClass::Unknown_66CB50()
{
	if (Target) {
		Target->Unknown_84((int)this);
	}
}


bool UIScutProxyGadgetClass::Can_Be_Shortcut(GadgetClass *gadget)
{
	if (!gadget) {
		return(false);
	}
	GadgetUnknown08Struct *robj_gadget = gadget->Unknown_08();
	if (!robj_gadget) {
		return(false);
	}
	if (gadget->Unknown_14()) {
		return(false);
	}
	return(robj_gadget->Unknown30 ? true : false);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137). The gadget files are linked in name order (gd_gauge.cpp
**	0x669570, gd_pip_scene.cpp, gd_proxy.cpp, gd_resize.cpp, gd_robj.cpp 0x66BDF0, gd_selct.cpp 0x66CC00,
**	gd_ship_equip_pip.cpp 0x66DA50), so this run's file name sorts between gd_robj.cpp and gd_selct.cpp; the two
**	classes may also be two files (no string in the run tells).
**
**	The run 0x66C8F0-0x66CBF0 (gd_scut_proxy.h): FullScreenGadgetClass 0x66C8F0 / 0x66C970 / 0x66C9A0 (table
**	0xAFAB64), UIScutProxyGadgetClass 0x66C9C0-0x66CBBB (table 0xAFAC78; slot +0x14's inline 0x66CA10 after the
**	constructor), then the headers' static initialiser 0x66CBD0 (atexit 0x42B320, as the other files' $E93).
**	0x66CB80's last test is `? true : false`: `!= NULL`, `(bool)`, `!!` and an inline bool helper give
**	xor eax, eax / test ecx, ecx / setne al where the client loads eax and tests it (attempts.py 0x66CB80).
*/
