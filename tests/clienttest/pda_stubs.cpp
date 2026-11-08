// Earth & Beyond client (reconstruction)
// clienttest: hand stand-ins for the by-value symbols gen_stubs.py --link cannot write that the real PDA
// (pdaint.cpp, since PD1) reaches through its windows' set-ups: ui_starmap.cpp's how-to windows and
// inv_render.cpp's inventory data. The same ones programs/clientpoc/poc_world4.cpp has. Delete one here when
// a tree file defines it (the linker reports the duplicate).
#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "clienttest.h"
#include "..\..\..\app\client\sclient.h"
#include "..\..\..\app\client\ui_dynamicwindow.h"
#include "..\..\..\app\client\dynamic_window_class.h"
#include "..\..\..\app\shared\money.h"
#include "..\..\..\app\shared\duration.h"

void UIDynamicWindowClass::Unknown_58A580(Vector2 position) { Trace("stub UIDynamicWindowClass::Unknown_58A580 (0x58A580)"); }

// inv_render.cpp's view of the inventory data (0x688830)
class Unknown688830Class { public: DurationClass Unknown_68A720(int slot); MoneyClass Unknown_689DA0(int slot); };
MoneyClass Unknown688830Class::Unknown_689DA0(int slot) { Trace("stub Unknown688830Class::Unknown_689DA0 (0x689DA0)"); return(MoneyClass()); }
DurationClass Unknown688830Class::Unknown_68A720(int slot) { Trace("stub Unknown688830Class::Unknown_68A720 (0x68A720)"); return(DurationClass(0)); }
