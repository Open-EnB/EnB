// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins under the PDA's windows (D179). NOT THE CLIENT'S CODE.
/*
**	The PDA's set-up (pdaint.cpp's 0x693230) runs the real set-ups of the character window (ui_cinfo.cpp: its tabs
**	ui_attrb_skills.cpp, ui_faction.cpp) and of the star map window (ui_starmap.cpp). These are the classes they
**	new whose constructors the tree does not have yet: the generated stand-ins leave the memory as it is and no
**	table, so the first list insert or virtual call crashes. Each gets its base constructed, its members zeroed
**	and a table whose own slots are generated stand-ins. Delete one here when its real class lands in app/.
*/

#define __INTRFACE_MEMBERS
#include <string.h>
#include <string>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\sclient.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\app\client\gadget.h"
#include "..\..\app\client\gd_robj.h"
#include "..\..\app\client\ui_attrb_skills.h"
#include "..\..\app\client\ui_dynamicwindow.h"
#include "..\..\app\client\dynamic_window_class.h"
#include "..\..\app\client\ui_starmap.h"
#include "..\..\app\client\starmap_pip_support.h"
#include "..\..\app\client\galaxy_map_pip.h"
#include "..\..\ww3d2\ww3d.h"

// ---------------------------------------------------------------- gd_starmap.cpp's gadget (0x7B4700, starmap_pip.h): 0x50 bytes
GadgetStarmapClass::GadgetStarmapClass(StarmapPipClass *pip) : GadgetPipSceneClass(pip), StarmapPIP(pip)
{
	STUB_CALLED("POC GadgetStarmapClass: the PIP only");
}

// ---------------------------------------------------------------- gd_galaxy_map.cpp's gadget (0x668E30, galaxy_map_pip.h): 0x50 bytes
// As the client's 0x668E30: the base, GalaxyMap (+0x4C), its table. The generated constructor left no table, and
// MapPipClass::Unknown_04's ViewportGadget->Unknown_40 crashed the second PIP the star map window opens.
GadgetGalaxyMapClass::GadgetGalaxyMapClass(GalaxyMapPipClass *pip) : GadgetPipSceneClass(pip), GalaxyMap(pip)
{
	STUB_CALLED("POC GadgetGalaxyMapClass: the PIP only");
}
bool GadgetGalaxyMapClass::Unknown_30(float x, float y)
{
	return(false);
}

// ---------------------------------------------------------------- the star map PIP's lines (0x466920, starmap_pip_support.h): 0x10C bytes
// MapPipClass's constructor makes two StarmapPipSupportClass (+0x48, +0x4C) over this base. The generated constructor left
// Line3DClass unbuilt (no reference count: the first release freed both, and StarmapPipClass::Unknown_14's Set_Hidden
// called through the freed object), so these three do what the client's 0x466920 / 0x466A80 / 0x466B20 do.
Unknown_466920Class::Unknown_466920Class(const Vector3 &start, const Vector3 &end, float width, float r, float g, float b, float opacity) :
	Line3DClass(start, end, width, r, g, b, opacity),
	SortLevel(0)
{
	STUB_CALLED("POC Unknown_466920Class (0x466920): Line3DClass with a sort level");
}
RenderObjClass *Unknown_466920Class::Clone() const
{
	return(new Unknown_466920Class(*this));
}
void Unknown_466920Class::Render(RenderInfoClass &rinfo)
{
	if (WW3D::Is_Sorting_Enabled() && SortLevel) {
		WW3D::Add_To_Static_Sort_List(this, SortLevel);
		return;
	}
	Line3DClass::Render(rinfo);
}
bool GadgetStarmapClass::Unknown_30(float x, float y)
{
	return(false);
}

// ---------------------------------------------------------------- inv_render.cpp's inventory data (0x688830; its view): by-value results gen_stubs cannot write
#include "..\..\app\shared\money.h"
#include "..\..\app\shared\duration.h"
class Unknown688830Class { public: DurationClass Unknown_68A720(int slot); MoneyClass Unknown_689DA0(int slot); };
MoneyClass Unknown688830Class::Unknown_689DA0(int slot)
{
	STUB_CALLED("POC Unknown688830Class::Unknown_689DA0 (0x689DA0): no price");
	return(MoneyClass());
}
DurationClass Unknown688830Class::Unknown_68A720(int slot)
{
	STUB_CALLED("POC Unknown688830Class::Unknown_68A720 (0x68A720): no time");
	return(DurationClass(0));
}
