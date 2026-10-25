// movietest: one of the game's Bink movies played the way the client plays its intro (docs/kernel-map.md
// 6, step 7): WinMain's start (a window, WW3D), a KernelClass, a MovieTaskClass with the movie
// (tsk_movie.cpp's Add_Movie, volume 0x8000), Add_Task (it takes the focus), and KernelClass::Run until
// the task removes itself. The player under the task is Renegade's BINKMovie (binkmovie_adapter.cpp).
//
//     movietest -movie FILE [-frames N] [-shots N,N,...] [-shotdir DIR] [-log FILE] [-resx N] [-resy N]
//
// Windowed; never a message box. -frames N stops after N movie frames drawn (the kernel's Quit flag;
// WM_QUIT doesn't reach it); -shots saves the back buffer after those frames are drawn as
// DIR\frame_NNNNN.bmp (the program's own back buffer, before End_Render presents it).
// Exit code: 0 the movie played to its end (or to -frames), 11 window, 12 WW3D::Init, 13 no device,
// 14 the movie didn't open, 16 a screenshot failed, 17 binkw32.dll missing.
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ww3d.h"
#include "dx8wrapper.h"
#include "assetmgr.h"
#include "wwmath.h"
#include "wwdebug.h"
#include "win.h"
#include "kernel.h"
#include "..\..\..\app\client\tasks\tsk_movie.h"

static FILE *LogFile;
static int Drawn;				// movie frames drawn (the first draw of each)
static int Renders;				// the task's Render calls
static int Frame_Limit;
static int Shots[64];
static int Shot_Count;
static const char *Shot_Dir = "shots";
static int Result = 0;

void Movie_Log(const char *format, ...)
{
	char line[1024];
	va_list va;
	va_start(va, format);
	_vsnprintf(line, sizeof(line) - 1, format, va);
	va_end(va);
	line[sizeof(line) - 1] = 0;
	if (LogFile) {
		fputs(line, LogFile);
		fflush(LogFile);
	}
}

static void Debug_Message(DebugType type, const char *message) { Movie_Log("[wwdebug %d] %s", (int)type, message); }
static void Debug_Assert(const char *message) { Movie_Log("[wwassert] %s\n", message); }

// ------------------------------------------------------------------ what kernel.cpp calls

static DWORD Ticks_Start;
unsigned int Unknown_Get_Ticks() { return timeGetTime() - Ticks_Start; }	// the client's 0x4A3B40
int Unknown_9179D0() { return 0; }											// WW3D's device check: fine
bool Unknown_42EA80(KernelEventClass *) { return false; }					// no input events
void Unknown_42E8A0(KernelEventClass *) {}
class UnknownBDADC0Class
{
public:
	char Unknown0000[0x4074];
	bool Unknown4074;
};
static UnknownBDADC0Class BDADC0;
UnknownBDADC0Class *Unknown_BDADC0 = &BDADC0;
class UnknownBFAEE0Class
{
public:
	virtual void Unknown_00() {}
	virtual void Unknown_04() {}
	virtual void Unknown_08() {}
	virtual void Unknown_0C() {}
	virtual void Unknown_10() {}
};
static UnknownBFAEE0Class BFAEE0;
UnknownBFAEE0Class *Unknown_BFAEE0 = &BFAEE0;
void Unknown_Assert_Failed(const char *expr, const char *file, int line) { Movie_Log("G_ASSERT %s (%s %d)\n", expr, file, line); }
void Unknown_Fail_If_Report(const char *expr, const char *file, int line) { Movie_Log("FAIL_IF %s (%s %d)\n", expr, file, line); }

// ------------------------------------------------------------------ screenshots

static bool Save_Back_Buffer(const char *filename)
{
	IDirect3DDevice8 *dev = DX8Wrapper::_Get_D3D_Device8();
	if (!dev) return false;
	IDirect3DSurface8 *bb = NULL;
	if (FAILED(dev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &bb)) || !bb) return false;
	D3DSURFACE_DESC desc;
	bb->GetDesc(&desc);
	IDirect3DSurface8 *copy = NULL;
	HRESULT hr = dev->CreateImageSurface(desc.Width, desc.Height, desc.Format, &copy);
	if (SUCCEEDED(hr)) hr = dev->CopyRects(bb, NULL, 0, copy, NULL);
	bb->Release();
	if (FAILED(hr)) {
		if (copy) copy->Release();
		return false;
	}
	D3DLOCKED_RECT lr;
	if (FAILED(copy->LockRect(&lr, NULL, D3DLOCK_READONLY))) {
		copy->Release();
		return false;
	}
	int w = desc.Width, h = desc.Height;
	int stride = ((w * 3) + 3) & ~3;
	unsigned char *rgb = new unsigned char[stride * h];
	memset(rgb, 0, stride * h);
	for (int y = 0; y < h; y++) {
		const unsigned char *src = (const unsigned char *)lr.pBits + lr.Pitch * y;
		unsigned char *dst = rgb + stride * (h - 1 - y);
		for (int x = 0; x < w; x++) {
			unsigned r = 0, g = 0, b = 0;
			if (desc.Format == D3DFMT_X8R8G8B8 || desc.Format == D3DFMT_A8R8G8B8) {
				b = src[x * 4]; g = src[x * 4 + 1]; r = src[x * 4 + 2];
			} else if (desc.Format == D3DFMT_R5G6B5) {
				unsigned v = ((const unsigned short *)src)[x];
				r = ((v >> 11) & 31) * 255 / 31; g = ((v >> 5) & 63) * 255 / 63; b = (v & 31) * 255 / 31;
			} else if (desc.Format == D3DFMT_X1R5G5B5 || desc.Format == D3DFMT_A1R5G5B5) {
				unsigned v = ((const unsigned short *)src)[x];
				r = ((v >> 10) & 31) * 255 / 31; g = ((v >> 5) & 31) * 255 / 31; b = (v & 31) * 255 / 31;
			}
			dst[x * 3] = (unsigned char)b; dst[x * 3 + 1] = (unsigned char)g; dst[x * 3 + 2] = (unsigned char)r;
		}
	}
	copy->UnlockRect();
	copy->Release();
	FILE *f = fopen(filename, "wb");
	bool ok = f != NULL;
	if (f) {
		BITMAPFILEHEADER fh;
		BITMAPINFOHEADER ih;
		memset(&fh, 0, sizeof(fh));
		memset(&ih, 0, sizeof(ih));
		fh.bfType = 0x4D42;
		fh.bfOffBits = sizeof(fh) + sizeof(ih);
		fh.bfSize = fh.bfOffBits + stride * h;
		ih.biSize = sizeof(ih);
		ih.biWidth = w;
		ih.biHeight = h;
		ih.biPlanes = 1;
		ih.biBitCount = 24;
		ih.biSizeImage = stride * h;
		fwrite(&fh, sizeof(fh), 1, f);
		fwrite(&ih, sizeof(ih), 1, f);
		fwrite(rgb, stride * h, 1, f);
		fclose(f);
	}
	delete [] rgb;
	return ok;
}

unsigned Movie_Frame_Number(void);

// binkmovie_adapter.cpp: after each movie frame is drawn (the task's Render, before End_Render)
void Movie_Frame_Drawn(void)
{
	// the task renders every cycle; a new movie frame is the first draw at its frame number
	static unsigned last = 0;
	unsigned frame = Movie_Frame_Number();
	Renders++;
	if (frame == last) {
		return;
	}
	last = frame;
	Drawn++;
	for (int i = 0; i < Shot_Count; i++) {
		if (Shots[i] == Drawn) {
			char path[MAX_PATH];
			_snprintf(path, sizeof(path) - 1, "%s\\frame_%05d.bmp", Shot_Dir, Drawn);
			path[sizeof(path) - 1] = 0;
			bool ok = Save_Back_Buffer(path);
			Movie_Log("screenshot %s (movie frame %u): %s\n", path, Movie_Frame_Number(), ok ? "saved" : "FAILED");
			if (!ok) Result = 16;
		}
	}
	if (Frame_Limit > 0 && Drawn == Frame_Limit) {
		Movie_Log("frame limit %d reached\n", Frame_Limit);
		// not WM_QUIT: the client's message pump (0x443E90) tests for WM_QUIT only after GetMessage
		// returned nonzero, which it never does for WM_QUIT, so a posted quit doesn't stop the kernel
		KernelClass::Get_Instance()->Quit = true;
	}
}

// ------------------------------------------------------------------ the program

static int Argc;
static char *Argv[32];
static const char *Arg(const char *name, const char *def)
{
	for (int i = 0; i + 1 < Argc; i++) {
		if (stricmp(Argv[i], name) == 0) return Argv[i + 1];
	}
	return def;
}

static LRESULT CALLBACK Wnd_Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_CLOSE) {
		if (KernelClass::Get_Instance() != NULL) {
			KernelClass::Get_Instance()->Quit = true;		// (WM_QUIT wouldn't stop it: see above)
		}
		return 0;
	}
	return DefWindowProcA(hwnd, msg, wparam, lparam);
}

bool Bink_Load(void);

static int Run(HINSTANCE instance)
{
	const char *movie_file = Arg("-movie", "eb_ws_logo.bik");
	Frame_Limit = atoi(Arg("-frames", "0"));
	Shot_Dir = Arg("-shotdir", "shots");
	const char *list = Arg("-shots", NULL);
	if (list) {
		CreateDirectoryA(Shot_Dir, NULL);
		for (const char *p = list; *p && Shot_Count < 64;) {
			Shots[Shot_Count++] = atoi(p);
			while (*p && *p != ',') p++;
			if (*p == ',') p++;
		}
	}
	int resx = atoi(Arg("-resx", "800"));
	int resy = atoi(Arg("-resy", "600"));
	if (!Bink_Load()) {
		Movie_Log("binkw32.dll missing\n");
		return 17;
	}
	WNDCLASSA wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = Wnd_Proc;
	wc.hInstance = instance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "MovieTestWndClass";
	RegisterClassA(&wc);
	HWND hwnd = CreateWindowExA(0, "MovieTestWndClass", "E&B movie task test", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL,
		instance, NULL);
	if (!hwnd) return 11;
	MainWindow = hwnd;
	ShowWindow(hwnd, SW_SHOWNORMAL);
	WWMath::Init();
	WW3DAssetManager *assets = new WW3DAssetManager;		// WW3D expects one (as vidtest)
	WW3DErrorType err = WW3D::Init(hwnd, NULL, false);
	Movie_Log("WW3D::Init = %d\n", (int)err);
	if (err != WW3D_ERROR_OK) return 12;
	Movie_Log("render devices: %d\n", WW3D::Get_Render_Device_Count());
	if (WW3D::Get_Render_Device_Count() <= 0) return 13;
	err = WW3D::Set_Render_Device(WW3D::Get_Render_Device_Name(0), resx, resy, 32, 1, true);
	Movie_Log("WW3D::Set_Render_Device(%s, %d, %d, 32, windowed) = %d\n", WW3D::Get_Render_Device_Name(0), resx, resy, (int)err);
	if (err != WW3D_ERROR_OK) return 13;

	// WinMain's intro (kernel-map.md 1): new KernelClass, the movie task, Add_Movie, Add_Task, Set_Focus
	Ticks_Start = timeGetTime();
	KernelClass *kernel = new KernelClass;
	kernel->Render_Enabled = true;
	MovieTaskClass *movie = new MovieTaskClass;
	movie->Add_Movie(movie_file, 0x8000);
	kernel->Add_Task(movie);
	kernel->Set_Focus(movie);
	Movie_Log("focus: %s\n", kernel->Get_Focus() == movie ? "the movie task" : "other");
	if (!kernel->Contains_Task(movie) || movie->Movies.size() != 1) {
		Result = 14;
	}
	DWORD start = timeGetTime();
	kernel->Run();
	DWORD ms = timeGetTime() - start;
	bool removed = !kernel->Contains_Task(movie);
	Movie_Log("kernel stopped after %lu ms: %d movie frames drawn in %d renders, the movie task %s\n", ms, Drawn, Renders,
		removed ? "removed itself (the movie ended)" : "still there (stopped by -frames or the window)");
	if (Drawn == 0) Result = 14;
	kernel->Shutdown();
	delete kernel;
	WW3D::Shutdown();
	assets->Free_Assets();
	delete assets;
	WWMath::Shutdown();
	DestroyWindow(hwnd);
	return Result;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR cmdline, int)
{
	static char line[1024];
	strncpy(line, cmdline, sizeof(line) - 1);
	for (char *p = strtok(line, " "); p && Argc < 32; p = strtok(NULL, " ")) {
		Argv[Argc++] = p;
	}
	ProgramInstance = instance;
	LogFile = fopen(Arg("-log", "movietest.log"), "w");
	WWDebug_Install_Message_Handler(Debug_Message);
	WWDebug_Install_Assert_Handler(Debug_Assert);
	int result = Run(instance);
	Movie_Log("exit code %d\n", result);
	if (LogFile) fclose(LogFile);
	return result;
}
