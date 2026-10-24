/*
** d3d8shot: a d3d8.dll proxy that saves a D3D8 program's own frames (M2.1 reference captures).
**
** Put the built d3d8.dll next to the program (in a copy of its folder). The program's
** LoadLibrary("D3D8.DLL") finds it first; it loads the system's d3d8.dll and passes every call on.
** It patches two vtable slots: IDirect3D8::CreateDevice, to find the device, and
** IDirect3DDevice8::Present, to copy the back buffer of chosen frames into a system-memory
** surface before it is presented. Only the program's own back buffer is read: never the screen.
** It also logs the program's CreateFileA calls (its import table entry) to show which files it
** opens (open question O1 in docs/vidtest.md).
**
** Environment:
**   D3D8SHOT_DIR    folder for the log and the screenshots (default: the current directory)
**   D3D8SHOT_EVERY  save every Nth presented frame (default 0: none)
**   D3D8SHOT_FRAMES also save these frames, comma-separated (1 = the first frame presented)
**   D3D8SHOT_MAX    at most this many screenshots (default 200)
** Output: d3d8shot.log, frame_NNNNN.bmp (24-bit).
**
** C89 for VC6; no SDK headers: the few D3D8 types and vtable slots used are declared here.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IDirect3D8 slot 15 CreateDevice; IDirect3DDevice8 slots 15 Present, 16 GetBackBuffer,
   27 CreateImageSurface, 28 CopyRects; IDirect3DSurface8 slots 2 Release, 8 GetDesc,
   9 LockRect, 10 UnlockRect */
#define SLOT_CREATEDEVICE		15
#define SLOT_PRESENT			15
#define SLOT_GETBACKBUFFER		16
#define SLOT_CREATEIMAGESURFACE	27
#define SLOT_COPYRECTS			28
#define SLOT_RELEASE			2
#define SLOT_GETDESC			8
#define SLOT_LOCKRECT			9
#define SLOT_UNLOCKRECT			10

typedef struct {
	UINT Format, Type;
	DWORD Usage;
	UINT Pool, Size, MultiSampleType, Width, Height;
} SurfaceDesc;

typedef struct {
	INT Pitch;
	void *Bits;
} LockedRect;

typedef void **Object;		/* a COM object: its first member is the vtable */
#define VT(obj, slot)	((*(void ***)(obj))[slot])

typedef void *(WINAPI *CreateType)(UINT);
typedef HRESULT (STDMETHODCALLTYPE *CreateDeviceType)(void *, UINT, UINT, HWND, DWORD, void *, void **);
typedef HRESULT (STDMETHODCALLTYPE *PresentType)(void *, const RECT *, const RECT *, HWND, const void *);
typedef HRESULT (STDMETHODCALLTYPE *GetBackBufferType)(void *, UINT, UINT, void **);
typedef HRESULT (STDMETHODCALLTYPE *CreateImageSurfaceType)(void *, UINT, UINT, UINT, void **);
typedef HRESULT (STDMETHODCALLTYPE *CopyRectsType)(void *, void *, const RECT *, UINT, void *, const POINT *);
typedef ULONG (STDMETHODCALLTYPE *ReleaseType)(void *);
typedef HRESULT (STDMETHODCALLTYPE *GetDescType)(void *, SurfaceDesc *);
typedef HRESULT (STDMETHODCALLTYPE *LockRectType)(void *, LockedRect *, const RECT *, DWORD);
typedef HRESULT (STDMETHODCALLTYPE *UnlockRectType)(void *);
typedef HANDLE (WINAPI *CreateFileAType)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

static HMODULE			Real = NULL;
static CreateType		RealCreate = NULL;
static CreateDeviceType	RealCreateDevice = NULL;
static PresentType		RealPresent = NULL;
static CreateFileAType	RealCreateFileA = NULL;
static FILE *			LogFile = NULL;
static char				Dir[MAX_PATH];
static int				Every = 0;
static int				Max = 200;
static int				Saved = 0;
static int				Frames[64];
static int				FrameCount = 0;
static int				Presented = 0;
static CRITICAL_SECTION	LogLock;

static void Log(const char *format, ...)
{
	va_list va;
	if (!LogFile) return;
	EnterCriticalSection(&LogLock);
	fprintf(LogFile, "%10lu ", GetTickCount());
	va_start(va, format);
	vfprintf(LogFile, format, va);
	va_end(va);
	fflush(LogFile);
	LeaveCriticalSection(&LogLock);
}

static void Patch(void **slot, void *value)
{
	DWORD old;
	VirtualProtect(slot, sizeof(void *), PAGE_EXECUTE_READWRITE, &old);
	*slot = value;
	VirtualProtect(slot, sizeof(void *), old, &old);
}

static int Wanted(int frame)
{
	int i;
	if (Saved >= Max) return 0;
	if (Every > 0 && frame % Every == 0) return 1;
	for (i = 0; i < FrameCount; i++) {
		if (Frames[i] == frame) return 1;
	}
	return 0;
}

static void Save_Back_Buffer(void *dev, int frame)
{
	void *bb = NULL, *copy = NULL;
	SurfaceDesc desc;
	LockedRect lr;
	HRESULT hr;
	int w, h, stride, x, y, ok = 1;
	unsigned char *rgb;
	char name[MAX_PATH];
	FILE *fp;

	hr = ((GetBackBufferType)VT(dev, SLOT_GETBACKBUFFER))(dev, 0, 0, &bb);
	if (FAILED(hr) || !bb) {
		Log("frame %d: GetBackBuffer failed %08lX\n", frame, hr);
		return;
	}
	((GetDescType)VT(bb, SLOT_GETDESC))(bb, &desc);
	hr = ((CreateImageSurfaceType)VT(dev, SLOT_CREATEIMAGESURFACE))(dev, desc.Width, desc.Height, desc.Format, &copy);
	if (SUCCEEDED(hr)) {
		hr = ((CopyRectsType)VT(dev, SLOT_COPYRECTS))(dev, bb, NULL, 0, copy, NULL);
	}
	((ReleaseType)VT(bb, SLOT_RELEASE))(bb);
	if (FAILED(hr)) {
		Log("frame %d: back buffer copy failed %08lX (format %u)\n", frame, hr, desc.Format);
		if (copy) ((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
		return;
	}
	hr = ((LockRectType)VT(copy, SLOT_LOCKRECT))(copy, &lr, NULL, 0x10 /* D3DLOCK_READONLY */);
	if (FAILED(hr)) {
		Log("frame %d: LockRect failed %08lX\n", frame, hr);
		((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
		return;
	}
	w = desc.Width;
	h = desc.Height;
	stride = ((w * 3) + 3) & ~3;
	rgb = (unsigned char *)calloc(stride, h);
	for (y = 0; y < h && rgb; y++) {
		const unsigned char *src = (const unsigned char *)lr.Bits + lr.Pitch * y;
		unsigned char *dst = rgb + stride * (h - 1 - y);
		for (x = 0; x < w; x++) {
			unsigned r, g, b, v;
			if (desc.Format == 22 || desc.Format == 21) {			/* X8R8G8B8, A8R8G8B8 */
				b = src[x * 4]; g = src[x * 4 + 1]; r = src[x * 4 + 2];
			} else if (desc.Format == 23) {						/* R5G6B5 */
				v = ((const unsigned short *)src)[x];
				r = (v >> 11) & 31; g = (v >> 5) & 63; b = v & 31;
				r = (r << 3) | (r >> 2); g = (g << 2) | (g >> 4); b = (b << 3) | (b >> 2);
			} else if (desc.Format == 24 || desc.Format == 25) {	/* X1R5G5B5, A1R5G5B5 */
				v = ((const unsigned short *)src)[x];
				r = (v >> 10) & 31; g = (v >> 5) & 31; b = v & 31;
				r = (r << 3) | (r >> 2); g = (g << 3) | (g >> 2); b = (b << 3) | (b >> 2);
			} else {
				ok = 0;
				r = g = b = 0;
			}
			dst[x * 3] = (unsigned char)b;
			dst[x * 3 + 1] = (unsigned char)g;
			dst[x * 3 + 2] = (unsigned char)r;
		}
	}
	((UnlockRectType)VT(copy, SLOT_UNLOCKRECT))(copy);
	((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
	if (!rgb || !ok) {
		Log("frame %d: format %u not handled\n", frame, desc.Format);
		free(rgb);
		return;
	}
	_snprintf(name, sizeof(name) - 1, "%s\\frame_%05d.bmp", Dir, frame);
	name[sizeof(name) - 1] = 0;
	fp = fopen(name, "wb");
	if (fp) {
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
		fwrite(&fh, sizeof(fh), 1, fp);
		fwrite(&bi, sizeof(bi), 1, fp);
		fwrite(rgb, stride * h, 1, fp);
		fclose(fp);
		Saved++;
		Log("frame %d: saved %s (%dx%d, format %u)\n", frame, name, w, h, desc.Format);
	}
	free(rgb);
}

static HRESULT STDMETHODCALLTYPE Hook_Present(void *dev, const RECT *src, const RECT *dst, HWND wnd, const void *dirty)
{
	Presented++;
	if (Wanted(Presented)) {
		Save_Back_Buffer(dev, Presented);
	}
	return RealPresent(dev, src, dst, wnd, dirty);
}

static HRESULT STDMETHODCALLTYPE Hook_Create_Device(void *d3d, UINT adapter, UINT type, HWND wnd, DWORD flags,
	void *params, void **device)
{
	HRESULT hr = RealCreateDevice(d3d, adapter, type, wnd, flags, params, device);
	Log("CreateDevice(adapter %u, type %u, flags %08lX) = %08lX\n", adapter, type, flags, hr);
	if (SUCCEEDED(hr) && device && *device) {
		void **slot = &VT(*device, SLOT_PRESENT);
		if (*slot != (void *)Hook_Present) {
			RealPresent = (PresentType)*slot;
			Patch(slot, (void *)Hook_Present);
		}
	}
	return hr;
}

static HANDLE WINAPI Hook_CreateFileA(LPCSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa,
	DWORD disposition, DWORD flags, HANDLE templ)
{
	HANDLE h = RealCreateFileA(name, access, share, sa, disposition, flags, templ);
	Log("CreateFileA(\"%s\", %s%s, disposition %lu) = %s\n", name ? name : "(null)",
		(access & GENERIC_READ) ? "R" : "", (access & GENERIC_WRITE) ? "W" : "", disposition,
		h == INVALID_HANDLE_VALUE ? "failed" : "ok");
	return h;
}

/* The program's own CreateFileA import (the main module's import table) */
static void Hook_Imports(void)
{
	unsigned char *base = (unsigned char *)GetModuleHandleA(NULL);
	IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
	IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	IMAGE_IMPORT_DESCRIPTOR *imp;
	if (!dir->VirtualAddress) return;
	for (imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir->VirtualAddress); imp->Name; imp++) {
		IMAGE_THUNK_DATA *names, *funcs;
		if (stricmp((const char *)(base + imp->Name), "kernel32.dll") != 0) continue;
		if (!imp->OriginalFirstThunk) continue;
		names = (IMAGE_THUNK_DATA *)(base + imp->OriginalFirstThunk);
		funcs = (IMAGE_THUNK_DATA *)(base + imp->FirstThunk);
		for (; names->u1.AddressOfData; names++, funcs++) {
			IMAGE_IMPORT_BY_NAME *ibn;
			if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
			ibn = (IMAGE_IMPORT_BY_NAME *)(base + (DWORD)names->u1.AddressOfData);
			if (strcmp((const char *)ibn->Name, "CreateFileA") == 0) {
				RealCreateFileA = (CreateFileAType)funcs->u1.Function;
				Patch((void **)&funcs->u1.Function, (void *)Hook_CreateFileA);
				Log("hooked the program's CreateFileA import\n");
			}
		}
	}
}

static void Init(void)
{
	char path[MAX_PATH], log[MAX_PATH], frames[512];
	char *p;
	InitializeCriticalSection(&LogLock);
	if (!GetEnvironmentVariableA("D3D8SHOT_DIR", Dir, sizeof(Dir))) {
		GetCurrentDirectoryA(sizeof(Dir), Dir);
	}
	_snprintf(log, sizeof(log) - 1, "%s\\d3d8shot.log", Dir);
	log[sizeof(log) - 1] = 0;
	LogFile = fopen(log, "w");
	if (GetEnvironmentVariableA("D3D8SHOT_EVERY", frames, sizeof(frames))) Every = atoi(frames);
	if (GetEnvironmentVariableA("D3D8SHOT_MAX", frames, sizeof(frames))) Max = atoi(frames);
	if (GetEnvironmentVariableA("D3D8SHOT_FRAMES", frames, sizeof(frames))) {
		for (p = strtok(frames, ","); p && FrameCount < 64; p = strtok(NULL, ",")) {
			Frames[FrameCount++] = atoi(p);
		}
	}
	GetSystemDirectoryA(path, sizeof(path));
	strcat(path, "\\d3d8.dll");
	Real = LoadLibraryA(path);
	if (Real) RealCreate = (CreateType)GetProcAddress(Real, "Direct3DCreate8");
	Log("d3d8shot: real %s = %p, Direct3DCreate8 = %p; every %d, %d listed frames, max %d\n", path, Real,
		RealCreate, Every, FrameCount, Max);
	Hook_Imports();
}

void * WINAPI Direct3DCreate8(UINT version)
{
	void *d3d;
	if (!Real) Init();
	if (!RealCreate) return NULL;
	d3d = RealCreate(version);
	Log("Direct3DCreate8(%u) = %p\n", version, d3d);
	if (d3d) {
		void **slot = &VT(d3d, SLOT_CREATEDEVICE);
		if (*slot != (void *)Hook_Create_Device) {
			RealCreateDevice = (CreateDeviceType)*slot;
			Patch(slot, (void *)Hook_Create_Device);
		}
	}
	return d3d;
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_DETACH && LogFile) {
		Log("detach: %d frames presented, %d saved\n", Presented, Saved);
		fclose(LogFile);
		LogFile = NULL;
	}
	return TRUE;
}
