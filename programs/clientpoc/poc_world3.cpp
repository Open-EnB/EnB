// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins for the PDA's windows (D179). NOT THE CLIENT'S CODE.
/*
**	pdaint.cpp's set-up (0x693230) news the equipment (0x5D10C0), cargo (0x5CF9B0) and vault (0x656FF0) windows
**	and the experience display (0x58C450), whose classes the tree does not have yet; it calls each window's slot
**	+0x54 at once and the display's slots every frame, so the generated stand-ins (no table) would crash: these give
**	each the real size's zeroed memory and InterfaceClass's own table (no mesh, no keys). Delete one here when its
**	real class lands in app/.
*/

#define __INTRFACE_MEMBERS
#define POC_WINDOW_CTOR
#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\pdaint.h"
#include "..\..\app\client\sclient.h"

// ---------------------------------------------------------------- the equipment window (0x5D10C0): 0x154 bytes
Unknown5D10C0Class::Unknown5D10C0Class(SClient *client) : PDAInventoryWindowClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC Unknown5D10C0Class (the PDA's equipment window): an empty InterfaceClass");
	memset(Unknown84, 0, sizeof(Unknown84));
}

// ---------------------------------------------------------------- the cargo window (0x5CF9B0): 0x10C bytes
Unknown5CF9B0Class::Unknown5CF9B0Class(SClient *client, PDAInterfaceClass *pda) : PDAInventoryWindowClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC Unknown5CF9B0Class (the PDA's cargo window): an empty InterfaceClass");
	memset(Unknown84, 0, sizeof(Unknown84));
}

// ---------------------------------------------------------------- the vault window (0x656FF0): 0xFC bytes
Unknown656FF0Class::Unknown656FF0Class(SClient *client) : PDAInventoryWindowClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC Unknown656FF0Class (the PDA's vault window): an empty InterfaceClass");
	memset(Unknown84, 0, sizeof(Unknown84));
}

// ---------------------------------------------------------------- 0x5A0450 (ui_inv.cpp): no items
void PDAInventoryWindowClass::Unknown_5A0450(int type, int *num_items, int *num_slots)
{
	STUB_CALLED("POC PDAInventoryWindowClass::Unknown_5A0450 (0x5A0450): no items");
	*num_items = 0;
	*num_slots = 0;
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
