// Earth & Beyond client (reconstruction): the POC's bridge from the app files' local views of GadgetClass to the real one (D179). NOT THE CLIENT'S CODE.
/*
**	ui_scut.cpp and ui_manu.cpp declare their own GadgetClass with neutral slots. Their derived classes' tables are
**	therefore built from ?Unknown_NN@GadgetClass@@... names no real function has, and gen_stubs' stand-ins of them
**	return with the wrong stack (the real slot +0x54 takes two arguments). poc_gadget_views*.cpp define those names
**	as jumps into ButtonGadgetClass's real table, which this file provides. UIScutButtonGadgetClass is 0x675240.
*/
#include <string.h>
#include "..\..\stubs\stub_log.h"
#define __INTRFACE_MEMBERS
#include "..\..\app\client\sclient.h"
#include "..\..\app\client\gadget.h"
#include "..\..\app\client\unknown_675120.h"
#include "layer.h"

extern "C" void **POC_Button_Table()
{
	static void **table = NULL;
	if (table == NULL) {
		STUB_CALLED("POC GadgetClass views: forwarding to ButtonGadgetClass's table");
		table = *(void ***)new ButtonGadgetClass((RenderObjClass *)NULL, LayerClass(), 0x55, 4);
	}
	return(table);
}

class UIScutButtonGadgetClass : public ButtonGadgetClass
{
public:
	UIScutButtonGadgetClass(RenderObjClass *object, LayerClass *layer, int unknown1, int unknown2);
	~UIScutButtonGadgetClass();
	virtual void Unknown_40(bool on);
};
UIScutButtonGadgetClass::UIScutButtonGadgetClass(RenderObjClass *object, LayerClass *layer, int unknown1, int unknown2) : ButtonGadgetClass(object, *layer, unknown1, unknown2)
{
}
UIScutButtonGadgetClass::~UIScutButtonGadgetClass()
{
}

void UIScutButtonGadgetClass::Unknown_40(bool on)
{
	ButtonGadgetClass::Unknown_40(on);
}

// ---------------------------------------------------------------- gd_proxy.cpp's proxy button (0x66C9C0, 0x80 bytes): a plain button, what it shows empty
class UIScutBarClass;
class UIScutProxyGadgetClass : public ButtonGadgetClass
{
public:
	UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *object, LayerClass *layer, int unknown, int index);
	// +0xA4, the first slot past RObjGadgetClass's table (ui_scut.cpp's 0x611BD0 calls it for every shortcut it shows:
	// without it the slot read past ButtonGadgetClass's table and called 0): only the shortcut kept
	virtual void Unknown_A4(GadgetClass *gadget) { STUB_CALLED("POC UIScutProxyGadgetClass::Unknown_A4: the shortcut kept, not shown"); Unknown64 = gadget; }
	GadgetClass *Unknown64; char Unknown68[0x6C - 0x68]; bool Unknown6C; char Unknown6D[0x78 - 0x6D];
	UIScutBarClass *Owner; int Unknown7C;
};
UIScutProxyGadgetClass::UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *object, LayerClass *layer, int unknown, int index) :
	ButtonGadgetClass(object, *layer, unknown, 3), Unknown64(NULL), Unknown6C(false), Owner(owner), Unknown7C(index)
{
	STUB_CALLED("POC UIScutProxyGadgetClass: a plain button");
	memset(Unknown68, 0, sizeof(Unknown68));
	memset(Unknown6D, 0, sizeof(Unknown6D));
}
