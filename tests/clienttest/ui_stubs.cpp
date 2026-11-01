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

class UIChatF8EventClass {};
class UIChatIntHandlerClass
{
public:
	UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *));
	char Unknown00[0x10];
};
UIChatIntHandlerClass::UIChatIntHandlerClass(UIChatPanelClass *, void (UIChatPanelClass::*)(const UIChatIntEventClass &, UIChatIntClass *))
{
	memset(Unknown00, 0, sizeof(Unknown00));
	Trace("stub UIChatIntHandlerClass::UIChatIntHandlerClass (0x56D0F0)");
}
class UnknownEventDispatcherClass
{
public:
	void Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatF8EventClass &, InterfaceClass *), const UIChatF8EventClass &type);
	void Unknown_56CEC0(UIChatIntClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *), const UIChatIntEventClass &type);
};
void UnknownEventDispatcherClass::Unknown_56CE40(InterfaceClass *, UIChatPanelClass *, void (UIChatPanelClass::*)(const UIChatF8EventClass &, InterfaceClass *), const UIChatF8EventClass &) { Trace("stub UnknownEventDispatcherClass::Unknown_56CE40"); }
void UnknownEventDispatcherClass::Unknown_56CEC0(UIChatIntClass *, UIChatPanelClass *, void (UIChatPanelClass::*)(const UIChatIntEventClass &, UIChatIntClass *), const UIChatIntEventClass &) { Trace("stub UnknownEventDispatcherClass::Unknown_56CEC0"); }

// ------------------------------------------------------------------------- ui_cpit.cpp's effect node and string

void UnknownAF0DD0Class::Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *, ClientObjectClass *) { Trace("stub UnknownAF0DD0Class::Unknown_73FDB0 (0x73FDB0)"); }
std::string Unknown_BE5BD0;		// 0xBE5BD0 (ui_cpit.cpp's)

// ------------------------------------------------------------------------- ro_funcs.cpp, fontmanager.cpp, the globals

class RenderObjClass;
void Unknown_496110(RenderObjClass *, float, float, float) { Trace("stub Unknown_496110"); }	// 0x496110 (a model's highlight)
class LoadingViewFontManagerClass;
LoadingViewFontManagerClass *Unknown_BE67C4 = 0;	// 0xBE67C4 (loadingview.cpp's font manager; the loading view's text is not drawn here)
Random2Class Unknown_BDADC8;		// 0xBDADC8 (soundlib.cpp's)
