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

// ---------------------------------------------------------------- pdaint.cpp (0x693140, table 0xAFD148): 0xC0 bytes
PDAInterfaceClass::PDAInterfaceClass(SClient *client) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC PDAInterfaceClass: an empty InterfaceClass");
	memset(Unknown84, 0, sizeof(Unknown84));
	UnknownA8 = NULL;
	memset(UnknownAC, 0, sizeof(UnknownAC));
}

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

// ---------------------------------------------------------------- enbscene.cpp's scene (0x75ECF0, 0xF4 bytes; its base 0x494EE0 is claimed): a plain SimpleSceneClass
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\dazzle.h"
class MainViewSceneUnknown98Class;
class MainViewSceneClass : public SimpleSceneClass
{
public:
	MainViewSceneClass();
	void Unknown_75F690(const Vector3 &sun, const Vector3 &camera);
	void Unknown_75F700(const Vector3 &color);
	virtual void Unknown_68(CameraClass *camera);
	MainViewSceneUnknown98Class *Unknown98; char Unknown9C[0xF4 - 0x9C];
};
MainViewSceneClass::MainViewSceneClass()
{
	STUB_CALLED("POC MainViewSceneClass: a plain SimpleSceneClass");
	void Poc_Start_Watchdog();
	Poc_Start_Watchdog();
}

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

// ---------------------------------------------------------------- orbital manager (0x4F3250, 0x1D4 bytes; no table)
OrbitalManager::OrbitalManager(SClient *client, MainViewOrbitalCallbackClass *callback)
{
	STUB_CALLED("POC OrbitalManager: zero");
	memset(this, 0, 0x1D4);
}

// ---------------------------------------------------------------- the main view's windows that have no class in the tree: empty windows on its view (0x53E970, 0x53D250 and +0x1DC's)
MainViewUnknown53E970Class::MainViewUnknown53E970Class(MainViewClass *view, SimpleSceneClass *scene) : InterfaceClass((UIViewClass *)view, (int)scene)
{
	STUB_CALLED("POC MainViewUnknown53E970Class: an empty window");
}
MainViewUnknown138Class::MainViewUnknown138Class(MainViewClass *view, SimpleSceneClass *scene, INIClass &ini) : InterfaceClass((UIViewClass *)view, (int)scene)
{
	STUB_CALLED("POC MainViewUnknown138Class: an empty window");
}
MainViewUnknown1DCClass::MainViewUnknown1DCClass(MainViewClass *view, SimpleSceneClass *scene) : InterfaceClass((UIViewClass *)view, (int)scene)
{
	STUB_CALLED("POC MainViewUnknown1DCClass: an empty window");
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
UIScanClass::UIScanClass(UIViewClass *view, LayerClass *layer) : InterfaceClass(view, (int)layer->Peek_Scene())
{
	STUB_CALLED("POC UIScanClass: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
	Unknown88 = NULL;
	memset(Unknown8C, 0, sizeof(Unknown8C));
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

// ---------------------------------------------------------------- the trade window's parts (0x59EDC0, 0x649890)
#include "..\..\app\client\ui_manu.h"
UIManuInvClass::UIManuInvClass(SClient *client, RenderObjClass *model, int count, const char *format, int unknown) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIManuInvClass: an empty window");
	memset(Unknown84, 0, sizeof(Unknown84));
}
UIPTradeCreditsClass::UIPTradeCreditsClass(SClient *client, int a, int b) : Unknown5BB320Class(new UIMeshClass("TRADE01A", 0), client)
{
	STUB_CALLED("POC UIPTradeCreditsClass: the model only");
	memset(Unknown0AC, 0, sizeof(Unknown0AC));
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
UIChatIntClass::UIChatIntClass(SClient *client, int a, int b, int kind) : UIChatPanelWindowClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIChatIntClass: an empty window");
}
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, LayerClass *layer, RenderObjClass *model, const char *name, int id, int key, int a, int b, int c) : InterfaceClass(owner->Client)
{
	STUB_CALLED("POC UIComboBoxClass: an empty window");
}
void UIChatPanelClass::Unknown_5689D0(int index)
{
	Combo[index] = Create_NEW(new UIComboBoxClass(this, NULL, NULL, NULL, 0, 0, 0, 0, 0));
}
class UIChatF8EventClass {};
class UIChatIntHandlerClass
{
public:
	UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *));
};
UIChatIntHandlerClass::UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *))
{
	STUB_CALLED("POC UIChatIntHandlerClass: nothing");
}
class UnknownEventDispatcherClass
{
public:
	void Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatF8EventClass &, InterfaceClass *), const UIChatF8EventClass &type);
	void Unknown_56CEC0(UIChatIntClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *), const UIChatIntEventClass &type);
};
void UnknownEventDispatcherClass::Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatF8EventClass &, InterfaceClass *), const UIChatF8EventClass &type)
{
}
void UnknownEventDispatcherClass::Unknown_56CEC0(UIChatIntClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const UIChatIntEventClass &, UIChatIntClass *), const UIChatIntEventClass &type)
{
}

// ---------------------------------------------------------------- the render states rs_list.cpp makes from files not in the tree yet
// (rs_hanim.cpp 'anim', 'link', 'sound', rs_eff_p.cpp 'effect_pass'): plain states that do nothing when activated
#include "..\\..\\app\\shared\\rs_track.h"
#include "shader.h"
Unknown4596A0Class::Unknown4596A0Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *anim, const char *suffix, int next_id, HAnimComboDataClass *combo) :
	RenderStateClass(list, id, link_id, flags)
{
	STUB_CALLED("POC Unknown4596A0Class ('anim' state): does nothing");
	memset(Unknown2C, 0, sizeof(Unknown2C));
}
Unknown4596A0Class::~Unknown4596A0Class() {}
RenderStateClass *Unknown4596A0Class::Unknown_04(RenderStateListClass *list) { return(new Unknown4596A0Class(list, ID, LinkID, Flags, NULL, NULL, -1, NULL)); }
void Unknown4596A0Class::Unknown_14(RsTrackClass *owner) {}

Unknown45A090Class::Unknown45A090Class(RenderStateListClass *list, int id, int link_id, unsigned int flags) :
	RenderStateClass(list, id, link_id, flags)
{
	STUB_CALLED("POC Unknown45A090Class ('link' state): does nothing");
}
Unknown45A090Class::~Unknown45A090Class() {}
RenderStateClass *Unknown45A090Class::Unknown_04(RenderStateListClass *list) { return(new Unknown45A090Class(list, ID, LinkID, Flags)); }
void Unknown45A090Class::Unknown_14(RsTrackClass *owner) {}

Unknown45F390Class::Unknown45F390Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *sound, int message) :
	RenderStateClass(list, id, link_id, flags)
{
	STUB_CALLED("POC Unknown45F390Class ('sound' state): does nothing");
	memset(Unknown2C, 0, sizeof(Unknown2C));
}
Unknown45F390Class::~Unknown45F390Class() {}
RenderStateClass *Unknown45F390Class::Unknown_04(RenderStateListClass *list) { return(new Unknown45F390Class(list, ID, LinkID, Flags, NULL, 0)); }
void Unknown45F390Class::Unknown_14(RsTrackClass *owner) {}

Unknown458C10Class::Unknown458C10Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, bool two_sided, bool hidden, const char *texture, int a, int b, ShaderClass shader) :
	RenderStateClass(list, id, link_id, flags)
{
	STUB_CALLED("POC Unknown458C10Class ('effect_pass' state): does nothing");
	memset(Unknown2C, 0, sizeof(Unknown2C));
}
Unknown458C10Class::~Unknown458C10Class() {}
RenderStateClass *Unknown458C10Class::Unknown_04(RenderStateListClass *list) { return(new Unknown458C10Class(list, ID, LinkID, Flags, false, false, "", 1, 1, ShaderClass())); }
void Unknown458C10Class::Unknown_14(RsTrackClass *owner) {}
void Unknown458C10Class::Unknown_459120(TextureMapperClass *mapper) {}

char *Unknown_B70E40[] = { "Axial", "Silhouette", "Environment", "Screen", NULL };
