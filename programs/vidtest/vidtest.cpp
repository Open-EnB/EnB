// Earth & Beyond client (reconstruction): VIDTEST : the program's own code
/*
**	The shape of VIDTEST.EXE (docs/vidtest.md): WinMain 0x4235C0 (1.1), WndProc 0x4239D0 (1.2),
**	Setup_Device 0x423BE0 (1.3), Frame 0x424E90 (1.4), Build_Scenes 0x423F20 (2), the cameras and
**	per-frame rendering of each test (2.2, 2.3), Update_Lighting 0x4250B0 (2.4) and the text overlay
**	Draw_Overlay 0x425F80 (2.5). Built on the tree's 2004 Westwood libraries (D36: functional, not
**	byte-exact).
**
**	L2: the first executable built from the tree. M2.1: one model (BST29A). M2.2: all W3D scenes
**	(mu02 with the camera bone, back01 sky, mu01a overlay through the 2D camera), the font.tga text,
**	the command line, and the test state machine as far as the scenes switch (fill rate quads,
**	polygon scene, land scene).
**
**	M2.4: E&B's own scene classes from app\shared (the "E&B scene classes" section below): the star
**	field (stars.cpp, stars.ini), the spline database (spln_db.cpp, splines.wsd) for the lighting, and
**	the Land (land.cpp, heightfield.cpp: land.ini, the river height field, lightmap, LOD) with its
**	decorations (decoration.ini).
**	M2.3: results.ini (Info, Results) is written to the working directory as VIDTEST does (3).
**
**	VIDTEST's arguments (1.3): -device NAME -resx N (640) -resy N (480) -depth N (16) -windowed B (true)
**	-sync B (false) -filter N (0).
**	Test options (ours, not VIDTEST's):
**	  -art DIR       the folder the assets are read from (default the working directory if it holds
**	                 mu01a.w3d, as VIDTEST, else the EBCONFIG install folder; VIDTEST
**	                 reads its working directory, but this program must never run in the install)
**	  -frames N      quit by itself (exit code 0) after N presented frames (default 0: run the tests)
**	  -shot FILE     save the back buffer of presented frame -shotframe N to FILE (.bmp)
**	                 (default N: the -frames count, else 60)
**	  -shot2 FILE    a second screenshot, of frame -shot2frame N (default: half of -shotframe)
**	  -shots LIST    save frames N,N,... as DIR\frame_NNNNN.bmp (-shotdir DIR, default "shots"), the
**	                 numbering of tools/harness/run_reference.py (one per Present, from 1)
**	  -log FILE      init results (default vidtest.log)
**	  -noondemand    leave WW3D_Load_On_Demand off (VIDTEST turns it on: Build_Scenes)
**	  -sunangle A    fix the sun's angle at A degrees (0..180; VIDTEST's turns with GetTickCount)
**	  -state N       start in test state N (0 fill rate, 1 polygon, 2 land) instead of 0
**	  -noscene       L2's test: no scenes, every frame only clears to dark blue
**	  -nomsgbox      VIDTEST's message boxes go to the log instead (implied by -frames, -shot, -shots)
**	Exit code (VIDTEST's, 1.1): 1 tests finished, -3 aborted by a key or a window close, -1 video mode
**	not supported, -2 scene build failed; ours: 0 after -frames N, 11 window, 12 WW3D::Init,
**	13 no device, 15 Begin_Render/End_Render, 16 screenshot.
*/


#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "ww3d.h"
#include "assetmgr.h"
#include "dx8wrapper.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "dx8fvf.h"
#include "vertmaterial.h"
#include "shader.h"
#include "wwmath.h"
#include "vector3.h"
#include "matrix4.h"
#include "wwdebug.h"
#include "win.h"
#include "ffactory.h"
#include "wwfile.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "rendobj.h"
#include "hlod.h"
#include "hanim.h"
#include "matrix3d.h"
#include "sortingrenderer.h"
#include "render2d.h"
#include "font3d.h"
#include "predlod.h"
#include "statistics.h"
#include "rect.h"
#include "dx8caps.h"
#include "ini.h"
#include "stars.h"							// app\shared (M2.4)
#include "spln_db.h"						// app\shared (M2.4)
#include "curve.h"
#include "land.h"							// app\shared (M2.4)
#include "registry.h"

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
** The file factory (ours: VIDTEST sets none and reads from its working directory, docs/vidtest.md
** 1.1 step 4). Requests go to a SimpleFileFactoryClass on the art folder, and each one is logged
** with whether the file exists (open question O1: which files the 2004 loaders ask for).
*/
class LoggingFileFactoryClass : public FileFactoryClass
{
public:
	SimpleFileFactoryClass	Inner;
	int						Requests;

	LoggingFileFactoryClass(void) : Requests(0) {}
	virtual FileClass * Get_File(char const *filename)
	{
		FileClass *file = Inner.Get_File(filename);
		Requests++;
		Log("file: %s %s\n", filename ? filename : "(null)", (file && file->Is_Available()) ? "found" : "MISSING");
		return file;
	}
	virtual void Return_File(FileClass *file)
	{
		Inner.Return_File(file);
	}
};

static LoggingFileFactoryClass	ArtFiles;

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

static bool Arg_Present(const char *name)
{
	for (int i = 0; i < ArgCount; i++) {
		if (stricmp(Args[i], name) == 0) return true;
	}
	return false;
}

/*
** VIDTEST's message boxes (1.1 steps 7, 8). Ours: with -nomsgbox, or any of our test options
** (-frames, -shot, -shots), the text goes to the log and the box's default answer (IDOK) is returned:
** an unattended run never blocks on a modal box on the user's desktop.
*/
static int Message_Box(const char *text, const char *caption, UINT flags)
{
	if (Arg_Present("-nomsgbox") || Arg_Present("-frames") || Arg_Present("-shot") || Arg_Present("-shots")) {
		Log("message box (not shown): %s: %s\n", caption, text);
		return IDOK;
	}
	return MessageBoxA(NULL, text, caption, flags);
}

static Vector3 Arg_Vector(const char *name, const Vector3 &def)
{
	const char *s = Arg_String(name, NULL);
	Vector3 v = def;
	if (s) sscanf(s, "%f,%f,%f", &v.X, &v.Y, &v.Z);
	return v;
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

/*
** ------------------------------------------------------------------------------------------------
** The E&B scene classes (M2.4, docs/vidtest.md 4; app\shared). VIDTEST builds a star field
** (stars.cpp, StarField 0x4228A0 with budget 200, Init 0x422B20) into the star scene, a Land
** (Create("land.ini", true) 0x4068F0) with its decorations (0x415C40) into the land scene, and
** loads splines.wsd (SplineDatabase 0x41C660) for the lighting curves.
** ------------------------------------------------------------------------------------------------
*/
static int Stars_Get_Num_Polys(StarFieldClass *s)	{ return s->Get_Num_Polys(); }	// vtable +0x20
static void Stars_Set_Budget(StarFieldClass *s, int n)	{ s->Set_Budget(n); }	// field +0xA0
static void Land_Set_Dirty(LandClass *l)			{ if (l) l->Set_Dirty(); }	// field +0x136
static void Land_Update_Patches(LandClass *l, CameraClass *c, int lod)	{ if (l) l->Update_Patches(*c, lod); }	// 0x4078F0
static void Land_Update_Lightmap(LandClass *l, const LandLightingStruct &lighting, bool full)	// 0x40A9F0 / 0x40AB10
{
	if (l == NULL) return;
	if (full) l->Update_Lightmap_Full(lighting);
	else l->Update_Lightmap_ManualThreaded(lighting);
}
static SplineDatabaseClass *	Splines = NULL;			// 0x5EF19C: splines.wsd
static char					ArtDir[MAX_PATH] = "";	// ours: the -art folder (VIDTEST opens splines.wsd in its working directory)
static bool Splines_Loaded(void)					{ return Splines != NULL && Splines->Has_Splines(); }

/*
** ours: a colour spline's value at t (VIDTEST 0x4250B0 evaluates the curves of name_R, _G, _B as
** the client's lighting does through 0x485440)
*/
static bool Evaluate_Color(const char *base_name, float t, float *r, float *g, float *b)
{
	Curve1DClass *cr, *cg, *cb;
	if (!Splines->Get_Color_Curves(base_name, &cr, &cg, &cb)) {
		return false;
	}
	cr->Evaluate(t, r);
	cg->Evaluate(t, g);
	cb->Evaluate(t, b);
	return true;
}

/*
** ------------------------------------------------------------------------------------------------
** VIDTEST's globals (docs/vidtest.md 2, 3)
** ------------------------------------------------------------------------------------------------
*/
static HWND					MainWnd = NULL;
static int					ResX = 640;					// 0x5EF194
static int					ResY = 480;					// 0x5EF198
static WW3DAssetManager *	Assets = NULL;				// TheInstance
static LightClass *			Sun = NULL;					// 0x5EF150
static CameraClass *		Camera3D = NULL;			// 0x5EF148
static CameraClass *		Camera2D = NULL;			// 0x5EF14C
static Font3DInstanceClass *Font = NULL;				// 0x5EF158
static Render2DTextClass *	Text = NULL;				// 0x5EF154
static SimpleSceneClass *	OverlayScene = NULL;		// 0x5EF144
static SimpleSceneClass *	BackScene = NULL;			// 0x5EF13C
static SimpleSceneClass *	StarScene = NULL;			// 0x5EF138
static SimpleSceneClass *	SpaceScene = NULL;			// 0x5EF134
static SimpleSceneClass *	LandScene = NULL;			// 0x5EF140
static RenderObjClass *		Mu01a = NULL;				// 0x5EF170
static RenderObjClass *		Back01 = NULL;				// 0x5EF164
static RenderObjClass *		Mu02 = NULL;				// 0x5EF160
static StarFieldClass *		StarField = NULL;			// 0x5EF16C
static LandClass *			Land = NULL;				// 0x5EF168

static int					TestState = 0;				// 0x5EF15C: 0 fill, 1 polygon, 2 land, 3 done
static unsigned char		SpaceBudget = 9;			// 0x5C5508 byte 0 (x1000 polygons)
static unsigned char		LandBudget = 9;				// byte 1 (x1000)
static unsigned char		StarBudget = 2;				// byte 2 (x200 stars)
static int					PolyCounter = -1;			// 0x5C550C
static int					LandCounter = -1;			// 0x5C5510
static int					FillCounter = -1;			// 0x5C5514
static unsigned char		LandLOD = 0;				// 0x5EF17C
static bool					LandToggle = false;			// 0x5EF1A4
static float				MaxPolysSpace = 0;			// 0x5EF180
static float				MaxPolysLand = 0;			// 0x5EF184
static unsigned				PeakPolyRate = 0;			// 0x5EF174
static int					FillRateFPS = 0;			// 0x5EF178
static float				TimeOfDay = 0;				// 0x5EF190
static bool					TestsFinished = false;		// 0x5EF188
// results.ini (3): Setup_Device puts [Info] FreeTextureRam first, so [Info] comes before [Results];
// Write_Results adds the test results and saves it. INIClass::Save(name) writes through
// _TheWritingFileFactory (the default one: the working directory), never through the art factory.
static INIClass				ResultsIni;					// VIDTEST's global INIClass
static unsigned				FreeTextureRam = 0;
static int					TextureReduction = 0;		// 0x5EF1A0

// The frame-time ring (0x423E70, 2,000 slots at 0x5ED1E0): the frames presented in the last second
static DWORD				FrameTimes[2000];
static int					FrameTimeCount = 0;

// Ours: screenshots and the frame limit
static int					Presented = 0;
static int					FrameLimit = 0;
static const char *			ShotFile = NULL;
static int					ShotFrame = 0;
static const char *			Shot2File = NULL;
static int					Shot2Frame = 0;
static int					ShotList[64];
static int					ShotListCount = 0;
static const char *			ShotDir = "shots";
static int					Result = 0;

static void Write_Results(void);

static LRESULT CALLBACK Wnd_Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		case WM_KEYDOWN:
		case WM_CLOSE:
		case WM_DESTROY:
			// 0x4239D0: before the end of the tests, quit with -3; after it, write the results
			if (!TestsFinished) {
				PostQuitMessage(-3);
			} else {
				Write_Results();
			}
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
** End_Render, with our screenshots before it (the back buffer of the frame about to be presented)
*/
static void End_Frame(void)
{
	Presented++;
	const char *name = NULL;
	char path[MAX_PATH];
	if (ShotFile && Presented == ShotFrame) {
		name = ShotFile;
	} else if (Shot2File && Presented == Shot2Frame) {
		name = Shot2File;
	} else {
		for (int i = 0; i < ShotListCount; i++) {
			if (ShotList[i] == Presented) {
				_snprintf(path, sizeof(path) - 1, "%s\\frame_%05d.bmp", ShotDir, Presented);
				path[sizeof(path) - 1] = 0;
				name = path;
			}
		}
	}
	if (name) {
		// End_Render draws the sorted (alpha) polygons first; draw them now so the screenshot has them
		SortingRendererClass::Flush();
		bool ok = Save_Back_Buffer(name);
		Log("screenshot %s of frame %d (state %d, sync time %u, timeGetTime %lu): %s\n", name, Presented, TestState,
			WW3D::Get_Sync_Time(), timeGetTime(), ok ? "saved" : "failed");
		if (!ok) Result = 16;
	}
	WW3DErrorType e = WW3D::End_Render(true);
	if (e != WW3D_ERROR_OK) {
		Log("frame %d: End_Render = %d\n", Presented, (int)e);
		Result = 15;
		PostQuitMessage(15);
	}
	if (Presented == 1) {
		Log("first frame presented\n");
	}
	if (FrameLimit > 0 && Presented == FrameLimit) {
		Log("frame limit %d reached in state %d\n", FrameLimit, TestState);
		PostQuitMessage(Result);
	}
}

static void Begin_Frame(bool clear, bool clearz, const Vector3 &color)
{
	WW3DErrorType b = WW3D::Begin_Render(clear, clearz, color);
	if (b != WW3D_ERROR_OK) {
		Log("frame %d: Begin_Render = %d\n", Presented + 1, (int)b);
		Result = 15;
		PostQuitMessage(15);
	}
}

/*
** Max_LOD_And_Cost (0x424AA0): an HLOD goes to its finest LOD and gives its cost, plus that of its
** additional models, recursively. An object named BST29A gets its "%s.%s-1" loop.
*/
static float Max_LOD_And_Cost(RenderObjClass *obj)
{
	float cost = 0;
	if (obj->Class_ID() == RenderObjClass::CLASSID_HLOD) {
		obj->Set_LOD_Level(obj->Get_LOD_Count() - 1);
		cost = obj->Get_Cost();
		HLodClass *hlod = (HLodClass *)obj;
		for (int i = 0; i < hlod->Get_Additional_Model_Count(); i++) {
			cost += Max_LOD_And_Cost(hlod->Peek_Additional_Model(i));
		}
	}
	const char *name = obj->Get_Name();
	if (strcmp(name, "BST29A") == 0) {
		char anim_name[200];
		sprintf(anim_name, "%s.%s-1", name, name);
		HAnimClass *anim = Assets->Get_HAnim(anim_name);
		Log("Get_HAnim(\"%s\") = %p\n", anim_name, anim);
		if (anim) {
			obj->Set_Animation(anim, 0, RenderObjClass::ANIM_MODE_LOOP);
			anim->Release_Ref();
		}
	}
	return cost;
}

/*
** The overlay (0x424E40): mu01a through the 2D camera
*/
static void Render_Overlay(void)
{
	WW3D::Render(OverlayScene, Camera2D, false, false, Vector3(0, 0, 0));
}

/*
** The frame-time ring (0x423E70): adds now, drops the entries older than a second
*/
static void Push_Frame_Time(void)
{
	DWORD now = timeGetTime();
	int keep = 0;
	for (int i = 0; i < FrameTimeCount; i++) {
		if (now - FrameTimes[i] <= 1000) FrameTimes[keep++] = FrameTimes[i];
	}
	FrameTimeCount = keep;
	if (FrameTimeCount < 2000) FrameTimes[FrameTimeCount++] = now;
}

/*
** Draw_Overlay (0x425F80): the test's text at (0, resy-150), white
*/
static void Draw_Overlay(void)
{
	if (!Text) return;
	char text[500];
	char line[256];
	text[0] = 0;
	Debug_Statistics::Record_Texture_Mode(Debug_Statistics::RECORD_TEXTURE_NONE);
	if (TestState == 0) {
		strcat(text, "Fill Rate Test\n");
	} else if (TestState == 1) {
		strcat(text, "Polygon Budget Test\n");
		sprintf(line, "Budget = %d\n", SpaceBudget * 1000);			strcat(text, line);
		sprintf(line, "Max Polys = %6.0f\n", MaxPolysSpace);		strcat(text, line);
		sprintf(line, "Star Budget = %d\n", StarBudget * 200);		strcat(text, line);
	} else if (TestState == 2) {
		strcat(text, "Land LOD Test\n");
		sprintf(line, "LOD = %d\n", LandLOD);						strcat(text, line);
		sprintf(line, "Budget = %d\n", LandBudget * 1000);			strcat(text, line);
		sprintf(line, "Max Polys = %6.0f\n", MaxPolysLand);		strcat(text, line);
	} else {
		sprintf(line, "Fill Rate FPS= %d\n", FillRateFPS);			strcat(text, line);
		sprintf(line, "Space Budget= %d\n", SpaceBudget * 1000);	strcat(text, line);
		sprintf(line, "Star Budget= %d\n", StarBudget * 200);		strcat(text, line);
		sprintf(line, "Land LOD Level= %d\n", LandLOD);			strcat(text, line);
		sprintf(line, "Land Budget= %d\n", LandBudget * 1000);		strcat(text, line);
	}
	int fps = FrameTimeCount;
	unsigned rate = (unsigned)Debug_Statistics::Get_DX8_Polygons() * (unsigned)fps;
	if (rate / 1000 > PeakPolyRate) PeakPolyRate = rate / 1000;
	sprintf(line, "%d KPoly/s %d FPS\n", rate / 1000, fps);
	strcat(text, line);
	Text->Reset();
	Text->Set_Location(Vector2(0.0f, (float)ResY - 150.0f));
	if (TestsFinished) {
		Text->Draw_Text("Press a key to continue...", 0xFFFF0000);
	}
	Text->Draw_Text(text, 0xFFFFFFFF);
	Text->Render();
}

/*
** Update_Lighting (0x4250B0): the sun goes round once every 125.7 s. The colours at the time of day:
** light = LightColor.Earth x LightIntensity.Sun, sky = SkyColor.Earth x SkyIntensity.Sun, emissive =
** SkyEmissive.Default (each an _R/_G/_B curve of the spline database); without splines (1, 1, 0.2),
** (0, 0, 0.3) and (0.1, 0.1, 0.1) (the asm's initial values). The sun's diffuse is the clamped light,
** the land scene's ambient the clamped sky + emissive, *out (the fog and clear colour) the clamped sky.
*/
static void Update_Lighting(Vector3 *out, bool full)
{
	float a = (float)fmod(GetTickCount() * 5e-5, 2 * WWMATH_PI);
	if (Arg_Present("-sunangle")) a = Arg_Int("-sunangle", 90) * WWMATH_PI / 180;	// ours (test option)
	if (a > WWMATH_PI) a = 2 * WWMATH_PI - a;
	Vector3 sun((float)cos(a), 0.0f, (float)sin(a));
	sun.Normalize();
	float e = (float)acos(sun.Z);						// the sun's angle from the zenith (0x5286A0 is the CRT's acos: atan(sqrt(1 - z*z) / z))
	if (e < 0) {
		sun = -sun;
		e = -e;
	}
	TimeOfDay = (WWMATH_PI / 2 - e) / WWMATH_PI + 0.5f;
	Vector3 light(1.0f, 1.0f, 0.2f);
	Vector3 sky(0.0f, 0.0f, 0.3f);
	Vector3 emissive(0.1f, 0.1f, 0.1f);
	if (Splines_Loaded()) {
		Vector3 c, i;
		Evaluate_Color("LightColor.Earth", TimeOfDay, &c.X, &c.Y, &c.Z);
		Evaluate_Color("LightIntensity.Sun", TimeOfDay, &i.X, &i.Y, &i.Z);
		light.Set(c.X * i.X, c.Y * i.Y, c.Z * i.Z);
		Evaluate_Color("SkyColor.Earth", TimeOfDay, &c.X, &c.Y, &c.Z);
		Evaluate_Color("SkyIntensity.Sun", TimeOfDay, &i.X, &i.Y, &i.Z);
		sky.Set(c.X * i.X, c.Y * i.Y, c.Z * i.Z);
		Evaluate_Color("SkyEmissive.Default", TimeOfDay, &emissive.X, &emissive.Y, &emissive.Z);
	}
	Sun->Set_Spot_Direction(-sun);
	Vector3 diffuse(WWMath::Clamp(light.X), WWMath::Clamp(light.Y), WWMath::Clamp(light.Z));
	Sun->Set_Diffuse(diffuse);
	Vector3 ambient = sky + emissive;
	LandScene->Set_Ambient_Light(Vector3(WWMath::Clamp(ambient.X), WWMath::Clamp(ambient.Y), WWMath::Clamp(ambient.Z)));
	*out = Vector3(WWMath::Clamp(sky.X), WWMath::Clamp(sky.Y), WWMath::Clamp(sky.Z));
	LandLightingStruct lighting;								// the record at esp+0x74
	lighting.LightColor = diffuse;
	lighting.SunDirection = sun;
	lighting.SunAngle = WWMATH_PI / 2 - e;
	lighting.ShadowBias = 0.005f;
	lighting.SkyColor = *out;
	lighting.Valid = true;
	Land_Update_Lightmap(Land, lighting, full);
}

/*
** Draw_Fullscreen_Quad (0x426EC0): one additive quad over the screen, with identity transforms
*/
static void Draw_Fullscreen_Quad(const Vector3 &color)
{
	Matrix4 world, view, proj;
	DX8Wrapper::Get_Transform(D3DTS_WORLD, world);
	DX8Wrapper::Get_Transform(D3DTS_VIEW, view);
	DX8Wrapper::Get_Transform(D3DTS_PROJECTION, proj);
	Matrix4 identity(true);
	DX8Wrapper::Set_Transform(D3DTS_PROJECTION, identity);
	DX8Wrapper::Set_Transform(D3DTS_VIEW, identity);
	DX8Wrapper::Set_Transform(D3DTS_WORLD, identity);

	ShaderClass shader = ShaderClass::_PresetAdditiveSolidShader;		// 0x00084433
	shader.Set_Depth_Compare(ShaderClass::PASS_ALWAYS);
	DX8Wrapper::Set_Shader(shader);
	DX8Wrapper::Set_Texture(0, NULL);
	VertexMaterialClass *mat = VertexMaterialClass::Get_Preset(VertexMaterialClass::PRELIT_DIFFUSE);
	DX8Wrapper::Set_Material(mat);
	REF_PTR_RELEASE(mat);

	unsigned diffuse = ((unsigned)(color.X * 255.0f) << 16) | ((unsigned)(color.Y * 255.0f) << 8)
		| (unsigned)(color.Z * 255.0f);
	DynamicVBAccessClass vb(BUFFER_TYPE_DYNAMIC_DX8, dynamic_fvf_type, 4);
	{
		DynamicVBAccessClass::WriteLockClass lock(&vb);
		VertexFormatXYZNDUV2 *v = lock.Get_Formatted_Vertex_Array();
		static const float corner[4][2] = { { 1, 1 }, { 1, -1 }, { -1, -1 }, { -1, 1 } };
		for (int i = 0; i < 4; i++) {
			memset(&v[i], 0, sizeof(v[i]));
			v[i].x = corner[i][0];
			v[i].y = corner[i][1];
			v[i].z = 1.0f;
			v[i].diffuse = diffuse;
		}
	}
	DynamicIBAccessClass ib(BUFFER_TYPE_DYNAMIC_DX8, 6);
	{
		DynamicIBAccessClass::WriteLockClass lock(&ib);
		unsigned short *idx = lock.Get_Index_Array();
		idx[0] = 0; idx[1] = 2; idx[2] = 1; idx[3] = 0; idx[4] = 3; idx[5] = 2;
	}
	DX8Wrapper::Set_Vertex_Buffer(vb);
	DX8Wrapper::Set_Index_Buffer(ib, 0);
	DX8Wrapper::Draw_Triangles(0, 2, 0, 4);

	DX8Wrapper::Set_Transform(D3DTS_PROJECTION, proj);
	DX8Wrapper::Set_Transform(D3DTS_VIEW, view);
	DX8Wrapper::Set_Transform(D3DTS_WORLD, world);
}

/*
** The fill rate frame (0x426E20): ten additive quads of (0.04, 0.04, 0.05), the overlay, the text
*/
static void Render_Fill_Rate(void)
{
	if (IsIconic(MainWnd)) return;
	Begin_Frame(true, true, Vector3(0, 0, 0));
	for (int i = 0; i < 10; i++) {
		Draw_Fullscreen_Quad(Vector3(0.04f, 0.04f, 0.05f));
	}
	Render_Overlay();
	Draw_Overlay();
	End_Frame();
}

/*
** The camera on mu02's "camera" bone (0x425A80, 0x4269A0): Max's camera axes turned by
** Rotate_Y(-90) then Rotate_Z(-90)
*/
static void Set_Camera_From_Bone(void)
{
	Matrix3D tm = Mu02->Get_Bone_Transform("camera");
	tm.Rotate_Y(-WWMATH_PI / 2);
	tm.Rotate_Z(-WWMATH_PI / 2);
	Camera3D->Set_Transform(tm);
}

/*
** The polygon budget frame (0x425A80)
*/
static void Render_Polygon(void)
{
	Set_Camera_From_Bone();
	// back01 follows the camera, turned +90 degrees about Z
	Matrix3D sky(1);
	sky.Rotate_Z(WWMATH_PI / 2);
	sky.Set_Translation(Camera3D->Get_Position());
	Back01->Set_Transform(sky);
	if (IsIconic(MainWnd)) return;
	PredictiveLODOptimizerClass::Clear();
	SpaceScene->Visibility_Check(Camera3D);
	PredictiveLODOptimizerClass::Optimize_LODs((float)(SpaceBudget * 1000));
	Begin_Frame(false, true, Vector3(0, 0, 0));
	Camera3D->Set_Clip_Planes(20.0f, 100000.0f);
	WW3D::Render(BackScene, Camera3D, false, false, Vector3(0, 0, 0));
	Camera3D->Set_Clip_Planes(50.0f, 150.0f);
	Camera3D->Set_Depth_Range(1.0f, 1.0f);
	WW3D::Render(StarScene, Camera3D, false, false, Vector3(0, 0, 0));
	Camera3D->Set_Clip_Planes(100.0f, 100000.0f);
	Camera3D->Set_Depth_Range(0.0f, 1.0f);
	WW3D::Render(SpaceScene, Camera3D, false, false, Vector3(0, 0, 0));
	Render_Overlay();
	Draw_Overlay();
	End_Frame();
}

/*
** The land LOD frame (0x4269A0): mu02 above the terrain, the land scene fogged in the sky colour
*/
static void Render_Land(void)
{
	Matrix3D tm(1);
	tm.Set_Translation(Vector3(0.0f, -20000.0f, 500.0f));
	Mu02->Set_Transform(tm);
	Set_Camera_From_Bone();
	Vector3 sky;
	Update_Lighting(&sky, false);
	Land_Update_Patches(Land, Camera3D, LandLOD);
	LandScene->Set_Fog_Enable(true);
	LandScene->Set_Fog_Color(sky);
	LandScene->Set_Fog_Range(1000.0f, 80000.0f);
	if (IsIconic(MainWnd)) return;
	PredictiveLODOptimizerClass::Clear();
	LandScene->Visibility_Check(Camera3D);
	PredictiveLODOptimizerClass::Optimize_LODs((float)(LandBudget * 1000));
	Begin_Frame(true, true, sky);
	WW3D::Render(LandScene, Camera3D, false, false, Vector3(0, 0, 0));
	Render_Overlay();
	Draw_Overlay();
	End_Frame();
}

/*
** The tests (docs/vidtest.md 3). The measurements are VIDTEST's.
*/
static void Fill_Rate_Test(void)
{
	static DWORD t0 = timeGetTime();					// 0x5ED1DC, set on the first call
	FillCounter++;
	Render_Fill_Rate();
	if (FillCounter > 60) {
		TestState++;
		// 0x426DBF: fild of the unsigned (zero-extended) time, fmul 0.001f, fdivr 60.0f, ftol
		FillRateFPS = (int)(60.0f / ((double)(timeGetTime() - t0) * 0.001f));
		Log("fill rate test: %d FPS (peak %u KPoly/s)\n", FillRateFPS, PeakPolyRate);
	}
}

static void Polygon_Test(void)
{
	if (PolyCounter < 0) {
		for (int i = 0; i < 10; i++) Render_Polygon();
		PolyCounter = 0;
	}
	static DWORD t0 = GetTickCount();					// 0x5EF128
	if (PolyCounter == 15) {
		DWORD now = GetTickCount();
		float fps = ((float)PolyCounter * 1000.0f) / (float)(int)(now - t0);
		if (fps < 30.0f || SpaceBudget == 0xFF || MaxPolysSpace < (float)(SpaceBudget * 1000)) {
			TestState++;
			StarBudget--;
			SpaceBudget--;
			Log("polygon test: budget %d, star budget %d (%.0f fps at the end, peak %u KPoly/s)\n",
				SpaceBudget * 1000, StarBudget * 200, fps, PeakPolyRate);
			Vector3 tmp;
			Update_Lighting(&tmp, true);				// the land lightmap's full build
		} else {
			if (StarBudget < 11) StarBudget++;
			Stars_Set_Budget(StarField, StarBudget * 200);
			PolyCounter = 0;
			SpaceBudget++;
			Render_Polygon();
			Render_Polygon();
			t0 = GetTickCount();
		}
	}
	PolyCounter++;
	Render_Polygon();
}

static void Land_Test(void)
{
	if (LandCounter < 0) {
		for (int i = 0; i < 10; i++) Render_Land();
		LandCounter = 0;
	}
	static DWORD t0 = GetTickCount();					// 0x5ED1C0
	if (LandCounter == 15) {
		DWORD now = GetTickCount();
		float fps = ((float)LandCounter * 1000.0f) / (float)(int)(now - t0);
		if (fps >= 30.0f && LandBudget != 0xFF && (float)(LandBudget * 1000) <= MaxPolysLand) {
			if (!LandToggle && LandLOD < 7) {
				LandLOD++;
				LandToggle = true;
				Land_Set_Dirty(Land);
			} else {
				LandBudget++;
				LandToggle = false;
			}
			LandCounter = 0;
			Render_Land();
			Render_Land();
			t0 = GetTickCount();
			LandCounter++;
			Render_Land();
			return;
		}
		if (!LandToggle) {
			LandBudget--;
		} else {
			LandLOD--;
			Land_Set_Dirty(Land);
		}
		TestState++;
		Log("land test: LOD %d, budget %d (%.0f fps at the end, peak %u KPoly/s)\n", LandLOD, LandBudget * 1000,
			fps, PeakPolyRate);
	}
	LandCounter++;
	Render_Land();
}

/*
** Write_Results (0x427FC0): the results into the INIClass, Save("results.ini") to the working
** directory, quit with 1. Frame calls it on every frame in the finished state, the window
** procedure on a key or a close after the end (each time it saves again).
*/
static void Write_Results(void)
{
	ResultsIni.Put_Int("Results", "FillRateFPS", FillRateFPS);
	ResultsIni.Put_Int("Results", "PolyBudget", SpaceBudget * 1000);
	ResultsIni.Put_Int("Results", "LandPolyBudget", LandBudget * 1000);
	ResultsIni.Put_Int("Results", "LandLOD", LandLOD);
	ResultsIni.Put_Int("Results", "StarBudget", StarBudget * 200);
	ResultsIni.Put_Int("Info", "PeakPolyRate", (int)PeakPolyRate);
	int saved = ResultsIni.Save("results.ini");
	Log("results: FillRateFPS=%d PolyBudget=%d LandPolyBudget=%d LandLOD=%d StarBudget=%d PeakPolyRate=%u; "
		"results.ini %d bytes\n", FillRateFPS, SpaceBudget * 1000, LandBudget * 1000, LandLOD, StarBudget * 200,
		PeakPolyRate, saved);
	PostQuitMessage(1);
}

/*
** Frame (0x424E90)
*/
static void Frame(void)
{
	WW3D::Sync(GetTickCount());
	Push_Frame_Time();
	switch (TestState) {
		case 0:		Fill_Rate_Test(); break;
		case 1:		Polygon_Test(); break;
		case 2:		Land_Test(); break;
		default:
			TestsFinished = true;								// 0x5EF188
			Write_Results();
			break;
	}
}

/*
** Setup_Device (0x423BE0): the command line and the device
*/
static bool Setup_Device(void)
{
	const char *device = Arg_String("-device", WW3D::Get_Render_Device_Name(0));
	ResX = Arg_Int("-resx", 640);
	ResY = Arg_Int("-resy", 480);
	int depth = Arg_Int("-depth", 16);
	bool windowed = Arg_Bool("-windowed", true);
	bool sync = Arg_Bool("-sync", false);
	int filter = Arg_Int("-filter", 0);
	if (!device) {
		Log("no render device\n");
		return false;
	}
	WW3DErrorType err = WW3D::Set_Render_Device(device, ResX, ResY, depth, windowed ? 1 : 0, true);
	Log("WW3D::Set_Render_Device(\"%s\", %d, %d, %d, %d) = %d\n", device, ResX, ResY, depth, (int)windowed, (int)err);
	if (err != WW3D_ERROR_OK) {
		return false;
	}
	WW3D::Set_Ext_Swap_Interval(sync ? 1 : 0);
	WW3D::Set_Texture_Filter(filter);
	// The texture reduction from the free texture memory. VIDTEST compares the value as a signed int,
	// so a GPU reporting 2 GB or more gets 2 (O7); ours compares it unsigned (D53).
	// VIDTEST writes the value with Put_Int (signed: -3145728 on the host); ours writes it unsigned
	// (D53). E&BConfig reads it with Get_Int (atoi), which wraps it back to the same int.
	unsigned ram = DX8Wrapper::Get_Free_Texture_RAM();
	FreeTextureRam = ram;
	char value[16];
	sprintf(value, "%u", ram);
	ResultsIni.Put_String("Info", "FreeTextureRam", value);
	TextureReduction = ram < 8000000 ? 2 : ram < 16000000 ? 1 : 0;
	ResultsIni.Put_Int("Results", "TextureReduction", TextureReduction);
	WW3D::Set_Texture_Reduction(TextureReduction);
	// caps byte +0xDD: any DXTn format supported (DX8Caps), here Support_DXTC
	const DX8Caps *caps = DX8Wrapper::Get_Current_Caps();
	WW3D::Enable_Large_Texture_Extra_Reduction(!(caps && caps->Support_DXTC()));
	Log("free texture RAM %u (signed %d): texture reduction %d; DXTC %d\n", ram, (int)ram, TextureReduction,
		caps ? (int)caps->Support_DXTC() : -1);
	ResultsIni.Put_String("Info", "Device", device);
	ResultsIni.Put_Int("Info", "Width", ResX);
	ResultsIni.Put_Int("Info", "Height", ResY);
	ResultsIni.Put_Int("Info", "BitDepth", depth);
	ResultsIni.Put_Bool("Info", "Windowed", windowed);
	return true;
}

/*
** Build_Scenes (0x423F20)
*/
static bool Build_Scenes(void)
{
	if (!Arg_Present("-noondemand")) {
		Assets->Set_WW3D_Load_On_Demand(true);					// 0x423F44 (O1)
	}
	WW3D::Enable_Sorting(false);								// 0x446080(false) (O3)
	WW3D::Enable_Static_Sort_Lists(true);						// 0x5EFFD8
	WW3D::Enable_Munge_Sort_On_Load(true);						// 0x5EFFD9
	WW3D::Set_Thumbnail_Enabled(false);							// 0x5C60AC

	Sun = new LightClass(LightClass::DIRECTIONAL);
	Sun->Set_Intensity(1.0f);
	Sun->Set_Force_Visible(1);
	Sun->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Sun->Set_Ambient(Vector3(0, 0, 0));
	Sun->Set_Diffuse(Vector3(1, 1, 1));
	Sun->Set_Specular(Vector3(1, 1, 1));
	Sun->Set_Spot_Direction(Vector3(0, 0, -1));
	Sun->Set_Position(Vector3(0, 0, 6000));

	Camera3D = new CameraClass;
	Camera3D->Set_Transform(Matrix3D(1));
	Camera3D->Set_Clip_Planes(10.0f, 10000.0f);
	Camera3D->Set_View_Plane(0.7853982f, 0.5759587f);

	Camera2D = new CameraClass;
	Camera2D->Set_View_Plane(Vector2(-1.0f, -0.75f), Vector2(1.0f, 0.75f));
	Camera2D->Set_Position(Vector3(0, 0, 320));
	Camera2D->Set_Clip_Planes(300.0f, 1000.0f);

	Font = Assets->Get_Font3DInstance("font.tga");
	Log("Get_Font3DInstance(\"font.tga\") = %p\n", Font);
	if (!Font) return false;
	Text = new Render2DTextClass(Font);
	RectClass screen(0, 0, (float)ResX, (float)ResY);
	Render2DClass::Set_Screen_Resolution(screen);
	Text->Set_Coordinate_Range(screen);
	Text->Enable_Additive(false);
	Text->Enable_Alpha(true);
	WW3D::Set_Screen_UV_Bias(true);								// 0x5EFFD7

	OverlayScene = new SimpleSceneClass;
	Assets->Load_3D_Assets("mu01a.w3d");
	Mu01a = Assets->Create_Render_Obj("mu01a");
	Log("Create_Render_Obj(\"mu01a\") = %p\n", Mu01a);
	if (!Mu01a) return false;
	OverlayScene->Add_Render_Object(Mu01a);

	// The loading screen
	Text->Reset();
	Text->Set_Location(Vector2((float)(ResX / 2 - 50), (float)(ResY / 2)));
	Text->Draw_Text("Loading...", 0xFFFFFFFF);
	WW3D::Sync(GetTickCount());
	Begin_Frame(true, true, Vector3(0.2f, 0.2f, 0.2f));
	Render_Overlay();
	Text->Render();
	End_Frame();
	WW3D::Enable_Overbright_Modify_On_Load(true);				// 0x5EFFDA

	static const char *files[] = { "back01.w3d", "ths01.w3d", "jvc01.w3d", "bst29a.w3d", "pvs01.w3d",
		"pvs02.w3d", "pvs03.w3d", "pvs04.w3d",
		"pjve01a.w3d", "pjve01a-1.w3d", "pjvf01a.w3d", "pjvf01a-1.w3d", "pjvt01a.w3d", "pjvt01a-1.w3d",
		"ptve01a.w3d", "ptve01a-1.w3d", "ptvf01a.w3d", "ptvf01a-1.w3d", "ptvt01a.w3d", "ptvt01a-1.w3d",
		"ppve01a.w3d", "ppve01a-1.w3d", "ppvf01a.w3d", "ppvf01a-1.w3d", "ppvt01a.w3d", "ppvt01a-1.w3d" };
	for (int i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
		bool ok = Assets->Load_3D_Assets(files[i]);
		Log("Load_3D_Assets(\"%s\") = %d\n", files[i], (int)ok);
	}

	BackScene = new SimpleSceneClass;
	Back01 = Assets->Create_Render_Obj("back01");
	Log("Create_Render_Obj(\"back01\") = %p\n", Back01);
	if (!Back01) return false;
	BackScene->Add_Render_Object(Back01);
	float polys = Back01->Get_Cost();

	StarScene = new SimpleSceneClass;
	StarField = new StarFieldClass(200);						// 0x4228A0 (client 0x4826E0)
	StarField->Init(0);										// 0x422B20 (client 0x4829E0): stars.ini
	StarScene->Add_Render_Object(StarField);
	polys += (float)Stars_Get_Num_Polys(StarField);

	SpaceScene = new SimpleSceneClass;
	SpaceScene->Set_Ambient_Light(Vector3(0.5f, 0.5f, 0.5f));
	SpaceScene->Add_Render_Object(Sun);
	Mu02 = Assets->Create_Render_Obj("mu02");
	Log("Create_Render_Obj(\"mu02\") = %p\n", Mu02);
	if (!Mu02) return false;
	SpaceScene->Add_Render_Object(Mu02);
	MaxPolysLand = Max_LOD_And_Cost(Mu02);
	MaxPolysSpace = MaxPolysLand + polys;
	Log("Max Polys: space %.0f (back01 %.0f), land %.0f\n", MaxPolysSpace, Back01->Get_Cost(), MaxPolysLand);

	LandScene = new SimpleSceneClass;
	LandScene->Set_Ambient_Light(Vector3(0.5f, 0.5f, 0.5f));
	LandScene->Add_Render_Object(Sun);
	RegistryClass::Set_Read_Only(true);						// D69: the land's companion reads the DMap key (0x468120)
	Land = LandClass::Create("land.ini", true);				// 0x4068F0
	Log("LandClass::Create(\"land.ini\") = %p\n", Land);
	if (Land) {
		Land->Create_Decorations(0);							// 0x415C40: the trees into the land's patch records, drawn by its Render
		LandScene->Add_Render_Object(Land);
	}
	LandScene->Add_Render_Object(Mu02);
	return true;
}

/*
** Release_Scenes (0x424C10)
*/
static void Release_Scenes(void)
{
	REF_PTR_RELEASE(Sun);
	REF_PTR_RELEASE(Camera3D);
	REF_PTR_RELEASE(Camera2D);
	REF_PTR_RELEASE(SpaceScene);
	REF_PTR_RELEASE(BackScene);
	REF_PTR_RELEASE(LandScene);
	REF_PTR_RELEASE(StarScene);
	REF_PTR_RELEASE(OverlayScene);
	REF_PTR_RELEASE(Back01);
	REF_PTR_RELEASE(Mu01a);
	REF_PTR_RELEASE(Mu02);
	REF_PTR_RELEASE(StarField);
	REF_PTR_RELEASE(Land);
}

static int Run(HINSTANCE instance, int show)
{
	FrameLimit = Arg_Int("-frames", 0);
	ShotFile = Arg_String("-shot", NULL);
	ShotFrame = Arg_Int("-shotframe", FrameLimit > 0 ? FrameLimit : 60);
	Shot2File = Arg_String("-shot2", NULL);
	Shot2Frame = Arg_Int("-shot2frame", ShotFrame / 2);
	ShotDir = Arg_String("-shotdir", "shots");
	const char *list = Arg_String("-shots", NULL);
	if (list) {
		CreateDirectoryA(ShotDir, NULL);
		const char *p = list;
		while (*p && ShotListCount < 64) {
			ShotList[ShotListCount++] = atoi(p);
			while (*p && *p != ',') p++;
			if (*p == ',') p++;
		}
	}

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
	// As VIDTEST: no size; WW3D sizes the window in Set_Render_Device
	HWND hwnd = CreateWindowExA(0, "VidTestWndClass", "E&B Video Test", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL,
		instance, NULL);
	if (!hwnd) {
		Log("CreateWindow failed: %lu\n", GetLastError());
		return 11;
	}
	MainWindow = hwnd;
	MainWnd = hwnd;
	ShowWindow(hwnd, show);
	UpdateWindow(hwnd);
	Log("window created %p\n", hwnd);

	WWMath::Init();

	// The art folder, read through our file factory (VIDTEST reads its working directory)
	// Without -art: the working directory when it holds VIDTEST's art (as VIDTEST, e.g. started by
	// E&BConfig in a copy of EBCONFIG), else the install folder (read only)
	const char *defart = GetFileAttributesA("mu01a.w3d") != (DWORD)-1 ? ".\\"
		: "C:\\Program Files (x86)\\EA GAMES\\Earth & Beyond\\EBCONFIG";
	char art[MAX_PATH];
	strncpy(art, Arg_String("-art", defart), MAX_PATH - 2);
	art[MAX_PATH - 2] = 0;
	if (art[0] && art[strlen(art) - 1] != '\\') strcat(art, "\\");
	ArtFiles.Inner.Set_Sub_Directory(art);
	strcpy(ArtDir, art);
	_TheFileFactory = &ArtFiles;
	Log("art folder: %s\n", art);

	// The asset manager exists before WW3D::Init (DX8Wrapper's device setup reaches
	// ShatterSystem::Init, which asks it for htrees)
	Assets = new WW3DAssetManager;
	Assets->Set_Activate_Fog_On_Load(true);

	WW3DErrorType err = WW3D::Init(hwnd, NULL, false);
	Log("WW3D::Init = %d\n", (int)err);
	if (err != WW3D_ERROR_OK) {
		WWMath::Shutdown();
		return 12;
	}
	int count = WW3D::Get_Render_Device_Count();
	for (int i = 0; i < count; i++) {
		Log("  device %d: %s\n", i, WW3D::Get_Render_Device_Name(i));
	}
	if (count <= 0) {
		WW3D::Shutdown();
		WWMath::Shutdown();
		return 13;
	}

	int result;
	if (!Setup_Device()) {
		Message_Box("Your hardware does not support the requested video mode settings.", "E&B Video Test",
			MB_ICONINFORMATION);
		result = -1;
	} else if (Arg_Present("-noscene")) {
		// Ours (L2's test): no scenes, every frame only clears to dark blue (0, 0, 0.5)
		MSG msg;
		for (;;) {
			while (!PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE)) {
				WW3D::Sync(GetTickCount());
				Begin_Frame(true, true, Vector3(0.0f, 0.0f, 0.5f));
				End_Frame();
			}
			if (!GetMessageA(&msg, NULL, 0, 0)) break;
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		result = (int)msg.wParam;
	} else if (!Build_Scenes()) {
		Log("Build_Scenes failed\n");
		Message_Box("Failed to build the scenes required for the test.", "E&B Video Test", MB_ICONEXCLAMATION);
		result = -2;
	} else {
		Splines = new SplineDatabaseClass(NULL);							// 0x41C660
		char wsd[MAX_PATH];
		_snprintf(wsd, MAX_PATH, "%ssplines.wsd", ArtDir);
		wsd[MAX_PATH - 1] = 0;
		bool loaded = Splines->Load(wsd);									// 0x41CB20
		Log("splines: Load(\"%s\") = %d, %d entries\n", wsd, (int)loaded, Splines->Get_Count());
		TestState = Arg_Int("-state", 0);
		MSG msg;
		for (;;) {
			do {
				Frame();
			} while (!PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE));
			if (!GetMessageA(&msg, NULL, 0, 0)) break;
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		result = (int)msg.wParam;
		Log("frames presented: %d, state %d\n", Presented, TestState);
	}

	Release_Scenes();
	delete Splines;															// 0x41C880
	Splines = NULL;
	if (Text) {
		delete Text;
		Text = NULL;
	}
	REF_PTR_RELEASE(Font);
	Log("scene released; file requests: %d\n", ArtFiles.Requests);
	err = WW3D::Shutdown();
	Log("WW3D::Shutdown = %d\n", (int)err);
	Assets->Free_Assets();
	delete Assets;
	WWMath::Shutdown();
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
