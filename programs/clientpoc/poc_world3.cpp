// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins for the PDA's windows (D179). NOT THE CLIENT'S CODE.
/*
**	pdaint.cpp's set-up (0x693230) news the equipment (ui_pda_equip.cpp), cargo (ui_pda_cargo.cpp) and vault (0x656FF0)
**	windows and the experience display (0x58C450). The vault window and the display have no class in the tree yet: it
**	calls each window's slot +0x54 at once and the display's slots every frame, so the generated stand-ins (no table)
**	would crash: these give each the real size's zeroed memory and InterfaceClass's own table (no mesh, no keys).
**	The cargo and equipment windows are app.lib's; what they make that the tree does not have yet (the combo box's
**	second constructor 0x5789E0, the ship's PIP 0x541DE0, its gadget 0x66DA50 and the
**	stats display 0x6A2ED0) is stood in for below. Delete one here when its real class lands in app/.
*/

#define __INTRFACE_MEMBERS
#define POC_WINDOW_CTOR
#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\pdaint.h"
#include "..\..\app\client\sclient.h"
#include "..\..\app\client\ui_combobox.h"
#include "..\..\app\client\unknown_675120.h"
#include "rendobj.h"
#include "nullrobj.h"

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
// An empty box: no drop-down, the text field left alone (+0x10C set); the arrow button (+0xD0, 0x57B570's) a button on a
// null object of its own ('filter' is a bone of the window's mesh: a button on the mesh would hide the whole window)
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, SClient *client, RenderObjClass *model, const char *name, int a, int b, int c, int d, const char *style) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIComboBoxClass 0x5789E0 (the cargo window's filter): an empty box");
	memset((char *)this + sizeof(InterfaceClass), 0, sizeof(UIComboBoxClass) - sizeof(InterfaceClass));
	Unknown10C = 1;
	RenderObjClass *robj = new Null3DObjClass;
	Unknown0D0 = new ButtonGadgetClass(robj, 0x55, 3);
	robj->Release_Ref();
}
void UIComboBoxClass::Unknown_57B480(const char *text)	// 0x57B480: the box's +0x104 string (nothing shown here)
{
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

// ---------------------------------------------------------------- dynamic_window_move.cpp's 0x51A9C0: the window's move gadget, here one that is never hit
class UIDynamicWindowClass;
class PocNoHitGadgetClass : public GadgetClass
{
public:
	PocNoHitGadgetClass() : GadgetClass(0) {}
	virtual bool Unknown_30(float x, float y) { return(false); }
};
GadgetClass *Unknown_51A9C0(UIDynamicWindowClass *window)
{
	STUB_CALLED("POC Unknown_51A9C0 (0x51A9C0, the window's move gadget): never hit");
	return(new PocNoHitGadgetClass);
}

// ---------------------------------------------------------------- the inventory's quantity gadget (0x66CC00; inv_transaction.cpp's 0x6908E0 news it): never hit
// declared as inv_transaction.cpp declares it
class Unknown66CC00Class : public GadgetClass { public: Unknown66CC00Class(UIViewClass *view, int arg); virtual bool Unknown_30(float x, float y); char Unknown30[0x10]; bool Unknown40; int Unknown44; int Unknown48; char Unknown4C[0x6C - 0x4C]; };
Unknown66CC00Class::Unknown66CC00Class(UIViewClass *view, int arg) : GadgetClass(0)
{
	STUB_CALLED("POC Unknown66CC00Class (0x66CC00, the inventory's quantity gadget): never hit");
	memset(Unknown30, 0, sizeof(Unknown30));
	Unknown40 = false;
	Unknown44 = 0;
	Unknown48 = 0;
	memset(Unknown4C, 0, sizeof(Unknown4C));
}
bool Unknown66CC00Class::Unknown_30(float x, float y)
{
	return(false);
}
