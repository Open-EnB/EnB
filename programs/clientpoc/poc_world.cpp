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

// ---------------------------------------------------------------- ui_hotkeys.cpp (0x59C6F0, table 0xAF20D0): 0x84 bytes, no members
class UIHotKeysClass : public InterfaceClass
{
public:
	UIHotKeysClass(SClient *client);
};
UIHotKeysClass::UIHotKeysClass(SClient *client) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIHotKeysClass: an empty InterfaceClass");
}

// pdaint.cpp's PDA is real app code since PD1 (pdaint.cpp); its windows' stand-ins are in poc_world3.cpp.

// ---------------------------------------------------------------- bridge (0x53B040): 0xC4 bytes, zero
class BridgeInterface
{
public:
	BridgeInterface(ClientClass *client);
	~BridgeInterface();
	char Unknown00[0xC4];
};
BridgeInterface::BridgeInterface(ClientClass *client)
{
	STUB_CALLED("POC BridgeInterface: zero");
	memset(this, 0, sizeof(*this));
}
BridgeInterface::~BridgeInterface()
{
}

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
// debris.cpp's DebrisFieldClass (0x6B8F20, not in the tree): a field that draws nothing, so the debris interface's
// fields are real objects (the gen stand-in constructor would leave a garbage table for Add / Remove / Release_Ref)
DebrisFieldClass::DebrisFieldClass(float spacing, float size, float min_dist, float max_dist, float r, float g, float b, float a, bool additive, TextureClass *texture, float fade_in, float fade_out, int primitive, unsigned rows_columns_log2, float blur_time) :
	Spacing(spacing), Size(size), MinDist(min_dist), MaxDist(max_dist), Color(r, g, b), Alpha(a), FadeIn(fade_in), FadeOut(fade_out), UnknownB4(10000), Additive(additive), UnknownBC(2), UnknownC0(0), UnknownC4(0), UnknownC8(1.0f), BlurTime(blur_time)
{
	STUB_CALLED("POC DebrisFieldClass: a debris field that draws nothing");
}
RenderObjClass *DebrisFieldClass::Clone(void) const { return(NULL); }
void DebrisFieldClass::Render(RenderInfoClass &rinfo) {}
int DebrisFieldClass::Unknown_6BB440() { return(0); }
void DebrisFieldClass::Unknown_6BB470() {}
void DebrisFieldClass::Unknown_6BB640(const Vector3 &offset) {}

// ui_mesh.cpp (0x5B8120-0x5BA4C0) is real app code since UM1: its stand-in left here.
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\rendobj.h"

// The interface base over a mesh and a layer (0x5BB250-0x5BB6D0) is real app code since L3 (app/client/unknown_5bb250.cpp): its
// stand-in left here. The real slot +0x60 runs the mesh's processes (0x5B9410), which the stand-in left out.

// ---------------------------------------------------------------- the cockpit's parts with no class in the tree (ui_cpit.h, ui_cpit.cpp's views)
#include "..\..\app\client\unknown_671300.h"
UICpitUnknownC8Class::UICpitUnknownC8Class(UICpitClass *cpit, SClient *sclient, int type) : InterfaceClass((LineDisplayClientClass *)sclient)
{
	STUB_CALLED("POC UICpitUnknownC8Class: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
}
UICpitUnknownE0Class::UICpitUnknownE0Class(SClient *sclient) : InterfaceClass((LineDisplayClientClass *)sclient)
{
	STUB_CALLED("POC UICpitUnknownE0Class: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
}
class UICpitGadget6B56AClass : public GadgetClass { public: UICpitGadget6B56AClass(int x, int y, int width, int height, UIViewClass *view); virtual bool Unknown_30(float x, float y); char Unknown30[0x49 - 0x30]; bool Unknown49; char Unknown4A[0x5C - 0x4A]; };
UICpitGadget6B56AClass::UICpitGadget6B56AClass(int x, int y, int width, int height, UIViewClass *view) : GadgetClass(0xFF)
{
	STUB_CALLED("POC UICpitGadget6B56AClass: a gadget that takes nothing");
	memset(Unknown30, 0, sizeof(Unknown30));
	Unknown49 = false;
	memset(Unknown4A, 0, sizeof(Unknown4A));
}

// ---------------------------------------------------------------- ui_symbolchat (0x64A480): an empty window
#include "..\..\app\client\ui_group.h"
UISymbolChatClass::UISymbolChatClass(SClient *sclient) : InterfaceClass((LineDisplayClientClass *)sclient)
{
	STUB_CALLED("POC UISymbolChatClass: an empty window");
}

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
class UIChatReplyListClass : public InterfaceClass { public: UIChatReplyListClass(SClient *client); char Unknown84[0xA4 - 0x84]; };
UIChatReplyListClass::UIChatReplyListClass(SClient *client) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIChatReplyListClass: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
}
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
// The chat panel's combos while 0x5689D0 is a stand-in: an empty interface of the combo box's size, its state zero (the
// drop-down up: the real Close_Drop_Down 0x57B440 reads +0xC8), with InterfaceClass's slots. Not a UIComboBoxClass: the
// real constructor 0x5786C0 and destructor 0x578D50 (ui_combobox.cpp) need the layer, the model and the gadgets.
class PocEmptyComboClass : public InterfaceClass
{
public:
	PocEmptyComboClass(InterfaceClass *owner) : InterfaceClass(owner->Client)
	{
		STUB_CALLED("POC UIComboBoxClass: an empty window");
		memset(Zero, 0, sizeof(Zero));
	}
	char Zero[sizeof(UIComboBoxClass) - sizeof(InterfaceClass)];
};
void ChatPanelInterface::Unknown_5689D0(int index)
{
	Combo[index] = Create_NEW((UIComboBoxClass *)new PocEmptyComboClass(this));
}
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

