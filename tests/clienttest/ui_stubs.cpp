// Earth & Beyond client (reconstruction)
// clienttest: hand stand-ins for the symbols gen_stubs.py --link cannot write (a pointer to a member function
// or a nested class as a parameter, a string or an object as a global) that the real app.lib objects
// (intrface.obj, ui_chat_panel.obj, ui_cpit.obj, loadingview.obj, soundlib.obj) reach since they are linked
// instead of stand-ins. The same ones programs/clientpoc/poc_link.cpp and poc_data.cpp have. Delete one here
// when a tree file defines it (the linker reports the duplicate).
#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "clienttest.h"
#include "..\..\..\app\client\unknown_signalbase.h"
#include "..\..\..\app\client\ui_chat_panel.h"
#include "random.h"

// ------------------------------------------------------------------------- the UI kit's SignalMap (intrface.cpp)

// (the real SignalMap: app/client/signal_map.cpp; the per-file views of it are gone)

// ------------------------------------------------------------------------- ui_chat_panel.cpp's dispatcher and handler

// (InterfaceClass::Get_Signal_Map, the dispatcher, is rebuilt in intrface.cpp)

// ------------------------------------------------------------------------- ui_cpit.cpp's effect node and string


// ------------------------------------------------------------------------- ro_funcs.cpp, fontmanager.cpp, the globals

class RenderObjClass;
void Unknown_496110(RenderObjClass *, float, float, float) { Trace("stub Unknown_496110"); }	// 0x496110 (a model's highlight)
Random2Class Unknown_BDADC8;		// 0xBDADC8 (soundlib.cpp's)
RandomClass Unknown_BDBB84;		// 0xBDBB84 (avatar.cpp's)

// ------------------------------------------------------------------------- wlib's Wstring (message.cpp)

// The message reporters' common path 0x4BC5A0 and the server (app.lib's message*.obj, linked for the real
// reporters) use these; gen_stubs.py --link writes no operators. Never reached here: no message server.
class Wstring
{
public:
	Wstring &operator+=(const char *other);
	Wstring &operator+=(const Wstring &other);
	Wstring &operator=(const Wstring &other);
	char operator==(const Wstring &other) const;
	char operator!=(const Wstring &other) const;
};
Wstring &Wstring::operator+=(const char *) { Trace("stub Wstring::operator+=(const char *) (0x8B6640)"); return *this; }
Wstring &Wstring::operator+=(const Wstring &) { Trace("stub Wstring::operator+=(const Wstring &) (0x8B6750)"); return *this; }
Wstring &Wstring::operator=(const Wstring &) { Trace("stub Wstring::operator=(const Wstring &) (0x8B61D0)"); return *this; }
// the message filter's match 0x4BE2C0 (message_filter.obj); never reached here
char Wstring::operator==(const Wstring &) const { Trace("stub Wstring::operator==(const Wstring &) (0x8B5F70)"); return 0; }
char Wstring::operator!=(const Wstring &) const { Trace("stub Wstring::operator!=(const Wstring &) (0x8B6070)"); return 1; }

// the message server's 0x4BD470 (message_server.obj) stamps its record with this; never reached here
class Wtime
{
public:
	Wtime &operator=(const unsigned long other);
};
Wtime &Wtime::operator=(const unsigned long) { Trace("stub Wtime::operator=(uint32) (0x8BA250)"); return *this; }
