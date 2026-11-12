// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins for the PDA's windows (D179). NOT THE CLIENT'S CODE.
/*
**	pdaint.cpp's set-up (0x693230) news the equipment (ui_pda_equip.cpp), cargo (ui_pda_cargo.cpp) and vault (0x656FF0)
**	windows and the experience display (0x58C450, ui_experience.cpp's, in app.lib). The vault window has no class in the
**	tree yet: it calls each window's slot +0x54 at once, so the generated stand-in (no table) would crash: this gives it
**	the real size's zeroed memory and InterfaceClass's own table (no mesh, no keys).
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

// ---------------------------------------------------------------- the equipment window's ship PIP (0x541DE0; its gadget is gd_ship_equip_pip.cpp's), the stats (0x6A2ED0)
Unknown541DE0Class::Unknown541DE0Class(SClient *client) : PipSceneClass(client)
{
	STUB_CALLED("POC Unknown541DE0Class (the equipment window's ship PIP): an empty scene");
	memset(Unknown34, 0, sizeof(Unknown34));
}
void Unknown541DE0Class::Unknown_542250(const RefCountPtr<RenderObjClass> &robj)
{
}
void Unknown541DE0Class::Unknown_542A10(const Vector2 &point, bool on)	// 0x542A10: the drag (gd_ship_equip_pip.cpp's gadget)
{
}
void Unknown541DE0Class::Unknown_542EE0(float zoom)	// 0x542EE0: the zoom (gd_ship_equip_pip.cpp's gadget)
{
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

