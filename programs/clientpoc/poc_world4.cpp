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

// ---------------------------------------------------------------- gd_gauge.cpp's gauge (0x669570; ui_faction.cpp's view): 0x6C bytes
class Unknown669570Class : public RObjGadgetClass
{
public:
	Unknown669570Class(RenderObjClass *robj, const char *name);
	virtual void Unknown_A4();
	virtual float Unknown_A8(float value);
	char Unknown60[0x6C - 0x60];
};
Unknown669570Class::Unknown669570Class(RenderObjClass *robj, const char *name) : RObjGadgetClass(robj, 0)
{
	STUB_CALLED("POC Unknown669570Class (a gauge): the model only");
	memset(Unknown60, 0, sizeof(Unknown60));
}
void Unknown669570Class::Unknown_A4()
{
}
float Unknown669570Class::Unknown_A8(float value)
{
	return(value);
}

// ---------------------------------------------------------------- gd_pip_scene.cpp's gadget base (0x66ACE0, map_pip.h): 0x4C bytes
GadgetPipSceneClass::GadgetPipSceneClass(PipSceneClass *pip) : GadgetClass(0x1077)
{
	STUB_CALLED("POC GadgetPipSceneClass: an empty GadgetClass");
	memset(Unknown_Pad30, 0, sizeof(Unknown_Pad30));
	*(PipSceneClass **)Unknown_Pad30 = pip;
	Unknown38.X = 0.0f;
	Unknown38.Y = 0.0f;
	memset(Unknown_Pad40, 0, sizeof(Unknown_Pad40));
	Unknown49 = false;
}

// ---------------------------------------------------------------- gd_starmap.cpp's gadget (0x7B4700, starmap_pip.h): 0x50 bytes
GadgetStarmapClass::GadgetStarmapClass(StarmapPipClass *pip) : GadgetPipSceneClass(pip), StarmapPIP(pip)
{
	STUB_CALLED("POC GadgetStarmapClass: the PIP only");
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
