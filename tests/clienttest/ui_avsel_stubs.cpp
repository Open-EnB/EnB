// Earth & Beyond client (reconstruction)
// clienttest: hand stand-ins for what ui_avsel.obj, ui_login.obj and tsk_game.obj (linked from app.lib since the
// login screen) reach and gen_stubs.py --link cannot write: a pointer-to-nested-class parameter, a by-value
// class, and globals whose type is a file-local class (the client's one address, 0xBE67C4 / 0xBE67B8, has a
// different view in each file, so the mangled names differ). textgadg.h's flags (the client's .rdata
// values) are gadget.cpp's, app.lib.
#include <windows.h>
#include <string>
#include "clienttest.h"
#include "..\..\..\app\client\unknown_signalbase.h"

// (0x5594B0, CharacterViewClass's descriptor by value: ui_avatr.obj's, linked from app.lib)




// (textgadg.h's flags 0xB08C90-0xB08CC4 are gadget.cpp's, app.lib)
