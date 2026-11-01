// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\main\main_client.cpp (client 0x526DE0-0x5279BC)
**
**	WinMain: the start-up steps, the kernel's loop, the shut-down in reverse (docs/kernel-map.md 1).
**	The report 'Server initialized & ready to go' sits on the client's line 596 (D25). Notes before
**	WinMain.
*/



#include <windows.h>
#include <winsock.h>
#include <stdlib.h>
#include "..\..\wwlib\argv.h"
#include "..\..\wwlib\ffactory.h"
#include "..\..\wwlib\wwfile.h"
#include "..\..\wwlib\Except.h"
#include "..\..\wwlib\win.h"
#include "wwmemlog.h"
#include "wwmath.h"
#include "ww3d.h"
#include "assetmgr.h"
#include "dx8webbrowser.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\combo_ffactory.h"
#include "..\kernel\kernel.h"
#include "..\common\udpconn.h"
#include "..\audio\soundlib.h"
#include "..\client\option.h"
#include "..\client\buff_database.h"
#include "..\client\tutorial_manager.h"
#include "..\client\tasks\tsk_movie.h"
#include "..\client\tasks\tsk_smovie.h"
#include "..\client\tasks\tsk_menu.h"
#include "..\client\tasks\tsk_login.h"

// dbgmgr.cpp's log (the step texts, D38)
void Unknown_Debug_Printf(const char *format, ...);	// 0x509DC0 (dbgmgr.cpp, D38)
void Unknown_Debug_Begin_Step(const char *text);	// NAME 1 [Unknown_Debug_Begin_Step]: 0x509EF0 (dbgmgr.cpp), WinMain's step log
void Unknown_Debug_End_Step(const char *text);		// NAME 1 [Unknown_Debug_End_Step]: 0x50A1E0 (dbgmgr.cpp)
void Unknown_509E70();								// NAME 1 [Unknown_509E70]: dbgmgr.cpp, opens the log ('Earth and Beyond')
void Unknown_509EB0();								// NAME 1 [Unknown_509EB0]: dbgmgr.cpp, closes it (the allocation count assert, WWMemoryLog's Release_Log)
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)

// ui_registration.cpp and the dialogs (optdlg.cpp's neighbours)
bool Unknown_5FADE0();								// NAME 1 [Unknown_5FADE0]: ui_registration.cpp
bool Unknown_5FAD50();								// NAME 1 [Unknown_5FAD50]: ui_registration.cpp
bool Unknown_847D20(const char *filename);			// NAME 1 [Unknown_847D20]: the text file in a dialog (EULA, ROC); false when declined  // NAME 1 [filename]: what WinMain passes
bool Unknown_846C90();								// NAME 1 [Unknown_846C90]: the -MENU dialog; false ends the program

// The object at 0x445330-0x445700 ('Main.cpp:' in its log texts): the exception hooks, the build texts, the
// window-message handlers
int Unknown_445330(EXCEPTION_POINTERS *info);		// NAME 1 [Unknown_445330]: Exception_Handler(code, info) (cdecl)  // NAME 1 [info]: its type
void Unknown_445350();								// NAME 1 [Unknown_445350]: the application's exception callback
char *Unknown_445560();								// NAME 1 [Unknown_445560]: string resource 1
char *Unknown_445590();								// NAME 1 [Unknown_445590]: string resource 2
char *Unknown_445610();								// NAME 1 [Unknown_445610]: 'Release'
void Unknown_445420(int unknown, int active);		// NAME 1 [Unknown_445420]: WM_ACTIVATEAPP: the kernel's rendering on / off  // NAME 1 [unknown, active]: its reads
void Unknown_4454A0(int unknown, int unknown2);		// NAME 1 [Unknown_4454A0]: WM_DESTROY  // NAME 1 [unknown, unknown2]: neutral
void Unknown_4454C0(int unknown, int unknown2);		// NAME 1 [Unknown_4454C0]: WM_CLOSE  // NAME 1 [unknown, unknown2]: neutral
extern unsigned long Unknown_BDBE4C;				// NAME 1 [Unknown_BDBE4C]: GetTickCount at start-up

// input.cpp, the window, the render state, display.cpp, deffont.cpp
HWND Unknown_431CA0(HINSTANCE instance, int width, int height, int unknown);	// NAME 1 [Unknown_431CA0]: registers the class, makes the 'Earth & Beyond' window  // NAME 1 [instance, width, height, unknown]: what WinMain passes
void Unknown_42E4A0(HWND window);					// NAME 1 [Unknown_42E4A0]: input.cpp's start-up (input.ini, the DirectInput devices)  // NAME 1 [window]: what WinMain passes
void Unknown_42EE00(int unknown);					// NAME 1 [Unknown_42EE00]: input.cpp (the 'KeyRepeatCount' assert)
void Unknown_42E7B0();								// NAME 1 [Unknown_42E7B0]: input.cpp's shut-down
void Unknown_42F940(int message, void (*handler)(int, int));	// NAME 1 [Unknown_42F940]: adds a window-message handler  // NAME 1 [message, handler]: what WinMain passes
void Unknown_42FAC0(int message, void (*handler)(int, int));	// NAME 1 [Unknown_42FAC0]: removes it
void Unknown_4581E0();								// NAME 1 [Unknown_4581E0]: the render state class's start-up (a critical section, an event)
void Unknown_458250();								// NAME 1 [Unknown_458250]: its shut-down
void Unknown_442AC0(HWND window);					// NAME 1 [Unknown_442AC0]: display.cpp's start-up  // NAME 1 [window]: what WinMain passes
void Unknown_442FF0();								// NAME 1 [Unknown_442FF0]: display.cpp's shut-down (WW3D::Shutdown)
extern int Unknown_BDADB8;							// NAME 1 [Unknown_BDADB8]: display.cpp's (0: no display; WinMain skips to the assets)
void Unknown_684E00();								// NAME 1 [Unknown_684E00]: loads fontpal.pcx into 0xB8F870 (deffont.cpp)
extern char Unknown_B8F870[];						// NAME 1 [Unknown_B8F870]: the 768-byte palette, WW3D::Init's default palette
void Unknown_6832C0();								// NAME 1 [Unknown_6832C0]: deffont.cpp's start-up ('!FontData[style]')
void Unknown_683940();								// NAME 1 [Unknown_683940]: deffont.cpp's shut-down
struct Unknown_C006A0Struct { char Unknown00[0xDD]; bool UnknownDD; };	// NAME 1 [Unknown_C006A0Struct]: neutral (D10), as far as WinMain reads it
extern Unknown_C006A0Struct *Unknown_C006A0;		// NAME 1 [Unknown_C006A0]: neutral (D10)

// WW3D2's flags E&B sets after WW3D::Init
extern bool Unknown_BFAE4F;							// NAME 1 [Unknown_BFAE4F]: neutral (D10)
extern bool Unknown_BFAE50;							// NAME 1 [Unknown_BFAE50]: neutral (D10)
extern bool Unknown_BFAE51;							// NAME 1 [Unknown_BFAE51]: neutral (D10)
extern bool Unknown_BFAE52;							// 0xBFAE52: set while Create_Object makes a mesh  // NAME 1 [Unknown_BFAE52]: neutral (D10)

// E&B's asset manager, 0x408C bytes (ENBAssetManager in the client's strings; the tree's files call it UnknownBDADC0Class)
class UnknownBDADC0Class : public WW3DAssetManager	// NAME 1: neutral (D10)
{
public:
	UnknownBDADC0Class();							// 0x48B070 (0x408C bytes: the tree's WW3DAssetManager is that size already)
};
extern UnknownBDADC0Class *Unknown_BDADC0;			// NAME 1 [Unknown_BDADC0]: 0xBDADC0

// msgserv.cpp's message server, a task (0x48 bytes)
class Unknown_8B20D0Class : public TaskClass		// NAME 1 [Unknown_8B20D0Class]: neutral (D10)
{
public:
	Unknown_8B20D0Class();							// 0x8B20D0 (WSAStartup)
	virtual int Get_Interval();
	virtual bool Delete_On_Remove();
	void Unknown_8B2230();							// 0x8B2230 (the assert 'Get_Internal_Server_Protocol() == NULL')
	char Unknown0C[0x48 - 0x0C];
};

// sound_manager.cpp's sound manager (0x340 bytes)
class SoundManagerClass	// NAME 3: class-from-file (D24) for app\client\sound_manager.cpp
{
public:
	SoundManagerClass();							// 0x75AB10
	~SoundManagerClass();							// 0x75AC50
	char Unknown00[0x340];
};
extern SoundManagerClass *SoundManager;				// 0xBE82A4  // NAME 2: what it holds

// The font system's four objects (fontdatabase.h's declarations clash with tsk_game.h's 0xBE67B8: see the notes)
class FontDatabaseClass { public: FontDatabaseClass(); ~FontDatabaseClass(); char Unknown00[0xC8]; };	// 0x6BE5C0, 0x6BEBF0 (fontdatabase.h)
class FontManagerClass { public: FontManagerClass(); ~FontManagerClass(); char Unknown00[0x1C]; };		// 0x6D16E0, 0x6D1920 (fontmanager.h)
class FontStringFilesClass { public: FontStringFilesClass(); ~FontStringFilesClass(); char Unknown00[0x18]; };	// 0x6C4C80, 0x6C4F50 (fontdatabase.h)
class Unknown_6BCFB0Class { public: Unknown_6BCFB0Class(); ~Unknown_6BCFB0Class(); char Unknown00[0x18]; };	// NAME 1 [Unknown_6BCFB0Class]: 0x6BCFB0, 0x6BD060
extern FontManagerClass *Unknown_BE67C4;			// NAME 1: neutral (D10)
extern Unknown_6BCFB0Class *Unknown_BE67B4;			// NAME 1 [Unknown_BE67B4]: neutral (D10)

// The other managers WinMain makes
class Unknown_790560Class { public: Unknown_790560Class(); ~Unknown_790560Class(); char Unknown00[0x14]; };	// NAME 1 [Unknown_790560Class]: the personality manager (personalities\%sPersonality%02d.ini); 0x790560, 0x790870
void Unknown_790940(Unknown_790560Class *manager);	// NAME 1 [Unknown_790940]: sets 0xBE856C  // NAME 1 [manager]: neutral
extern Unknown_790560Class *Unknown_BE856C;			// NAME 1 [Unknown_BE856C]: neutral (D10)
class Unknown_764480Class { public: Unknown_764480Class(); ~Unknown_764480Class(); char Unknown00[0x10]; };	// NAME 1 [Unknown_764480Class]: the starbase lights manager (starbase_lights.ini); 0x764480, 0x7644F0
void Unknown_764510(Unknown_764480Class *manager);	// NAME 1 [Unknown_764510]: sets 0xBE82D0  // NAME 1 [manager]: neutral
extern Unknown_764480Class *Unknown_BE82D0;			// NAME 1 [Unknown_BE82D0]: neutral (D10)
class Unknown_6A9B60Class { public: Unknown_6A9B60Class(int unknown); ~Unknown_6A9B60Class(); void Unknown_6A9E40(); char Unknown00[0x10]; };	// NAME 1 [Unknown_6A9B60Class]: 0x10 bytes, constructor 0x6A9B60, destructor 0x6A9DB0
class SClientFontDatabaseClass;					// sclient.h's view of 0xBE67BC (the font string files)
extern SClientFontDatabaseClass *Unknown_BE67BC;	// NAME 1: neutral (D10)

/*
**	Reconstruction notes (WinMain 0x526DE0, 0xBDD bytes, byte-exact with appfile.py match on 2026-11-01).
**
**	The order of the steps is the client's log (dbgmgr.cpp's Begin_Step / End_Step texts); every callee
**	is called as the client calls it, the ones not rebuilt yet declared above as far as WinMain uses them
**	(their NAME 1 tags give the evidence). The POC client links them against
**	programs/clientpoc/poc_standins.cpp (D179) until they are rebuilt.
**
**	Forms (from the client's bytes):
**	* DisableExceptions: the flag is the inverse (`sete` on the value, then `je` past the hooks), so the
**	  local holds "enable" = value == 0, true when the key or the value is missing.
**	* The winsock step: if / else with the two End_Step texts (the client pushes one or the other before
**	  one call); `?:` gives two `mov eax, imm`.
**	* No EH frame although the ComboFileFactoryClass local and the file_auto_ptr have destructors: the
**	  file compiles to the client's bytes with the app flags (/GX) all the same.
**	* 0x7CA2D0 (the function-static settings store) is called once before DX8WebBrowser::Initialize, its
**	  value unused, and twice at the end around 0x7CB3D0 (tsk_game.h's Unknown_7CA2D0Class).
**	* The kernel's slot +0x00 (KernelClass::Cycle_Focus) right after `new KernelClass`; the WM_ACTIVATEAPP
**	  (0x1C), WM_DESTROY (2) and WM_CLOSE (0x10) handlers 0x445420, 0x4454A0, 0x4454C0 go to 0x42F940 and
**	  come off with 0x42FAC0.
**	* 0x919050 is WW3D::Enable_Sorting (it stores IsSortingEnabled 0xBB9644 and invalidates
**	  TheDX8MeshRenderer); 0x9CEA10 / 0x9CEA30 are Except.cpp's Register_Application_Exception_Callback
**	  and Set_Exit_On_Exception; 0xBC77DC / 0xBC77E0 / 0xBC77E4 are ffactory.cpp's _TheFileFactory,
**	  _TheSimpleFileFactory and _TheWritingFileFactory (the last used as a SimpleFileFactoryClass);
**	  0xC1E1B4 / 0xC1E1B8 are win.cpp's MainWindow and ProgramInstance; 0xBFAEE0 is
**	  WW3DAssetManager::TheInstance (Delete_This, twice).
**	* The font globals: tsk_game.h's Unknown_BE67B8 (Unknown_BE67B8Class) and sclient.h's Unknown_BE67BC
**	  (SClientFontDatabaseClass) are views of fontdatabase.h's FontDatabaseClass and FontStringFilesClass,
**	  and tsk_login.h (needed for LoginTaskClass) brings tsk_game.h, whose 0xBE67B8 clashes with
**	  fontdatabase.h's: so this file declares the four font classes as far as it uses them and casts the
**	  two globals. One declaration of each (as D107 did for OptionClass) removes the casts.
*/



















































































































































































































// 0x526DE0
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)	// NAME 1 [hInstance, hPrevInstance, lpCmdLine, nCmdShow]: Win32's
{
	ArgvClass::Init(lpCmdLine, "@");
	ArgvClass argv(false, true);
	bool purchase = argv.Find("-PURCHASE") != NULL;	// NAME 1 [purchase]: its text
	WWMemoryLogClass::Init();

	SetLastError(0);
	CreateMutex(NULL, TRUE, "enb_mutex_lock");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, "Earth & Beyond is already running.", "Error:", MB_OK);
		exit(-1);
	}

	if (Unknown_5FADE0() && !Unknown_847D20("..\\data\\client\\text\\demoEULA.txt")) {
		return 0;
	}
	if (!purchase && !Unknown_847D20("..\\data\\client\\text\\roc.txt")) {
		return 0;
	}

	Register_Thread_ID(GetCurrentThreadId(), "Main Thread", true);

	bool enable_exceptions = true;					// NAME 1 [enable_exceptions]: the value's name, inverted
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Westwood Studios\\Earth and Beyond", 0, KEY_READ, &key) == ERROR_SUCCESS) {
		DWORD type = REG_DWORD;
		DWORD value = 0;
		DWORD size = sizeof(value);
		LONG result = RegQueryValueEx(key, "DisableExceptions", NULL, &type, (LPBYTE)&value, &size);
		RegCloseKey(key);
		if (result == ERROR_SUCCESS && type == REG_DWORD) {
			enable_exceptions = value == 0;
		}
	}
	if (enable_exceptions) {
		Register_Application_Exception_Callback(Unknown_445350);
		Set_Exit_On_Exception(true);
		SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)Unknown_445330);
	}

	CoInitialize(NULL);

	_TheSimpleFileFactory->Set_Sub_Directory(".\\;..\\data\\client\\art\\;..\\data\\client\\avatar\\;..\\data\\client\\Font\\;..\\data\\client\\htmldocs\\;..\\data\\client\\ini\\;..\\data\\client\\movie\\;..\\data\\client\\sound\\;..\\data\\client\\sound\\talktree\\eng\\;..\\data\\common\\;..\\data\\common\\heightfields\\;..\\data\\client\\;..\\data\\client\\output\\;");
	ComboFileFactoryClass file_factory("..\\data\\client\\mixfiles\\");	// NAME 1 [file_factory]: what it is
	_TheFileFactory = &file_factory;
	((SimpleFileFactoryClass *)_TheWritingFileFactory)->Set_Sub_Directory("..\\data\\client\\output\\;");

	WWMath::Init();
	Unknown_BDBE4C = GetTickCount();
	Unknown_509E70();
	Unknown_Debug_Printf("Build: %s %s %s\n", Unknown_445610(), Unknown_445590(), Unknown_445560());

	Unknown_Debug_Begin_Step("initializing the asset manager");
	Unknown_BDADC0 = new UnknownBDADC0Class;
	if (Unknown_BDADC0 == NULL) {
		MessageBox(MainWindow, "Unable to initialize the asset manager", "WW3D Error", MB_OK);
		exit(0);
	}
	Unknown_Debug_End_Step("ok");

	Unknown_684E00();
	if (argv.Find("-MENU") && !Unknown_846C90()) {
		WW3DAssetManager::Delete_This();
		Unknown_509EB0();
		ArgvClass::Free();
		WWMath::Shutdown();
		CoUninitialize();
		return -1;
	}

	MainWindow = Unknown_431CA0(hInstance, 640, 480, 0);
	Unknown_Debug_Begin_Step("initializing WW3D");
	if (WW3D::Init(MainWindow, Unknown_B8F870, false) != WW3D_ERROR_OK) {
		MessageBox(MainWindow, "Unable to initialize WW3D", "WW3D Error", MB_OK);
		exit(0);
	}
	Unknown_Debug_End_Step("ok");
	Unknown_BFAE50 = true;
	WW3D::Enable_Sorting(false);
	Unknown_BFAE51 = true;
	Unknown_BFAE52 = false;
	const char *test_path = argv.Find_Value("-TEST_PATH");	// NAME 1 [test_path]: its text
	if (test_path != NULL) {
		_TheSimpleFileFactory->Prepend_Sub_Directory(test_path);
	}
	Unknown_BFAE4F = true;
	ProgramInstance = hInstance;

	Unknown_8B20D0Class *message_server = NULL;		// NAME 1 [message_server]: its step text
	if (argv.Find("-ENABLE_MESSAGES")) {
		Unknown_Debug_Begin_Step("starting message server");
		message_server = new Unknown_8B20D0Class;
		message_server->Unknown_8B2230();
		message_server->Run();
		Unknown_Debug_End_Step("ok");
	}

	Unknown_Debug_Begin_Step("initializing input handler");
	Unknown_42E4A0(MainWindow);
	Unknown_42EE00(0);
	Unknown_Debug_End_Step("ok");

	Unknown_Debug_Begin_Step("starting winsock services");
	WSADATA wsa_data;								// NAME 1 [wsa_data]: its type
	if (WSAStartup(0x101, &wsa_data) != 0) {
		Unknown_Debug_End_Step("failed");
	} else {
		Unknown_Debug_End_Step("ok");
	}

	Unknown_Debug_Begin_Step("initializing render state class");
	Unknown_4581E0();
	Unknown_Debug_End_Step("ok");

	Unknown_Debug_Begin_Step("initializing display handler");
	Unknown_442AC0(MainWindow);
	Unknown_Debug_End_Step("ok");

	bool reduction = false;							// NAME 1 [reduction]: what it gives
	if (Unknown_C006A0 != NULL) {
		reduction = Unknown_C006A0->UnknownDD;
	}
	WW3D::Enable_Large_Texture_Extra_Reduction(!reduction);

	if (Unknown_BDADB8 != 0) {
		Unknown_6A9B60Class *splash;				// NAME 1 [splash]: 0x6A9E40's 'Splash::Render'
		if (Unknown_5FADE0() || !Unknown_5FAD50()) {
			splash = new Unknown_6A9B60Class(2);
		} else {
			splash = new Unknown_6A9B60Class(0);
		}
		splash->Unknown_6A9E40();
		delete splash;
		Sleep(4000);

		Unknown_Debug_Begin_Step("initializing options manager");
		OptionClass::Set_Instance(new OptionClass);
		Unknown_Debug_End_Step("ok");
		Unknown_6832C0();

		Unknown_Debug_Begin_Step("initializing sound library");
		SoundLibraryClass::Init();
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("initializing sound manager");
		SoundManager = new SoundManagerClass;
		Unknown_Debug_End_Step("ok");

		KernelClass *kernel = new KernelClass;		// NAME 1 [kernel]: its class
		kernel->Cycle_Focus();
		kernel->Render_Enabled = true;
		Unknown_42F940(WM_ACTIVATEAPP, Unknown_445420);
		Unknown_42F940(WM_DESTROY, Unknown_4454A0);
		Unknown_42F940(WM_CLOSE, Unknown_4454C0);

		if (!purchase) {
			MovieTaskClass *movie = new MovieTaskClass;	// NAME 1 [movie]: its class
			movie->Add_Movie("..//data//client//mixfiles//eb_sizzle.bik", 0x8000);
			kernel->Add_Task(movie);
			kernel->Set_Focus(movie);
			if (argv.Find("-ENABLE_MESSAGES")) {
				Unknown_Debug_Begin_Step("adding message server to kernel");
				kernel->Add_Task(message_server);
				message_server->Run();
				Unknown_Debug_End_Step("ok");
			}
		}

		Unknown_Debug_Begin_Step("starting up font system");
		Unknown_BE67B8 = (Unknown_BE67B8Class *)new FontDatabaseClass;
		Unknown_BE67C4 = new FontManagerClass;
		Unknown_BE67B4 = new Unknown_6BCFB0Class;
		Unknown_BE67BC = (SClientFontDatabaseClass *)new FontStringFilesClass;
		if (!purchase) {
			file_auto_ptr script(_TheFileFactory, "moviescript1.txt");	// NAME 1 [script]: its text
			if (script->Is_Available()) {
				ScriptedMovieTaskClass *scripted = new ScriptedMovieTaskClass("moviescript", 1);	// NAME 1 [scripted]: its class
				scripted->Init(kernel);
				kernel->Add_Task(scripted);
			}
		}
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("starting up Personality manager system");
		Unknown_790940(new Unknown_790560Class);
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("starting up Buff Database system");
		BuffDatabaseClass::Set_Instance(new BuffDatabaseClass);
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("starting up Tutorial Manager system");
		TutorialManagerClass::Set_Instance(new TutorialManagerClass);
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("initializing starbase lights manager");
		Unknown_764510(new Unknown_764480Class);
		Unknown_Debug_End_Step("ok");

		Unknown_7CA2D0();
		DX8WebBrowser::Initialize();

		if (!purchase) {
			Unknown_Debug_Begin_Step("starting menu task");
			kernel->Add_Task(new MenuTaskClass);
			Unknown_Debug_End_Step("ok");
		} else {
			LoginTaskClass *login = new LoginTaskClass(7);	// NAME 1 [login]: its class
			login->Set_Step(0, false);
			kernel->Add_Task(login);
			kernel->Set_Focus(login);
		}

		Unknown_4BC000("", "", __FILE__, __LINE__, "Server initialized & ready to go");
		kernel->Run();

		Unknown_Debug_Begin_Step("shutting down kernel");
		kernel->Shutdown();
		delete kernel;
		Unknown_42FAC0(WM_ACTIVATEAPP, Unknown_445420);
		Unknown_42FAC0(WM_DESTROY, Unknown_4454A0);
		Unknown_42FAC0(WM_CLOSE, Unknown_4454C0);
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down starbase lights manager");
		delete Unknown_BE82D0;
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down tutorial manager system");
		delete TutorialManagerClass::Instance;
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down buff database system");
		delete BuffDatabaseClass::Instance;
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down Personality manager system");
		delete Unknown_BE856C;
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down font system");
		delete Unknown_BE67B4;
		delete (FontStringFilesClass *)Unknown_BE67BC;
		delete Unknown_BE67C4;
		delete (FontDatabaseClass *)Unknown_BE67B8;
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down sound manager");
		delete SoundManager;
		Unknown_Debug_End_Step("ok");

		Unknown_Debug_Begin_Step("shutting down sound library");
		SoundLibraryClass::Shutdown();
		Unknown_Debug_End_Step("ok");
		Unknown_683940();

		Unknown_Debug_Begin_Step("shutting down options manager");
		delete OptionClass::Instance;
		Unknown_Debug_End_Step("ok");
	}

	Unknown_Debug_Begin_Step("freeing assets");
	Unknown_BDADC0->Free_Assets();
	Unknown_Debug_End_Step("ok");

	Unknown_Debug_Begin_Step("shutting down the asset manager");
	WW3DAssetManager::Delete_This();
	Unknown_Debug_End_Step("ok");

	Unknown_7CA2D0()->Unknown_7CB3D0();
	Unknown_7CA2D0();
	DX8WebBrowser::Shutdown();

	Unknown_Debug_Begin_Step("shutting down display handler");
	Unknown_442FF0();
	Unknown_Debug_End_Step("ok");

	Unknown_Debug_Begin_Step("shutting down render state class");
	Unknown_458250();
	Unknown_Debug_End_Step("ok");

	Unknown_Debug_Begin_Step("uninitializing input handler");
	Unknown_42E7B0();
	Unknown_Debug_End_Step("ok");

	Unknown_Debug_Begin_Step("shutting down winsock services");
	WSACleanup();
	Unknown_Debug_End_Step("ok");

	WWMath::Shutdown();
	Unknown_509EB0();
	Unregister_Thread_ID(GetCurrentThreadId(), "Main Thread");
	ArgvClass::Free();
	CordMgrTaskClass::Shutdown_Task();
	CoUninitialize();
	return 0;
}
