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
class BridgeInterfaceClass
{
public:
	BridgeInterfaceClass(SClient *client);
	~BridgeInterfaceClass();
	char Unknown00[0xC4];
};
BridgeInterfaceClass::BridgeInterfaceClass(SClient *client)
{
	STUB_CALLED("POC BridgeInterfaceClass: zero");
	memset(this, 0, sizeof(*this));
}
BridgeInterfaceClass::~BridgeInterfaceClass()
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

// ---------------------------------------------------------------- the main view's windows that have no class in the tree: empty windows on its view (0x53E970, 0x53D250 and +0x1DC's)
MainViewUnknown53E970Class::MainViewUnknown53E970Class(MainViewClass *view, SimpleSceneClass *scene) : InterfaceClass((UIViewClass *)view, (int)scene)
{
	STUB_CALLED("POC MainViewUnknown53E970Class: an empty window");
}
MainViewUnknown138Class::MainViewUnknown138Class(MainViewClass *view, SimpleSceneClass *scene, INIClass &ini) : InterfaceClass((UIViewClass *)view, (int)scene)
{
	STUB_CALLED("POC MainViewUnknown138Class: an empty window");
}

// ui_mesh.cpp (0x5B8120-0x5BA4C0) is real app code since UM1: its stand-in left here.
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\rendobj.h"

// ---------------------------------------------------------------- the interface base over a mesh and a layer (0x5BB250-0x5BB6B0)
Unknown5BB320Class::Unknown5BB320Class(UIMeshClass *mesh, SClient *client) : InterfaceClass((LineDisplayClientClass *)client), InterfaceMesh(mesh)
{
	STUB_CALLED("POC Unknown5BB320Class");
}
Unknown5BB320Class::Unknown5BB320Class(UIMeshClass *mesh, UIViewClass *view, const LayerClass &layer) : InterfaceClass(view, (int)((LayerClass &)layer).Peek_Scene()), InterfaceMesh(mesh), Layer(layer)
{
	STUB_CALLED("POC Unknown5BB320Class");
}
Unknown5BB320Class::~Unknown5BB320Class()
{
	delete InterfaceMesh;
}
void Unknown5BB320Class::Unknown_58()
{
	InterfaceMesh->Unknown_5B9290((SimpleSceneClass *)Layer.Peek_Scene());
	InterfaceClass::Unknown_58();
}
void Unknown5BB320Class::Unknown_5C()
{
	InterfaceMesh->Unknown_5B92B0();
	InterfaceClass::Unknown_5C();
}
void Unknown5BB320Class::Unknown_60()
{
}
// 0x5BB4F0: the client's code (a gadget onto the mesh's list, ui_mesh.cpp 0x5B99C0; was a do-nothing gen stand-in)
void Unknown5BB320Class::Unknown_5BB4F0(GadgetClass *gadget)
{
	InterfaceMesh->Unknown_5B99C0(gadget);
}
void Unknown5BB320Class::Unknown_7C(UIViewClass *view, bool arg)
{
	if (view == NULL) {
		Layer.Set_Scene(NULL);
		Layer.Set_Camera(NULL);
	} else {
		Layer.Set_Scene((SceneClass *)view->Unknown_50());
		Layer.Set_Camera((CameraClass *)view->Unknown_54());
	}
	InterfaceClass::Unknown_7C(view, arg);
}
void Unknown5BB320Class::Unknown_88()
{
}
void Unknown5BB320Class::Unknown_8C()
{
}

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
UICpitUnknownD0Class::UICpitUnknownD0Class(RenderObjClass *model, LayerClass *layer, UICpitClass *cpit, int arg)
{
	STUB_CALLED("POC UICpitUnknownD0Class: an empty watch");
	memset(Unknown18, 0, sizeof(Unknown18));
}
class UICpitWarpGadgetClass : public Unknown_671300Class { public: UICpitWarpGadgetClass(RenderObjClass *object, RenderObjClass *bar, const LayerClass &layer); void Unknown_676190(int a, int b); char Unknown80[0x94 - 0x80]; };
UICpitWarpGadgetClass::UICpitWarpGadgetClass(RenderObjClass *object, RenderObjClass *bar, const LayerClass &layer) : Unknown_671300Class(object, (int)bar, layer, 0, 0, 3)
{
	STUB_CALLED("POC UICpitWarpGadgetClass: a plain button");
	memset(Unknown80, 0, sizeof(Unknown80));
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

// ---------------------------------------------------------------- the trade window's parts (0x59EDC0)
#include "..\..\app\client\ui_manu.h"
UIManuInvClass::UIManuInvClass(SClient *client, RenderObjClass *model, int count, const char *format, int unknown) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIManuInvClass: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
}

// ---------------------------------------------------------------- the +0x1358 view's window (0x5B7AB0): an empty window
Unknown5B7AB0Class::Unknown5B7AB0Class(UIViewClass *view, const LayerClass &layer) : InterfaceClass(view, (int)layer.Peek_Scene())
{
	STUB_CALLED("POC Unknown5B7AB0Class: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
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
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, LayerClass *layer, RenderObjClass *model, const char *name, int id, int key, int a, int b, int c) : InterfaceClass(owner->Client)
{
	STUB_CALLED("POC UIComboBoxClass: an empty window");
	memset((char *)this + sizeof(InterfaceClass), 0, sizeof(UIComboBoxClass) - sizeof(InterfaceClass));	// the real 0x57B440 (slot +0x7C) reads the drop-down state +0xC8
}
void ChatPanelInterface::Unknown_5689D0(int index)
{
	Combo[index] = Create_NEW(new UIComboBoxClass(this, NULL, NULL, NULL, 0, 0, 0, 0, 0));
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

