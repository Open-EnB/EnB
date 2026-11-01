// Earth & Beyond client (reconstruction)
// clienttest: the one stand-in that needs fontmanager.h, which clashes with sclient.h's / ui_chat_panel.h's
// declarations in a file that has them (FontStyleClass's constructor from a string template, 0x6D4C70, not rebuilt).
#include <windows.h>
#include <string>
#include <vector>
#include "clienttest.h"
#include "..\..\..\app\client\fontmanager.h"

FontStyleClass::FontStyleClass(FontStringStruct, unsigned int, int) : DynamicMeshClass(2, 4) { Trace("stub FontStyleClass::FontStyleClass (0x6D4C70)"); }
