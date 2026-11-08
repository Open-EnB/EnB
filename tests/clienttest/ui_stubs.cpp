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

class InterfaceSignalMapClass
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback);
	void Remove(void *sender, const SignalBaseClass::CallbackClass &callback);
};
void InterfaceSignalMapClass::Add(void *, SignalBaseClass::CallbackClass *) { Trace("stub InterfaceSignalMapClass::Add (0x4F9740)"); }
void InterfaceSignalMapClass::Remove(void *, const SignalBaseClass::CallbackClass &) { Trace("stub InterfaceSignalMapClass::Remove (0x4F98D0)"); }

// tooltip.cpp's view of the same two functions (its own TooltipSignalMapClass; unresolved since 627f1bd4)
class TooltipSignalMapClass
{
public:
	void Add(void *sender, SignalBaseClass::CallbackClass *callback);
	void Remove(void *sender, const SignalBaseClass::CallbackClass &callback);
};
void TooltipSignalMapClass::Add(void *, SignalBaseClass::CallbackClass *) { Trace("stub TooltipSignalMapClass::Add (0x4F9740)"); }
void TooltipSignalMapClass::Remove(void *, const SignalBaseClass::CallbackClass &) { Trace("stub TooltipSignalMapClass::Remove (0x4F98D0)"); }

// ------------------------------------------------------------------------- sclient.cpp's notifier (0x79B9E0's object)

typedef SignalBaseClass::CallbackClass SClientCallbackBaseClass;
class SClientNotifierClass
{
public:
	void Add(int id, SClientCallbackBaseClass *callback);
	void Remove(int id, SClientCallbackBaseClass *callback);
};
void SClientNotifierClass::Add(int id, SClientCallbackBaseClass *) { Trace("stub SClientNotifierClass::Add %d", id); }
void SClientNotifierClass::Remove(int id, SClientCallbackBaseClass *) { Trace("stub SClientNotifierClass::Remove %d", id); }

// ------------------------------------------------------------------------- ui_chat_panel.cpp's dispatcher and handler

class OptionFontInterfaceEvent {};
class UIChatIntHandlerClass
{
public:
	UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const ChatWindowInterfaceEvent &, ChatWindowInterface *));
	char Unknown00[0x10];
};
UIChatIntHandlerClass::UIChatIntHandlerClass(UIChatPanelClass *, void (UIChatPanelClass::*)(const ChatWindowInterfaceEvent &, ChatWindowInterface *))
{
	memset(Unknown00, 0, sizeof(Unknown00));
	Trace("stub UIChatIntHandlerClass::UIChatIntHandlerClass (0x56D0F0)");
}
class UnknownEventDispatcherClass
{
public:
	void Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const OptionFontInterfaceEvent &, InterfaceClass *), const OptionFontInterfaceEvent &type);
	void Unknown_56CEC0(ChatWindowInterface *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const ChatWindowInterfaceEvent &, ChatWindowInterface *), const ChatWindowInterfaceEvent &type);
};
void UnknownEventDispatcherClass::Unknown_56CE40(InterfaceClass *, UIChatPanelClass *, void (UIChatPanelClass::*)(const OptionFontInterfaceEvent &, InterfaceClass *), const OptionFontInterfaceEvent &) { Trace("stub UnknownEventDispatcherClass::Unknown_56CE40"); }
void UnknownEventDispatcherClass::Unknown_56CEC0(ChatWindowInterface *, UIChatPanelClass *, void (UIChatPanelClass::*)(const ChatWindowInterfaceEvent &, ChatWindowInterface *), const ChatWindowInterfaceEvent &) { Trace("stub UnknownEventDispatcherClass::Unknown_56CEC0"); }

// ------------------------------------------------------------------------- ui_cpit.cpp's effect node and string

void UnknownAF0DD0Class::Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *, ClientObjectClass *) { Trace("stub UnknownAF0DD0Class::Unknown_73FDB0 (0x73FDB0)"); }
std::string Unknown_BE5BD0;		// 0xBE5BD0 (ui_cpit.cpp's)

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
