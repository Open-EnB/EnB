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
// The split chat window (ui_chat_window_split.cpp, not rebuilt): the real ChatWindowInterface (ui_chat_window.cpp)
// with a set-up that loads the kind's model (0x5723B0's first steps: slot +0x98 with 0xB7D938's names, the base's
// set-up, slot +0xA0 with one window, two when split) and leaves the frame's resize gadgets out
UIChatWindowSplitClass::UIChatWindowSplitClass(SClient *client, Unknown67D430Class *history, Unknown65BFD0Class *edit, int kind) :
	ChatWindowInterface(client, history, edit)
{
	STUB_CALLED("POC UIChatWindowSplitClass: the chat window without its split");
	UnknownE8 = kind;
	UnknownEC = UnknownF0 = UnknownF4 = UnknownFC = Unknown100 = Unknown104 = Unknown108 = Unknown110 = Unknown114 = 0;
	UnknownF8 = Unknown10C = -1;
	Unknown118 = (kind == 2);
	Unknown11C = NULL;
}
UIChatWindowSplitClass::~UIChatWindowSplitClass() {}
void UIChatWindowSplitClass::Unknown_54()
{
	static const char *models[] = { "tsi05f", "tsi05d", "tsi05e" };
	if (!Unknown_98(models[UnknownE8], NULL)) return;
	ChatWindowInterface::Unknown_54();
	Unknown_A0((UnknownE8 != 0) ? 2 : 1);
}
void UIChatWindowSplitClass::Unknown_58() { ChatWindowInterface::Unknown_58(); }
void UIChatWindowSplitClass::Unknown_5C() { ChatWindowInterface::Unknown_5C(); }
void UIChatWindowSplitClass::Unknown_60() { ChatWindowInterface::Unknown_60(); }
bool UIChatWindowSplitClass::Unknown_68(int key) { return(ChatWindowInterface::Unknown_68(key)); }
void UIChatWindowSplitClass::Unknown_A8() { ChatWindowInterface::Unknown_A8(); }

// 0x765DF0 (file unknown, not rebuilt): the client's code, the chat line colour by its index (0..23), else white;
// a generated stub cannot return a Vector3 (the call links to 0)
Vector3 Unknown764FD0Class::Unknown_765DF0(int index)
{
	if (index < 24 && index >= 0) return(Unknown00[index]);
	return(Vector3(1.0f, 1.0f, 1.0f));
}

// SClient 0x74D990 (parked, docs/attempts/74D990: clienttest's trace stub collides with the real one): the chat
// line with its type's prefix onto the chat panel (0x563760), as the stash has it, without its sounds and line display
void SClient::Unknown_74D990(int color, char *text, int a)
{
	if (!text || !*text) return;
	UIChatPanelClass *chatpanel = ChatPanel.Peek();
	if (!chatpanel) return;
	std::string line("");
	if (!(char)a) {
		if (color == 0 || color == 17) line = "COMPUTER: ";
		else if (color == 6) line = "TIP: ";
		else if (color == 21) line = "WARNING: ";
		else if (color == 19) line = "SYSTEM: ";
		else if (color == 20 || color == 1 || color == 18) line = "OUTDATED MESSAGE TYPE: ";
	}
	if (color == 22) return;
	line += text;
	chatpanel->Unknown_563760(color, line);
}

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

// chatgadg.cpp's text entry (0x65BFD0, not rebuilt): a text gadget that holds no text, so the chat window's calls
// through its table reach real code (a generated constructor leaves no table)
#include "..\..\app\client\textgadg.h"
class Unknown65BFD0Class : public TextBaseGadgetClass
{
public:
	Unknown65BFD0Class(LineDisplayClientClass *client);
	void Unknown_65E7C0(const char *text);
	void Unknown_65EBF0(char *buffer, int size, int &cursorpos, bool &insert);
	char Unknown44[0x80 - 0x44];
};
Unknown65BFD0Class::Unknown65BFD0Class(LineDisplayClientClass *client) : TextBaseGadgetClass(0xFF, 0, NULL)
{
	STUB_CALLED("POC Unknown65BFD0Class (chatgadg.cpp's text entry): no text");
	memset(Unknown44, 0, sizeof(Unknown44));
}
void Unknown65BFD0Class::Unknown_65E7C0(const char *text) {}
void Unknown65BFD0Class::Unknown_65EBF0(char *buffer, int size, int &cursorpos, bool &insert)
{
	buffer[0] = 0;
	cursorpos = 0;
	insert = false;
}
UIComboBoxClass::UIComboBoxClass(InterfaceClass *owner, LayerClass *layer, RenderObjClass *model, const char *name, int id, int key, int a, int b, int c) : InterfaceClass(owner->Client)
{
	STUB_CALLED("POC UIComboBoxClass: an empty window");
	memset((char *)this + sizeof(InterfaceClass), 0, sizeof(UIComboBoxClass) - sizeof(InterfaceClass));	// the real 0x57B440 (slot +0x7C) reads the drop-down state +0xC8
}
void UIChatPanelClass::Unknown_5689D0(int index)
{
	Combo[index] = Create_NEW(new UIComboBoxClass(this, NULL, NULL, NULL, 0, 0, 0, 0, 0));
}
class OptionFontInterfaceEvent {};
class UIChatIntHandlerClass
{
public:
	UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const ChatWindowInterfaceEvent &, ChatWindowInterface *));
};
UIChatIntHandlerClass::UIChatIntHandlerClass(UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const ChatWindowInterfaceEvent &, ChatWindowInterface *))
{
	STUB_CALLED("POC UIChatIntHandlerClass: nothing");
}
class UnknownEventDispatcherClass
{
public:
	void Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const OptionFontInterfaceEvent &, InterfaceClass *), const OptionFontInterfaceEvent &type);
	void Unknown_56CEC0(ChatWindowInterface *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const ChatWindowInterfaceEvent &, ChatWindowInterface *), const ChatWindowInterfaceEvent &type);
};
void UnknownEventDispatcherClass::Unknown_56CE40(InterfaceClass *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const OptionFontInterfaceEvent &, InterfaceClass *), const OptionFontInterfaceEvent &type)
{
}
void UnknownEventDispatcherClass::Unknown_56CEC0(ChatWindowInterface *sender, UIChatPanelClass *receiver, void (UIChatPanelClass::*handler)(const ChatWindowInterfaceEvent &, ChatWindowInterface *), const ChatWindowInterfaceEvent &type)
{
}

// ---------------------------------------------------------------- the render states rs_list.cpp makes from files not in the tree yet
// ('link', 'sound'): plain states that do nothing when activated
#include "..\\..\\app\\shared\\rs_track.h"
#include "shader.h"
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

// (rs_eff_p.cpp's 'effect_pass' state 0x458C10 is rebuilt in app.lib)

char *Unknown_B70E40[] = { "Axial", "Silhouette", "Environment", "Screen", NULL };

