/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\Client\sclient.cpp (client 0x7405E0-0x7429B0, the skeleton)
**	SClient (sclient.h): D90 9d's skeleton, the functions before the file's first assert. Notes at the end.
*/
#include <windows.h>
#include "sclient.h"
#include "..\common\connect.h"
#include "..\packets\pk.h"
#include "ini.h"
#include "wwfile.h"
#include "..\packets\pkaction.h"
#include "..\packets\pklogoff.h"
#include "point.h"
#include "refcount.h"
#include "wwprofile.h"
#include "font3d.h"
#include "camera.h"
#include "scene.h"
#include "render2d.h"
#include "ww3d.h"
#include "rect.h"
#include "tasks\tsk_login.h"
#include <string>
#include <vector>

// Callees in files that aren't rebuilt (cdecl, declared here as far as SClient calls them)
void Unknown_719E10(SClient *client);					// 0x719E10 (control_keys_manager.cpp): the destructor's
void Unknown_7228C0();									// NAME 1 [Unknown_7228C0]: neutral (D10), as tsk_login.cpp
void Unknown_723150();									// NAME 1 [Unknown_723150]: neutral (D10), as tsk_login.cpp
void Unknown_6766E0(const char *ini);					// 0x6766E0: 'action.ini'
void Unknown_676A00();									// 0x676A00: Load_INIs' 0x6766E0 undone
void Unknown_817070(const char *ini, const char *ex_ini);	// 0x817070: 'cskill_t.ini', 'cskill_ex_t.ini'
void Unknown_816E60();									// 0x816E60
void Unknown_814620(const char *ini, const char *ex_ini);	// 0x814620: 'cskill_ability_t.ini', 'cskill_ability_ex_t.ini'
void Unknown_814430();									// 0x814430
void Unknown_74CA90(const char *ini);					// 0x74CA90 (sclient.cpp's attributed run): 'testeffects.ini'
void Unknown_70A7E0(const std::string &ini);			// 0x70A7E0: 'gutter_color.ini'
void Unknown_70AD70();									// 0x70AD70
void Unknown_Load_Assets(const std::string &ini);		// NAME 2 [Unknown_Load_Assets]: 0x729BF0 (client_lego_impl.cpp), 'cbasset.ini' (as tsk_smovie.cpp)
void Unknown_Free_Assets();								// NAME 2 [Unknown_Free_Assets]: 0x729D60 (client_lego_impl.cpp) (as tsk_smovie.cpp)

extern HWND Unknown_Main_Window;						// 0xBDAA7C (input.cpp's neighbourhood)  // NAME 2: what Pump_Messages redraws

// The object at 0xC404D8 (soundlib.cpp's neighbourhood): the constructor sets its byte +0x19A5
struct Unknown_C404D8Struct		// NAME 1: neutral (D10)
{
	char Unknown0000[0x19A5];
	bool Unknown19A5;
};
extern Unknown_C404D8Struct *Unknown_C404D8;			// NAME 1: neutral (D10)

// The members' classes as far as the constructor and destructor build and free them
class Unknown619280Class		// NAME 1: neutral (D10): 0x44 bytes, constructor 0x619280 (before ui_ship_cust.cpp)
{
public:
	Unknown619280Class();								// 0x619280
	~Unknown619280Class();

	char Unknown00[0x44];
};

class SClientUnknown1334Class
{
public:
	~SClientUnknown1334Class();							// 0x618A50
	void Load(const char *textures, const char *swatches, const char *secret_textures);	// 0x75BE00 (decal.cpp)  // NAME 2: what it does  // NAME 3 [textures, swatches, secret_textures]: the INI names

	std::vector<int> Unknown00;							// +0x00
	std::vector<int> Unknown10;							// +0x10
	std::vector<int> Unknown20;							// +0x20
	Unknown619280Class Unknown30;						// +0x30
	Unknown619280Class Unknown74;						// +0x74
};

class Unknown67D360Class		// NAME 1: neutral (D10): 0x14 bytes, destructor 0x67D360
{
public:
	~Unknown67D360Class();								// 0x67D360

	char Unknown00[0x14];
};

class SClientUnknown1338Class
{
public:
	SClientUnknown1338Class() : Unknown10(2) {}
	void Load(const char *ini);							// 0x760360 (env_map_manager.cpp): 'envmaps.ini'  // NAME 2: what it does
	void Unknown_760770(int base_asset, bool a);		// 0x760770 (env_map_manager.cpp): 0x745000 / 0x745050 pass Unknown126C and Unknown1320  // NAME 1 [base_asset, a]: SERVER_PARAMETERS 66's BackdropBaseAsset; neutral

	std::vector<Unknown67D360Class> Unknown00;			// +0x00 (its destructor 0x67CEA0)
	int Unknown10;										// +0x10 (2)
	INIClass INI;										// +0x14  // NAME 2: what it is
};

class Unknown758F40Class		// NAME 1: neutral (D10): 0x0C bytes, destructor 0x758F40
{
public:
	~Unknown758F40Class();								// 0x758F40

	char Unknown00[0x0C];
};

class ProjDecalManagerClass
{
public:
	void Load(const char *ini);							// 0x763050 (proj_decal_manager.cpp): 'proj_dec.ini'  // NAME 2: what it does

	std::vector<Unknown758F40Class> Unknown00;			// +0x00 (its destructor 0x755CE0)
};

// The interface objects Init creates (each file's constructor before its first assert; kernel-map-step9.md
// 9f's interface base not rebuilt). The reference-counted ones keep their count in a second base at +0x38
// (RefCountPtr's release reads +0x3C and calls +0x38's slot 0; Shutdown releases the raw pointers so)
class SClientUnknownWindowClass	// NAME 1: neutral (D10): their first base (0x38 bytes, the interface base of kernel-map-step9.md 9f)
{
public:
	virtual ~SClientUnknownWindowClass();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();
	virtual void Unknown_3C();
	virtual void Unknown_40();
	virtual void Unknown_44();
	virtual void Unknown_48();
	virtual void Unknown_4C();
	virtual void Unknown_50();
	virtual void Unknown_54();							// +0x54 Init calls it after construction
	virtual void Unknown_58();
	virtual void Unknown_5C();
	virtual void Unknown_60();
	virtual void Unknown_64();
	virtual void Unknown_68();
	virtual void Unknown_6C();
	virtual void Unknown_70();
	virtual void Unknown_74();
	virtual void Unknown_78();
	virtual void Unknown_7C(ViewClass *view, int arg);	// +0x7C Init: the talk tree gets the main view and 0

	char Unknown04[0x38 - 0x04];
};

// What the chat panel's +0xE0 points to, as far as SClient calls it
class UIChatPanelUnknownE0Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_81BF40();								// 0x81BF40 (0x750210)

	char Unknown00[0x7C];
	bool Unknown7C;										// +0x7C (tested before every call)
};

class UIChatPanelClass : public SClientUnknownWindowClass, public RefCountClass
{
public:
	UIChatPanelClass(SClient *client);					// 0x561DD0

	char Unknown40[0xE0 - 0x40];
	UIChatPanelUnknownE0Class *UnknownE0;				// +0xE0 (0x750210, 0x7517E0, 0x751850 test its +0x7C)
	char UnknownE4[0x138 - 0xE4];
	UIChatPanelUnknown138Class *Unknown138;				// +0x138 (0x744E70 calls its slot +0xAC)
	char Unknown13C[0x268 - 0x13C];
};

class UILineDisplayClass : public SClientUnknownWindowClass, public RefCountClass
{
public:
	UILineDisplayClass(SClient *client);				// 0x5A5410

	char Unknown40[0x100 - 0x40];
};

class PDAInterfaceClass : public SClientUnknownWindowClass, public RefCountClass
{
public:
	PDAInterfaceClass(SClient *client);					// 0x693140

	char Unknown40[0xC0 - 0x40];
};

class UIHotKeysClass : public SClientUnknownWindowClass, public RefCountClass
{
public:
	UIHotKeysClass(SClient *client);					// 0x59C6F0

	char Unknown40[0x84 - 0x40];
};

class UITalkTreeSpaceClass : public SClientUnknownWindowClass, public RefCountClass
{
public:
	UITalkTreeSpaceClass(SClient *client);				// 0x64F310

	char Unknown40[0x184 - 0x40];
};

class BridgeInterfaceClass
{
public:
	BridgeInterfaceClass(SClient *client);				// 0x53B040
	~BridgeInterfaceClass();							// 0x53B380

	char Unknown00[0xC4];
};

class GalaxyMapClientClass
{
public:
	GalaxyMapClientClass();								// 0x7A4830
	virtual ~GalaxyMapClientClass();

	char Unknown04[0xCC - 0x04];
};

class UIStarmapClass
{
public:
	UIStarmapClass(SClient *client);					// 0x7C6120
	virtual ~UIStarmapClass();

	char Unknown04[0x14 - 0x04];
};

class CameraManagerClass
{
public:
	CameraManagerClass(SClient *client);				// 0x798D20
	~CameraManagerClass();								// 0x799220
	void Set_Heading(float heading);					// 0x79B370  // NAME 2: Init passes the camera mode's Heading
	void Set_Distance(float distance);					// 0x79B3B0  // NAME 2: Init passes the camera mode's Distance
	void Set_Tilt(float tilt);							// 0x79B3F0  // NAME 2: Init passes the camera mode's Tilt
	void Set_Elevation_Offset(float offset);			// 0x79B470  // NAME 2: Init passes the camera mode's ElevationOffset
	void Unknown_79B4B0(bool first_mode);				// 0x79B4B0: Init passes CameraMode == 0  // NAME 1 [first_mode]: what it is

	int Unknown00;										// +0x00
	int Unknown04;										// +0x04 (Init and Shutdown hand it to the notifier)
	char Unknown08[0x48 - 0x08];						// +0x08
	float FOV;											// +0x48 Init: the camera mode's FOV  // NAME 2: what it holds
};

class MusicManagerClass
{
public:
	MusicManagerClass();								// 0x439E40
	~MusicManagerClass();								// 0x43A040
	void Unknown_43AD70();								// 0x43AD70 (Shutdown, before the delete)
	void Unknown_43B760(unsigned sector);				// 0x43B760 (0x750A70: SERVER_PARAMETERS 66's SectorNum)  // NAME 1 [sector]: what it gets

	char Unknown00[0x1FC];
};

class SClientUnknown1330Class
{
public:
	SClientUnknown1330Class(unsigned sector);			// 0x43C4A0 (0x750A70, 0x58 bytes)  // NAME 1 [sector]: what 0x750A70 passes
	~SClientUnknown1330Class();							// 0x43C560
	char Unknown00[0x58];
	void Unknown_43C660(int a, int b);					// 0x43C660 (0x750B30 passes the base's Unknown10BC and its argument)  // NAME 1 [a, b]: neutral
};

class SClientUnknown12BCClass
{
public:
	virtual ~SClientUnknown12BCClass();
};

// The 0x50-byte view at SClient +0x1358 (constructor 0x514CC0, between mainview.cpp and smouse.cpp)
class Unknown514CC0ViewClass : public ViewClass	// NAME 1: neutral (D10)
{
public:
	Unknown514CC0ViewClass(ClientTaskClass *task);		// 0x514CC0

	char Unknown38[0x50 - 0x38];
};

// The camera manager's callback (vtable 0xB032D8 over the base's 0xAEDB58): an object and a member function
class SClientCallbackBaseClass	// NAME 1: neutral (D10): vtable 0xAEDB58 (slot 0 0x51E220)
{
public:
	virtual void Call();
};

class SClientCallbackClass : public SClientCallbackBaseClass	// NAME 1: neutral (D10): vtable 0xB032D8 (slot 0 0x7599B0)
{
public:
	SClientCallbackClass(SClient *object, void (SClient::*function)()) : Object(object), Function(function) {}
	virtual void Call();

	SClient *Object;									// +0x04
	void (SClient::*Function)();						// +0x08 (8 bytes: SClient has two bases)
};

// What 0x79B9E0 returns (camera_manager.cpp's neighbourhood): it keeps the callbacks per camera manager id
class SClientNotifierClass		// NAME 1: neutral (D10)
{
public:
	void Add(int id, SClientCallbackBaseClass *callback);		// 0x4F9740  // NAME 2: what Init does with it
	void Remove(int id, SClientCallbackBaseClass *callback);	// 0x4F98D0  // NAME 2: what Shutdown does with it
};
SClientNotifierClass *Unknown_79B9E0();					// NAME 1 [Unknown_79B9E0]: neutral (D10)

// The font database Init asks (fontdatabase.cpp's at 0xBE67BC; the options at 0xBE8B38 are tsk_game.h's)
class SClientFontDatabaseClass	// NAME 1: neutral (D10): fontdatabase.cpp's class
{
public:
	void Unknown_6C52B0(int arg);						// 0x6C52B0 (Init passes 0)
};
extern SClientFontDatabaseClass *Unknown_BE67BC;		// NAME 1: neutral (D10)

extern bool Unknown_BBB084;								// NAME 1: neutral (D10): Init clears it
extern RectClass Unknown_C07550;						// NAME 1: neutral (D10): the text's coordinate range
void Unknown_455D10(PxProcessClass *process);			// 0x455D10 (px_process.cpp's neighbourhood)
void Unknown_455F60();									// 0x455F60 (px_process.cpp's neighbourhood)
void Unknown_6E6A90(EffectObjectClass *effect);			// 0x6E6A90 (eff_o.cpp): Shutdown, per effect left
void Unknown_719D50(SClient *client);					// 0x719D50 (control_keys_manager.cpp): Init's

static int INIUsers;									// 0xBE81C0: Load_INIs / Unload_INIs  // NAME 2: what it counts

// 0x7405E0
SClient::SClient(ClientTaskClass *task) :
	Unknown1198(true),
	Unknown126C(-1),
	Unknown1270(NULL),
	Unknown1274(NULL),
	Unknown1278(0),
	LineDisplay(NULL),
	PDAInterface(NULL),
	Unknown1288(0),
	BridgeInterface(NULL),
	HotKeys(NULL),
	VerbManager(NULL),
	GalaxyMap(NULL),
	Starmap(NULL),
	CameraManager(NULL),
	Unknown12B0(0),
	Unknown12B4(0),
	Unknown12B8(0),
	Unknown12BC(0),
	AuxData(0),
	Unknown12C4(1),
	Unknown12D4(0),
	Ticket(0),
	Unknown1320(true),
	Unknown1324(0),
	MusicManager(NULL),
	Unknown1330(0),
	Unknown1334(NULL),
	Unknown1338(NULL),
	ProjDecalManager(NULL),
	CurrentView(0),
	MainView(NULL),
	Unknown1358(NULL),
	Unknown135C(NULL),
	Movie3DView(NULL),
	ScriptedMovieView(NULL),
	LoadingView(NULL),
	ViewMode(2),
	Unknown1370(3),
	Unknown1374(3),
	Unknown1378(0),
	Unknown137C(false),
	Unknown1380(0),
	Task(task),
	Unknown1388(false),
	Unknown138C(0),
	Unknown1390(0),
	Unknown13AC(0),
	Unknown13B0(0),
	Unknown13B4(0),
	Unknown13B8(0),
	Unknown13BC(0),
	Unknown13C0(false),
	Unknown13C1(false),
	Text(NULL),
	Unknown14EC(0),
	Unknown14F0(false),
	Unknown14F4(0)
{
	Unknown12C4[0] = NULL;
	Unknown1334 = new SClientUnknown1334Class;
	Unknown1338 = new SClientUnknown1338Class;
	ProjDecalManager = new ProjDecalManagerClass;
	Unknown_C404D8->Unknown19A5 = true;
	Unknown119C.size();		// dead: keeps the string's _Tidy out of line, as the client has it (D40's lever)
}

// NOT EXACT: inline budget (the client inlines the 0x24-byte object's destructor at the second delete, ours calls it: then every EH state after it is one lower; five forms, attempts.py show 0x740BB0)
// 0x740BB0
SClient::~SClient()
{
	Unknown_719E10(this);
	delete Unknown1334;
	Unknown1334 = NULL;
	delete Unknown1338;
	Unknown1338 = NULL;
	delete ProjDecalManager;
	ProjDecalManager = NULL;
	std::hash_map<unsigned, SClientUnknown1394Class *>::iterator it;
	for (it = Unknown1394.begin(); it != Unknown1394.end(); it++) {
		SClientUnknown1394Class *object = (*it).second;
		if (object) {
			object->Unknown_38();
			object->Release_Ref();
		}
	}
	Unknown1394.clear();
}

// 0x740F60
void SClient::Load_INIs()
{
	if (INIUsers == 0) {
		Unknown_7228C0();
		Unknown_6766E0("action.ini");
		Unknown_817070("cskill_t.ini", "cskill_ex_t.ini");
		Unknown_814620("cskill_ability_t.ini", "cskill_ability_ex_t.ini");
		Unknown_74CA90("testeffects.ini");
		Unknown_70A7E0(std::string("gutter_color.ini"));
		Unknown_Load_Assets(std::string("cbasset.ini"));
	}
	INIUsers++;
}

// NOT EXACT: field offset (explain.py; 103 masked rows): the client keeps the options base (+0x1150) in a register from the Bridge_Mode branch and reads the camera mode through it, and its frame is 4 bytes larger; one form (attempts.py show 0x741180)
// 0x741180
void SClient::Init()
{
	Unknown_BE67BC->Unknown_6C52B0(0);
	VerbManager = new VerbManagerClass(this);
	Unknown_455D10(PxProcess);
	Unknown_BBB084 = false;
	Unknown_745350();
	Unknown13CC.Unknown_765010();
	INIClass metals;
	metals.Load("metals.ini");
	Unknown18->Unknown_763E70("rs.ini");
	Unknown1334->Load("decal_textures.ini", "decal_swatches.ini", "secret_decal_textures.ini");
	ProjDecalManager->Load("proj_dec.ini");
	Unknown1270 = new LoginUnknown0CClass;
	Unknown_7451D0();
	Load_INIs();
	Unknown_719D50(this);
	Unknown1274 = new LoginUnknown18Class((int)this);
	Unknown1274->Unknown_515120(2);
	ChatPanel = Create_NEW(new UIChatPanelClass(this));
	ChatPanel->Unknown_54();
	LineDisplay = new UILineDisplayClass(this);
	LineDisplay->Unknown_54();
	GalaxyMap = new GalaxyMapClientClass;
	Starmap = new UIStarmapClass(this);
	PDAInterface = new PDAInterfaceClass(this);
	PDAInterface->Unknown_54();
	BridgeInterface = new BridgeInterfaceClass(this);
	HotKeys = new UIHotKeysClass(this);
	MainView = new MainViewClass(Task);
	MainView->Unknown_04();
	MainView->Unknown_08();
	LoadingView = new LoadingViewClass(Task);
	LoadingView->Unknown_04();
	Unknown1338->Load("envmaps.ini");
	Unknown1358 = new Unknown514CC0ViewClass(Task);
	Unknown1358->Unknown_04();
	Unknown1358->Unknown_08();
	Unknown129C = Create_NEW(new UITalkTreeSpaceClass(this));
	Unknown129C->Unknown_54();
	Unknown129C->Unknown_7C(MainView, 0);
	{
		RefCountPtr<UITalkTreeSpaceClass> talk(Unknown129C);
		if (talk.Peek() != Unknown1298.Peek()) {
			Unknown1298 = talk;
		}
	}

	CameraClass *camera = new CameraClass;
	camera->Set_Clip_Planes(6.0f, 600000.0f);
	MainView->Set_Camera(camera);
	camera->Release_Ref();

	CameraManager = new CameraManagerClass(this);
	int id = CameraManager->Unknown04;
	SClientNotifierClass *notifier = Unknown_79B9E0();
	notifier->Add(id, new SClientCallbackClass(this, &SClient::Unknown_744E50));
	if (Unknown_BE8B38->Get_Bool("Bridge_Mode")) {
		Set_Camera_Mode(0);
	} else {
		Unknown_75A5D0();
	}
	Unknown12B0 = Unknown10BC;
	Unknown12B4 = 0;
	CameraModeStruct &mode = CameraModes[CameraMode];
	CameraManager->Set_Heading(mode.Heading);
	CameraManager->Set_Distance(mode.Distance);
	CameraManager->Set_Tilt(mode.Tilt);
	CameraManager->Set_Elevation_Offset(mode.ElevationOffset);
	CameraManager->Unknown_79B4B0(CameraMode == 0);
	CameraManager->FOV = mode.FOV;
	Unknown_744C10();

	MusicManager = new MusicManagerClass;
	Unknown_7518A0();
	Unknown_455F60();
	Unknown1380 = 0;
	if (Unknown137C) {
		MainView->Unknown_514220();
	} else {
		MainView->Unknown_5141A0();
	}
	Unknown1374 = 3;

	Font = Create_NEW(new Font3DInstanceClass("font_med10.tga"));
	Text = new Render2DTextClass(Font.Peek());
	Text->Set_Coordinate_Range(Unknown_C07550);
	Text->Enable_Alpha(true);
	Text->Reset();
}

// 0x741B70
void SClient::Unload_INIs()
{
	if (--INIUsers == 0) {
		Unknown_723150();
		Unknown_676A00();
		Unknown_816E60();
		Unknown_814430();
		Unknown_70AD70();
		Unknown_Free_Assets();
	}
}

// NOT EXACT: shape (explain.py: field offset first; 470 masked rows): the talk tree's reset goes through a null RefCountPtr temporary with its own EH state and the out-of-line assignment 0x758970, and the effect list's removal is inlined differently; one form (attempts.py show 0x741BB0)
// 0x741BB0
void SClient::Shutdown()
{
	Unknown129C = NULL;
	if (Unknown1298.Peek() != NULL) {
		Unknown1298 = Unknown129C;
	}
	Unknown_742530(0);
	Unknown_455D10(PxProcess);

	// two empty frames
	SimpleSceneClass *scene = new SimpleSceneClass;
	CameraClass *camera = new CameraClass;
	WW3D::Begin_Render(true, true, Vector3(0, 0, 0));
	WW3D::Render(scene, camera, true, true, Vector3(0, 0, 0));
	WW3D::End_Render(true);
	WW3D::Begin_Render(true, true, Vector3(0, 0, 0));
	WW3D::Render(scene, camera, true, true, Vector3(0, 0, 0));
	WW3D::End_Render(true);
	scene->Release_Ref();
	camera->Release_Ref();

	MusicManager->Unknown_43AD70();
	delete MusicManager;
	MusicManager = NULL;
	delete Unknown1330;
	Unknown1330 = NULL;

	int id = CameraManager->Unknown04;
	SClientNotifierClass *notifier = Unknown_79B9E0();
	SClientCallbackClass callback(this, &SClient::Unknown_744E50);
	notifier->Remove(id, &callback);
	delete CameraManager;
	CameraManager = NULL;

	if (MainView) {
		MainView->Unknown_10();
	}
	if (Unknown1358) {
		Unknown1358->Unknown_10();
	}
	if (Unknown135C) {
		Unknown135C->Unknown_10();
	}
	if (Movie3DView) {
		Movie3DView->Unknown_10();
	}
	if (ScriptedMovieView) {
		ScriptedMovieView->Unknown_10();
	}
	if (LoadingView) {
		LoadingView->Unknown_10();
	}
	REF_PTR_RELEASE(LineDisplay);
	REF_PTR_RELEASE(PDAInterface);
	delete GalaxyMap;
	GalaxyMap = NULL;
	delete Starmap;
	Starmap = NULL;
	delete BridgeInterface;
	BridgeInterface = NULL;
	REF_PTR_RELEASE(HotKeys);
	ChatPanel = NULL;
	delete MainView;
	MainView = NULL;
	delete Unknown1358;
	Unknown1358 = NULL;
	delete Unknown135C;
	Unknown135C = NULL;
	delete Movie3DView;
	Movie3DView = NULL;
	delete ScriptedMovieView;
	ScriptedMovieView = NULL;
	delete Unknown1274;
	Unknown1274 = NULL;
	delete LoadingView;
	LoadingView = NULL;
	if (Unknown18) {
		Unknown18->Unknown_763FF0();
		delete Unknown18;
		Unknown18 = NULL;
	}
	delete Unknown1334;
	Unknown1334 = NULL;
	delete Unknown1338;
	Unknown1338 = NULL;
	delete ProjDecalManager;
	ProjDecalManager = NULL;
	Unload_INIs();
	delete Unknown1270;
	Unknown1270 = NULL;
	delete Unknown12BC;
	Unknown12BC = NULL;
	Unknown_7555B0();

	// the effects left: out of the list, then eff_o.cpp's 0x6E6A90 for each
	HashNodeClass<EffectObjectClass *> *node = EffectList.First_Valid();
	while (node) {
		HashNodeClass<EffectObjectClass *> *next = node->Next_Valid();
		EffectObjectClass *effect = node->Get_Record();
		EffectList.Remove(node);
		Unknown_6E6A90(effect);
		node = next;
	}

	delete VerbManager;
	VerbManager = NULL;
	WW3D::Stop_Movie_Capture();
	Font = NULL;
	delete Text;
	Text = NULL;
	Unknown_455F60();
}

// 0x7427A0
void SClient::Unknown_08()
{
}

// 0x7427C0
void SClient::Process_Packets()
{
	WWPROFILE("SClient::Process_Packets");

	PacketClass *packet;
	while ((packet = ((PeekConnectionClass *)Connection)->Peek()) != NULL) {
		if (Task->Is_Focused() && LoadingView->Unknown68) {
			LoadingView->Unknown_0C(true);
		}
		if (!packet->Process_Packet(this)) {
			return;
		}
		delete Connection->Receive();
	}
}

// 0x7428F0
void SClient::Unknown_10()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			if (msg.message == WM_PAINT) {
				RedrawWindow(Unknown_Main_Window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
			}
			DispatchMessage(&msg);
		}
	}
}

// 0x743330
void SClient::Set_Debug_Mode(bool on)
{
	ClientClass::Set_Debug_Mode(on);
	Print(0x17, "Debug mode turned %s.", on ? "ON" : "OFF");
}

// 0x744B90
void SClient::Add_Object(ClientObjectClass *object)
{
	ClientClass::Add_Object(object);
	MainView->Add_Object(object);
}

// 0x744BD0
ClientObjectClass *SClient::Remove_Object(unsigned id)
{
	ClientObjectClass *object = ClientClass::Remove_Object(id);
	if (object) {
		MainView->Remove_Object(object);
	}
	return(object);
}

// 0x745730
unsigned SClient::Set_Debug_Object(unsigned id)
{
	return(ClientClass::Set_Debug_Object(id));
}





























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// NOT EXACT: shape (explain.py; 35 masked rows): the client lays the chat panel's false call last and jumps to it from both tests; four forms (attempts.py show 0x744E70)
// 0x744E70
void SClient::Unknown_744E70(bool arg)
{
	if ((CurrentView != NULL && CurrentView == Unknown135C) || arg) {
		if (ChatPanel && ChatPanel->Unknown138) {
			ChatPanel->Unknown138->Unknown_AC(false);
		}
		return;
	}

	ClientObjectClass *target = MainView->Unknown_5C();



	FAIL_IF(!target) return;
	if (target->Type == 1) {
		if (ChatPanel && ChatPanel->Unknown138) {
			ChatPanel->Unknown138->Unknown_AC(true);
		}
		return;
	}
	ActionPacketClass packet(DebugObjectID, 2, target->Get_Game_ID(), 0);
	Send_Packet(&packet);
}


































// 0x745160
void SClient::Unknown_745160()
{
	Unknown1198 = false;
	FAIL_IF(!MainView) return;
	UICpitClass *cockpit = MainView->Cpit;
	FAIL_IF(!cockpit) return;
	cockpit->Unknown_584A60();
}

















// 0x745000
void SClient::Unknown_745000(int base_asset)
{
	Unknown126C = base_asset;
	MainView->Unknown_513540(base_asset);
	Unknown1338->Unknown_760770(base_asset, Unknown1320);
}

// 0x745050
void SClient::Unknown_745050()
{
	Unknown1338->Unknown_760770(Unknown126C, Unknown1320);
}

// NOT EXACT: shape (explain.py; 15 masked rows): the client copies Get_Point's returned point through eax into a local before the fld/fstp into the element; four forms (attempts.py show 0x7451D0)
// 0x7451D0
void SClient::Unknown_7451D0()
{
	TPoint3D<float> white(1.0f, 1.0f, 1.0f);
	INIClass ini;
	ini.Load("colors.ini");
	char section[256];
	int i = 0; while (true) {
		_snprintf(section, 255, "Color%02d", i);
		if (!ini.Find_Section(section)) break;
		if (i >= Unknown1340.Length()) {
			((VectorClass<Vector3> &)Unknown1340).Resize(i + 10);
		}
		TPoint3D<float> color = ini.Get_Point(section, "RGB", white);
		Unknown1340[i] = Vector3(color.X, color.Y, color.Z);
		i++; }
}




























// 0x7456B0
void SClient::Unknown_7456B0()
{
	MainView->Unknown_513CD0(Unknown10F4);
	MainView->Unknown_513D20(1.0f);
	MainView->Unknown_513600(0);
	MainView->Unknown130 = true;
	MainView->Unknown_64()->Unknown_24(0);
}



// 0x745350
void SClient::Unknown_745350()
{
	INIClass ini;
	char filename[256];
	_snprintf(filename, 255, "cam%d.ini", Ticket.Character_ID);
	ini.Load(filename);
	if (!ini.Is_Loaded()) {
		ini.Load("spacecam.ini");
		ini.Save(filename);
	}
	char section[256];
	int i = 0; while (true) {
		sprintf(section, "Mode%02d", i);
		if (!ini.Find_Section(section)) break;
		Load_Camera_Mode(ini, section);
		i++; }
}



// NOT EXACT: field offset (explain.py; first: the FOV load): sprintf's CameraMode load is scheduled into another register and the Put_Float loads shift; four forms (attempts.py show 0x745490)
// 0x745490
bool SClient::Unknown_745490()
{
	INIClass ini;
	char section[256];
	char filename[256];
	_snprintf(filename, 255, "cam%d.ini", Ticket.Character_ID);
	ini.Load(filename);
	sprintf(section, "Mode%02d", CameraMode);
	if (!ini.Find_Section(section)) {
		return(false);
	}
	CameraModeStruct &mode = CameraModes[CameraMode];
	ini.Put_Float(section, "Heading", RAD_TO_DEG(mode.Heading));
	ini.Put_Float(section, "Distance", mode.Distance);
	ini.Put_Float(section, "Tilt", RAD_TO_DEG(mode.Tilt));
	ini.Put_Float(section, "FOV", RAD_TO_DEG(mode.FOV));
	ini.Put_Float(section, "ElevationOffset", RAD_TO_DEG(mode.ElevationOffset));
	return(ini.Save(filename) > 0);
}






// 0x745EC0
void SClient::Unknown_745EC0(FileClass *file, int indent)
{
	file->Printf_Indented(indent, "Client %x\n", this);
	file->Printf_Indented(indent, "--------------------\n");
	if (MainView) {
		MainView->Unknown_513F60(file, indent + 2);
	}
}





















// 0x745F30
void SClient::Unknown_745F30(int type)
{
	int old_mode = ViewMode; for (int i = 0; i < 0; i++) {}
	ViewMode = type;
	switch (type) {
		case 0:
		case 1:
		case 5:
			Unknown_742530(MainView);
			break;

		case 2:
			Unknown_742530(Unknown1358);
			break;

		case 4:
			Unknown_742530(Unknown135C);
			break;

		case 3:
			Unknown_742530(Movie3DView);
			break;

		case 6:
			Unknown_742530(ScriptedMovieView);
			break;
	}
	MainView->Unknown_514010(old_mode, type);
	Unknown1374 = 3;
}

































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// 0x74BB50
void SClient::Unknown_74BB50(int type)
{
	if (Unknown138C) {
		switch (type) {
			case 1:
				Unknown_74D990(0x13, "Logging off to desktop.", 0);
				break;

			case 2:
				Unknown_74D990(0x13, "Logging off to login screen.", 0);
				break;

			case 3:
				Unknown_74D990(0x13, "Logging off to character selection.", 0);
				break;
		}
	}
	if (Connection) {
		;
		Connection->Send_Packet(&LogoffRequestPacketClass(Unknown1138));
	}
	Unknown138C = type;
}




// 0x74BC20
void SClient::Unknown_74BC20(int arg)
{

	G_ASSERT(!Movie3DView);
	Movie3DView = new Movie3DViewClass(Task, arg);
	Movie3DView->Unknown_04();
	Movie3DView->Unknown_08();
	Unknown1370 = ViewMode;
	Unknown_745F30(3);
}

// 0x74BD10
void SClient::Unknown_74BD10()
{



	G_ASSERT(Movie3DView);
	Unknown_745F30(Unknown1370);
	Movie3DView->Unknown_10();
	delete Movie3DView;
	Movie3DView = NULL;
}

// 0x74BD90
void SClient::Unknown_74BD90(int arg)
{



	G_ASSERT(!ScriptedMovieView);
	ScriptedMovieView = new ScriptedMovieViewClass(Task, arg, new SClientMovieCallbackClass(this));
	Unknown1370 = ViewMode;
	Unknown_745F30(6);
}

// 0x74BF00
void SClient::Unknown_74BF00(int mode)
{

	G_ASSERT(ScriptedMovieView);
	Unknown_745F30(mode);
	ScriptedMovieView->Unknown_10();
	delete ScriptedMovieView;
	ScriptedMovieView = NULL;
}







// 0x74BF80
ClientObjectClass *SClient::Unknown_74BF80()
{
	if (Unknown1328) {
		return(Find_Object(Unknown1328));
	}
	return(NULL);
}






































































































// 0x74C370
void SClient::Unknown_74C370(RefCountClass *object)
{
	if (Unknown12B8) {
		Unknown12B8->Release_Ref();
	}
	Unknown12B8 = object;
	if (object) {
		object->Add_Ref();
	}
}





























// 0x74C7B0
Unknown77E3F0Class *SClient::Unknown_74C7B0(unsigned id)
{
	std::hash_map<unsigned, SClientUnknown1394Class *>::iterator it = Unknown1394.find(id);
	if (it != Unknown1394.end() && (*it).second) {
		Unknown77E3F0Class *object = (*it).second->Unknown_77E3F0();
		object->Unknown_780810(Unknown1338);
		return(object);
	}
	return(NULL);
}

// 0x74C840
Unknown77E3F0Class *SClient::Unknown_74C840(unsigned id)
{
	std::hash_map<unsigned, SClientUnknown1394Class *>::iterator it = Unknown1394.find(id);
	if (it != Unknown1394.end() && (*it).second) {
		Unknown77E3F0Class *object = (*it).second->Unknown_77E610();
		object->Unknown_780810(Unknown1338);
		return(object);
	}
	return(NULL);
}

// 0x74C960
int *SClient::Unknown_74C960(unsigned id)
{
	std::hash_map<unsigned, SClientUnknown1394Class *>::iterator it = Unknown1394.find(id);
	if (it != Unknown1394.end() && (*it).second) {
		return(&(*it).second->UnknownF8);
	}
	return(NULL);
}

// 0x74C9D0
void SClient::Unknown_74C9D0(const RefCountPtr<UITalkTreeSpaceClass> &tree)
{
	if (!(tree == Unknown1298)) {
		if (tree) {
			Unknown1298 = tree;
		} else {
			Unknown1298 = Unknown129C;
		}
	}
}




















































































































































































bool Unknown_70BAC0(SClient *client, ClientObjectClass *co);	// 0x70BAC0 (cdecl; 0x74D780 skips such an object)  // NAME 1 [client, co]: what 0x74D780 passes

// 0x74D780
bool SClient::Unknown_74D780(ClientObjectClass *co)
{

	FAIL_IF(!co) return(false);

	ClientObjectClass *avatar = Get_Debug_Object();
	FAIL_IF(!avatar) return(false);
	if (co == avatar) {
		return(false);
	}
	if (Unknown_70BAC0(this, co)) {
		return(false);
	}
	return(Unknown_74D870() && co->Unknown_6F5ED0());
}








// 0x74D870
bool SClient::Unknown_74D870()
{
	ClientObjectClass *co = Get_Debug_Object();
	FAIL_IF(!co) return(false);
	return(co->Unknown_6F5ED0());
}

bool SClient::Unknown_74D8B0(ClientObjectClass *co)	// 0x74D8B0
{
	ClientObjectClass *player_co = Get_Debug_Object();
	FAIL_IF(!player_co) return(false);
	FAIL_IF(!co) return(false);
	return(player_co->Unknown_6F5F60() || co->Unknown_6F5F60());
}
































































































































































































































































































































































































































































































































































// 0x750210
void SClient::Unknown_750210()
{
	Unknown_74FA30();
	Unknown_74FEC0();
	if (ChatPanel) {
		UIChatPanelUnknownE0Class *panel = ChatPanel->UnknownE0;
		if (panel && panel->Unknown7C && Unknown_728300()) {
			panel->Unknown_81BF40();
		}
	}
}


















































































































// 0x750A70
void SClient::Unknown_750A70(unsigned sector)
{
	if (MusicManager) {
		MusicManager->Unknown_43B760(sector);
	}
	delete Unknown1330;
	Unknown1330 = new SClientUnknown1330Class(sector);
}















// 0x750B30
void SClient::Unknown_750B30(int arg)
{
	if (Unknown1330) {
		Unknown1330->Unknown_43C660(Unknown10BC, arg);
	}
}






































































































































































































































































































































































/*
**	Reconstruction notes
**
**	The skeleton (D90 9d): the layout (sclient.h) and the functions in the window before the first
**	assert (0x7429B0, l.749), plus the base slots' overrides the brief names. Init 0x741180 and Shutdown
**	0x741BB0 are functional rewrites (marked); the frame 0x742B60, Render 0x744690 and the chat commands 0x74ABC0
**	are declared only (clienttest stands in for them).
**	The body (T18): the small helpers at their place between the client's assert lines (the file keeps the
**	client's __LINE__s through blank padding, as cpacket.cpp: 0x745160's FAIL_IFs on lines 2158 / 2160).
**	0x745F30's empty loop compiles to nothing; it spends the inline budget so that VC6 calls the view-mode
**	setter out of line from the movie views (0x74BC20-0x74BF00), as the client does (workflow.md 3.1: the
**	budget counts loops in an inlined callee). The movie callback stores its vtable before Client: a body
**	assignment, not an initialiser.
**	cpacket.cpp's handlers are PacketClass's slot +0x08 (Process_Packet(SClient *)): Process_Packets
**	peeks the connection (PeekConnectionClass +0x34), hands the packet to its handler and, when that
**	returns true, takes it off the connection (+0x0C) and deletes it; false leaves it for the next frame.
*/
