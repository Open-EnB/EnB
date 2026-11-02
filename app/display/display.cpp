// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\display\display.cpp (client 0x442AC0-0x443003)
**
**	The display handler: WinMain's start-up (0x442AC0) picks the render device from the registry
**	(read only), sets the window's size, registers the WM_PAINT handler and E&B's three prototype
**	loaders; its shut-down (0x442FF0). The texture depth report sits on the client's line 260 (D25).
**	Notes at the end.
*/



#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "..\..\wwlib\argv.h"
#include "ww3d.h"
#include "assetmgr.h"
#include "texturethumbnail.h"
#include "part_ldr.h"
#include "ringobj.h"
#include "sphereobj.h"
#include "..\input\winmsg.h"

// dbgmgr.cpp's log
void Unknown_Debug_Printf(const char *format, ...);	// 0x509DC0 (dbgmgr.cpp, D38)

// E&B's asset manager (main_client.cpp makes it)
class UnknownBDADC0Class : public WW3DAssetManager	// NAME 1: neutral (D10)
{
public:
	UnknownBDADC0Class();							// 0x48B070
};
extern UnknownBDADC0Class *Unknown_BDADC0;			// NAME 1 [Unknown_BDADC0]: 0xBDADC0

// a render device's settings: the default and the registry's (two 0x114-byte locals of 0x442AC0)
struct UnknownDisplayStruct							// NAME 1 [UnknownDisplayStruct]: neutral (D10)
{
	char Name[256];									// NAME 2 [Name]: the device name Set_Render_Device takes
	int Width;										// NAME 2 [Width]: Registry_Load_Render_Device's argument
	int Height;										// NAME 2 [Height]: Registry_Load_Render_Device's argument
	int Bits;										// NAME 2 [Bits]: Registry_Load_Render_Device's depth
	int Windowed;									// NAME 2 [Windowed]: Registry_Load_Render_Device's argument
	int TextureDepth;								// NAME 2 [TextureDepth]: the assert text 'texture depth'
};

int Unknown_BDADB8;									// 0xBDADB8: the display's window (0: no display)  // NAME 1 [Unknown_BDADB8]: as main_client.cpp

static int Unknown_442FA0(int message, int wparam);	// NAME 1 [Unknown_442FA0, message, wparam]: neutral (D10)





















































































































































// 0x442AC0: the display's start-up
void Unknown_442AC0(HWND window)						// NAME 1 [Unknown_442AC0]: as main_client.cpp  // NAME 1 [window]: what WinMain passes
{
	Unknown_BDADB8 = (int)window;

	UnknownDisplayStruct current;					// NAME 1 [current]: neutral (D10)
	current.Width = 800;
	current.Height = 600;
	current.Bits = 32;
	current.Windowed = 0;
	current.TextureDepth = 32;
	strcpy(current.Name, "DirectX7(0)");

	WW3D::Set_Default_Native_Screen_Size(0.25f);

	UnknownDisplayStruct reg;						// NAME 1 [reg]: neutral (D10)
	reg.Width = -1;
	reg.Height = -1;
	reg.Bits = -1;
	reg.Windowed = -1;
	reg.TextureDepth = -1;
	reg.Name[0] = 0;
	if (WW3D::Registry_Load_Render_Device("Software\\Westwood Studios\\Earth and Beyond\\Render", reg.Name, sizeof(reg.Name),
			reg.Width, reg.Height, reg.Bits, reg.Windowed, reg.TextureDepth)) {
		if (reg.Name[0] != 0) {
			strcpy(current.Name, reg.Name);
		}
		if (reg.Width != -1 && reg.Height != -1) {
			current.Width = reg.Width;
			current.Height = reg.Height;
		}
		if (reg.Bits != -1) {
			current.Bits = reg.Bits;
		}
		if (reg.Windowed != -1) {
			current.Windowed = reg.Windowed;
		}
		if (reg.TextureDepth != -1) {
			current.TextureDepth = reg.TextureDepth;
		}
	}

	if (current.Name[0] == 0 || WW3D::Set_Render_Device(current.Name, current.Width, current.Height, current.Bits, current.Windowed, true) != WW3D_ERROR_OK) {
		if (WW3D::Set_Render_Device(0, current.Width, current.Height, current.Bits, current.Windowed, false) != WW3D_ERROR_OK) {
			WW3D::Set_Any_Render_Device();
		}
	}

	WW3D::Set_Gamma(1.2f, 0.0f, 1.0f, true);

	ArgvClass args(false, true);
	if (args.Find("-NOTFC") == NULL) {
		if (current.Name[0] != 0) {
			sprintf(reg.Name, "G-%s-%d", current.Name, current.Bits);
		} else {
			strcpy(reg.Name, "G");
		}
	}

	if (current.TextureDepth != 32 && current.TextureDepth != 16) {
		Unknown_Debug_Printf("%s(%d): texture depth = %d\n", __FILE__, __LINE__, current.TextureDepth);
	}
	WW3D::Set_Texture_Bitdepth(current.TextureDepth);
	ThumbnailManagerClass::Create_Thumbnail_If_Not_Found(true);
	WW3D::Set_NPatches_Level(4);

	int set_width;									// NAME 1 [set_width]: Get_Device_Resolution's
	int set_height;									// NAME 1 [set_height]: Get_Device_Resolution's
	int set_bits;									// NAME 1 [set_bits]: Get_Device_Resolution's
	bool set_windowed;								// NAME 1 [set_windowed]: Get_Device_Resolution's
	WW3D::Get_Device_Resolution(set_width, set_height, set_bits, set_windowed);
	if (set_windowed) {
		RECT rect;
		GetWindowRect((HWND)Unknown_BDADB8, &rect);
		SetWindowPos((HWND)Unknown_BDADB8, HWND_NOTOPMOST, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0);
	} else {
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos((HWND)Unknown_BDADB8, HWND_TOP, 0, 0, cx, cy, 0);
	}
	ShowWindow((HWND)Unknown_BDADB8, SW_SHOW);
	SetFocus((HWND)Unknown_BDADB8);
	Unknown_42F940(WM_PAINT, Unknown_442FA0);
	UpdateWindow((HWND)Unknown_BDADB8);

	Unknown_BDADC0->Register_Prototype_Loader(&_ParticleEmitterLoader);
	Unknown_BDADC0->Register_Prototype_Loader(&_RingLoader);
	Unknown_BDADC0->Register_Prototype_Loader(&_SphereLoader);
}

// 0x442FA0: WM_PAINT
static int Unknown_442FA0(int message, int wparam)
{
	PAINTSTRUCT paint;
	HDC dc = BeginPaint((HWND)Unknown_BDADB8, &paint);
	FillRect(dc, &paint.rcPaint, (HBRUSH)COLOR_ACTIVECAPTION);
	EndPaint((HWND)Unknown_BDADB8, &paint);
	return(-1);
}

// 0x442FF0: the display's shut-down
void Unknown_442FF0()								// NAME 1 [Unknown_442FF0]: as main_client.cpp
{
	Unknown_42FAC0(WM_PAINT, Unknown_442FA0);
	WW3D::Shutdown();
}
/*
**	Reconstruction notes
**
**	client.exe: display.cpp's object, 0x442AC0-0x443003 (__FILE__ in the texture depth report, line
**	260). Exact: the start-up 0x442AC0, its WM_PAINT handler 0x442FA0 (file-local; the tools'
**	function starts lack it, so appfile reports 0x442AC0 PREFIX), the shut-down 0x442FF0. Its
**	static is the window 0xBDADB8 (kernel.cpp's statics follow at 0xBDADBC).
**	The two settings blocks are one struct: the client keeps both at 0x114-byte strides (name, then
**	width, height, bits, windowed, texture depth), the registry's filled by Registry_Load_Render_Device.
**	The '-NOTFC' test builds a name ("G-<device>-<bits>", else "G") into the registry block's buffer
**	and nothing reads it: the client's code as compiled. The stores 0xBB964C (0.25) and 0xBFAEA5 (1)
**	are WW3D::DefaultNativeScreenSize and ThumbnailManagerClass::CreateThumbnailIfNotFound; the three
**	loaders 0xBFAE1C / 0xBFA960 / 0xBFA954 are _ParticleEmitterLoader, _RingLoader, _SphereLoader
**	(their static initialisers 0x914300 / 0x9134F0 and the sphere's uses).
*/
