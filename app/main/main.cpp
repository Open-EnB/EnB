// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; the object at client 0x445330-0x445725 has no __FILE__ string, and
**	its log texts start 'Main.cpp:', so the name is main.cpp; the folder is a guess (app\main, beside
**	main_client.cpp, whose WinMain calls nearly everything here). Interim placement (D137).
**
**	WinMain's helpers: the exception hooks, the window-message handlers, the build texts and
**	versions. Notes at the end.
*/



#include <windows.h>
#include "..\..\wwlib\Except.h"
#include "..\..\wwlib\win.h"
#include "ww3d.h"
#include "WWAudio.h"
#include "..\kernel\kernel.h"

void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line

struct Unknown_VersionStruct							// NAME 1 [Unknown_VersionStruct]: two ints (tsk_login.cpp reads Major / Minor of 0x4456C0's)
{
	Unknown_VersionStruct(int major = 0, int minor = 0) : Major(major), Minor(minor) {}	// NAME 1 [major, minor]: its members
	int Major;											// NAME 1: tsk_login.cpp's
	int Minor;											// NAME 1: tsk_login.cpp's
};

unsigned long Unknown_BDBE4C;							// 0xBDBE4C: WinMain's GetTickCount at start-up  // NAME 1 [Unknown_BDBE4C]: neutral (D10)
static bool Unknown_BDBE50;								// 0xBDBE50  // NAME 1 [Unknown_BDBE50]: neutral (D10)

// 0x445330
int Unknown_445330(EXCEPTION_POINTERS *info)
{
	return Exception_Handler(info->ExceptionRecord->ExceptionCode, info);
}

// 0x445350
void Unknown_445350()
{
	if (!Unknown_BDBE50) {
		Unknown_BDBE50 = true;
	}
}

// 0x445420
int Unknown_445420(int unknown, int active)
{
	if (WW3D::Is_Windowed()) {
		return -1;
	}
	KernelClass *kernel = KernelClass::Get_Instance();
	if (kernel == NULL) {
		return -1;
	}
	if (active) {
		Unknown_Debug_Print("Main.cpp: activating kernel rendering.");
		kernel->Render_Enabled = true;
		if (WWAudioClass::Get_Instance() != NULL) {
			WWAudioClass::Get_Instance()->Temp_Disable_Audio(false);
		}
		WW3D::On_Activate_App();
	} else {
		Unknown_Debug_Print("Main.cpp: deactivating kernel rendering.");
		kernel->Render_Enabled = false;
		if (WWAudioClass::Get_Instance() != NULL) {
			WWAudioClass::Get_Instance()->Temp_Disable_Audio(true);
		}
		WW3D::On_Deactivate_App();
	}
	return -1;
}

// 0x4454A0
int Unknown_4454A0(int unknown, int unknown2)
{
	KernelClass *kernel = KernelClass::Get_Instance();
	if (kernel != NULL) {
		kernel->Render_Enabled = false;
		kernel->Quit = true;
	}
	return -1;
}

// 0x4454C0
int Unknown_4454C0(int unknown, int unknown2)
{
	return 0;
}

static char Unknown_BDBE90[128];						// 0xBDBE90  // NAME 1 [Unknown_BDBE90]: neutral (D10)
static char Unknown_BDBF10[128];						// 0xBDBF10  // NAME 1 [Unknown_BDBF10]: neutral (D10)
static char Unknown_BDBF90[128];						// 0xBDBF90  // NAME 1 [Unknown_BDBF90]: neutral (D10)

// 0x445560
char *Unknown_445560()
{
	LoadString(ProgramInstance, 1, Unknown_BDBE90, sizeof(Unknown_BDBE90));
	return Unknown_BDBE90;
}

// 0x445590
char *Unknown_445590()
{
	LoadString(ProgramInstance, 2, Unknown_BDBF10, sizeof(Unknown_BDBF10));
	return Unknown_BDBF10;
}

// 0x4455C0
char *Unknown_4455C0()
{
	LoadString(ProgramInstance, 3, Unknown_BDBF90, sizeof(Unknown_BDBF90));
	return Unknown_BDBF90;
}

// 0x4455F0
char *Unknown_4455F0()
{
	return "Windows";
}

char *Unknown_B704A0 = "Release";						// 0xB704A0  // NAME 1 [Unknown_B704A0]: neutral (D10)

// 0x445610
char *Unknown_445610()
{
	return Unknown_B704A0;
}

static Unknown_VersionStruct Unknown_BDBE78(42);		// 0xBDBE78 (initialiser 0x445630)  // NAME 1 [Unknown_BDBE78]: neutral (D10)
static Unknown_VersionStruct Unknown_BDBE80;			// 0xBDBE80 (0x445660)  // NAME 1 [Unknown_BDBE80]: neutral (D10)
static Unknown_VersionStruct Unknown_BDBE88;			// 0xBDBE88 (0x445680)  // NAME 1 [Unknown_BDBE88]: neutral (D10)
static Unknown_VersionStruct Unknown_BDBE70;			// 0xBDBE70 (0x4456A0)  // NAME 1 [Unknown_BDBE70]: neutral (D10)

// 0x4456C0
Unknown_VersionStruct *Unknown_4456C0()
{
	return &Unknown_BDBE78;
}

// 0x4456E0
Unknown_VersionStruct *Unknown_4456E0()
{
	return &Unknown_BDBE80;
}

// 0x445700
Unknown_VersionStruct *Unknown_445700()
{
	return &Unknown_BDBE88;
}

// 0x445720
Unknown_VersionStruct *Unknown_445720()
{
	return &Unknown_BDBE70;
}


/*
**	Reconstruction notes (2026-11-01, builder MC): every function above is byte-exact with appfile.py
**	match, the four version initialisers ($E functions 0x445630, 0x445660, 0x445680, 0x4456A0: the
**	first stores 42, 0) included.
**
**	* 0x445330 goes to SetUnhandledExceptionFilter (cast, it is cdecl) and 0x445350 to Except.cpp's
**	  Register_Application_Exception_Callback, both from WinMain; the handlers 0x445420 (WM_ACTIVATEAPP),
**	  0x4454A0 (WM_DESTROY) and 0x4454C0 (WM_CLOSE) go to input.cpp's 0x42F940. Their first argument is
**	  not read; the second is WM_ACTIVATEAPP's wParam.
**	* 0x917840 is WW3D::Is_Windowed (it returns DX8Wrapper's IsWindowed 0xC0063D); 0x918FA0 is
**	  WW3D::On_Activate_App (WW3D_DX8 is not defined: `return WW3D_ERROR_OK`); 0xC404D8 is
**	  WWAudioClass's instance.
**	* The object's edges: it ends at 0x445725 (the next function 0x445740 does vector arithmetic on a
**	  transform: another file). Before 0x445330 lie 0x444290 (SysFreeString of a member: a _bstr_t
**	  Data_t's), 0x444F60 and 0x4450B0 (copies of a string with a trailing field), after kernel.cpp's
**	  list helpers; whether they belong to this object is not known. 0x445510 (`delete` of the pointer
**	  at the object's start; no reference in the client) sits between the handlers and the texts and is
**	  not written: its class is unknown.
**	* The bss order (0xBDBE4C, 0xBDBE50, 0xBDBE70-0xBDBE8C, 0xBDBE90-0xBDC00F) is not checked.
*/
