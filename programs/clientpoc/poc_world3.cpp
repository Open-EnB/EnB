// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins for the PDA's windows (D179). NOT THE CLIENT'S CODE.
/*
**	pdaint.cpp's set-up (0x693230) news the equipment (ui_pda_equip.cpp), cargo (ui_pda_cargo.cpp) and vault (0x656FF0)
**	windows and the experience display (0x58C450). The vault window and the display have no class in the tree yet: it
**	calls each window's slot +0x54 at once and the display's slots every frame, so the generated stand-ins (no table)
**	would crash: these give each the real size's zeroed memory and InterfaceClass's own table (no mesh, no keys).
**	The cargo and equipment windows are app.lib's; what they make that the tree does not have yet (the combo box's
**	second constructor 0x5789E0, gd_inv_slider.cpp's slider, the ship's PIP 0x541DE0, its gadget 0x66DA50 and the
**	stats display 0x6A2ED0) is stood in for below. Delete one here when its real class lands in app/.
*/

#define __INTRFACE_MEMBERS
#define POC_WINDOW_CTOR
#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\pdaint.h"
#include "..\..\app\client\sclient.h"
#include "..\..\app\client\ui_combobox.h"
#include "..\..\app\client\gd_inv_slider.h"
#include "..\..\app\client\unknown_675120.h"
#include "rendobj.h"

// ---------------------------------------------------------------- the vault window (0x656FF0): 0xFC bytes
Unknown656FF0Class::Unknown656FF0Class(SClient *client) : UIInvClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC Unknown656FF0Class (the PDA's vault window): an empty InterfaceClass");
	memset(UnknownD0, 0, sizeof(UnknownD0));
}

// ---------------------------------------------------------------- the experience display (0x58C450): 0x28 bytes, nothing shown
Unknown58C450Class::Unknown58C450Class(RenderObjClass *mesh, InterfaceClass *parent)
{
	STUB_CALLED("POC Unknown58C450Class (the PDA's experience display): nothing shown");
	Unknown04 = 0;
	memset(Unknown08, 0, sizeof(Unknown08));
}
Unknown58C450Class::~Unknown58C450Class()
{
}
void Unknown58C450Class::Unknown_04()
{
}
void Unknown58C450Class::Unknown_08(int value)
{
	Unknown04 = value;
}
void Unknown58C450Class::Unknown_0C()
{
}
void Unknown58C450Class::Unknown_10()
{
}
void Unknown58C450Class::Unknown_58CF60()
{
}

// ---------------------------------------------------------------- the cargo window's filter: the combo box's second constructor (0x5789E0)
// An empty box: no drop-down, the text field left alone (+0x10C set); the arrow button (+0xD0, 0x57B570's) a hidden
// button on the 'filter' sub-object, else on the model itself
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, SClient *client, RenderObjClass *model, const char *name, int a, int b, int c, int d, const char *style) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIComboBoxClass 0x5789E0 (the cargo window's filter): an empty box");
	memset((char *)this + sizeof(InterfaceClass), 0, sizeof(UIComboBoxClass) - sizeof(InterfaceClass));
	Unknown10C = 1;
	RenderObjClass *robj = (model && name) ? model->Get_Sub_Object_By_Name(name) : NULL;
	if (robj == NULL && model) {
		robj = model;
		robj->Add_Ref();
	}
	Unknown0D0 = robj ? new ButtonGadgetClass(robj, 0x55, 3) : NULL;
	if (robj) robj->Release_Ref();
}
void UIComboBoxClass::Unknown_57B480(const char *text)	// 0x57B480: the box's +0x104 string (nothing shown here)
{
}

// ---------------------------------------------------------------- gd_inv_slider.cpp's slider (0x669FB0): a plain GadgetSlider
Unknown669FB0Class::Unknown669FB0Class(UIInvClass *inventory, LayerClass *layer) : GadgetSlider(false, layer), Inventory(inventory), SlotData(NULL), Unknown130(0), Unknown134(0), Unknown138(0)
{
	STUB_CALLED("POC Unknown669FB0Class (the inventory slider): a plain slider");
}
Unknown669FB0Class::~Unknown669FB0Class()
{
}
void Unknown669FB0Class::Unknown_A4(float val, bool arg2)
{
	GadgetSlider::Unknown_A4(val, arg2);
}
void Unknown669FB0Class::Unknown_66A300(int a, int b)
{
	Unknown130 = a;
	Unknown134 = b;
}

// ---------------------------------------------------------------- the equipment window's ship PIP (0x541DE0), its gadget (0x66DA50), the stats (0x6A2ED0)
Unknown541DE0Class::Unknown541DE0Class(SClient *client) : PipSceneClass(client)
{
	STUB_CALLED("POC Unknown541DE0Class (the equipment window's ship PIP): an empty scene");
	memset(Unknown34, 0, sizeof(Unknown34));
}
void Unknown541DE0Class::Unknown_542250(const RefCountPtr<RenderObjClass> &robj)
{
}
Unknown66DA50Class::Unknown66DA50Class(Unknown541DE0Class *pip) : GadgetClass(0)
{
	STUB_CALLED("POC Unknown66DA50Class (the ship PIP's gadget): never hit");
}
Unknown66DA50Class::~Unknown66DA50Class()
{
}
bool Unknown66DA50Class::Unknown_30(float x, float y)
{
	return(false);
}
Unknown6A2ED0Class::Unknown6A2ED0Class(SClient *client, RefCountPtr<RenderObjClass> &mesh)
{
	STUB_CALLED("POC Unknown6A2ED0Class (the equipment window's stats): nothing shown");
	memset(Unknown00, 0, sizeof(Unknown00));
}
Unknown6A2ED0Class::~Unknown6A2ED0Class()
{
}
void Unknown6A2ED0Class::Unknown_6A34E0(bool on)
{
}
void Unknown6A2ED0Class::Unknown_6A3C10()
{
}
