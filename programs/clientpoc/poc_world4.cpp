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
#include "..\..\app\client\galaxy_map_pip.h"
#include "..\..\ww3d2\ww3d.h"

// (inv_render.cpp's inventory data view, 0x689DA0 / 0x68A720, is rebuilt: inv_data_peek.cpp's InvDataPeekClass::Get_Price / Get_Ready_Time)
