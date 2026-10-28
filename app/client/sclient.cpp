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
// (UIUnknownBaseClass, their first base, is in sclient.h with pdaint.cpp's PDAInterfaceClass: the main view reads the PDA)

// The camera manager's +0x04, as far as SClient 0x744C10 reads it
class ChaseCameraModuleClass	// NAME 2: the FAIL_IF '!CameraManager->Get_Player_Chase_Camera_Module()' (0x744C10)
{
public:
	char Unknown00[0x34];
	bool Unknown34;										// +0x34 (0x744C10 asks the main view's slot +0x78 only when set)
};

typedef char Unknown_RefCountPtr_Size[sizeof(RefCountPtr<RefCountClass>)];	// NAME 1: not the client's: instantiates RefCountPtr<RefCountClass>, so 0x744C10's Create_Peek(RefCountClass *) 0x514960 is visible (VC6 injects a template's friends only once it is instantiated)




























// What the chat panel's +0xE0 points to, as far as SClient calls it
class UIChatPanelUnknownE0Class	// NAME 1: neutral (D10)
{
public:
	void Unknown_81BF40();								// 0x81BF40 (0x750210)

	char Unknown00[0x7C];
	bool Unknown7C;										// +0x7C (tested before every call)
};

class UIChatPanelClass : public UIUnknownBaseClass, public RefCountClass
{
public:
	UIChatPanelClass(SClient *client);					// 0x561DD0
	bool Unknown_56BA80();								// 0x56BA80 (Do_Frame compares it with 'more than 15' from the connection's Get_Stats)
	void Unknown_56BA40(bool on);						// 0x56BA40 (Do_Frame, when that changes)  // NAME 1 [on]: what Do_Frame passes
	void Unknown_565F30(int arg); void Unknown_56BAA0(int a, const char *text);	// 0x565F30 (0x7436A0: Esc, 2); 0x56BAA0 (0x74E710: 1 and the confirmed action's message)  // NAME 1 [arg]: what 0x7436A0 passes  // NAME 1 [a, text]: what 0x74E710 passes
	char Unknown40[0xE0 - 0x40];
	UIChatPanelUnknownE0Class *UnknownE0;				// +0xE0 (0x750210, 0x7517E0, 0x751850 test its +0x7C)
	char UnknownE4[0x138 - 0xE4];
	UIChatPanelUnknown138Class *Unknown138;				// +0x138 (0x744E70 calls its slot +0xAC)
	char Unknown13C[0x264 - 0x13C]; bool MessageButtonShowing; char Unknown265[3]; bool Is_Message_Button_Showing() { return(MessageButtonShowing); }	// +0x264 (0x74FEC0's FAIL_IF names the getter)  // NAME 2 [MessageButtonShowing]: its getter's
};

class UILineDisplayClass : public UIUnknownBaseClass, public RefCountClass
{
public:
	UILineDisplayClass(SClient *client);				// 0x5A5410

	char Unknown40[0x100 - 0x40];
};









class UIHotKeysClass : public UIUnknownBaseClass, public RefCountClass
{
public:
	UIHotKeysClass(SClient *client);					// 0x59C6F0

	char Unknown40[0x84 - 0x40];
};

class UITalkTreeSpaceClass : public UIUnknownBaseClass, public RefCountClass
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
	void Unknown_53BD40(int camera);					// 0x53BD40 (Do_Frame passes the main view's +0x7C when +0x14 is set)  // NAME 1 [camera]: Process_AI reads that result as a render object's position
	void Unknown_53BBC0(const RefCountPtr<RefCountClass> &scene); void Unknown_53B7A0(int *value);	// 0x53BBC0 (0x744C10: Create_Peek 0x514960 of the main view's slot +0x78 scene, or NULL); 0x53B7A0 (0x74C3B0: the player's new +0x1394 value's +0xF8)  // NAME 1 [scene, value]: what 0x744C10 / 0x74C3B0 pass
	char Unknown00[0x14];
	int Unknown14;										// +0x14 (Do_Frame tests it)
	char Unknown18[0xC4 - 0x18];
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
	void Set_Heading(float heading); float Get_Heading();	// 0x79B370, 0x79B390 (0x7436A0's Save Camera)  // NAME 2: Init passes the camera mode's Heading
	void Set_Distance(float distance); float Get_Distance();	// 0x79B3B0, 0x79B3D0  // NAME 2: Init passes the camera mode's Distance
	void Set_Tilt(float tilt); float Get_Tilt();		// 0x79B3F0, 0x79B410  // NAME 2: Init passes the camera mode's Tilt
	void Set_Elevation_Offset(float offset); float Get_Elevation_Offset();	// 0x79B470, 0x79B490  // NAME 2: Init passes the camera mode's ElevationOffset
	void Unknown_79B4B0(bool first_mode);				// 0x79B4B0: Init passes CameraMode == 0  // NAME 1 [first_mode]: what it is
	ChaseCameraModuleClass *Get_Player_Chase_Camera_Module() { return((ChaseCameraModuleClass *)Unknown04); }	void Unknown_799E40(); void Unknown_79B2B0(bool on);	// inline (0x744C10's FAIL_IF); 0x799E40 (Process_AI, unless the current view is +0x135C); 0x79B2B0 (0x745750 passes true)  // NAME 1 [on]: what 0x745750 passes

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
	virtual bool Unknown_04();							// +0x04 (Do_Frame deletes the object when it is true)
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
// (Init, Shutdown and Do_Frame push PxProcess on px_process.h's PxProcessClass::Push 0x455D10 and pop it with
// PxProcessClass::Pop 0x455F60)
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
	PxProcessClass::Push(PxProcess);
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
	PxProcessClass::Pop();
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
	PxProcessClass::Push(PxProcess);

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
	PxProcessClass::Pop();
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
#include "..\packets\pkclientchat.h"
// 0x744D80 (inline: Init's camera block, 0x7436A0's camera keys; 0x6B58A0 calls the out-of-line copy)
inline void SClient::Unknown_744D80() {
	Unknown12B0 = Unknown10BC; Unknown12B4 = 0;
	CameraModeStruct &mode = CameraModes[CameraMode];
	CameraManager->Set_Heading(mode.Heading);
	CameraManager->Set_Distance(mode.Distance);
	CameraManager->Set_Tilt(mode.Tilt);
	CameraManager->Set_Elevation_Offset(mode.ElevationOffset);
	CameraManager->Unknown_79B4B0(CameraMode == 0);
	CameraManager->FOV = mode.FOV;
	Unknown_744C10(); }
// NOT EXACT: register (explain.py register-only, 2 rows): the client loads the dialog's table into eax for the button > 1 call, ours into edx; the packet is a temporary (a named local cost 6 rows)
// 0x7429B0 (Do_Frame, after the lost-connection check: the reset-ship question; out of line as the client has it)
#pragma auto_inline(off)
void SClient::Unknown_7429B0()
{
	if (Unknown1378 == 1) {


		FAIL_IF(!CurrentView) return;
		ViewClass *ui_view = CurrentView->Unknown_1C();

		FAIL_IF(!ui_view) return;
		ViewUnknown34Class *dialog = ui_view->Get_Unknown34();

		FAIL_IF(!dialog) return;
		int button = dialog->Unknown_5890B0("YES_NO");
		if (button == 1) {
			dialog->Unknown_5C();
			Unknown1378 = 0;
			Send_Packet(&ClientChatPacketClass(AvatarID, "/resetship", 4, NULL));

			return;
		} else if (button > 1) {

			dialog->Unknown_5C();
			Unknown1378 = 0;
		}
	}
}
#pragma auto_inline(on)


// sound_manager.cpp's sound manager (the global 0xBE82A4), as far as Do_Frame calls it (cpacket.cpp declares its other members)
class SoundManagerClass	// NAME 3: class-from-file (D24) for app\client\sound_manager.cpp
{
public:
	void Unknown_75B320();								// 0x75B320 (Do_Frame, after Process_Packets)
};
extern SoundManagerClass *SoundManager;					// 0xBE82A4  // NAME 2: what it holds
extern int Unknown_BE350C;								// NAME 1: neutral (D10): Do_Frame sets it to the base's toggled +0x1128 for the frame
extern int Unknown_BE5F34;								// NAME 1: neutral (D10): Do_Frame takes up to 20 more events while it has 0x1100
unsigned int Unknown_Get_Ticks();						// 0x4A3B40, the aux-data clock (unknown_root.h)
void Unknown_918200(int ticks);							// 0x918200 (Do_Frame passes the new +0x10BC when focused)  // NAME 1 [ticks]: what it gets
void Unknown_42EDB0();									// NAME 1 [Unknown_42EDB0]: neutral (D10) (input.cpp's neighbourhood), as tsk_login.cpp
bool Unknown_42EA80(KernelEventClass *event);			// NAME 1 [Unknown_42EA80]: the input queue's next event (input.cpp), as tsk_login.cpp
void Unknown_42E8A0(KernelEventClass *event);			// NAME 1 [Unknown_42E8A0]: hands the event on (input.cpp), as tsk_login.cpp
KernelEventClass Unknown_65F780(void *unknown);			// NAME 1 [Unknown_65F780]: neutral (D10) (gadget.cpp), as tsk_login.cpp

#include "..\audio\soundlib.h"
#include "..\shared\auxdata\delta.h"

// 0x742B60 (placed in the blank run after Process_AI: no assert line moves)
int SClient::Do_Frame(bool focused)
{
	if (Connection->Is_Dead()) {
		if (Unknown1198) {
			return(1);
		}
		SoundLibraryClass::Play_2D_Sound("Lost_Connection", 2);
		if (CurrentView && CurrentView == Unknown135C) {
			Unknown135C->Get_Unknown34()->Unknown_588A70("OK", 0, "Lost connection to server.", 0x140, 0, 0);
		} else {
			MainView->Get_Unknown34()->Unknown_588A70("OK", 0, "Lost connection to server.", 0x140, 0, 0);
		}
		Unknown1388 = true;
		Unknown_74D990(0x13, "Lost connection to server.", 0);
		delete Connection;
		Connection = new PeekConnectionClass(new NullConnectionClass);
	} else {
		float a, b, c, d, e, f;
		int g = 0;
		Connection->Get_Stats(&a, &b, &c, &d, &e, &f, &g);
		bool lagging = false; if (g > 15) lagging = true;
		if (lagging != ChatPanel->Unknown_56BA80()) {
			ChatPanel->Unknown_56BA40(lagging);
		}
	}

	// the age step: the player's aux data, then every object's (co.cpp's slot +0x1C 0x6F5120)
	if (!Unknown1198) {
		((DeltaClass *)AuxData)->Unknown_20();
		HashNodeClass<ClientObjectClass *> *node = ObjectList.First_Valid();
		while (node && node->Next()) {
			node->Get_Record()->Unknown_1C();
			node = node->Next();
		}
	}
	PxProcessClass::Push(PxProcess);
	int saved = Unknown_BE350C;
	Unknown_BE350C = Unknown1128 ^ 1; Unknown1128 = Unknown_BE350C;
	Unknown12D4 = 0;
	Unknown10BC = Unknown_Get_Ticks();

	// input
	if (focused) {
		Unknown_918200(Unknown10BC);
		if (!LoadingView || !LoadingView->Unknown68) {
			KernelEventClass event;
			Unknown_42EDB0();
			if (Unknown1278) {
				int i = 0;
				do {
					event = Unknown_65F780((void *)Unknown1278);
					if (!Unknown1270->Unknown_429A20(&event) && (!CurrentView || !CurrentView->Get_Unknown04()->Unknown_429A20(&event))) {
						if (ScriptedMovieView) {
							ScriptedMovieView->Unknown_802960(event);
						} else if (!Unknown_7436A0(event)) {
							Unknown_744240(event);
						}
					}
				} while ((Unknown_BE5F34 & 0x1100) && i++ < 20);
			} else {
				if (Unknown_42EA80(&event)) {
					Unknown_42E8A0(&event);
				}
				Unknown_743380(&event);
			}
		}
	}

	Process_Packets();
	SoundManager->Unknown_75B320();
	if (!Unknown1198) {
		Process_AI();
		CurrentView->Unknown_14();
		if (BridgeInterface->Unknown14) {
			BridgeInterface->Unknown_53BD40(MainView->Unknown_7C());
		}
		if (CurrentView == MainView) {
			MainView->Unknown_512A00();
		}
		if (Unknown12BC && Unknown12BC->Unknown_04()) {
			delete Unknown12BC;
			Unknown12BC = NULL;
		}
		Unknown_7429B0();
		Unknown_74EDB0();
		Unknown_751660();
		Unknown_74F5C0();
	}

	// the movie views that are done (0x74BD10 and 0x74BF00 inline)
	if (ViewMode == 3 && Movie3DView->Unknown_7EFCC0()) {
		Unknown_74BD10();
	}
	if (ViewMode == 6 && ScriptedMovieView->Unknown_802930()) {
		Unknown_74BF00(Unknown1370);
	}

	// the lost-connection box answered
	if (Unknown1388) {
		ViewClass *view = (CurrentView && CurrentView == Unknown135C) ? Unknown135C : MainView;
		if (view->Get_Unknown34()->Unknown_5890B0(NULL) > 0) {
			Unknown12D4 = 1;
			MainView->Get_Unknown34()->Unknown_5C();
			Unknown1388 = false;
		}
	}
	Unknown_BE350C = saved;
	PxProcessClass::Pop();
	return(Unknown12D4);
}

































































































// NOT EXACT: shape, 10 masked rows (the effect loop: ours reads the record before the next-node test and loops with a jmp, the client tests the next node again; the dead First_Valid spends the inline budget so that the third list's Remove 0x7572E0 is called out of line, as the client's; 30 forms)
// 0x744260 (placed in the blank run after Do_Frame, before 0x744C10: no assert line moves; no asserts of its own)
void SClient::Process_AI()
{
	WWPROFILE("SClient::Process_AI"); Unknown86CList.First_Valid();

	// every object's per-frame update (co.cpp's slot +0x20 0x6F3C60)
	HashNodeClass<ClientObjectClass *> *node = ObjectList.First_Valid();
	while (node && node->Next()) {
		node->Get_Record()->Unknown_20();
		node = node->Next();
	}
	if (!CurrentView || CurrentView != Unknown135C) {
		CameraManager->Unknown_799E40();
	}

	// the effects that are done: out of the list, then eff_o.cpp's 0x6E6A90
	HashNodeClass<EffectObjectClass *> *enode = EffectList.First_Valid();
	while (enode) {
		EffectObjectClass *effect = enode->Get_Record(); HashNodeClass<EffectObjectClass *> *next = enode->Next(); if (!next) break;

		if (!effect->Unknown_6E6AD0()) {
			EffectList.Remove(enode);
			Unknown_6E6A90(effect);
		}
		enode = next;
	}

	Unknown_7456B0();

	// the third list's records against the main view's camera position: those that refuse go
	Vector3 position = ((RenderObjClass *)MainView->Unknown_7C())->Get_Position();
	HashNodeClass<Unknown86CRecordClass *> *rnode = Unknown86CList.First_Valid();
	while (rnode && rnode->Next()) {
		HashNodeClass<Unknown86CRecordClass *> *next = rnode->Next();
		Unknown86CRecordClass *record = rnode->Get_Record();
		if (!record->Unknown_08(position)) {
			Unknown86CList.Remove(rnode);
			delete record;
		}
		rnode = next;
	}
}

































#include "option.h"
#include "..\packets\pkmove.h"
#include "control_keys_manager.h"
// NOT EXACT: inline budget (explain.py: 731 instructions, the client's about twice as many): the client inlines more of std::string's construction (the last three key names copied in place) and its five camera blocks (0x744D80's body); first round
// 0x7436A0 (Do_Frame: an event no dispatcher took; the module 'sclient' keys, registered on the first call)
bool SClient::Unknown_7436A0(KernelEventClass event)
{
	if (!(event.Flags & 0x1000) && event.Key != 0) {
		if (event.Key == 0x1B) {
			ChatPanel->Unknown_565F30(2);
			return(true);
		}
		static std::string module("sclient");
		static bool first = true;
		if (first) {
			first = false;
			ControlKeysManager::Register_Key(module, "Screenshot", 0);
			ControlKeysManager::Register_Key(module, "Toggle Auto Level", 1);
			ControlKeysManager::Register_Key(module, "Restore Camera", 2);
			ControlKeysManager::Register_Key(module, "First Person", 3);
			ControlKeysManager::Register_Key(module, "Save Camera", 4);
			ControlKeysManager::Register_Key(module, "Next Camera", 5);
			ControlKeysManager::Register_Key(module, "Prev Camera", 6);
		}
		int key = ControlKeysManager::Get_Key_Id((ControlKeysSClientClass *)this, module, *(ControlKeyValueStruct *)&event);
		switch (key) {
			case -1:
				break;
			case 0:
				Unknown_74D990(0x17, "Taking Screenshot...", 0);
				WW3D::Make_Screen_Shot("ScreenShot", 1.3f, 2);
				Unknown_74D990(0x17, "*SNAP* Done!", 0);
				return(true);
			case 1:
				Connection->Send_Packet(&MovePacketClass(AvatarID, 13));
				return(true);
			case 2:
				Unknown_744D80();
				return(true);
			case 3:
				if (!CurrentView || CurrentView != Unknown135C) {
					BoolOptionStruct *option = Unknown_BE8B38->Find_Bool("Bridge_Mode");
					if (CameraMode == 0) {
						Print(0x17, "Camera set to 3rd person mode %i,", Unknown_75A5D0());
						if (option) option->Value = false;
					} else {
						Set_Camera_Mode(0);
						Unknown_74D990(0x17, "Camera set to 1st person mode.", 0);
						if (option) option->Value = true;
					}
					Unknown_744D80();
				}
				return(true);
			case 4:
				if (CurrentView && CurrentView == Unknown135C) {
					Unknown_74D990(0x17, "Cannot save camera in starbase.", 0);
				} else if (CameraMode == 0) {
					Unknown_74D990(0x17, "Cannot save camera in first person mode.", 0);
				} else {
					CameraModeStruct &mode = CameraModes[CameraMode];
					mode.Tilt = CameraManager->Get_Tilt();
					mode.Distance = CameraManager->Get_Distance();
					mode.Heading = CameraManager->Get_Heading();
					mode.ElevationOffset = CameraManager->Get_Elevation_Offset();
					mode.FOV = CameraManager->FOV;
					if (Unknown_745490()) {
						Print(0x17, "Camera Slot %i Saved: H%.2f, T:%.2f, D:%.2f FOV:%.2f EO:%.2f", CameraMode, mode.Heading, mode.Tilt, mode.Distance, mode.FOV, mode.ElevationOffset);
						return(true);
					}
					Print(0x17, "Failed to save Camera Slot %i!", CameraMode);
					return(true);
				}
				break;
			case 5:
			case 6:
				if (CurrentView && CurrentView == Unknown135C) break;
				if (CameraMode == 0) {
					Unknown28 = !Unknown28;
					if (CameraMode == 0 && Unknown28) {
						Unknown_74D990(0x17, "Camera set to 1st person mode with bridge.", 0);
					} else {
						Unknown_74D990(0x17, "Camera set to 1st person mode without bridge.", 0);
					}
					Unknown_744D80();
					return(true);
				}
				switch (key) {
					case 5: Unknown_75A570(); break;
					case 6: Unknown_75A5A0(); break;
				}
				Unknown_744D80();
				Print(0x17, "Camera set to 3rd person mode %i", CameraMode);
				return(true);
			default:
				G_ASSERT(false);
				break;
		}
	}
	return(false);
}




























































































































#include "dx8renderer.h"
// NOT EXACT: shape (explain.py: 25 calls, the client 20; 283 instructions, 234): the client inlines the first two lists' First_Valid and Remove's next-valid helper for the objects (0x7559E0 out of line for the effects), ours calls GenericList::First_Valid 0x71BCC0 out of line
// 0x745750 (SERVER_HANDOFF 58: the sector's objects, effects and third-list records go, then the +0x1394 values)
void SClient::Unknown_745750()
{
	TheDX8MeshRenderer.Invalidate();
	HashNodeClass<ClientObjectClass *> *node = ObjectList.First_Valid();
	CameraManager->Unknown_79B2B0(true);
	while (node && node->Next()) {
		HashNodeClass<ClientObjectClass *> *next = node->Next();
		ClientObjectClass *object = node->Get_Record();
		ObjectList.Remove(node);
		delete object;
		node = next;
	}
	HashNodeClass<EffectObjectClass *> *enode = EffectList.First_Valid();
	while (enode && enode->Next()) {
		HashNodeClass<EffectObjectClass *> *next = enode->Next();
		EffectObjectClass *effect = enode->Get_Record();
		EffectList.Remove(enode);
		Unknown_6E6A90(effect);
		enode = next;
	}
	HashNodeClass<Unknown86CRecordClass *> *rnode = Unknown86CList.First_Valid();
	while (rnode && rnode->Next()) {
		HashNodeClass<Unknown86CRecordClass *> *next = rnode->Next();
		Unknown86CRecordClass *record = rnode->Get_Record();
		Unknown86CList.Remove(rnode);
		delete record;
		rnode = next;
	}
	Unknown_755600();
	std::hash_map<unsigned, SClientUnknown1394Class *>::iterator it;
	for (it = Unknown1394.begin(); it != Unknown1394.end(); ++it) {
		SClientUnknown1394Class *object = (*it).second;
		if (object) {
			object->Unknown_38();
			object->Release_Ref();
		}
	}
	Unknown1394.clear();
}





#include "postate.h"

// The player's steering (ui_cpit.cpp 0x5806E0, 0x6B58A0): each hands the avatar's type-4 state (postate.cpp) the client's
// +0x10D4 limits, the avatar's aux data and the clock +0x10BC
// 0x745AA0
void SClient::Unknown_745AA0(float *turn, float *tilt, int *move_direction, bool fixed_speed)
{
	ClientObjectClass *avatar = Get_Avatar();
	if (avatar && avatar->Unknown98->Unknown_04() == 4) {
		((Unknown717010Class *)avatar->Unknown98)->Unknown_717430((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, turn, tilt, move_direction, fixed_speed);
	}
}

// 0x745B20
void SClient::Unknown_745B20(bool level, bool auto_level)
{
	ClientObjectClass *avatar = Get_Avatar();
	if (avatar && avatar->Unknown98->Unknown_04() == 4) {
		((Unknown717010Class *)avatar->Unknown98)->Unknown_7179A0((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, level, auto_level);
	}
}

// 0x745B90
void SClient::Unknown_745B90(float turn)
{
	if (fabs(turn) < 0.0001) {
		ClientObjectClass *avatar = Get_Avatar();
		if (avatar && avatar->Unknown98->Unknown_04() == 4) {
			((Unknown717010Class *)avatar->Unknown98)->Unknown_7179A0((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, true, false);
		}
	} else {
		ClientObjectClass *avatar = Get_Avatar();
		if (avatar && avatar->Unknown98->Unknown_04() == 4) {
			((Unknown717010Class *)avatar->Unknown98)->Unknown_717430((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, &turn, NULL, NULL, false);
		}
	}
}

// 0x745C70
void SClient::Unknown_745C70(float tilt)
{
	if (fabs(tilt) < 0.0001) {
		ClientObjectClass *avatar = Get_Avatar();
		if (avatar && avatar->Unknown98->Unknown_04() == 4) {
			((Unknown717010Class *)avatar->Unknown98)->Unknown_7179A0((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, false, true);
		}
	} else {
		ClientObjectClass *avatar = Get_Avatar();
		if (avatar && avatar->Unknown98->Unknown_04() == 4) {
			((Unknown717010Class *)avatar->Unknown98)->Unknown_717430((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, NULL, &tilt, NULL, false);
		}
	}
}

// 0x745D50
void SClient::Unknown_745D50(float turn, float tilt)
{
	ClientObjectClass *avatar = Get_Avatar();
	if (avatar && avatar->Unknown98->Unknown_04() == 4) {
		((Unknown717010Class *)avatar->Unknown98)->Unknown_717430((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, &turn, &tilt, NULL, false);
	}
}

// 0x745DD0
void SClient::Unknown_745DD0()
{
	ClientObjectClass *avatar = Get_Avatar();
	if (avatar && avatar->Unknown98->Unknown_04() == 4) {
		((Unknown717010Class *)avatar->Unknown98)->Unknown_7179A0((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, true, true);
	}
}

// 0x745E40
void SClient::Unknown_745E40(int move_direction, bool fixed_speed)
{
	ClientObjectClass *avatar = Get_Avatar();
	if (avatar && avatar->Unknown98->Unknown_04() == 4) {
		((Unknown717010Class *)avatar->Unknown98)->Unknown_717430((int)&Unknown10D4, (DeltaCompoundClass *)avatar->AuxData, Unknown10BC, NULL, NULL, &move_direction, fixed_speed);
	}
}













// 0x74B5E0 (0x639630, 0x63A100)
bool SClient::Unknown_74B5E0()
{
	return(Task->Is_Focused());
}

// 0x74B600 (cpacket.cpp: mode change 76, mode 0)
void SClient::Unknown_74B600()
{
	Unknown1380 = 0;
	if (Unknown137C) {
		MainView->Unknown_514220();
	} else {
		MainView->Unknown_5141A0();
	}
}

// 0x74B640 (cpacket.cpp: mode change 76, mode 1)
void SClient::Unknown_74B640()
{
	Unknown1380 = 1;
	if (Unknown137C) {
		MainView->Unknown_514220();
	} else {
		MainView->Unknown_5141E0();
	}
}

// 0x74CA70 (0x69F8F0)
void SClient::Unknown_74CA70()
{
	Unknown1374 = 3;
}

// 0x74D730 (camera_manager.cpp 0x799E40: some object is attacking, from RELATIONSHIP 137's IsAttacking)
bool SClient::Unknown_74D730()
{
	for (HashNodeClass<ClientObjectClass *> *node = ObjectList.First_Valid(); node; node = node->Next_Valid()) {
		if (node->Get_Record()->Relationship.IsAttacking) return(true);
	}
	return(false);
}




































































































































































































































































































































































































































































































































































































// NOT EXACT: shape (explain.py): the client keeps the chase module in ecx for the +0x34 test and zeroes the argument before the FAIL_IFs (attempts.py show 0x744C10)
// 0x744C10
void SClient::Unknown_744C10()
{
	if (!BridgeInterface) return;
	SimpleSceneClass *scene = NULL;
	FAIL_IF(!CameraManager) return;
	FAIL_IF(!CameraManager->Get_Player_Chase_Camera_Module()) return;
	if (CurrentView == MainView && CameraMode == 0 && Unknown28 && CameraManager->Get_Player_Chase_Camera_Module()->Unknown34) {
		scene = MainView->Unknown_78();
	}
	BridgeInterface->Unknown_53BBC0(Create_Peek((RefCountClass *)scene));
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
	ActionPacketClass packet(AvatarID, 2, target->Get_Game_ID(), 0);
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
	MainView->Unknown_64()->Set_Fog_Enable(false);
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
// NOT EXACT: inline budget (explain.py: 11 calls, the client 7): ours inlines the hash table's resize 0x759660 into operator[] (and so std::__stl_next_prime), the client calls it and its bucket helper 0x756D10
// 0x74C3B0
void SClient::Unknown_74C3B0(int game_id, AvatarDescClass &desc)
{
	if (Unknown1394.find(game_id) != Unknown1394.end()) return;
	SClientUnknown1394Class *object = new SClientUnknown1394Class(desc, 0, 0, 0);
	Unknown1394[game_id] = object;
	if (AvatarID == (unsigned)game_id) BridgeInterface->Unknown_53B7A0(&object->UnknownF8);
}
// 0x74C690
void SClient::Unknown_74C690()
{
	std::hash_map<unsigned, SClientUnknown1394Class *>::iterator it;
	for (it = Unknown1394.begin(); it != Unknown1394.end(); ++it) {
		SClientUnknown1394Class *object = (*it).second;
		if (object) {
			object->Unknown_38();
			object->Release_Ref();
		}
	}
	Unknown1394.clear();
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

// NOT EXACT: inline budget (explain.py frame, 31 masked rows): the client calls hash_map::find out of line (0x756820, the key in the argument slot) where ours inlines it, as itself or through 0x74C840; two forms
// 0x74C8D0 (0x74C840 for the named object's GameID)
Unknown77E3F0Class *SClient::Unknown_74C8D0(const char *name) {
	ClientObjectClass *object = Find_Object(name);
	if (object) return(Unknown_74C840(object->GameID));
	return(NULL); }

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









































































































#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h: its Type_Name(int *) for kv_pair.h, C2665 without)

// 0x74D570 (the player is in a group: one of GroupInfo.Members' first five has a name)
bool SClient::Unknown_74D570()
{
	NameClass name("GroupInfo"); DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(AuxData, name);
	if (group && group->m_Unknown70) {
		for (int i = 0; i < 5; i++) {
			const char *name = ((DeltaGroupMemberClass *)group->Members.Children[i])->Name.String;
			if (name && strcmp(name, "") != 0) return(true);
		}
	}
	return(false);
}

// 0x74D690 (the player leads its group: GroupInfo.IsGroupLeader)
bool SClient::Unknown_74D690()
{
	NameClass name("GroupInfo"); DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(AuxData, name);
	return(group && group->m_Unknown70 && group->IsGroupLeader.Value);
}





















































bool Unknown_70BAC0(SClient *client, ClientObjectClass *co);	// 0x70BAC0 (cdecl; 0x74D780 skips such an object)  // NAME 1 [client, co]: what 0x74D780 passes

// 0x74D780
bool SClient::Unknown_74D780(ClientObjectClass *co)
{

	FAIL_IF(!co) return(false);

	ClientObjectClass *avatar = Get_Avatar();
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
	ClientObjectClass *co = Get_Avatar();
	FAIL_IF(!co) return(false);
	return(co->Unknown_6F5ED0());
}

bool SClient::Unknown_74D8B0(ClientObjectClass *co)	// 0x74D8B0
{
	ClientObjectClass *player_co = Get_Avatar();
	FAIL_IF(!player_co) return(false);
	FAIL_IF(!co) return(false);
	return(player_co->Unknown_6F5F60() || co->Unknown_6F5F60());
}



































































































































































// 0x750210 (defined before 0x74EB00, which inlines it as the client does)
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





// 0x74E580 (GuildRecruitConfirmSectorPacket 0x73E720, GuildGMRenameConfirmSectorPacket 0x73EEF0): the current view's
// dialog asks text with the type's buttons; value is kept in +0x119C for the answer
void SClient::Unknown_74E580(const char *text, const char *type, const char *value)
{

	FAIL_IF(!CurrentView) return;
	ViewClass *ui_view = CurrentView->Unknown_1C();

	FAIL_IF(!ui_view) return;
	ViewUnknown34Class *dialog = ui_view->Get_Unknown34();

	FAIL_IF(!dialog) return;
	dialog->Unknown_588A70(type, 0, text, 0x140, 0, 0);
	Unknown119C = value;
}
#include "..\packets\pkguildranknamesrequestclient.h"
enum { ACTIONTYPE_MAX = 2 };							// NAME 3: the assert's (Unknown120C's two queues)
// 0x74E710 (cpacket.cpp: CONFIRMED_ACTION_OFFER 190): type 0 asks in the current view's dialog, type 1 in the
// chat panel; the id joins its type's offer queue
void SClient::Unknown_74E710(const std::string &message, int id, int type)
{

	FAIL_IF(!CurrentView) return;
	ViewClass *ui_view = CurrentView->Unknown_1C();

	FAIL_IF(!ui_view) return;
	switch (type) {
		case 0:
		{
			ViewUnknown34Class *dialog = ui_view->Get_Unknown34();


			FAIL_IF(!dialog) return;
			Unknown11AC[0].push_back(id);
			dialog->Unknown_588A70("CONFIRMED_ACTION_DIALOG", 0, message.c_str(), 0x140, 0, 0);
			break;
		}
		case 1:
			if (ChatPanel) {
				ChatPanel->Unknown_56BAA0(1, message.c_str());
				Unknown11AC[1].push_back(id);
			}
			break;
		default:









			G_ASSERT(false);
	}
}
// 0x74EB00 (cpacket.cpp: the confirmed action's timeout 191): the id joins its type's queue, then 0x750210's work
void SClient::Unknown_74EB00(int type, int id)
{

	G_ASSERT(type < ACTIONTYPE_MAX);
	Unknown120C[type].push_back(id);
	Unknown_750210();
}
// NOT EXACT: shape (explain.py: 503 instructions, the client 508; 131 masked rows, 25 with registers masked): the client saves ebp with ebx in the prologue and takes each c_str() before the player id; two forms (the flag declared first gains 5 rows)
// 0x74EDB0 (Do_Frame, after 0x7429B0): the guild confirmations' answers to the server
void SClient::Unknown_74EDB0()
{	bool changed = false;
	FAIL_IF(!CurrentView) return;
	ViewClass *ui_view = CurrentView->Unknown_1C();

	FAIL_IF(!ui_view) return;
	ViewUnknown34Class *dialog = ui_view->Get_Unknown34();

	FAIL_IF(!dialog) return;

	int button;
	button = dialog->Unknown_5890B0("GUILD_LEADER_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildLeaderAcceptClientPacketClass(Unknown1138, Unknown119C.c_str(), true));
		changed = true;
	} else if (button > 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildLeaderAcceptClientPacketClass(Unknown1138, Unknown119C.c_str(), false));
	}
	button = dialog->Unknown_5890B0("GUILD_PROMOTE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(2, Unknown1138, Unknown119C.c_str()));
		changed = true;
	} else if (button > 0) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_NEWLEADER_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(4, Unknown1138, Unknown119C.c_str()));
		changed = true;
	} else if (button > 0) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_DEMOTE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(6, Unknown1138, Unknown119C.c_str()));
		changed = true;
	} else if (button > 0) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_REMOVE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(8, Unknown1138, Unknown119C.c_str()));
		changed = true;
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_LEAVE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(10, Unknown1138, Unknown119C.c_str()));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_DISBAND_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(12, Unknown1138, Unknown119C.c_str()));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_RECRUIT_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildRecruitAcceptClientPacketClass(Unknown1138, true));
		changed = true;
	} else if (button > 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildRecruitAcceptClientPacketClass(Unknown1138, false));
	}
	button = dialog->Unknown_5890B0("GUILD_GM_DISBAND_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(14, Unknown1138, Unknown119C.c_str()));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_GM_REMOVE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(17, Unknown1138, Unknown119C.c_str()));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_GM_PROMOTE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(19, Unknown1138, Unknown119C.c_str()));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_GM_DEMOTE_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(21, Unknown1138, Unknown119C.c_str()));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	button = dialog->Unknown_5890B0("GUILD_GM_RENAME_CONFIRM");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&GuildSimpleClientSectorPacketClass(15, Unknown1138, ""));
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
	if (changed && ChatPanel) {
		UIChatPanelUnknownE0Class *panel = ChatPanel->UnknownE0;
		if (panel && panel->Unknown7C && Unknown_728300()) {
			panel->Unknown_81BF40();
		}
	}
}




















































#include "..\packets\pkconfirmedaction.h"
#include <algorithm>
// NOT EXACT: shape (explain.py: 272 instructions, the client 270, 10 masked rows): the client loads AvatarID before testing button == 1 and keeps this in esi; three forms (one 0x750210 per branch and the named queues gained the rest)
// 0x74F5C0 (Do_Frame, after 0x751660): the answer to the confirmed-action dialog (0x74E710's type 0) goes to the
// server and the offer leaves both queues
void SClient::Unknown_74F5C0()
{
	FAIL_IF(!CurrentView) return;
	ViewClass *ui_view = CurrentView->Unknown_1C();

	FAIL_IF(!ui_view) return;
	ViewUnknown34Class *dialog = ui_view->Get_Unknown34();

	FAIL_IF(!dialog) return;
	if (strcmp(dialog->Type, "CONFIRMED_ACTION_DIALOG") == 0) {
		int button = dialog->Unknown_5890B0("CONFIRMED_ACTION_DIALOG");
		if (button <= 0) {
			Unknown_750210();
		} else {
			dialog->Unknown_5C();
			std::deque<int> &confirmedlist = Unknown11AC[0]; std::deque<int> &timedlist = Unknown120C[0];	// NAME 1 [timedlist]: 0x74EB00's queue









			G_ASSERT(!confirmedlist.empty());
			if (!confirmedlist.empty()) {
				int id = confirmedlist.front();
				confirmedlist.pop_front();
				std::deque<int>::iterator it = std::find(timedlist.begin(), timedlist.end(), id);
				if (it != timedlist.end()) {
					timedlist.erase(it);
				}
				if (button == 1) {
					Send_Packet(&ConfirmedActionResponsePacketClass(AvatarID, id, true));
				} else {
					Send_Packet(&ConfirmedActionResponsePacketClass(AvatarID, id, false));
				}
			}
			Unknown_750210();
		}
	}
}




// 0x74FA30 (0x750210): while the dialog shows a confirmed action, the oldest offer leaves both type 0 queues once
// its timeout has come (it is in +0x120C[0]) and the dialog closes
void SClient::Unknown_74FA30()
{

	FAIL_IF(!CurrentView) return;
	ViewClass *ui_view = CurrentView->Unknown_1C();

	FAIL_IF(!ui_view) return;
	ViewUnknown34Class *dialog = ui_view->Get_Unknown34();

	FAIL_IF(!dialog) return;
	std::deque<int> &confirmedlist = Unknown11AC[0]; std::deque<int> &timedlist = Unknown120C[0];	// NAME 3 [confirmedlist]: the FAIL_IF's  // NAME 1 [timedlist]: 0x74EB00's queue
	if (!timedlist.empty()) {
		while (strcmp(dialog->Type, "CONFIRMED_ACTION_DIALOG") == 0) {








			FAIL_IF(confirmedlist.empty()) {
				timedlist.clear();
				return;
			}
			int id = confirmedlist.front(); std::deque<int>::iterator it = std::find(timedlist.begin(), timedlist.end(), id);
			if (it == timedlist.end()) {
				return;
			}
			timedlist.erase(it);
			confirmedlist.pop_front();
			dialog->Unknown_5C();
		}
	}
}






// 0x74FEC0 (0x750210): the chat panel's offers (type 1) as 0x74FA30, while its message button shows; the button goes
// with the last one
void SClient::Unknown_74FEC0()
{
	std::deque<int> &timedlist = Unknown120C[1]; std::deque<int> &confirmedlist = Unknown11AC[1]; UIChatPanelClass *chat_panel = ChatPanel.Peek();	// NAME 3 [confirmedlist, chat_panel]: the FAIL_IFs'  // NAME 1 [timedlist]: 0x74EB00's queue
	FAIL_IF(!chat_panel) return;

	FAIL_IF(chat_panel->Is_Message_Button_Showing() && confirmedlist.empty()) {
		timedlist.clear();
		return;
	}
	if (chat_panel->Is_Message_Button_Showing()) {
		int id = confirmedlist.front(); std::deque<int>::iterator it = std::find(timedlist.begin(), timedlist.end(), id);
		if (it != timedlist.end()) {
			timedlist.erase(it);
			confirmedlist.pop_front();
			if (confirmedlist.empty()) {
				chat_panel->Unknown_56BAA0(0, NULL);
			}
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

// 0x750B60 (camera_manager.cpp 0x7992F0 / 0x7993E0: the avatar's aux data IsIncapacitated)
bool SClient::Unknown_750B60()
{
	ClientObjectClass *avatar = Get_Avatar();
	if (avatar && avatar->AuxData) {
		NameClass name("IsIncapacitated"); DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)avatar->AuxData, name);
		if (t && t->m_Unknown70) return(t->Value);
	}
	return(false);
}



































































































































#include "..\packets\pkunknown_8a5510.h"

// 0x751660 (Do_Frame, every frame: the SPEND_BUDDY_POINTS question; yes sends opcode 0xE8 with +0x14F4)
void SClient::Unknown_751660()
{



	FAIL_IF(!CurrentView) return;
	ViewClass *ui_view = CurrentView->Unknown_1C();

	FAIL_IF(!ui_view) return;
	ViewUnknown34Class *dialog = ui_view->Get_Unknown34();

	FAIL_IF(!dialog) return;
	int button = dialog->Unknown_5890B0("SPEND_BUDDY_POINTS");
	if (button == 1) {
		dialog->Unknown_5C();
		Send_Packet(&Packet_00E8Class(Unknown14F4));
		Unknown14F4 = 0;
	} else if (button > 1) {
		dialog->Unknown_5C();
	}
}













































































#include "..\packets\pkmessagestring.h"
#include "..\packets\pkincapacitancerequest.h"
// sbaseview.h's two classes as far as '/starbase reset' needs them (the header includes ui_line_display.h, whose
// UILineDisplayClass clashes with this file's own, C2011)
class UIStarbaseClass : public UIUnknownBaseClass, public RefCountClass { public: void Unknown_640150(); };	// NAME 3: sbaseview.h's (ui_starbase.cpp); 0x640150: '/starbase reset'
class SBaseViewUnknown174Class { public: char Unknown00[0x18C]; bool Unknown18C; };	// NAME 1: neutral (D10): the starbase view's +0x174 (0x74D950 sets its +0x18C)
class SBaseViewClass : public ViewClass { public: RefCountPtr<UIStarbaseClass> Get_Starbase() { return(Starbase); } char Unknown38[0x60 - 0x38]; RefCountPtr<UIStarbaseClass> Starbase; char Unknown64[0x174 - 0x64]; SBaseViewUnknown174Class *Unknown174; };	// NAME 3: sbaseview.h's; Get_Starbase 0x54EDA0

// The chat commands' tables (0xB98088, 0xB980F8, 0xB98138) and the client that shows the names under the mouse
// (0xBE5F20, '/art name')
static const char *ChatCommandHelp[] = {	// NAME 2: 0x745FF0's '/chat' help lines
	"/chat", "/chat tell PERSON TEXT...", "/chat all TEXT", "/chat ignore PERSON", "/chat unignore PERSON", "/chat friend PERSON", "/chat unfriend PERSON",
	"/chat list channels", "/chat list all_channels", "/chat list ignores", "/chat list friends", "/chat channel CHANNEL create ", "/chat channel CHANNEL remove ",
	"/chat channel CHANNEL enter [PASSWORD]", "/chat channel CHANNEL exit ", "/chat channel CHANNEL say TEXT", "/chat channel CHANNEL invite PERSON",
	"/chat channel CHANNEL uninvite PERSON", "/chat channel CHANNEL ban\tPERSON", "/chat channel CHANNEL unban PERSON", "/chat channel CHANNEL gag PERSON",
	"/chat channel CHANNEL ungag PERSON", "/chat channel CHANNEL owner PERSON", "/chat channel CHANNEL unowner PERSON", "/chat channel CHANNEL access TYPE",
	"/chat channel CHANNEL password PASSWORD", "/chat channel CHANNEL members", NULL };
struct LogoffCommandStruct { const char *Name; bool Quit; };	// NAME 2: what 0x74ABC0 does with an entry
static const LogoffCommandStruct LogoffCommands[] = { { "/logoff", false }, { "/log", false }, { "/quit", true }, { "", false } };	// NAME 2: what 0x74ABC0 sends for them
static const char *CampCommands[] = { "/camp", NULL };	// NAME 2: the command 0x74ABC0 refuses
SClient *Unknown_BE5F20;	// NAME 1: neutral (D10)

// NOT EXACT: shape (explain.py: 606 instructions, the client 640, 127 masked rows; 69 calls, the client 70): VC6 merges the two beacon sends' tails here, the client keeps both; the logoff packet a named local, '/fps' enable first and the dialog read once moved nothing; two rounds
// 0x74ABC0 (slot +0x34: the chat line's '/' commands; true when the line was a command)
bool SClient::Unknown_34(const char *text)
{
	if (strncmp(text, "/group", strlen("/group")) == 0) {
		return(Unknown_747510(text));
	}
	if (strncmp(text, ChatCommandHelp[0], strlen(ChatCommandHelp[0])) == 0) {
		return(Unknown_745FF0(text));
	}
	if (Unknown_747C70(text)) {
		return(true);
	}
	std::string reason;
	int len;
	for (const LogoffCommandStruct *command = LogoffCommands; (len = strlen(command->Name)) != 0; command++) {
		if (strnicmp(command->Name, text, len) == 0) {
			const char *p = text + len;
			while (*p == ' ') {
				p++;
			}
			reason = p;
			bool quit = command->Quit;
			Unknown138C = 1;
			LogoffRequestPacketClass packet(Unknown1138);
			Send_Packet(&packet);
			if (quit) {
				Unknown_743290();
			}
			return(true);
		}
	}
	if (strnicmp(text, CampCommands[0], 4) == 0) {
		MessageStringPacketClass message(0x11, "Unable to camp - insufficient marshmallows.");
		message.Process_Packet(this);
		return(true);
	}
	if (strncmp(text, "/fps", 4) == 0) {
		if (Unknown14EC != 1) {
			Unknown14EC = 1;
			Unknown05 = true;
			Unknown_74D990(0x17, "FPS reporting enabled.", 0);
		} else {
			Unknown14EC = 0;
			Unknown05 = false;
			Unknown_74D990(0x17, "FPS reporting disabled.", 0);
		}
		return(true);
	}
	if (Unknown14EC == 3) {
		if (!WWProfileManager::Is_Profile_Enabled()) {
			WWProfileManager::Enable_Profile(true);
			WWProfileManager::Reset();
		}
	} else if (WWProfileManager::Is_Profile_Enabled()) {
		WWProfileManager::Enable_Profile(false);
	}
	if (stricmp(text, "/resetship") == 0) {
		if (CurrentView && Unknown1378 == 0) {
			ViewClass *ui_view = CurrentView->Unknown_1C();
			ViewUnknown34Class *dialog;
			if (ui_view && (dialog = ui_view->Get_Unknown34()) != NULL) {
				Unknown1378 = 1;
				dialog->Unknown_588A70("YES_NO", 0, "This will reset all ship systems, leaving you vulnerable to attack for a short time. Continue?", 0x140, 0, 0);
			}
		}
		return(true);
	}
	if (stricmp(text, "/beacon") == 0) {
		Connection->Send_Packet(&IncapacitanceDistressBeaconPacketClass(AvatarID, true));
		return(true);
	}
	if (stricmp(text, "/beacon off") == 0) {
		Connection->Send_Packet(&IncapacitanceDistressBeaconPacketClass(AvatarID, false));
		return(true);
	}
	if (stricmp(text, "/rescue") == 0) {
		Connection->Send_Packet(&Packet_008FClass(AvatarID));
		return(true);
	}
	if (stricmp(text, "/starbase reset") == 0 && CurrentView && CurrentView == Unknown135C) {
		UIStarbaseClass *starbase = ((SBaseViewClass *)Unknown135C)->Get_Starbase().Peek();
		starbase->Unknown_640150();
		return(true);
	}
	if (stricmp(text, "/deco") == 0) {
		LandClass *land = MainView->Land;
		if (land) {
			bool &decorations = ((bool *)land)[0x192];	// LandClass's m_Unknown192 (protected)
			decorations = !decorations;
			if (decorations) {
				Unknown_74D990(0x17, "Decoration rendering is on.", 0);
			} else {
				Unknown_74D990(0x17, "Decoration rendering is off.", 0);
			}
		}
		return(true);
	}
	if (stricmp(text, "/self tag on") == 0) {
		BoolOptionStruct *option = Unknown_BE8B38->Find_Bool("Show_Self_Name");
		if (option) {
			option->Value = true;
			Unknown_74D990(0x17, "Your own avatar name tag will now be displayed.", 0);
		}
		return(true);
	}
	if (stricmp(text, "/self tag off") == 0) {
		BoolOptionStruct *option = Unknown_BE8B38->Find_Bool("Show_Self_Name");
		if (option) {
			option->Value = false;
			Unknown_74D990(0x17, "Your own avatar name tag will now be hidden.", 0);
		}
		return(true);
	}
	if (Unknown_747FA0(text) || Unknown_749020(text) || Unknown_749590(text) || Unknown_749A70(text) || Unknown_749ED0(text)) {
		return(true);
	}
	if (text[0] == '/' && text[1] == 'g' && text[2] == ' ') {
		Connection->Send_Packet(&ClientChatPacketClass(AvatarID, (char *)text + 3, 1, NULL));
		return(true);
	}
	if (strnicmp(text, "/gu ", 4) == 0) {
		Connection->Send_Packet(&ClientChatPacketClass(AvatarID, (char *)text + 4, 2, NULL));
		return(true);
	}
	ClientObjectClass *ship;
	if (stricmp(text, "/art name") == 0 && (ship = Get_Ship()) != NULL) {
		std::string name;
		ship->Unknown_6FF7A0(name);
		RadarSystemClass *radar;
		if (strstr(name.c_str(), "[DEV]") && MainView && (radar = MainView->Radar) != NULL) {
			radar->Unknown4D = !radar->Unknown4D;
			if (radar->Unknown4D) {
				Unknown_BE5F20 = this;
				Unknown_74D990(0x17, "Showing of objects under mouse.", 0);
			} else {
				Unknown_BE5F20 = NULL;
				Unknown_74D990(0x17, "No longer showing name of objects under mouse.", 0);
			}
			return(true);
		}
	}
	if (strcmp(text, "/texture_flush") != 0) {
		return(ClientClass::Unknown_34(text));
	}
	WW3D::_Invalidate_Textures();
	Unknown_74D990(0, "TEXTURES FLUSHED", 0);
	return(true);
}

// 0x74D950 (cpacket.cpp: CHANGE_BASE_ASSET 38, after 0x744B60, for the objects 0x6F5A40 picks): in the starbase view,
// its +0x174 object's +0x18C is set
void SClient::Unknown_74D950()
{
	if (CurrentView && CurrentView == Unknown135C && ((SBaseViewClass *)Unknown135C)->Unknown174) {
		((SBaseViewClass *)Unknown135C)->Unknown174->Unknown18C = true;
	}
}

// 0x74DF70 (cdecl): printf into a line of the chat panel (0x74D990)
void SClient::Print(int channel, const char *format, ...)
{
	char buffer[512];
	va_list va;
	va_start(va, format);
	_vsnprintf(buffer, sizeof(buffer), format, va);
	va_end(va);
	buffer[sizeof(buffer) - 1] = 0;
	Unknown_74D990(channel, buffer, 0);
}

/*
**	Reconstruction notes
**
**	The skeleton (D90 9d): the layout (sclient.h) and the functions in the window before the first
**	assert (0x7429B0, l.749), plus the base slots' overrides the brief names. Init 0x741180 and Shutdown
**	0x741BB0 are functional rewrites (marked); Render 0x744690 is declared only (clienttest stands in for
**	it); the chat commands 0x74ABC0 (SC2) sit after the last assert, with the tables they read.
**	The frame Do_Frame 0x742B60 (DF1, byte-exact) sits in the blank run after Process_AI; it inlines the movie
**	views' ends 0x74BD10 / 0x74BF00 (their G_ASSERTs on ll.4104 / 4127) and writes the 'current view is +0x135C'
**	test (0x744B60, not rebuilt) out: once as an if / else of two whole calls (the lost-connection box), once as
**	a choice of view (its answer). The frame toggle is two statements (+0x1128 ^ 1 into the global, then back).
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
