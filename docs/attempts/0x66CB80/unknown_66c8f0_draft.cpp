// Earth & Beyond client (reconstruction): the run 0x66C8F0-0x66CBF0 between gd_robj.cpp and gd_selct.cpp (DRAFT, not in
// the tree: stashed by L12, run 2026-11-11). Two classes: a full-screen rectangle gadget in the last sort group
// (table 0xAFAB64, no caller in the client) and the shortcut bar's proxy button (table 0xAFAC78; ui_scut.cpp's
// UIScutProxyGadgetClass, over gd_proxy.h's RObjProxyGadgetClass).
#include "gd_proxy.h"
#include "intrface.h"
#include "ui_scut.h"
#include "sclient.h"

// A gadget over the whole 640 x 480 screen, sorted last (0x40 bytes, table 0xAFAB64)
class Unknown66C8F0Class : public RectangleGadgetClass	// NAME 1: neutral (D10)
{
public:
	Unknown66C8F0Class(int arg);						// 0x66C8F0  // NAME 1 [arg]: the base's fifth
	virtual ~Unknown66C8F0Class();						// +0x00 0x66C9A0 (deleting 0x66C970)
};

// The shortcut bar's proxy button (0x80 bytes, table 0xAFAC78)
class UIScutProxyGadgetClass : public RObjProxyGadgetClass	// NAME 1: neutral (D10): ui_scut.cpp's name for it
{
public:
	UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *robj, const LayerClass &layer, unsigned int mask, int index);	// 0x66C9C0
	virtual ~UIScutProxyGadgetClass();					// +0x00 0x66CA60 (deleting 0x66CA30)
	virtual int Unknown_14() { return((int)this); }		// +0x14 0x66CA10 (itself: 'are you a shortcut button')
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66CA80
	void Unknown_66CB20(GadgetClass *gadget);			// 0x66CB20 (ui_scut.cpp's Create_Shortcut 0x614360: the gadget at this button)
	void Unknown_66CB50();								// 0x66CB50 (every frame: the target's slot +0x84)
	static bool Unknown_66CB80(GadgetClass *gadget);	// 0x66CB80 (no caller: a model gadget with a model that is no shortcut button)

	UIScutBarClass *Owner;								// +0x78 (G_ASSERT 'ActiveShortcut->Owner == this' l.904 of ui_scut.cpp)
	int Index;											// +0x7C (the button)
};

// @0x66C8F0
Unknown66C8F0Class::Unknown66C8F0Class(int arg) :
	RectangleGadgetClass(0, 0, 640, 480, arg)
{
	Set_Last_Sort_Group(true);
}

// @0x66C9A0
Unknown66C8F0Class::~Unknown66C8F0Class()
{
}

// @0x66C9C0
UIScutProxyGadgetClass::UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *robj, const LayerClass &layer, unsigned int mask, int index) :
	RObjProxyGadgetClass(robj, layer, mask),
	Owner(owner),
	Index(index)
{
}

// @0x66CA60
UIScutProxyGadgetClass::~UIScutProxyGadgetClass()
{
}

// @0x66CA80
bool UIScutProxyGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (RObjProxyGadgetClass::Unknown_2C(event, x, y, mask)) {
		if (mask & 1) {
			((InterfaceClass *)Owner->Unknown04)->View->Unknown_44();
			Owner->Unknown_6124F0(this);
			return(true);
		}
		if (mask & 4) {
			((InterfaceClass *)Owner->Unknown04)->View->Unknown_44();
			Owner->Unknown_6124F0(NULL);
		}
		return(true);
	}
	return(false);
}

// @0x66CB20
void UIScutProxyGadgetClass::Unknown_66CB20(GadgetClass *gadget)
{
	Owner->Unknown_611BD0(gadget, Index, CURRENT_GROUP, true);
}

// @0x66CB50
void UIScutProxyGadgetClass::Unknown_66CB50()
{
	if (Target) {
		Target->Unknown_84((int)this);
	}
}

// @0x66CB80
bool UIScutProxyGadgetClass::Unknown_66CB80(GadgetClass *gadget)
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
	return(robj_gadget->Unknown30 != NULL);	// NOT EXACT (0.78): the client has mov eax / test / setne al, this form xor eax first; (bool) cast, implicit conversion and an inline helper give the same
}
