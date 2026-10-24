/*
**	Earth & Beyond VIDTEST (reconstruction, D45): the program's own code
**
**	L2 (docs/milestones.md 3.2): the first executable built from the tree. The shape of VIDTEST's
**	WinMain (0x4235C0, docs/vidtest.md 1.1 and 1.3): a window, WWMath::Init, WW3D::Init(hwnd),
**	WW3D::Set_Render_Device, a render loop, then WW3D::Shutdown and WWMath::Shutdown. No scenes
**	yet: each frame only clears to a colour.
**
**	Test options (ours, not VIDTEST's):
**	  -frames N    quit by itself after N frames (default 0: run until the window is closed)
**	  -shot FILE   save the back buffer of frame -shotframe N to FILE (.bmp)
**	               (default N: the -frames count, else 60)
**	  -log FILE    init results (default vidtest.log)
**	  -resx N -resy N -depth N -windowed B -device NAME   as VIDTEST
**	Exit code: 0 on success, 10+ on an init failure (11 window, 12 WW3D::Init, 13 no device,
**	14 Set_Render_Device, 15 Begin_Render/End_Render, 16 screenshot).
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ww3d.h"
#include "assetmgr.h"
#include "dx8wrapper.h"
#include "wwmath.h"
#include "vector3.h"
#include "wwdebug.h"
#include "win.h"

static FILE *		LogFile = NULL;
static bool			Quit = false;

static void Log(const char *format, ...)
{
	char line[1024];
	va_list va;
	va_start(va, format);
	_vsnprintf(line, sizeof(line) - 1, format, va);
	va_end(va);
	line[sizeof(line) - 1] = 0;
	OutputDebugStringA(line);
	if (LogFile) {
		fputs(line, LogFile);
		fflush(LogFile);
	}
}

static void Debug_Message(DebugType type, const char *message)
{
	Log("[wwdebug %d] %s", (int)type, message);
}

static void Debug_Assert(const char *message)
{
	Log("[wwassert] %s\n", message);
}

/*
** A crash report for unattended runs (ours, not VIDTEST's): the exception, the registers and the
** stack words that point into this image's code (return-address candidates; the libraries are
** built without frame pointers), to be named with the map file
** (tools/harness/symbolize.py).
*/
static LONG WINAPI Crash_Filter(EXCEPTION_POINTERS *ep)
{
	HMODULE base = GetModuleHandleA(NULL);
	IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)((char *)base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
	DWORD lo = (DWORD)base + nt->OptionalHeader.BaseOfCode;
	DWORD hi = lo + nt->OptionalHeader.SizeOfCode;
	CONTEXT *c = ep->ContextRecord;
	Log("CRASH: exception %08lX at %08lX (image base %08lX)\n", ep->ExceptionRecord->ExceptionCode,
		(DWORD)ep->ExceptionRecord->ExceptionAddress, (DWORD)base);
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		Log("CRASH: %s address %08lX\n", ep->ExceptionRecord->ExceptionInformation[0] ? "write" : "read",
			ep->ExceptionRecord->ExceptionInformation[1]);
	}
	Log("CRASH: eax=%08lX ebx=%08lX ecx=%08lX edx=%08lX esi=%08lX edi=%08lX ebp=%08lX esp=%08lX\n",
		c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
	DWORD *sp = (DWORD *)c->Esp;
	for (int i = 0; i < 2048; i++) {
		if (IsBadReadPtr(sp + i, 4)) break;
		DWORD v = sp[i];
		if (v >= lo && v < hi) {
			Log("CRASH: stack +%04X %08lX\n", i * 4, v);
		}
	}
	if (LogFile) fflush(LogFile);
	return EXCEPTION_EXECUTE_HANDLER;
}

/*
** The command line, as VIDTEST's ArgvClass helpers read it: the token after the name.
*/
static char		ArgBuffer[1024];
static char *	Args[64];
static int		ArgCount = 0;

static void Split_Args(const char *cmdline)
{
	strncpy(ArgBuffer, cmdline ? cmdline : "", sizeof(ArgBuffer) - 1);
	char *p = ArgBuffer;
	while (*p && ArgCount < 64) {
		while (*p == ' ' || *p == '\t') *p++ = 0;
		if (!*p) break;
		if (*p == '"') {
			Args[ArgCount++] = ++p;
			while (*p && *p != '"') p++;
			if (*p) *p++ = 0;
		} else {
			Args[ArgCount++] = p;
			while (*p && *p != ' ' && *p != '\t') p++;
		}
	}
}

static const char *Arg_String(const char *name, const char *def)
{
	for (int i = 0; i + 1 < ArgCount; i++) {
		if (stricmp(Args[i], name) == 0) return Args[i + 1];
	}
	return def;
}

static int Arg_Int(const char *name, int def)
{
	const char *s = Arg_String(name, NULL);
	return s ? atoi(s) : def;
}

static bool Arg_Bool(const char *name, bool def)
{
	const char *s = Arg_String(name, NULL);
	if (!s) return def;
	return s[0] == 'T' || s[0] == 't' || s[0] == '1' || s[0] == 'Y' || s[0] == 'y';
}

static LRESULT CALLBACK Wnd_Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		case WM_CLOSE:
		case WM_DESTROY:
			Quit = true;
			PostQuitMessage(0);
			return 0;
		case WM_ACTIVATEAPP:
			if (wparam) {
				WW3D::On_Activate_App();
			} else {
				WW3D::On_Deactivate_App();
			}
			break;
	}
	return DefWindowProcA(hwnd, msg, wparam, lparam);
}

/*
** Saves the frame being rendered to a 24-bit .bmp: the D3D back buffer, copied into a system-memory
** surface (CreateImageSurface + CopyRects) after the clear and before End_Render presents it.
** Only this program's own frame is read; nothing on the screen is.
*/
static bool Save_Back_Buffer(const char *filename)
{
	IDirect3DDevice8 *dev = DX8Wrapper::_Get_D3D_Device8();
	if (!dev) return false;
	IDirect3DSurface8 *bb = NULL;
	HRESULT hr = dev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &bb);
	if (FAILED(hr) || !bb) {
		Log("GetBackBuffer failed: %08lX\n", hr);
		return false;
	}
	D3DSURFACE_DESC desc;
	bb->GetDesc(&desc);
	IDirect3DSurface8 *copy = NULL;
	hr = dev->CreateImageSurface(desc.Width, desc.Height, desc.Format, &copy);
	if (SUCCEEDED(hr)) {
		hr = dev->CopyRects(bb, NULL, 0, copy, NULL);
	}
	bb->Release();
	if (FAILED(hr)) {
		Log("back buffer copy failed: %08lX (format %d)\n", hr, (int)desc.Format);
		if (copy) copy->Release();
		return false;
	}
	D3DLOCKED_RECT lr;
	hr = copy->LockRect(&lr, NULL, D3DLOCK_READONLY);
	if (FAILED(hr)) {
		Log("LockRect failed: %08lX\n", hr);
		copy->Release();
		return false;
	}
	int w = desc.Width;
	int h = desc.Height;
	int stride = ((w * 3) + 3) & ~3;
	unsigned char *rgb = new unsigned char[stride * h];
	memset(rgb, 0, stride * h);
	bool ok = true;
	for (int y = 0; y < h; y++) {
		const unsigned char *src = (const unsigned char *)lr.pBits + lr.Pitch * y;
		unsigned char *dst = rgb + stride * (h - 1 - y);		// .bmp rows go bottom-up
		for (int x = 0; x < w; x++) {
			unsigned r, g, b;
			if (desc.Format == D3DFMT_X8R8G8B8 || desc.Format == D3DFMT_A8R8G8B8) {
				b = src[x * 4]; g = src[x * 4 + 1]; r = src[x * 4 + 2];
			} else if (desc.Format == D3DFMT_R5G6B5) {
				unsigned v = ((const unsigned short *)src)[x];
				r = (v >> 11) & 31; g = (v >> 5) & 63; b = v & 31;
				r = (r << 3) | (r >> 2); g = (g << 2) | (g >> 4); b = (b << 3) | (b >> 2);
			} else if (desc.Format == D3DFMT_X1R5G5B5 || desc.Format == D3DFMT_A1R5G5B5) {
				unsigned v = ((const unsigned short *)src)[x];
				r = (v >> 10) & 31; g = (v >> 5) & 31; b = v & 31;
				r = (r << 3) | (r >> 2); g = (g << 3) | (g >> 2); b = (b << 3) | (b >> 2);
			} else {
				ok = false;
				r = g = b = 0;
			}
			dst[x * 3] = (unsigned char)b;
			dst[x * 3 + 1] = (unsigned char)g;
			dst[x * 3 + 2] = (unsigned char)r;
		}
	}
	copy->UnlockRect();
	copy->Release();
	if (!ok) {
		Log("back buffer format %d not handled\n", (int)desc.Format);
	} else {
		BITMAPFILEHEADER fh;
		BITMAPINFOHEADER bi;
		memset(&fh, 0, sizeof(fh));
		memset(&bi, 0, sizeof(bi));
		bi.biSize = sizeof(bi);
		bi.biWidth = w;
		bi.biHeight = h;
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;
		fh.bfType = 0x4D42;
		fh.bfOffBits = sizeof(fh) + sizeof(bi);
		fh.bfSize = fh.bfOffBits + stride * h;
		FILE *fp = fopen(filename, "wb");
		if (fp) {
			fwrite(&fh, sizeof(fh), 1, fp);
			fwrite(&bi, sizeof(bi), 1, fp);
			fwrite(rgb, stride * h, 1, fp);
			fclose(fp);
		} else {
			ok = false;
		}
	}
	delete [] rgb;
	return ok;
}

static int Run(HINSTANCE instance, int show)
{
	int frames = Arg_Int("-frames", 0);
	const char *shot = Arg_String("-shot", NULL);
	int shot_frame = Arg_Int("-shotframe", frames > 0 ? frames : 60);
	int resx = Arg_Int("-resx", 640);
	int resy = Arg_Int("-resy", 480);
	int depth = Arg_Int("-depth", 32);
	bool windowed = Arg_Bool("-windowed", true);

	WNDCLASSA wc;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Wnd_Proc;
	wc.hInstance = instance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "VidTestWndClass";
	if (!RegisterClassA(&wc)) {
		Log("RegisterClass failed: %lu\n", GetLastError());
		return 11;
	}
	RECT r = { 0, 0, resx, resy };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindowExA(0, "VidTestWndClass", "E&B Video Test (reconstruction)", WS_OVERLAPPEDWINDOW,
		0, 0, r.right - r.left, r.bottom - r.top, NULL, NULL, instance, NULL);
	if (!hwnd) {
		Log("CreateWindow failed: %lu\n", GetLastError());
		return 11;
	}
	MainWindow = hwnd;
	ShowWindow(hwnd, show);
	UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);
	Log("window created %p\n", hwnd);

	WWMath::Init();
	Log("WWMath::Init done\n");

	// As VIDTEST (docs/vidtest.md 1.1 step 4): the asset manager exists before WW3D::Init
	// (DX8Wrapper's device setup reaches ShatterSystem::Init, which asks it for htrees).
	WW3DAssetManager *assets = new WW3DAssetManager;
	assets->Set_Activate_Fog_On_Load(true);
	Log("WW3DAssetManager created\n");

	WW3DErrorType err = WW3D::Init(hwnd, NULL, false);
	Log("WW3D::Init = %d\n", (int)err);
	if (err != WW3D_ERROR_OK) {
		WWMath::Shutdown();
		return 12;
	}

	int count = WW3D::Get_Render_Device_Count();
	Log("render devices: %d\n", count);
	for (int i = 0; i < count; i++) {
		Log("  device %d: %s\n", i, WW3D::Get_Render_Device_Name(i));
	}
	if (count <= 0) {
		WW3D::Shutdown();
		WWMath::Shutdown();
		return 13;
	}
	const char *device = Arg_String("-device", WW3D::Get_Render_Device_Name(0));
	err = WW3D::Set_Render_Device(device, resx, resy, depth, windowed ? 1 : 0, true);
	Log("WW3D::Set_Render_Device(\"%s\", %d, %d, %d, %d) = %d\n", device, resx, resy, depth, windowed, (int)err);
	if (err != WW3D_ERROR_OK && depth == 32) {
		depth = 16;
		err = WW3D::Set_Render_Device(device, resx, resy, depth, windowed ? 1 : 0, true);
		Log("WW3D::Set_Render_Device(\"%s\", %d, %d, %d, %d) = %d\n", device, resx, resy, depth, windowed, (int)err);
	}
	if (err != WW3D_ERROR_OK) {
		WW3D::Shutdown();
		WWMath::Shutdown();
		return 14;
	}

	// The clear colour: dark blue (0, 0, 0.5)
	Vector3 clear(0.0f, 0.0f, 0.5f);
	int result = 0;
	int frame = 0;
	MSG msg;
	while (!Quit) {
		while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				Quit = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		if (Quit) break;
		WW3D::Sync(GetTickCount());
		WW3DErrorType b = WW3D::Begin_Render(true, true, clear);
		if (shot && frame + 1 == shot_frame && b == WW3D_ERROR_OK) {
			bool ok = Save_Back_Buffer(shot);
			Log("screenshot %s of frame %d: %s\n", shot, frame + 1, ok ? "saved" : "failed");
			if (!ok) result = 16;
		}
		WW3DErrorType e = WW3D::End_Render(true);
		if (b != WW3D_ERROR_OK || e != WW3D_ERROR_OK) {
			Log("frame %d: Begin_Render = %d, End_Render = %d\n", frame, (int)b, (int)e);
			result = 15;
			break;
		}
		frame++;
		if (frame == 1) {
			Log("first frame rendered\n");
		}
		if (frames > 0 && frame >= frames) {
			break;
		}
	}
	Log("frames rendered: %d\n", frame);

	err = WW3D::Shutdown();
	Log("WW3D::Shutdown = %d\n", (int)err);
	assets->Free_Assets();
	delete assets;
	WWMath::Shutdown();
	Log("WWMath::Shutdown done\n");
	if (IsWindow(hwnd)) {
		DestroyWindow(hwnd);
	}
	return result;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR cmdline, int show)
{
	ProgramInstance = instance;
	Split_Args(cmdline);
	LogFile = fopen(Arg_String("-log", "vidtest.log"), "w");
	Log("vidtest (reconstruction) start: %s\n", cmdline);
	SetUnhandledExceptionFilter(Crash_Filter);
	WWDebug_Install_Message_Handler(Debug_Message);
	WWDebug_Install_Assert_Handler(Debug_Assert);
	int result = Run(instance, show);
	Log("exit code %d\n", result);
	if (LogFile) fclose(LogFile);
	return result;
}
