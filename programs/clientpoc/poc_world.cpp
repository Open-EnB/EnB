// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins on the way into the world (D179). NOT THE CLIENT'S CODE.
/*
**	The objects SClient::Init news whose classes the tree does not have yet. The generated stand-ins of their
**	constructors leave operator new's memory as it is and no vtable, so the first virtual call or member read
**	crashes: these give each the real size's zeroed memory (and InterfaceClass's own table for the windows: no
**	window, no keys). Delete one here when its real class lands in app/.
*/

#define __INTRFACE_MEMBERS
#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\sclient.h"

// ui_hot_keys.cpp's window (0x59C6F0) is real app code since O15 (ui_hot_keys.h): its stand-in left here.

// pdaint.cpp's PDA is real app code since PD1 (pdaint.cpp); its windows' stand-ins are in poc_world3.cpp.

// bridge_int.cpp's bridge (0x53B040) is real app code since L17 (bridge_int.h): its zeroed stand-in left here.

// camera_manager.cpp is real app code (camera_manager.h; its stand-in left here since CM2).

// enbscene.cpp's scene (0x75ECF0-0x75F852) is real app code since WE7: its stand-in and the POC light (WE3) left here;
// the space view is lit by mainview.cpp's four +0x144 lights from OrbitalManager (WE4).
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\dazzle.h"
#include "..\..\ww3d2\light.h"
#include "..\..\wwlib\multilist.h"

// ---------------------------------------------------------------- enbdazzle.cpp's visibility handler (0x75DD00, 8 bytes): the base's answer
class MainViewDazzleHandlerClass : public DazzleVisibilityClass
{
public:
	MainViewDazzleHandlerClass(SClient *client);
	~MainViewDazzleHandlerClass();
	SClient *Client;
};
MainViewDazzleHandlerClass::MainViewDazzleHandlerClass(SClient *client) : Client(client)
{
	STUB_CALLED("POC MainViewDazzleHandlerClass: the base's visibility");
	void Poc_Start_Watchdog();	// at world entry (the main view's constructor), as the scene stand-in did
	Poc_Start_Watchdog();
}
MainViewDazzleHandlerClass::~MainViewDazzleHandlerClass()
{
}

// ---------------------------------------------------------------- cameraeffects.cpp (0x6E2BD0): the table, no effects
CameraEffectsClass::CameraEffectsClass(MainViewClass *view, SClient *client)
{
	STUB_CALLED("POC CameraEffectsClass: no effects");
	Client = client;
}

// orbitalmanager.cpp (0x4F28A0-0x4F51C0) is real app code since WE4: its stand-in left here.

// debrsint.cpp (0x53D250-0x53E968) and frameint.cpp (0x53E970-0x53F338) are real app code since O5: their stand-ins left here.
// debris.cpp (0x6B8F20-0x6BB740) is real app code since O4: its stand-in left here.

// ui_mesh.cpp (0x5B8120-0x5BA4C0) is real app code since UM1: its stand-in left here.
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\rendobj.h"

// The interface base over a mesh and a layer (0x5BB250-0x5BB6D0) is real app code since L3 (app/client/unknown_5bb250.cpp): its
// stand-in left here. The real slot +0x60 runs the mesh's processes (0x5B9410), which the stand-in left out.

// ---------------------------------------------------------------- the cockpit's parts with no class in the tree (ui_cpit.h, ui_cpit.cpp's views)
#include "..\..\app\client\unknown_671300.h"
// (the hulk and harvest windows, 0x5850E0, are rebuilt: app/client/ui_cpit_inv.cpp, O15)
// (the cockpit's +0xE0, UIAvatarDialogClass 0x551FB0, is rebuilt: app/client/ui_avatar_dialog.cpp, O2)
class UICpitGadget6B56AClass : public GadgetClass { public: UICpitGadget6B56AClass(int x, int y, int width, int height, UIViewClass *view); virtual bool Unknown_30(float x, float y); char Unknown30[0x49 - 0x30]; bool Unknown49; char Unknown4A[0x5C - 0x4A]; };
UICpitGadget6B56AClass::UICpitGadget6B56AClass(int x, int y, int width, int height, UIViewClass *view) : GadgetClass(0xFF)
{
	STUB_CALLED("POC UICpitGadget6B56AClass: a gadget that takes nothing");
	memset(Unknown30, 0, sizeof(Unknown30));
	Unknown49 = false;
	memset(Unknown4A, 0, sizeof(Unknown4A));
}

// (ui_symbolchat.cpp's UISymbolChatClass, 0x64A480, is rebuilt: app/client/ui_symbolchat.cpp, O12)

// ---------------------------------------------------------------- 0x79B9E0: the camera modules' signal map, a function-local static (camera_manager.cpp's neighbourhood)
#include "..\..\app\client\unknown_signalbase.h"
class SClientNotifierClass;
SClientNotifierClass *Unknown_79B9E0()
{
	static SignalMap map;
	return((SClientNotifierClass *)&map);
}
class SClientNotifierClass { public: void Add(int id, SignalBaseClass::CallbackClass *callback); void Remove(int id, SignalBaseClass::CallbackClass *callback); };
void SClientNotifierClass::Add(int id, SignalBaseClass::CallbackClass *callback) { ((SignalMap *)this)->connect((void *)id, callback); }	// 0x4F9740 by sclient.cpp's name
void SClientNotifierClass::Remove(int id, SignalBaseClass::CallbackClass *callback) { ((SignalMap *)this)->disconnect((void *)id, *callback); }	// 0x4F98D0

// ---------------------------------------------------------------- the chat panel's parts (ui_chat_panel.cpp's slot +0x54): empty windows
#define POC_WINDOW_CTOR
#include "..\..\app\client\ui_chat_panel.h"
#include "..\..\app\client\ui_combobox.h"
// (the reply list UIChatReplyListClass, 0x56E2A0, is rebuilt: app/client/unknown_56e170.cpp, O12)
// (The windows ChatPanelInterface::Unknown_565F80 opens are rebuilt: ui_option_main.cpp, ui_option_quit.cpp,
// ui_option_slider.cpp and ui_popup_browser.h's UIPopupBrowserClass, CP1)
// (0x765DF0, the chat line colour by its index, is rebuilt: app/client/unknown_764fd0.cpp, GS1)

// FontSys 0x6CE420 (unknown_6cce60.cpp's object, not rebuilt): the client's code, the text from a format into Change_Text
// (a generated stub left the chat window's lines, the inventory counts and the mission texts empty)
#include <stdarg.h>
#include "..\..\app\client\fontmanager.h"
void __cdecl FontSys::Unknown_6CE420(int unknown, const char *format, ...)
{
	if (format == NULL) {
		Change_Text(NULL, unknown);
		return;
	}
	char buffer[0x400];
	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	buffer[sizeof(buffer) - 1] = 0;
	Change_Text(buffer, unknown);
}

// (chatgadg.cpp's text entry ChatGadgetClass 0x65BFD0 is rebuilt in app.lib)
#include "..\..\app\client\textgadg.h"
// The chat channels' names (.data 0xBA8C20-0xBA8C50, their file not in the tree yet; a generated stand-in left them NULL and
// ui_chat_panel.cpp's 0x569A00 then read a NULL name in its channel search): the client's strings
char *Unknown_BA8C20 = "General", *Unknown_BA8C24 = "Out of Context", *Unknown_BA8C28 = "Market", *Unknown_BA8C2C = "New Players",
	*Unknown_BA8C30 = "Terran", *Unknown_BA8C34 = "Jenquai", *Unknown_BA8C38 = "Progen", *Unknown_BA8C3C = "Explorers",
	*Unknown_BA8C40 = "Warriors", *Unknown_BA8C44 = "Tradesmen", *Unknown_BA8C48 = "Sentinels", *Unknown_BA8C4C = "Defenders",
	*Unknown_BA8C50 = "Enforcers";
// (the chat panel's combos, ChatPanelInterface::Unknown_5689D0, and the windows its options combo opens, 0x569A00, are
// rebuilt in ui_chat_panel.cpp: the real UIComboBoxClass from ui_combobox.cpp, O19)
class UnknownEventDispatcherClass;
UnknownEventDispatcherClass *Unknown_540A20()	// 0x540A20 (intrface.cpp's function-local static SignalMap 0xBE5AF0): a real map, ui_chat_panel.cpp connects to it
{
	static SignalMap map;
	return((UnknownEventDispatcherClass *)&map);
}

// ---------------------------------------------------------------- the render states rs_list.cpp makes from files not in the tree yet
// (unknown_45a090.cpp's 'link' state and unknown_45f390.cpp's 'sound' state are rebuilt in app.lib)
#include "..\\..\\app\\shared\\rs_track.h"
#include "shader.h"

// (rs_eff_p.cpp's 'effect_pass' state 0x458C10 is rebuilt in app.lib)

char *Unknown_B70E40[] = { "Axial", "Silhouette", "Environment", "Screen", NULL };

