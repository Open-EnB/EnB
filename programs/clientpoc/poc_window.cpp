// Earth & Beyond client (reconstruction): the proof-of-concept client's window and display (D179). NOT THE CLIENT'S CODE.
/*
**	Functional stand-ins, written from Ghidra's pseudocode of the client's functions, for what WinMain
**	(app\main\main_client.cpp) needs before anything can draw: display.cpp's start-up / shut-down (0x442AC0 / 0x442FF0) and E&B's asset manager's constructor
**	(0x48B070). Delete one here when its real function lands in app/ (the linker reports the duplicate).
**
**	ENB_POC_WINDOWED=1 in the environment makes the display windowed (the client's default is the registry's
**	render device, else 800x600x32 full screen).
*/



#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\wwlib\registry.h"

// ---------------------------------------------------------------- no registry changes (D69)

// RegistryClass opens every key read only and writes nothing from before WinMain on (IsLocked is constant-
// initialized, so this dynamic initializer runs after it whatever the objects' order).
static struct PocRegistryReadOnly
{
	PocRegistryReadOnly() { RegistryClass::Set_Read_Only(true); }
} Poc_Registry_Read_Only;

// ---------------------------------------------------------------- the crash line (stubs.log)

// A first-chance vectored handler (kernel32's, XP on: looked up, VC6's headers lack it) writes the faulting
// address and the code addresses on the stack as one 'CRASH:' line (tools/harness/symbolize.py names them
// with client.map) before the client's own filter (0x445330) ends the program. It only logs.
static LONG CALLBACK Poc_Crash_Log(EXCEPTION_POINTERS *info)
{
	static int count = 0;
	DWORD code = info->ExceptionRecord->ExceptionCode;
	if ((code != EXCEPTION_ACCESS_VIOLATION && code != EXCEPTION_ILLEGAL_INSTRUCTION && code != EXCEPTION_INT_DIVIDE_BY_ZERO
			&& code != EXCEPTION_STACK_OVERFLOW && code != EXCEPTION_PRIV_INSTRUCTION) || count >= 4) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	count++;
	char line[1024];
	int n = sprintf(line, "CRASH: code %08X at %08X (address %08X) stack:", code, (unsigned)info->ContextRecord->Eip,
		code == EXCEPTION_ACCESS_VIOLATION ? (unsigned)info->ExceptionRecord->ExceptionInformation[1] : 0);
	unsigned *sp = (unsigned *)info->ContextRecord->Esp;
	int found = 0;
	for (int i = 0; i < 2048 && found < 24 && n < (int)sizeof(line) - 12; i++) {
		if (IsBadReadPtr(sp + i, 4)) {
			break;
		}
		unsigned value = sp[i];
		if (value >= 0x401000 && value < 0x400000 + 0x300000) {		// the exe's code (client.map: .text)
			n += sprintf(line + n, " %08X", value);
			found++;
		}
	}
	Stub_Log_Line("%s", line);
	return EXCEPTION_CONTINUE_SEARCH;
}

typedef PVOID (WINAPI *PocAddVectoredHandler)(ULONG, LONG (CALLBACK *)(EXCEPTION_POINTERS *));

static struct PocCrashLog
{
	PocCrashLog()
	{
		PocAddVectoredHandler add = (PocAddVectoredHandler)GetProcAddress(GetModuleHandle("kernel32.dll"), "AddVectoredExceptionHandler");
		if (add != NULL) {
			add(1, Poc_Crash_Log);
		}
	}
} Poc_Crash_Log_Installed;

// ---------------------------------------------------------------- the window-message handlers

// The list is app\input\winmsg.cpp's (0x42F940 add, 0x42FAC0 remove, 0x42FBA0 dispatch).
void Unknown_42F940(int message, int (*handler)(int, int));
void Unknown_42FAC0(int message, int (*handler)(int, int));
int Unknown_42FBA0(int message, int wparam, int lparam);

// ---------------------------------------------------------------- the main window

// app\input\w32window.cpp has it (0x431CA0) and its window procedure (0x431DE0).

// ---------------------------------------------------------------- display.cpp

int Unknown_BDADB8;		// 0xBDADB8: the display's window (0: no display)

// 0x442FA0: display.cpp's WM_PAINT handler (validates the window)
static int Unknown_442FA0(int message, int wparam)
{
	ValidateRect((HWND)Unknown_BDADB8, NULL);
	return 0;
}

// 0x442AC0: the render device from the registry (read only), else the first device at 800x600x32 full screen
void Unknown_442AC0(HWND window)
{
	STUB_CALLED("POC Unknown_442AC0 (0x442AC0, display.cpp): the render device");
	Unknown_BDADB8 = (int)window;
	char device[256];
	strcpy(device, "DirectX7.0 ");
	int width = 800;
	int height = 600;
	int bits = 32;
	int windowed = 0;
	int texture_depth = 32;
	char name[256];
	name[0] = 0;
	int w = -1, h = -1, b = -1, win = -1, td = -1;
	if (WW3D::Registry_Load_Render_Device("Software\\Westwood Studios\\Earth and Beyond\\Render", name, sizeof(name), w, h, b, win, td)) {
		if (name[0] != 0) {
			strcpy(device, name);
		}
		if (w != -1 && h != -1) {
			width = w;
			height = h;
		}
		if (b != -1) {
			bits = b;
		}
		if (win != -1) {
			windowed = win;
		}
		if (td != -1) {
			texture_depth = td;
		}
	}
	const char *env = getenv("ENB_POC_WINDOWED");
	if (env != NULL && *env == '1') {
		windowed = 1;
	}
	Stub_Log_Line("POC display: device '%s' %dx%dx%d windowed %d", device, width, height, bits, windowed);
	bool ok = device[0] != 0 && WW3D::Set_Render_Device(device, width, height, bits, windowed, true) == WW3D_ERROR_OK;
	if (!ok) {
		WW3DErrorType error = WW3D::Set_Render_Device(0, width, height, bits, windowed, true);
		Stub_Log_Line("POC display: Set_Render_Device(0) -> %d", (int)error);
	}
	WW3D::Set_Gamma(1.2f, 0.0f, 1.0f, true);
	WW3D::Set_NPatches_Level(4);
	int set_w, set_h, set_bits;
	bool set_windowed;
	WW3D::Get_Device_Resolution(set_w, set_h, set_bits, set_windowed);
	int cx, cy;
	HWND after;
	if (!set_windowed) {
		cx = GetSystemMetrics(SM_CXSCREEN);
		cy = GetSystemMetrics(SM_CYSCREEN);
		after = HWND_TOP;
	} else {
		RECT rect;
		GetWindowRect(window, &rect);
		cx = rect.right - rect.left;
		cy = rect.bottom - rect.top;
		after = HWND_NOTOPMOST;
	}
	SetWindowPos(window, after, 0, 0, cx, cy, 0);
	ShowWindow(window, SW_SHOW);
	SetFocus(window);
	Unknown_42F940(WM_PAINT, Unknown_442FA0);
	UpdateWindow(window);
	// the client registers three of E&B's prototype loaders here (0xBFAE1C, 0xBFA960, 0xBFA954)
	Stub_Log_Line("POC display: %dx%dx%d windowed %d; E&B's prototype loaders 0xBFAE1C / 0xBFA960 / 0xBFA954 not registered",
		set_w, set_h, set_bits, (int)set_windowed);
}

// 0x442FF0: display.cpp's shut-down
void Unknown_442FF0()
{
	STUB_CALLED("POC Unknown_442FF0 (0x442FF0, display.cpp): WW3D::Shutdown");
	Unknown_42FAC0(WM_PAINT, Unknown_442FA0);
	WW3D::Shutdown();
}

// ---------------------------------------------------------------- E&B's asset manager

class UnknownBDADC0Class : public WW3DAssetManager
{
public:
	UnknownBDADC0Class();
};

// 0x48B070: the tree's WW3DAssetManager alone (E&B's overrides are not written yet)
UnknownBDADC0Class::UnknownBDADC0Class()
{
	STUB_CALLED("POC UnknownBDADC0Class (0x48B070): a plain WW3DAssetManager");
}
