/*
**	Earth & Beyond VIDTEST (reconstruction, D45): the program's own code
**
**	L2 (docs/milestones.md 3.2): the first executable built from the tree. The shape of VIDTEST's
**	WinMain (0x4235C0, docs/vidtest.md 1.1 and 1.3): a window, WWMath::Init, WW3D::Init(hwnd),
**	WW3D::Set_Render_Device, a render loop, then WW3D::Shutdown and WWMath::Shutdown. No scenes
**	yet: each frame only clears to a colour.
**
**	M2.1 (docs/milestones.md 4.2, docs/vidtest.md 7.2): one model. The assets are read from the
**	EBCONFIG folder through a SimpleFileFactoryClass (read only; the working directory, where the
**	log and screenshots go, stays this program's own), Load_3D_Assets("bst29a.w3d"),
**	Create_Render_Obj("BST29A") with its BST29A.BST29A-1 loop, a SimpleSceneClass with VIDTEST's
**	space-scene ambient (0.5) and its sun light (docs/vidtest.md 2), a camera framing the model.
**
**	Test options (ours, not VIDTEST's):
**	  -frames N    quit by itself after N frames (default 0: run until the window is closed)
**	  -shot FILE   save the back buffer of frame -shotframe N to FILE (.bmp)
**	               (default N: the -frames count, else 60)
**	  -shot2 FILE  a second screenshot, of frame -shot2frame N (default: half of -shotframe)
**	  -log FILE    init results (default vidtest.log)
**	  -art DIR     the folder the assets are read from (default the EBCONFIG install folder)
**	  -w3d FILE    the file to load (default bst29a.w3d); -model NAME (default BST29A)
**	  -anim NAME   the animation (default MODEL.MODEL-1; "none" for none)
**	  -camdir X,Y,Z  the direction from the model to the camera (default 1,-1,0.4)
**	  -camdist F   the camera distance in bounding-sphere radii (default 2.2)
**	  -noscene     L2's behaviour: only clear each frame
**	  -clear R,G,B the clear colour (default 0,0,0; with -noscene 0,0,0.5)
**	  -resx N -resy N -depth N -windowed B -device NAME   as VIDTEST
**	Exit code: 0 on success, 10+ on an init failure (11 window, 12 WW3D::Init, 13 no device,
**	14 Set_Render_Device, 15 Begin_Render/End_Render, 16 screenshot, 17 model not created).
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
#include "ffactory.h"
#include "wwfile.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "rendobj.h"
#include "hanim.h"
#include "sphere.h"
#include "matrix3d.h"
#include "sortingrenderer.h"

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

static Vector3 Arg_Vector(const char *name, const Vector3 &def)
{
	const char *s = Arg_String(name, NULL);
	Vector3 v = def;
	if (s) sscanf(s, "%f,%f,%f", &v.X, &v.Y, &v.Z);
	return v;
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

/*
** One model in a scene (M2.1). VIDTEST's space scene (docs/vidtest.md 2): a SimpleSceneClass with
** ambient (0.5, 0.5, 0.5) and the sun: a directional LightClass, intensity 1, ambient 0, diffuse
** and specular 1, far attenuation 1e6/1e6, spot direction (0, 0, -1), forced visible, at
** (0, 0, 6000) with an identity rotation. The 3D camera's view plane is VIDTEST's (45 x 33
** degrees); its position is ours: on -camdir from the bounding sphere's centre, -camdist radii
** away, looking at the centre. Like Max_LOD_And_Cost (0x424AA0), an HLOD is set to its finest
** LOD and gets the "%s.%s-1" animation in ANIM_MODE_LOOP.
*/
static int Build_Scene(WW3DAssetManager *assets, int resx, int resy, SimpleSceneClass **set_scene,
	CameraClass **set_camera, LightClass **set_sun, RenderObjClass **set_model)
{
	const char *w3d = Arg_String("-w3d", "bst29a.w3d");
	const char *name = Arg_String("-model", "BST29A");
	bool loaded = assets->Load_3D_Assets(w3d);
	Log("Load_3D_Assets(\"%s\") = %d\n", w3d, (int)loaded);
	RenderObjClass *model = assets->Create_Render_Obj(name);
	Log("Create_Render_Obj(\"%s\") = %p\n", name, model);
	if (!model) {
		return 17;
	}
	Log("  class id %d, name %s, sub objects %d, polys %d\n", model->Class_ID(), model->Get_Name(),
		model->Get_Num_Sub_Objects(), model->Get_Num_Polys());
	if (model->Class_ID() == RenderObjClass::CLASSID_HLOD) {
		model->Set_LOD_Level(model->Get_LOD_Count() - 1);
		Log("  LOD level %d of %d, cost %f\n", model->Get_LOD_Level(), model->Get_LOD_Count(), model->Get_Cost());
	}

	char anim_name[256];
	_snprintf(anim_name, sizeof(anim_name) - 1, "%s.%s-1", name, name);
	anim_name[sizeof(anim_name) - 1] = 0;
	const char *anim_arg = Arg_String("-anim", anim_name);
	if (stricmp(anim_arg, "none") != 0) {
		HAnimClass *anim = assets->Get_HAnim(anim_arg);
		Log("Get_HAnim(\"%s\") = %p\n", anim_arg, anim);
		if (!anim) {
			// VIDTEST loads no animation file (load on demand is off); see O1
			char file[256];
			const char *dot = strchr(anim_arg, '.');
			_snprintf(file, sizeof(file) - 1, "%s.w3d", dot ? dot + 1 : anim_arg);
			file[sizeof(file) - 1] = 0;
			Log("Load_3D_Assets(\"%s\") = %d\n", file, (int)assets->Load_3D_Assets(file));
			anim = assets->Get_HAnim(anim_arg);
			Log("Get_HAnim(\"%s\") = %p\n", anim_arg, anim);
		}
		if (anim) {
			Log("  %d frames at %f fps\n", anim->Get_Num_Frames(), anim->Get_Frame_Rate());
			model->Set_Animation(anim, 0, RenderObjClass::ANIM_MODE_LOOP);
			anim->Release_Ref();
		}
	}

	model->Set_Transform(Matrix3D(true));
	SphereClass sphere;
	model->Get_Obj_Space_Bounding_Sphere(sphere);
	Log("  bounding sphere centre (%f, %f, %f) radius %f\n", sphere.Center.X, sphere.Center.Y,
		sphere.Center.Z, sphere.Radius);
	float radius = sphere.Radius > 0 ? sphere.Radius : 100.0f;

	SimpleSceneClass *scene = new SimpleSceneClass;
	scene->Set_Ambient_Light(Vector3(0.5f, 0.5f, 0.5f));

	LightClass *sun = new LightClass(LightClass::DIRECTIONAL);
	sun->Set_Intensity(1.0f);
	sun->Set_Ambient(Vector3(0, 0, 0));
	sun->Set_Diffuse(Vector3(1, 1, 1));
	sun->Set_Specular(Vector3(1, 1, 1));
	sun->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	sun->Set_Spot_Direction(Vector3(0, 0, -1));
	sun->Set_Force_Visible(1);
	sun->Set_Position(Vector3(0, 0, 6000));
	scene->Add_Render_Object(sun);
	scene->Add_Render_Object(model);

	Vector3 dir = Arg_Vector("-camdir", Vector3(1.0f, -1.0f, 0.4f));
	dir.Normalize();
	float dist = radius * (float)atof(Arg_String("-camdist", "2.2"));
	Vector3 eye = sphere.Center + dir * dist;
	Matrix3D tm(1);
	tm.Look_At(eye, sphere.Center, 0.0f);
	CameraClass *camera = new CameraClass;
	camera->Set_Transform(tm);
	float znear = dist - radius * 1.5f;
	if (znear < 1.0f) znear = 1.0f;
	camera->Set_Clip_Planes(znear, dist + radius * 2.0f);
	camera->Set_View_Plane(0.7853982f, 0.5759587f);
	Log("  camera at (%f, %f, %f), clip %f..%f, screen %dx%d\n", eye.X, eye.Y, eye.Z, znear,
		dist + radius * 2.0f, resx, resy);

	*set_scene = scene;
	*set_camera = camera;
	*set_sun = sun;
	*set_model = model;
	return 0;
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
	// The art folder, read through our file factory (VIDTEST reads its working directory)
	char art[MAX_PATH];
	strncpy(art, Arg_String("-art", "C:\\Program Files (x86)\\EA GAMES\\Earth & Beyond\\EBCONFIG"), MAX_PATH - 2);
	art[MAX_PATH - 2] = 0;
	if (art[0] && art[strlen(art) - 1] != '\\') strcat(art, "\\");
	ArtFiles.Inner.Set_Sub_Directory(art);
	_TheFileFactory = &ArtFiles;
	Log("art folder: %s\n", art);

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

	bool noscene = Arg_Present("-noscene");
	const char *shot2 = Arg_String("-shot2", NULL);
	int shot2_frame = Arg_Int("-shot2frame", shot_frame / 2);

	SimpleSceneClass *scene = NULL;
	CameraClass *camera = NULL;
	LightClass *sun = NULL;
	RenderObjClass *model = NULL;
	int result = 0;
	if (!noscene) {
		result = Build_Scene(assets, resx, resy, &scene, &camera, &sun, &model);
		if (result) {
			noscene = true;
		}
	}

	// The clear colour: L2's dark blue (0, 0, 0.5); with the model, black (as VIDTEST's polygon test)
	Vector3 clear = Arg_Vector("-clear", Vector3(0.0f, 0.0f, noscene ? 0.5f : 0.0f));
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
		if (scene && b == WW3D_ERROR_OK) {
			WW3DErrorType r = WW3D::Render(scene, camera);
			if (frame == 0) Log("first WW3D::Render = %d\n", (int)r);
		}
		bool want1 = shot && frame + 1 == shot_frame;
		bool want2 = shot2 && frame + 1 == shot2_frame;
		if ((want1 || want2) && b == WW3D_ERROR_OK) {
			// End_Render draws the sorted (alpha) polygons first; draw them now so the
			// screenshot has them (the second Flush in End_Render then has nothing left)
			SortingRendererClass::Flush();
			const char *name = want1 ? shot : shot2;
			bool ok = Save_Back_Buffer(name);
			Log("screenshot %s of frame %d: %s\n", name, frame + 1, ok ? "saved" : "failed");
			if (!ok) result = 16;
			if (model && model->Peek_Animation()) {
				Log("  animation %s\n", model->Peek_Animation()->Get_Name());
			}
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

	// Release the scene before WW3D::Shutdown (VIDTEST's Release_Scenes, docs/vidtest.md 2)
	if (sun) sun->Release_Ref();
	if (camera) camera->Release_Ref();
	if (scene) scene->Release_Ref();
	if (model) model->Release_Ref();
	Log("scene released; file requests: %d\n", ArtFiles.Requests);

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
