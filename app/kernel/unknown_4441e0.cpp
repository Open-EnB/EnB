// Earth & Beyond client (reconstruction): app\kernel\unknown_4441e0.cpp. Notes at the end of the file.
#include "random.h"

RandomClass Unknown_BDBB84;				// 0xBDBB84 (initialiser 0x4441E0)  // NAME 1: neutral (D10)
Random2Class Unknown_BDADC8;			// 0xBDADC8 (initialiser 0x444200)  // NAME 1: neutral (D10)
Random3Class Unknown_BDB1B8;			// 0xBDB1B8 (initialiser 0x444220; nothing reads it)  // NAME 1: neutral (D10)
Random4Class Unknown_BDB1C0(0x1105);	// 0xBDB1C0 (initialiser 0x444240)  // NAME 1: neutral (D10)

#include <iostream>
#include <windows.h>
#include "..\..\wwlib\Except.h"
#include "ww3d.h"
#include "WWAudio.h"
#include "kernel.h"
#include <string>	// <string>'s guard 0x445540 (CRT entry 23) closes the object

void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line
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

/*
**	Notes
**
**	RECONSTRUCTED (XI, 2026-11-17): client.exe 0x4441E0-0x445560, an object of its own after kernel.cpp: the CRT
**	initialiser table lists, in this order, the four generators' initialisers (entries 17-20: 0x4441E0 RandomClass
**	0xBDBB84, 0x444200 Random2Class 0xBDADC8, 0x444220 Random3Class(0, 0) 0xBDB1B8, 0x444240 Random4Class(0x1105)
**	0xBDB1C0), then <iostream>'s ios_base::Init (0x4442C0, atexit 0x444380) and _Winit (0x445260, atexit 0x445300) and
**	<string>'s guard 0x445540 last; so the generators are defined before <iostream> is included. SG (run 2026-11-18):
**	the object also holds 0x445330-0x4454C0 (main.cpp's exception hooks and window-message handlers, whose log texts
**	start 'Main.cpp:'), before the guard; main.cpp's own initialisers are entries 24-27. Its other code is
**	library instances only (0x444290, 0x444F60, 0x4450B0); 0x444390-0x444F60 is int3 fill. No string names the file:
**	the name and directory are PROVISIONAL (D137).
*/
