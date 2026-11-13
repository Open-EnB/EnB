// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins for the PDA's windows (D179). NOT THE CLIENT'S CODE.
/*
**	pdaint.cpp's set-up (0x693230) news the equipment (ui_pda_equip.cpp), cargo (ui_pda_cargo.cpp) and vault (ui_vault.cpp)
**	windows and the experience display (0x58C450, ui_experience.cpp's, in app.lib).
**	The cargo, equipment and vault windows are app.lib's; what they make that the tree does not have yet (the combo box's
**	second constructor 0x5789E0, the ship's PIP 0x541DE0, its gadget 0x66DA50 and the
**	stats display 0x6A2ED0) was stood in for here; all are app/'s now (the ship's PIP: ship_equip_pip.cpp, the stats
**	display: ship_stats_display.cpp, O18), so no stand-in is left.
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
