// Earth & Beyond client (reconstruction)
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
**   D3D8SHOT_DUMP   dump every draw call of these frames, comma-separated (M2.4: the land's geometry). With it set
**                   (0: no frame, only the buffers shadowed from the start), a trigger shot named dump_* also dumps
**                   the frame after the shot (an e2e 'shot dump_NAME' step: a dump on an event, not a frame number)
**   D3D8SHOT_TRIGGER a file path: when it exists at a Present (looked at every 8th), that frame is saved as
**                   shot_<the file's first line>_NNNNN.bmp and the file deleted (tools/harness/e2e.py's 'shot' step:
**                   a shot on an event, not at a frame number). A first line 'mark NAME' is a marker instead:
**                   logged, and passed to the program's exported Stub_Log_Mark when it has one
**   D3D8SHOT_DRAWLOG 1: a trigger shot is taken one frame later, and that frame's draw calls, textures and view
**                   and projection matrices go to shot_<name>_NNNNN.json beside it (the draw log, below
**                   Log_Draw; test-plan C3). Off: nothing changes
**   D3D8SHOT_CAMERA a file path: at every 2nd Present the frame's view matrices are written to it, one line each
**                   ('frame N view K of M' and the 16 floats of the K-th SetTransform(D3DTS_VIEW) of the frame, the
**                   first 4 distinct ones): where the program's camera is, for a harness that steers by it
**                   (tools/harness/e2e.py's pose / walk steps). Written to <path>.tmp and renamed.
**   D3D8SHOT_INPUT  1: the program's user32 imports (GetMessageA, PeekMessageA, GetCursorPos, GetKeyState, ...)
**                   read the messages a harness posts: a virtual cursor and keyboard, the real ones never read
**                   or moved. The same for the retail client and the project's own builds
**   D3D8SHOT_INPUT_TAG 1: only messages carrying the harness's tag (IN_TAG in wParam's high word) are input;
**                   untagged mouse and key messages, a person's, are dropped and logged 'NOT REPLAYED'
**   D3D8SHOT_CURSOR off (without D3D8SHOT_INPUT): SetCursorPos and ClipCursor do nothing, so several clients
**                   side by side leave the real cursor alone
**   D3D8SHOT_NOREGISTER 1: the program's GetProcAddress(module, "DllRegisterServer") gets a function that does
**                   nothing and returns S_OK. The client registers BrowserEngine.dll's COM class at every start
**                   (ww3d2 dx8webbrowser.cpp, DX8WebBrowser::Initialize); the keys are that DLL's own
**                   (HKCR CLSID {2B2CC8B0-2DC0-48c6-B6FD-C07820A6477E} InprocServer32 = the registering copy's
**                   path, BrowserEngine.FEBrowserEngine2, its type library), and a test run must not write the
**                   registry (D69, D249): the class the install registered stays as it is
**   D3D8SHOT_DIALOGS 1: the program's DialogBoxParamA and MessageBoxA are its own. Without it (always, from
**                   DLL_PROCESS_ATTACH, before WinMain) neither opens anything: logged, IDOK returned (D69).
**                   Always: a first-chance 'CRASH:' / 'CRASH regs:' pair of lines in this log (Hook_Safety)
** Output: d3d8shot.log, frame_NNNNN.bmp (24-bit), draws_NNNNN.bin.
**
** The draw dump (D3D8SHOT_DUMP): every vertex and index buffer the program locks is shadowed (the
** bytes it wrote are copied at Unlock, so dynamic write-only buffers can be read back), and each
** DrawIndexedPrimitive of a chosen frame writes one record to draws_NNNNN.bin (little endian):
**   "DIP2", primitive type, min index, vertex count, start index, primitive count, base vertex
**   index, stream 0 stride, vertex shader (FVF), index size (2 or 4), texture 0 and 1 (pointer,
**   width, height, format), world, view and projection matrices (16 floats each, D3D's row
**   vectors), the render states in DumpStates (uint32 each), the texture stage states in
**   DumpStageStates for stages 0 and 1, the index count n, n indices (uint32), then the vertices
**   min..min+count-1 (relative to the base vertex index), stride bytes each.
** DrawPrimitive and the UP calls of those frames are only counted in the log.
**
** C89 for VC6; no SDK headers: the few D3D8 types and vtable slots used are declared here.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

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
static char				Trigger[MAX_PATH];		/* D3D8SHOT_TRIGGER, empty: none */
static char				TriggerName[64];		/* the shot's name while it is saved */
static CRITICAL_SECTION	LogLock;

/* The draw log (D3D8SHOT_DRAWLOG): the frame being logged (0: none), and the SetTransform hook's part */
static int				DrawLog = 0;
static int				LogFrame = 0;
static void Log_Set(DWORD state, const float *m);

/* The draw dump (D3D8SHOT_DUMP) */
static void Log(const char *format, ...);
static void Patch(void **slot, void *value);
#define SLOT_SETTRANSFORM		37
#define SLOT_SETTEXTURE			61
#define SLOT_DRAWPRIMITIVE		70
#define SLOT_DRAWINDEXED		71
#define SLOT_DRAWPRIMITIVEUP	72
#define SLOT_DRAWINDEXEDUP		73
#define SLOT_SETVERTEXSHADER	76
#define SLOT_SETSTREAMSOURCE	83
#define SLOT_SETINDICES			85
#define SLOT_CREATEVB			23
#define SLOT_CREATEIB			24
#define SLOT_BUF_GETTYPE		10		/* IDirect3DResource8 */
#define SLOT_BUF_LOCK			11		/* vertex and index buffers */
#define SLOT_BUF_UNLOCK			12
#define SLOT_BUF_GETDESC		13
#define SLOT_TEX_GETLEVELDESC	14

typedef HRESULT (STDMETHODCALLTYPE *SetTransformType)(void *, DWORD, const float *);
typedef HRESULT (STDMETHODCALLTYPE *SetTextureType)(void *, DWORD, void *);
typedef HRESULT (STDMETHODCALLTYPE *DrawPrimitiveType)(void *, UINT, UINT, UINT);
typedef HRESULT (STDMETHODCALLTYPE *DrawIndexedType)(void *, UINT, UINT, UINT, UINT, UINT);
typedef HRESULT (STDMETHODCALLTYPE *DrawPrimitiveUPType)(void *, UINT, UINT, const void *, UINT);
typedef HRESULT (STDMETHODCALLTYPE *DrawIndexedUPType)(void *, UINT, UINT, UINT, UINT, const void *, UINT, const void *, UINT);
typedef HRESULT (STDMETHODCALLTYPE *SetVertexShaderType)(void *, DWORD);
typedef HRESULT (STDMETHODCALLTYPE *SetStreamSourceType)(void *, UINT, void *, UINT);
typedef HRESULT (STDMETHODCALLTYPE *SetIndicesType)(void *, void *, UINT);
typedef HRESULT (STDMETHODCALLTYPE *CreateBufferType)(void *, UINT, DWORD, DWORD, UINT, void **);
typedef UINT (STDMETHODCALLTYPE *GetTypeType)(void *);
typedef HRESULT (STDMETHODCALLTYPE *BufLockType)(void *, UINT, UINT, unsigned char **, DWORD);
typedef HRESULT (STDMETHODCALLTYPE *BufUnlockType)(void *);
typedef HRESULT (STDMETHODCALLTYPE *BufGetDescType)(void *, UINT *);	/* Format, Type, Usage, Pool, Size[, FVF] */
typedef HRESULT (STDMETHODCALLTYPE *GetLevelDescType)(void *, UINT, SurfaceDesc *);

static SetTransformType		RealSetTransform;
static SetTextureType		RealSetTexture;
static DrawPrimitiveType	RealDrawPrimitive;
static DrawIndexedType		RealDrawIndexed;
static DrawPrimitiveUPType	RealDrawPrimitiveUP;
static DrawIndexedUPType	RealDrawIndexedUP;
static SetVertexShaderType	RealSetVertexShader;
static SetStreamSourceType	RealSetStreamSource;
static SetIndicesType		RealSetIndices;
static CreateBufferType		RealCreateVB, RealCreateIB;

static int				Dumps[64];
static int				DumpCount = 0;
static FILE *			DumpFile = NULL;
static int				DumpFrame = 0;
static int				DumpDraws = 0, DumpOther = 0;
static float			Matrices[3][16];		/* world (256), view (2), projection (3) */
static void *			Textures[2];
static void *			Stream0 = NULL;
static UINT				Stride0 = 0;
static void *			Indices = NULL;
static UINT				BaseVertex = 0;
static DWORD			Shader = 0;
static int				Calls[4];			/* SetTexture, SetVertexShader, SetStreamSource, SetIndices */

/* the shadows of the buffers (open addressing on the object pointer) */
typedef struct {
	void *			Obj;
	unsigned char *	Data;
	UINT			Size;
	unsigned char *	LockPtr;
	UINT			LockOff, LockSize;
	UINT			Index32;
} Shadow;
#define SHADOWS	16384
static Shadow			Shadows[SHADOWS];

/* the real Lock/Unlock per buffer vtable (the runtime may use several buffer classes) */
typedef struct { void **Vtbl; BufLockType Lock; BufUnlockType Unlock; } BufVtbl;
static BufVtbl			BufVtbls[16];
static int				BufVtblCount = 0;

static int Dumping(int frame)
{
	int i;
	for (i = 0; i < DumpCount; i++) {
		if (Dumps[i] == frame) return 1;
	}
	return 0;
}

static Shadow *Find_Shadow(void *obj, int add)
{
	unsigned h = ((unsigned)obj >> 4) % SHADOWS, n;
	for (n = 0; n < SHADOWS; n++, h = (h + 1) % SHADOWS) {
		if (Shadows[h].Obj == obj) return &Shadows[h];
		if (Shadows[h].Obj == NULL) {
			if (!add) return NULL;
			Shadows[h].Obj = obj;
			return &Shadows[h];
		}
	}
	return NULL;
}

static BufVtbl *Find_Vtbl(void *obj)
{
	int i;
	for (i = 0; i < BufVtblCount; i++) {
		if (BufVtbls[i].Vtbl == *(void ***)obj) return &BufVtbls[i];
	}
	return NULL;
}

static HRESULT STDMETHODCALLTYPE Hook_Buf_Lock(void *buf, UINT off, UINT size, unsigned char **data, DWORD flags)
{
	BufVtbl *v = Find_Vtbl(buf);
	HRESULT hr = v->Lock(buf, off, size, data, flags);
	Shadow *s = Find_Shadow(buf, 0);
	if (SUCCEEDED(hr) && s && s->Data && data) {
		if (size == 0 || off + size > s->Size) size = (off < s->Size) ? s->Size - off : 0;
		s->LockPtr = *data;
		s->LockOff = off;
		s->LockSize = (flags & 0x10 /* D3DLOCK_READONLY */) ? 0 : size;
	}
	return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_Buf_Unlock(void *buf)
{
	BufVtbl *v = Find_Vtbl(buf);
	Shadow *s = Find_Shadow(buf, 0);
	if (s && s->Data && s->LockPtr && s->LockSize) {
		memcpy(s->Data + s->LockOff, s->LockPtr, s->LockSize);
	}
	if (s) s->LockPtr = NULL;
	return v->Unlock(buf);
}

/* A new buffer: (re)make its shadow and hook its vtable's Lock and Unlock once */
static void Track_Buffer(void *buf, int index)
{
	UINT desc[6];
	Shadow *s;
	BufVtbl *v;
	memset(desc, 0, sizeof(desc));
	((BufGetDescType)VT(buf, SLOT_BUF_GETDESC))(buf, desc);
	s = Find_Shadow(buf, 1);
	if (!s) return;
	free(s->Data);
	s->Size = desc[4];
	s->Data = (unsigned char *)calloc(1, s->Size ? s->Size : 1);
	s->LockPtr = NULL;
	s->Index32 = index && desc[0] == 102;		/* D3DFMT_INDEX32 */
	v = Find_Vtbl(buf);
	if (!v && BufVtblCount < 16) {
		v = &BufVtbls[BufVtblCount++];
		v->Vtbl = *(void ***)buf;
		v->Lock = (BufLockType)VT(buf, SLOT_BUF_LOCK);
		v->Unlock = (BufUnlockType)VT(buf, SLOT_BUF_UNLOCK);
		Patch(&VT(buf, SLOT_BUF_LOCK), (void *)Hook_Buf_Lock);
		Patch(&VT(buf, SLOT_BUF_UNLOCK), (void *)Hook_Buf_Unlock);
		Log("buffer vtable %p hooked (%s)\n", v->Vtbl, index ? "index" : "vertex");
	}
}

static HRESULT STDMETHODCALLTYPE Hook_Create_VB(void *dev, UINT len, DWORD usage, DWORD fvf, UINT pool, void **vb)
{
	HRESULT hr = RealCreateVB(dev, len, usage, fvf, pool, vb);
	if (SUCCEEDED(hr) && vb && *vb) Track_Buffer(*vb, 0);
	return hr;
}

static HRESULT STDMETHODCALLTYPE Hook_Create_IB(void *dev, UINT len, DWORD usage, DWORD format, UINT pool, void **ib)
{
	HRESULT hr = RealCreateIB(dev, len, usage, format, pool, ib);
	if (SUCCEEDED(hr) && ib && *ib) Track_Buffer(*ib, 1);
	return hr;
}

static char Camera[MAX_PATH];
static char CameraTmp[MAX_PATH + 8];
static float Views[4][16];
static int ViewCount = 0, ViewSets = 0;
static HRESULT STDMETHODCALLTYPE Hook_Set_Transform(void *dev, DWORD state, const float *m)
{
	int k = state == 256 ? 0 : state == 2 ? 1 : state == 3 ? 2 : -1;
	if (k >= 0 && m) memcpy(Matrices[k], m, sizeof(Matrices[k]));
	if (LogFrame && m && (k == 1 || k == 2) && Presented + 1 == LogFrame) Log_Set(state, m);
	if (k == 1 && m && Camera[0]) {
		int i;
		ViewSets++;
		for (i = 0; i < ViewCount; i++) if (!memcmp(Views[i], m, sizeof(Views[i]))) break;
		if (i == ViewCount && ViewCount < 4) memcpy(Views[ViewCount++], m, sizeof(Views[0]));
	}
	return RealSetTransform(dev, state, m);
}

static HRESULT STDMETHODCALLTYPE Hook_Set_Texture(void *dev, DWORD stage, void *tex)
{
	Calls[0]++;
	if (stage < 2) Textures[stage] = tex;
	return RealSetTexture(dev, stage, tex);
}

static HRESULT STDMETHODCALLTYPE Hook_Set_Vertex_Shader(void *dev, DWORD handle)
{
	Calls[1]++;
	Shader = handle;
	return RealSetVertexShader(dev, handle);
}

static HRESULT STDMETHODCALLTYPE Hook_Set_Stream_Source(void *dev, UINT n, void *vb, UINT stride)
{
	Calls[2]++;
	if (n == 0) { Stream0 = vb; Stride0 = stride; }
	return RealSetStreamSource(dev, n, vb, stride);
}

static HRESULT STDMETHODCALLTYPE Hook_Set_Indices(void *dev, void *ib, UINT base)
{
	Calls[3]++;
	Indices = ib;
	BaseVertex = base;
	return RealSetIndices(dev, ib, base);
}

static void Open_Dump(int frame)
{
	char name[MAX_PATH];
	if (DumpFile && DumpFrame == frame) return;
	if (DumpFile) {
		Log("frame %d: dumped %d indexed draws (%d other draws)\n", DumpFrame, DumpDraws, DumpOther);
		fclose(DumpFile);
	}
	_snprintf(name, sizeof(name) - 1, "%s\\draws_%05d.bin", Dir, frame);
	name[sizeof(name) - 1] = 0;
	DumpFile = fopen(name, "wb");
	DumpFrame = frame;
	DumpDraws = DumpOther = 0;
}

static void Close_Dump(void)
{
	if (!DumpFile) return;
	Log("frame %d: dumped %d indexed draws (%d other draws); calls so far: SetTexture %d, SetVertexShader %d, "
		"SetStreamSource %d, SetIndices %d\n", DumpFrame, DumpDraws, DumpOther, Calls[0], Calls[1], Calls[2], Calls[3]);
	fclose(DumpFile);
	DumpFile = NULL;
}

/*
** The 32-bit textures (A8R8G8B8, X8R8G8B8) a dumped frame draws with are saved once each as
** tex_<frame>_<pointer>.bmp (level 0, 24-bit, alpha dropped), read with LockRect(READONLY): this
** works for managed textures (WW3D's), whose system-memory copy the runtime keeps.
*/
#define SLOT_TEX_LOCKRECT		16
#define SLOT_TEX_UNLOCKRECT		17
typedef HRESULT (STDMETHODCALLTYPE *TexLockRectType)(void *, UINT, LockedRect *, const RECT *, DWORD);
typedef HRESULT (STDMETHODCALLTYPE *TexUnlockRectType)(void *, UINT);
static void *			SavedTextures[256];
static int				SavedTextureCount = 0;

#define SLOT_TEX_GETSURFACELEVEL	15
typedef HRESULT (STDMETHODCALLTYPE *GetSurfaceLevelType)(void *, UINT, void **);

static void Save_Texture(void *dev, void *tex, const SurfaceDesc *d)
{
	void *copy = NULL;
	LockedRect lr;
	char name[MAX_PATH];
	FILE *fp;
	int i, x, y, stride;
	unsigned char *rgb;
	static int saved_frame = 0;
	if (saved_frame != DumpFrame) {			/* once per texture and dumped frame */
		saved_frame = DumpFrame;
		SavedTextureCount = 0;
	}
	for (i = 0; i < SavedTextureCount; i++) {
		if (SavedTextures[i] == tex) return;
	}
	if (SavedTextureCount >= 256 || (d->Format != 21 && d->Format != 22)) return;
	SavedTextures[SavedTextureCount++] = tex;
	if (FAILED(((TexLockRectType)VT(tex, SLOT_TEX_LOCKRECT))(tex, 0, &lr, NULL, 0x10 /* READONLY */))) {
		/* a default-pool texture: copy level 0 into an image surface first, as the back buffer */
		void *level = NULL;
		HRESULT hr = ((GetSurfaceLevelType)VT(tex, SLOT_TEX_GETSURFACELEVEL))(tex, 0, &level);
		if (SUCCEEDED(hr)) hr = ((CreateImageSurfaceType)VT(dev, SLOT_CREATEIMAGESURFACE))(dev, d->Width, d->Height, d->Format, &copy);
		if (SUCCEEDED(hr)) hr = ((CopyRectsType)VT(dev, SLOT_COPYRECTS))(dev, level, NULL, 0, copy, NULL);
		if (level) ((ReleaseType)VT(level, SLOT_RELEASE))(level);
		if (SUCCEEDED(hr)) hr = ((LockRectType)VT(copy, SLOT_LOCKRECT))(copy, &lr, NULL, 0x10);
		if (FAILED(hr)) {
			Log("texture %p: not readable (%08lX)\n", tex, hr);
			if (copy) ((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
			return;
		}
	}
	stride = ((d->Width * 3) + 3) & ~3;
	rgb = (unsigned char *)calloc(stride, d->Height);
	for (y = 0; rgb && y < (int)d->Height; y++) {
		const unsigned char *src = (const unsigned char *)lr.Bits + lr.Pitch * y;
		unsigned char *dst = rgb + stride * (d->Height - 1 - y);
		for (x = 0; x < (int)d->Width; x++) {
			dst[x * 3] = src[x * 4];
			dst[x * 3 + 1] = src[x * 4 + 1];
			dst[x * 3 + 2] = src[x * 4 + 2];
		}
	}
	if (copy) {
		((UnlockRectType)VT(copy, SLOT_UNLOCKRECT))(copy);
		((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
	} else {
		((TexUnlockRectType)VT(tex, SLOT_TEX_UNLOCKRECT))(tex, 0);
	}
	_snprintf(name, sizeof(name) - 1, "%s\\tex_%05d_%p.bmp", Dir, DumpFrame, tex);
	name[sizeof(name) - 1] = 0;
	fp = rgb ? fopen(name, "wb") : NULL;
	if (fp) {
		BITMAPFILEHEADER fh;
		BITMAPINFOHEADER bi;
		memset(&fh, 0, sizeof(fh));
		memset(&bi, 0, sizeof(bi));
		bi.biSize = sizeof(bi);
		bi.biWidth = d->Width;
		bi.biHeight = d->Height;
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;
		fh.bfType = 0x4D42;
		fh.bfOffBits = sizeof(fh) + sizeof(bi);
		fh.bfSize = fh.bfOffBits + stride * d->Height;
		fwrite(&fh, sizeof(fh), 1, fp);
		fwrite(&bi, sizeof(bi), 1, fp);
		fwrite(rgb, stride * d->Height, 1, fp);
		fclose(fp);
		Log("texture %p (%ux%u, format %u) saved: %s\n", tex, d->Width, d->Height, d->Format, name);
	}
	free(rgb);
}

static void Write_Texture(void *dev, void *tex)
{
	UINT rec[4];
	SurfaceDesc d;
	memset(rec, 0, sizeof(rec));
	rec[0] = (UINT)tex;
	if (tex && ((GetTypeType)VT(tex, SLOT_BUF_GETTYPE))(tex) == 3 /* D3DRTYPE_TEXTURE */) {
		memset(&d, 0, sizeof(d));
		if (SUCCEEDED(((GetLevelDescType)VT(tex, SLOT_TEX_GETLEVELDESC))(tex, 0, &d))) {
			rec[1] = d.Width; rec[2] = d.Height; rec[3] = d.Format;
			Save_Texture(dev, tex, &d);
		}
	}
	fwrite(rec, sizeof(rec), 1, DumpFile);
}

/*
** The device state at a dumped draw, read back with the Get calls: the runtime keeps a per-device
** copy of the device vtable and rewrites some of its slots after CreateDevice (SetTexture,
** SetVertexShader, SetStreamSource, SetIndices were never called through hooks set there), so the
** Set hooks alone don't see every change. The Get calls AddRef what they return.
*/
#define SLOT_GETTRANSFORM		38
#define SLOT_GETTEXTURE			60
#define SLOT_GETVERTEXSHADER	77
#define SLOT_GETSTREAMSOURCE	84
#define SLOT_GETINDICES			86
typedef HRESULT (STDMETHODCALLTYPE *GetTransformType)(void *, DWORD, float *);
typedef HRESULT (STDMETHODCALLTYPE *GetTextureType)(void *, DWORD, void **);
typedef HRESULT (STDMETHODCALLTYPE *GetVertexShaderType)(void *, DWORD *);
typedef HRESULT (STDMETHODCALLTYPE *GetStreamSourceType)(void *, UINT, void **, UINT *);
typedef HRESULT (STDMETHODCALLTYPE *GetIndicesType)(void *, void **, UINT *);

/* the render states and texture stage states written with each dumped draw (drawdump.py names them) */
#define SLOT_GETRENDERSTATE		51
#define SLOT_GETSTAGESTATE		62
typedef HRESULT (STDMETHODCALLTYPE *GetRenderStateType)(void *, DWORD, DWORD *);
typedef HRESULT (STDMETHODCALLTYPE *GetStageStateType)(void *, DWORD, DWORD, DWORD *);
static const DWORD DumpStates[] = {
	7 /* ZENABLE */, 14 /* ZWRITEENABLE */, 15 /* ALPHATESTENABLE */, 19 /* SRCBLEND */, 20 /* DESTBLEND */,
	22 /* CULLMODE */, 27 /* ALPHABLENDENABLE */, 28 /* FOGENABLE */, 34 /* FOGCOLOR */, 35 /* FOGTABLEMODE */,
	36 /* FOGSTART */, 37 /* FOGEND */, 47 /* ZBIAS */, 60 /* TEXTUREFACTOR */, 137 /* LIGHTING */,
	139 /* AMBIENT */, 140 /* FOGVERTEXMODE */, 141 /* COLORVERTEX */, 145 /* DIFFUSEMATERIALSOURCE */,
	147 /* AMBIENTMATERIALSOURCE */, 148 /* EMISSIVEMATERIALSOURCE */, 48 /* RANGEFOGENABLE */,
	9 /* SHADEMODE */, 24 /* ALPHAREF */
};
static const DWORD DumpStageStates[] = {
	1 /* COLOROP */, 2 /* COLORARG1 */, 3 /* COLORARG2 */, 4 /* ALPHAOP */, 5 /* ALPHAARG1 */, 6 /* ALPHAARG2 */,
	11 /* TEXCOORDINDEX */, 24 /* TEXTURETRANSFORMFLAGS */
};

static void Query_State(void *dev)
{
	static const DWORD states[3] = { 256, 2, 3 };
	void *obj;
	int k;
	for (k = 0; k < 3; k++) ((GetTransformType)VT(dev, SLOT_GETTRANSFORM))(dev, states[k], Matrices[k]);
	for (k = 0; k < 2; k++) {
		obj = NULL;
		((GetTextureType)VT(dev, SLOT_GETTEXTURE))(dev, k, &obj);
		Textures[k] = obj;
		if (obj) ((ReleaseType)VT(obj, SLOT_RELEASE))(obj);
	}
	((GetVertexShaderType)VT(dev, SLOT_GETVERTEXSHADER))(dev, &Shader);
	obj = NULL;
	((GetStreamSourceType)VT(dev, SLOT_GETSTREAMSOURCE))(dev, 0, &obj, &Stride0);
	Stream0 = obj;
	if (obj) ((ReleaseType)VT(obj, SLOT_RELEASE))(obj);
	obj = NULL;
	((GetIndicesType)VT(dev, SLOT_GETINDICES))(dev, &obj, &BaseVertex);
	Indices = obj;
	if (obj) ((ReleaseType)VT(obj, SLOT_RELEASE))(obj);
}

/* the material and the enabled lights at a dumped draw, written to d3d8shot.log (D3DMATERIAL8 17 floats,
** D3DLIGHT8 26 dwords) */
#define SLOT_GETMATERIAL		43
#define SLOT_GETLIGHT			45
#define SLOT_GETLIGHTENABLE		47
typedef HRESULT (STDMETHODCALLTYPE *GetMaterialType)(void *, float *);
typedef HRESULT (STDMETHODCALLTYPE *GetLightType)(void *, DWORD, float *);
typedef HRESULT (STDMETHODCALLTYPE *GetLightEnableType)(void *, DWORD, BOOL *);
static void Log_Lights(void *dev, int draw)
{
	float m[17], l[26];
	char line[1024];
	int n = 0;
	DWORD i;
	memset(m, 0, sizeof(m));
	((GetMaterialType)VT(dev, SLOT_GETMATERIAL))(dev, m);
	n += _snprintf(line + n, sizeof(line) - n, "draw %d: material D %.2f %.2f %.2f A %.2f %.2f %.2f E %.2f %.2f %.2f;",
		draw, m[0], m[1], m[2], m[4], m[5], m[6], m[12], m[13], m[14]);
	for (i = 0; i < 4; i++) {
		BOOL on = FALSE;
		if (((GetLightEnableType)VT(dev, SLOT_GETLIGHTENABLE))(dev, i, &on) != 0 || !on) continue;
		memset(l, 0, sizeof(l));
		((GetLightType)VT(dev, SLOT_GETLIGHT))(dev, i, l);
		n += _snprintf(line + n, sizeof(line) - n, " L%u type %u D %.2f %.2f %.2f A %.2f %.2f %.2f dir %.2f %.2f %.2f;",
			i, *(DWORD *)&l[0], l[1], l[2], l[3], l[9], l[10], l[11], l[16], l[17], l[18]);
	}
	Log("%s\n", line);
}

/*
** The draw log (D3D8SHOT_DRAWLOG=1; tools/harness/e2e.py sets it for every session; test-plan C3). A trigger shot
** is then taken one frame later than without it: the trigger seen at Present N arms frame N+1, every draw call of
** that frame is logged, and at Present N+1 shot_NAME_<N+1>.json is written, then the back buffer saved as
** shot_NAME_<N+1>.bmp, then the trigger file deleted (e2e waits for that). Between shots the hooks only compare
** the frame number. The JSON (one draw per line, fixed key order, floats "%.7g" with -0 written 0, NaN null):
**   format, shot, frame; counts {draws, DIP, DP, DPUP, DIPUP, views, projections, textures, lost};
**   render_states, stage_states: the names of the values in each draw's rs and ts[stage] (stages 0 and 1);
**   textures: each texture drawn in the frame, in first-draw order: id, type (3 texture, 4 volume, 5 cube),
**     w, h, format, levels, usage, pool and hash (FNV-1a 64 of level 0's rows read now, the X bits of X8R8G8B8,
**     X1R5G5B5 and X4R4G4B4 masked; null when the format or the surface can't be read);
**   views, projections: the distinct view and projection matrices of the frame (16 floats, D3D's rows);
**   sets: each SetTransform(VIEW / PROJECTION) of the frame in order: {draw: the draws before it, view|proj: index};
**   draws: i, call (DIP DrawIndexedPrimitive, DP DrawPrimitive, DPUP, DIPUP), prim (D3DPRIMITIVETYPE), prims,
**     verts (DIP / DIPUP: NumVertices; DP / DPUP: from the primitive count), stride (stream 0's, or the UP call's),
**     fvf (the vertex shader: an FVF code), tex [stage 0, stage 1] (an index into textures, -1 none), view and proj
**     (indices into views and projections, read back with GetTransform), world (16 floats), rs, ts.
** Pointers and buffer offsets (start index, base vertex, min index) are left out: they differ run to run.
*/
#define LOG_MATS	256
#define LOG_TEX		1024
typedef struct { char *Text; int Len, Cap; } Buf;
typedef struct {
	void *Tex;
	UINT Type, Width, Height, Format, Levels, Usage, Pool;
	int Hashed;
	unsigned __int64 Hash;
} LogTex;
static Buf				LogDraws, LogSets;
static int				LogDrawCount, LogCalls[4], LogLost;
static float			LogViews[LOG_MATS][16], LogProjs[LOG_MATS][16];
static int				LogViewCount, LogProjCount;
static LogTex			LogTexs[LOG_TEX];
static int				LogTexCount;
static const DWORD LogStates[] = {
	7, 14, 15, 19, 20, 22, 27, 28, 34, 35, 36, 37, 47, 60, 137, 139, 140, 141, 145, 147, 148, 48, 9, 24,
	8, 23, 25, 26, 29, 52, 143, 146, 168, 171
};
static const char *LogStateNames[] = {
	"ZENABLE", "ZWRITEENABLE", "ALPHATESTENABLE", "SRCBLEND", "DESTBLEND", "CULLMODE", "ALPHABLENDENABLE",
	"FOGENABLE", "FOGCOLOR", "FOGTABLEMODE", "FOGSTART", "FOGEND", "ZBIAS", "TEXTUREFACTOR", "LIGHTING", "AMBIENT",
	"FOGVERTEXMODE", "COLORVERTEX", "DIFFUSEMATERIALSOURCE", "AMBIENTMATERIALSOURCE", "EMISSIVEMATERIALSOURCE",
	"RANGEFOGENABLE", "SHADEMODE", "ALPHAREF",
	"FILLMODE", "ZFUNC", "ALPHAFUNC", "DITHERENABLE", "SPECULARENABLE", "STENCILENABLE", "NORMALIZENORMALS",
	"SPECULARMATERIALSOURCE", "COLORWRITEENABLE", "BLENDOP"
};
static const DWORD LogStageStates[] = { 1, 2, 3, 4, 5, 6, 11, 24, 13, 14, 16, 17, 18 };
static const char *LogStageStateNames[] = {
	"COLOROP", "COLORARG1", "COLORARG2", "ALPHAOP", "ALPHAARG1", "ALPHAARG2", "TEXCOORDINDEX",
	"TEXTURETRANSFORMFLAGS", "ADDRESSU", "ADDRESSV", "MAGFILTER", "MINFILTER", "MIPFILTER"
};
#define SLOT_TEX_GETLEVELCOUNT	13
typedef DWORD (STDMETHODCALLTYPE *GetLevelCountType)(void *);

static void Buf_Add(Buf *b, const char *format, ...)
{
	va_list va;
	int n;
	for (;;) {
		if (b->Text && b->Cap - b->Len > 1) {
			va_start(va, format);
			n = _vsnprintf(b->Text + b->Len, b->Cap - b->Len - 1, format, va);
			va_end(va);
			if (n >= 0 && n < b->Cap - b->Len - 1) {
				b->Len += n;
				b->Text[b->Len] = 0;
				return;
			}
		}
		{
			int cap = b->Cap ? b->Cap * 2 : 1 << 16;
			char *t = (char *)realloc(b->Text, cap);
			if (!t) return;
			b->Text = t;
			b->Cap = cap;
		}
	}
}

static void Fmt_Float(char *t, float f)
{
	double d = f;
	if (!_finite(d)) strcpy(t, "null");
	else if (d == 0) strcpy(t, "0");
	else { _snprintf(t, 31, "%.7g", d); t[31] = 0; }
}

static void Buf_Floats(Buf *b, const float *m, int n)
{
	char t[32];
	int i;
	Buf_Add(b, "[");
	for (i = 0; i < n; i++) {
		Fmt_Float(t, m[i]);
		Buf_Add(b, i ? ",%s" : "%s", t);
	}
	Buf_Add(b, "]");
}

static int Log_Matrix(float (*list)[16], int *count, const float *m)
{
	int i;
	for (i = 0; i < *count; i++) if (!memcmp(list[i], m, sizeof(list[i]))) return i;
	if (*count >= LOG_MATS) { LogLost++; return -1; }
	memcpy(list[*count], m, sizeof(list[0]));
	return (*count)++;
}

static void Log_Set(DWORD state, const float *m)
{
	int k = state == 2 ? Log_Matrix(LogViews, &LogViewCount, m) : Log_Matrix(LogProjs, &LogProjCount, m);
	Buf_Add(&LogSets, "%s{\"draw\":%d,\"%s\":%d}", LogSets.Len ? ",\n" : "", LogDrawCount,
		state == 2 ? "view" : "proj", k);
}

/* level 0's bytes as the program sees them: FNV-1a 64 over the rows (pitch padding left out) */
static int Hash_Texture(void *dev, void *tex, const SurfaceDesc *d, unsigned __int64 *hash)
{
	void *copy = NULL;
	LockedRect lr;
	unsigned __int64 h = 0xcbf29ce484222325ui64;
	UINT bpp = 0, block = 0, row, rows, x, y, mask = 0;
	switch (d->Format) {
	case 21: bpp = 4; break;
	case 22: bpp = 4; mask = 3; break;						/* X8R8G8B8: every 4th byte (x % 4 == 3) */
	case 20: bpp = 3; break;
	case 23: case 25: case 26: case 29: case 41: case 51: bpp = 2; break;
	case 24: case 30: bpp = 2; mask = 1; break;				/* X1R5G5B5, X4R4G4B4: the high byte's X bits */
	case 27: case 28: case 40: case 50: case 52: bpp = 1; break;
	case 0x31545844: block = 8; break;						/* DXT1 */
	case 0x32545844: case 0x33545844: case 0x34545844: case 0x35545844: block = 16; break;
	}
	if (block) { row = ((d->Width + 3) / 4) * block; rows = (d->Height + 3) / 4; }
	else if (bpp) { row = d->Width * bpp; rows = d->Height; }
	else return 0;
	if (FAILED(((TexLockRectType)VT(tex, SLOT_TEX_LOCKRECT))(tex, 0, &lr, NULL, 0x10 /* READONLY */))) {
		void *level = NULL;
		HRESULT hr = ((GetSurfaceLevelType)VT(tex, SLOT_TEX_GETSURFACELEVEL))(tex, 0, &level);
		if (SUCCEEDED(hr)) hr = ((CreateImageSurfaceType)VT(dev, SLOT_CREATEIMAGESURFACE))(dev, d->Width, d->Height, d->Format, &copy);
		if (SUCCEEDED(hr)) hr = ((CopyRectsType)VT(dev, SLOT_COPYRECTS))(dev, level, NULL, 0, copy, NULL);
		if (level) ((ReleaseType)VT(level, SLOT_RELEASE))(level);
		if (SUCCEEDED(hr)) hr = ((LockRectType)VT(copy, SLOT_LOCKRECT))(copy, &lr, NULL, 0x10);
		if (FAILED(hr)) {
			if (copy) ((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
			return 0;
		}
	}
	for (y = 0; y < rows; y++) {
		const unsigned char *p = (const unsigned char *)lr.Bits + lr.Pitch * (int)y;
		for (x = 0; x < row; x++) {
			unsigned char c = p[x];
			if (mask == 3 && (x & 3) == 3) c = 0;
			else if (mask == 1 && (x & 1) == 1) c = (unsigned char)(d->Format == 24 ? c & 0x7F : c & 0x0F);
			h ^= c;
			h *= 0x100000001b3ui64;
		}
	}
	if (copy) {
		((UnlockRectType)VT(copy, SLOT_UNLOCKRECT))(copy);
		((ReleaseType)VT(copy, SLOT_RELEASE))(copy);
	} else {
		((TexUnlockRectType)VT(tex, SLOT_TEX_UNLOCKRECT))(tex, 0);
	}
	*hash = h;
	return 1;
}

static int Log_Texture(void *dev, void *tex)
{
	LogTex *t;
	SurfaceDesc d;
	int i;
	if (!tex) return -1;
	for (i = 0; i < LogTexCount; i++) if (LogTexs[i].Tex == tex) return i;
	if (LogTexCount >= LOG_TEX) { LogLost++; return -1; }
	t = &LogTexs[LogTexCount];
	memset(t, 0, sizeof(*t));
	t->Tex = tex;
	t->Type = ((GetTypeType)VT(tex, SLOT_BUF_GETTYPE))(tex);
	if (t->Type == 3 /* D3DRTYPE_TEXTURE */) {
		memset(&d, 0, sizeof(d));
		if (SUCCEEDED(((GetLevelDescType)VT(tex, SLOT_TEX_GETLEVELDESC))(tex, 0, &d))) {
			t->Width = d.Width; t->Height = d.Height; t->Format = d.Format; t->Usage = d.Usage; t->Pool = d.Pool;
			t->Hashed = Hash_Texture(dev, tex, &d, &t->Hash);
		}
		t->Levels = ((GetLevelCountType)VT(tex, SLOT_TEX_GETLEVELCOUNT))(tex);
	}
	return LogTexCount++;
}

static UINT Prim_Verts(UINT type, UINT prims)
{
	return type == 1 ? prims : type == 2 ? prims * 2 : type == 3 ? prims + 1 : type == 4 ? prims * 3 : prims + 2;
}

static void Log_Draw(void *dev, int call, UINT type, UINT prims, UINT verts, UINT stride)
{
	static const char *calls[4] = { "DIP", "DP", "DPUP", "DIPUP" };
	int v, p, t0, t1, n;
	UINT i;
	char t[32];
	Query_State(dev);
	v = Log_Matrix(LogViews, &LogViewCount, Matrices[1]);
	p = Log_Matrix(LogProjs, &LogProjCount, Matrices[2]);
	t0 = Log_Texture(dev, Textures[0]);
	t1 = Log_Texture(dev, Textures[1]);
	Buf_Add(&LogDraws, "%s{\"i\":%d,\"call\":\"%s\",\"prim\":%u,\"prims\":%u,\"verts\":%u,\"stride\":%u,\"fvf\":%lu,"
		"\"tex\":[%d,%d],\"view\":%d,\"proj\":%d,\"world\":", LogDrawCount ? ",\n" : "", LogDrawCount, calls[call],
		type, prims, verts, call < 2 ? Stride0 : stride, Shader, t0, t1, v, p);
	Buf_Floats(&LogDraws, Matrices[0], 16);
	Buf_Add(&LogDraws, ",\"rs\":[");
	for (i = 0; i < sizeof(LogStates) / sizeof(LogStates[0]); i++) {
		DWORD x = 0;
		((GetRenderStateType)VT(dev, SLOT_GETRENDERSTATE))(dev, LogStates[i], &x);
		if (LogStates[i] == 36 || LogStates[i] == 37) {		/* FOGSTART, FOGEND: floats */
			Fmt_Float(t, *(float *)&x);
			Buf_Add(&LogDraws, i ? ",%s" : "%s", t);
		} else {
			Buf_Add(&LogDraws, i ? ",%lu" : "%lu", x);
		}
	}
	Buf_Add(&LogDraws, "],\"ts\":[");
	for (n = 0; n < 2; n++) {
		Buf_Add(&LogDraws, n ? ",[" : "[");
		for (i = 0; i < sizeof(LogStageStates) / sizeof(LogStageStates[0]); i++) {
			DWORD x = 0;
			((GetStageStateType)VT(dev, SLOT_GETSTAGESTATE))(dev, n, LogStageStates[i], &x);
			Buf_Add(&LogDraws, i ? ",%lu" : "%lu", x);
		}
		Buf_Add(&LogDraws, "]");
	}
	Buf_Add(&LogDraws, "]}");
	LogDrawCount++;
	LogCalls[call]++;
}

static void Start_Draw_Log(int frame)
{
	LogFrame = frame;
	LogDraws.Len = LogSets.Len = 0;
	LogDrawCount = LogLost = LogViewCount = LogProjCount = LogTexCount = 0;
	memset(LogCalls, 0, sizeof(LogCalls));
}

static void Write_Draw_Log(int frame)
{
	Buf b;
	char name[MAX_PATH];
	FILE *fp;
	int i;
	memset(&b, 0, sizeof(b));
	Buf_Add(&b, "{\"format\":\"d3d8shot-drawlog 1\",\"shot\":\"%s\",\"frame\":%d,\n", TriggerName, frame);
	Buf_Add(&b, "\"counts\":{\"draws\":%d,\"DIP\":%d,\"DP\":%d,\"DPUP\":%d,\"DIPUP\":%d,\"views\":%d,\"projections\":%d,"
		"\"textures\":%d,\"lost\":%d},\n", LogDrawCount, LogCalls[0], LogCalls[1], LogCalls[2], LogCalls[3],
		LogViewCount, LogProjCount, LogTexCount, LogLost);
	Buf_Add(&b, "\"render_states\":[");
	for (i = 0; i < (int)(sizeof(LogStateNames) / sizeof(LogStateNames[0])); i++) Buf_Add(&b, i ? ",\"%s\"" : "\"%s\"", LogStateNames[i]);
	Buf_Add(&b, "],\n\"stage_states\":[");
	for (i = 0; i < (int)(sizeof(LogStageStateNames) / sizeof(LogStageStateNames[0])); i++) Buf_Add(&b, i ? ",\"%s\"" : "\"%s\"", LogStageStateNames[i]);
	Buf_Add(&b, "],\n\"textures\":[\n");
	for (i = 0; i < LogTexCount; i++) {
		LogTex *t = &LogTexs[i];
		Buf_Add(&b, "%s{\"id\":%d,\"type\":%u,\"w\":%u,\"h\":%u,\"format\":%u,\"levels\":%u,\"usage\":%lu,\"pool\":%u,\"hash\":",
			i ? ",\n" : "", i, t->Type, t->Width, t->Height, t->Format, t->Levels, t->Usage, t->Pool);
		if (t->Hashed) Buf_Add(&b, "\"%016I64x\"}", t->Hash);
		else Buf_Add(&b, "null}");
	}
	Buf_Add(&b, "],\n\"views\":[\n");
	for (i = 0; i < LogViewCount; i++) { if (i) Buf_Add(&b, ",\n"); Buf_Floats(&b, LogViews[i], 16); }
	Buf_Add(&b, "],\n\"projections\":[\n");
	for (i = 0; i < LogProjCount; i++) { if (i) Buf_Add(&b, ",\n"); Buf_Floats(&b, LogProjs[i], 16); }
	Buf_Add(&b, "],\n\"sets\":[\n%s],\n\"draws\":[\n%s]}\n", LogSets.Text ? LogSets.Text : "",
		LogDraws.Text ? LogDraws.Text : "");
	_snprintf(name, sizeof(name) - 1, "%s\\shot_%s_%05d.json", Dir, TriggerName, frame);
	name[sizeof(name) - 1] = 0;
	fp = b.Text ? fopen(name, "wb") : NULL;
	if (fp) {
		fwrite(b.Text, b.Len, 1, fp);
		fclose(fp);
	}
	Log("frame %d: draw log %s: %d draws (DIP %d, DP %d, DPUP %d, DIPUP %d), %d textures, %d views, %d projections, "
		"%d lost, %d bytes\n", frame, fp ? name : "NOT WRITTEN", LogDrawCount, LogCalls[0], LogCalls[1], LogCalls[2],
		LogCalls[3], LogTexCount, LogViewCount, LogProjCount, LogLost, b.Len);
	free(b.Text);
	LogFrame = 0;
}

#define LOGGING()	(LogFrame && Presented + 1 == LogFrame)

static HRESULT STDMETHODCALLTYPE Hook_Draw_Indexed(void *dev, UINT type, UINT minidx, UINT count, UINT start, UINT prims)
{
	int frame = Presented + 1;
	if (LOGGING()) Log_Draw(dev, 0, type, prims, count, 0);
	if (Dumping(frame)) {
		Shadow *vs, *is;
		UINT head[10], n, i, isz;
		Query_State(dev);
		vs = Find_Shadow(Stream0, 0);
		is = Find_Shadow(Indices, 0);
		isz = (is && is->Index32) ? 4 : 2;
		Open_Dump(frame);
		memcpy(head, "DIP2", 4);
		head[1] = type; head[2] = minidx; head[3] = count; head[4] = start; head[5] = prims;
		head[6] = BaseVertex; head[7] = Stride0; head[8] = Shader; head[9] = isz;
		fwrite(head, sizeof(head), 1, DumpFile);
		Write_Texture(dev, Textures[0]);
		Write_Texture(dev, Textures[1]);
		fwrite(Matrices, sizeof(Matrices), 1, DumpFile);
		for (i = 0; i < sizeof(DumpStates) / sizeof(DumpStates[0]); i++) {
			DWORD v = 0;
			((GetRenderStateType)VT(dev, SLOT_GETRENDERSTATE))(dev, DumpStates[i], &v);
			fwrite(&v, 4, 1, DumpFile);
		}
		for (n = 0; n < 2; n++) {
			for (i = 0; i < sizeof(DumpStageStates) / sizeof(DumpStageStates[0]); i++) {
				DWORD v = 0;
				((GetStageStateType)VT(dev, SLOT_GETSTAGESTATE))(dev, n, DumpStageStates[i], &v);
				fwrite(&v, 4, 1, DumpFile);
			}
		}
		n = type == 4 ? prims * 3 : type == 5 || type == 6 ? prims + 2 : type == 2 ? prims * 2 : prims;
		fwrite(&n, 4, 1, DumpFile);
		for (i = 0; i < n; i++) {
			UINT v = 0, at = (start + i) * isz;
			if (is && is->Data && at + isz <= is->Size) {
				v = isz == 4 ? *(UINT *)(is->Data + at) : *(unsigned short *)(is->Data + at);
			}
			fwrite(&v, 4, 1, DumpFile);
		}
		for (i = 0; i < count; i++) {
			unsigned char zero[256];
			UINT at = (BaseVertex + minidx + i) * Stride0;
			memset(zero, 0, sizeof(zero));
			if (vs && vs->Data && Stride0 <= 256 && at + Stride0 <= vs->Size) fwrite(vs->Data + at, Stride0, 1, DumpFile);
			else fwrite(zero, Stride0 <= 256 ? Stride0 : 256, 1, DumpFile);
		}
		Log_Lights(dev, DumpDraws);
		DumpDraws++;
	} else if (DumpFile) {
		Close_Dump();
	}
	return RealDrawIndexed(dev, type, minidx, count, start, prims);
}

static HRESULT STDMETHODCALLTYPE Hook_Draw_Primitive(void *dev, UINT type, UINT start, UINT prims)
{
	if (LOGGING()) Log_Draw(dev, 1, type, prims, Prim_Verts(type, prims), 0);
	if (Dumping(Presented + 1)) { Open_Dump(Presented + 1); DumpOther++; }
	return RealDrawPrimitive(dev, type, start, prims);
}

static HRESULT STDMETHODCALLTYPE Hook_Draw_Primitive_UP(void *dev, UINT type, UINT prims, const void *data, UINT stride)
{
	if (LOGGING()) Log_Draw(dev, 2, type, prims, Prim_Verts(type, prims), stride);
	if (Dumping(Presented + 1)) { Open_Dump(Presented + 1); DumpOther++; }
	return RealDrawPrimitiveUP(dev, type, prims, data, stride);
}

static HRESULT STDMETHODCALLTYPE Hook_Draw_Indexed_UP(void *dev, UINT type, UINT minidx, UINT count, UINT prims,
	const void *idx, UINT fmt, const void *data, UINT stride)
{
	if (LOGGING()) Log_Draw(dev, 3, type, prims, count, stride);
	if (Dumping(Presented + 1)) { Open_Dump(Presented + 1); DumpOther++; }
	return RealDrawIndexedUP(dev, type, minidx, count, prims, idx, fmt, data, stride);
}

#define HOOK(dev, slot, real, type, hook) \
	if (VT(dev, slot) != (void *)(hook)) { real = (type)VT(dev, slot); Patch(&VT(dev, slot), (void *)(hook)); }

static void Hook_Dump(void *dev)
{
	HOOK(dev, SLOT_SETTRANSFORM, RealSetTransform, SetTransformType, Hook_Set_Transform);
	HOOK(dev, SLOT_SETTEXTURE, RealSetTexture, SetTextureType, Hook_Set_Texture);
	HOOK(dev, SLOT_DRAWPRIMITIVE, RealDrawPrimitive, DrawPrimitiveType, Hook_Draw_Primitive);
	HOOK(dev, SLOT_DRAWINDEXED, RealDrawIndexed, DrawIndexedType, Hook_Draw_Indexed);
	HOOK(dev, SLOT_DRAWPRIMITIVEUP, RealDrawPrimitiveUP, DrawPrimitiveUPType, Hook_Draw_Primitive_UP);
	HOOK(dev, SLOT_DRAWINDEXEDUP, RealDrawIndexedUP, DrawIndexedUPType, Hook_Draw_Indexed_UP);
	HOOK(dev, SLOT_SETVERTEXSHADER, RealSetVertexShader, SetVertexShaderType, Hook_Set_Vertex_Shader);
	HOOK(dev, SLOT_SETSTREAMSOURCE, RealSetStreamSource, SetStreamSourceType, Hook_Set_Stream_Source);
	HOOK(dev, SLOT_SETINDICES, RealSetIndices, SetIndicesType, Hook_Set_Indices);
	HOOK(dev, SLOT_CREATEVB, RealCreateVB, CreateBufferType, Hook_Create_VB);
	HOOK(dev, SLOT_CREATEIB, RealCreateIB, CreateBufferType, Hook_Create_IB);
	if (!Presented) Log("draw dump hooks installed (%d frames)\n", DumpCount);
}

/* the draw log's hooks (D3D8SHOT_DRAWLOG): the draw calls and SetTransform, no buffer shadows */
static void Hook_Draw_Log(void *dev)
{
	HOOK(dev, SLOT_SETTRANSFORM, RealSetTransform, SetTransformType, Hook_Set_Transform);
	HOOK(dev, SLOT_DRAWPRIMITIVE, RealDrawPrimitive, DrawPrimitiveType, Hook_Draw_Primitive);
	HOOK(dev, SLOT_DRAWINDEXED, RealDrawIndexed, DrawIndexedType, Hook_Draw_Indexed);
	HOOK(dev, SLOT_DRAWPRIMITIVEUP, RealDrawPrimitiveUP, DrawPrimitiveUPType, Hook_Draw_Primitive_UP);
	HOOK(dev, SLOT_DRAWINDEXEDUP, RealDrawIndexedUP, DrawIndexedUPType, Hook_Draw_Indexed_UP);
}

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
	if (TriggerName[0]) {
		_snprintf(name, sizeof(name) - 1, "%s\\shot_%s_%05d.bmp", Dir, TriggerName, frame);
	} else {
		_snprintf(name, sizeof(name) - 1, "%s\\frame_%05d.bmp", Dir, frame);
	}
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
	if (Camera[0]) {
		HOOK(dev, SLOT_SETTRANSFORM, RealSetTransform, SetTransformType, Hook_Set_Transform);
		if ((Presented & 1) == 0 && ViewCount > 0) {
			FILE *c = fopen(CameraTmp, "w");
			if (c) {
				int i, j;
				for (i = 0; i < ViewCount; i++) {
					fprintf(c, "frame %d view %d of %d:", Presented, i, ViewSets);
					for (j = 0; j < 16; j++) fprintf(c, " %.5f", Views[i][j]);
					fprintf(c, "\n");
				}
				fclose(c);
				MoveFileExA(CameraTmp, Camera, MOVEFILE_REPLACE_EXISTING);
			}
		}
		ViewCount = 0;
		ViewSets = 0;
	}
	if (DumpFile && !Dumping(Presented + 1)) Close_Dump();
	if (DumpCount > 0) Hook_Dump(dev);		/* again: the runtime rewrites slots of its vtable copy */
	if (DrawLog) Hook_Draw_Log(dev);
	if (Wanted(Presented)) {
		Save_Back_Buffer(dev, Presented);
	}
	if (LogFrame && LogFrame <= Presented) {	/* the logged frame is the one being presented: its log, then its shot */
		Write_Draw_Log(Presented);
		Save_Back_Buffer(dev, Presented);
		DeleteFileA(Trigger);
		TriggerName[0] = 0;
		return RealPresent(dev, src, dst, wnd, dirty);
	}
	if (Trigger[0] && !LogFrame && (Presented & 7) == 0 && GetFileAttributesA(Trigger) != (DWORD)-1) {
		FILE *t = fopen(Trigger, "r");
		int i;
		TriggerName[0] = 0;
		if (t) {
			if (!fgets(TriggerName, sizeof(TriggerName), t)) TriggerName[0] = 0;
			fclose(t);
		}
		if (!t || !TriggerName[0] || TriggerName[0] == '\r' || TriggerName[0] == '\n') {
			/* the file is there but not readable yet (just renamed into place by e2e's os.replace, or held open by
			   a scanner): read it again at a later Present. Taken now it was saved as shot_shot_N, its name lost
			   (2026-11-25: sb_rooms' room1_npc on retail); e2e writes every trigger with its name */
			TriggerName[0] = 0;
			return RealPresent(dev, src, dst, wnd, dirty);
		}
		if (!strncmp(TriggerName, "mark ", 5)) {
			/* a marker, not a shot: logged here, and passed to the program's own Stub_Log_Mark when it exports one
			   (stubs/stub_log.cpp: the stand-ins' first-call flags start over, so each part of a session lists
			   the stand-ins it hits); the retail client has none */
			typedef void (__cdecl *MarkType)(const char *);
			MarkType mark = (MarkType)GetProcAddress(GetModuleHandleA(NULL), "Stub_Log_Mark");
			for (i = 5; TriggerName[i] && TriggerName[i] != '\r' && TriggerName[i] != '\n'; i++) ;
			TriggerName[i] = 0;
			DeleteFileA(Trigger);
			Log("frame %d: mark %s%s\n", Presented, TriggerName + 5, mark ? "" : " (no Stub_Log_Mark export)");
			if (mark) mark(TriggerName + 5);
			TriggerName[0] = 0;
			return RealPresent(dev, src, dst, wnd, dirty);
		}
		for (i = 0; TriggerName[i]; i++) {		/* a file name: letters, digits, '-' and '_' only */
			char c = TriggerName[i];
			if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_')) {
				TriggerName[i] = 0;
				break;
			}
		}
		if (!TriggerName[0]) strcpy(TriggerName, "shot");
		if (DumpCount > 0 && DumpCount < 64 && !strncmp(TriggerName, "dump_", 5)) {
			Dumps[DumpCount++] = Presented + 1 + DrawLog;		/* the frame after the shot */
			Log("frame %d: dumping frame %d\n", Presented, Presented + 1 + DrawLog);
		}
		if (DrawLog) {			/* the next frame is logged and saved; the trigger file stays until then */
			Log("frame %d: trigger %s (draw log: frame %d)\n", Presented, TriggerName, Presented + 1);
			Start_Draw_Log(Presented + 1);
			return RealPresent(dev, src, dst, wnd, dirty);
		}
		DeleteFileA(Trigger);
		Log("frame %d: trigger %s\n", Presented, TriggerName);
		Save_Back_Buffer(dev, Presented);
		TriggerName[0] = 0;
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
		if (DumpCount > 0) Hook_Dump(*device);
		if (DrawLog) Hook_Draw_Log(*device);
	}
	return hr;
}

/* D3D8SHOT_NOREGISTER: no COM self-registration from a test run */
typedef FARPROC (WINAPI *GetProcAddressType)(HMODULE, LPCSTR);
static GetProcAddressType RealGetProcAddress = NULL;
static HRESULT WINAPI No_Register_Server(void)
{
	Log("DllRegisterServer: not called (D3D8SHOT_NOREGISTER); S_OK returned\n");
	return 0;
}
static FARPROC WINAPI Hook_GetProcAddress(HMODULE module, LPCSTR name)
{
	if (((DWORD)name >> 16) != 0 && strcmp(name, "DllRegisterServer") == 0) {		/* a name, not an ordinal */
		char path[MAX_PATH];
		path[0] = 0;
		GetModuleFileNameA(module, path, sizeof(path));
		Log("GetProcAddress(%s, DllRegisterServer): the no-op given\n", path);
		return (FARPROC)No_Register_Server;
	}
	return RealGetProcAddress(module, name);
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
	char v[8];
	int noregister = GetEnvironmentVariableA("D3D8SHOT_NOREGISTER", v, sizeof(v)) && v[0] == '1';
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
			if (noregister && strcmp((const char *)ibn->Name, "GetProcAddress") == 0) {
				RealGetProcAddress = (GetProcAddressType)funcs->u1.Function;
				Patch((void **)&funcs->u1.Function, (void *)Hook_GetProcAddress);
				Log("hooked the program's GetProcAddress import (DllRegisterServer gets a no-op)\n");
			}
		}
	}
}


/* Virtual input, for the retail client and for the project's own builds alike (the exe carries no test code):
   with D3D8SHOT_INPUT=1 the program's user32 imports read the messages a harness posts, never the real cursor
   or keyboard (an unattended run must not touch the desktop) */
static POINT In_Cursor = { 0, 0 };
static HWND In_Window = NULL;
static BYTE In_Keys[256];
typedef BOOL (WINAPI *GetMessageType)(LPMSG, HWND, UINT, UINT);
typedef BOOL (WINAPI *PeekMessageType)(LPMSG, HWND, UINT, UINT, UINT);
static GetMessageType Real_GetMessageA = NULL;
static PeekMessageType Real_PeekMessageA = NULL;
static void In_Screen(POINT *p) { *p = In_Cursor; if (In_Window) ClientToScreen(In_Window, p); }
/* The harness's tag (tools/harness/clientrun.py post_step(tag=True), e2e.py): IN_TAG in the high word of a mouse
   or key message's wParam (MK_ flags and virtual keys fit the low word), stripped here before the program sees
   the message. With D3D8SHOT_INPUT_TAG=1 an untagged mouse or key message (a person's click or keystroke in the
   test window, or the real cursor crossing it) is logged and dropped, so only replayed input reaches the program. */
#define IN_TAG 0x5043
static int In_Tag_Only = 0;
/* A keyboard's Shift, Ctrl and Alt come as VK_SHIFT / VK_CONTROL / VK_MENU; GetKeyState also knows which side:
   right Shift by its scan code (0x36), right Ctrl and Alt by lParam's extended bit (24). client.exe asks only the
   generic three (0x4307A0, 0x430CA0, 0x4314C0); the sides are kept as a keyboard would for any other program. */
static void In_Side(LPMSG msg, BYTE down)
{
	int k = (int)(msg->wParam & 0xFF), side;
	if (k == VK_SHIFT) side = ((msg->lParam >> 16) & 0xFF) == 0x36 ? VK_RSHIFT : VK_LSHIFT;
	else if (k == VK_CONTROL) side = (msg->lParam & (1 << 24)) ? VK_RCONTROL : VK_LCONTROL;
	else if (k == VK_MENU) side = (msg->lParam & (1 << 24)) ? VK_RMENU : VK_LMENU;
	else return;
	if (down && !(In_Keys[side] & 0x80)) In_Keys[side] ^= 0x01;
	In_Keys[side] = (BYTE)((In_Keys[side] & ~0x80) | down);
}
static void In_Take(LPMSG msg)
{
	POINT p;
	const char *who = "untagged";
	int input;
	if (msg->hwnd == NULL) return;
	input = (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MBUTTONDBLCLK) || msg->message == WM_KEYDOWN
		|| msg->message == WM_KEYUP || msg->message == WM_SYSKEYDOWN || msg->message == WM_SYSKEYUP;
	if (input && HIWORD(msg->wParam) == IN_TAG) {
		msg->wParam &= 0xFFFF;
		who = "replayed";
	} else if (input && In_Tag_Only) {
		if (msg->message != WM_MOUSEMOVE) {
			Log("input: NOT REPLAYED (dropped) 0x%X wparam 0x%X lparam 0x%X\n", msg->message, (unsigned)msg->wParam,
				(unsigned)msg->lParam);
		}
		msg->message = WM_NULL;
		return;
	}
	if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) {
		/* the modifiers down at this key (test-plan C6: what the program's GetKeyState / GetAsyncKeyState answer
		   while it handles the key), and sys for WM_SYSKEYDOWN; nothing added for a plain key (the scenarios
		   match '^input: replayed key 0xVK') */
		unsigned k = (unsigned)msg->wParam & 0xFF;
		Log("input: %s key 0x%X%s%s%s%s%s\n", who, k, (In_Keys[VK_SHIFT] & 0x80) && k != VK_SHIFT ? " +shift" : "",
			(In_Keys[VK_CONTROL] & 0x80) && k != VK_CONTROL ? " +ctrl" : "",
			(In_Keys[VK_MENU] & 0x80) && k != VK_MENU ? " +alt" : "", msg->message == WM_SYSKEYDOWN ? " sys" : "",
			(msg->lParam & 0x40000000) ? " repeat" : "");
	} else if (msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP) {
		Log("input: %s key-up 0x%X%s\n", who, (unsigned)msg->wParam & 0xFF, msg->message == WM_SYSKEYUP ? " sys" : "");
	}
	if ((msg->message >= WM_MOUSEFIRST && msg->message <= WM_MBUTTONDBLCLK) || msg->message == WM_KEYDOWN
		|| msg->message == WM_CHAR) {
		if (In_Window != msg->hwnd) Log("input: window %p\n", msg->hwnd);
		In_Window = msg->hwnd;		/* the harness posts to the main window */
	}
	if (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MBUTTONDBLCLK) {
		In_Cursor.x = (short)LOWORD(msg->lParam);
		In_Cursor.y = (short)HIWORD(msg->lParam);
		if (msg->message == WM_LBUTTONDOWN) In_Keys[VK_LBUTTON] |= 0x80;
		if (msg->message == WM_LBUTTONUP) In_Keys[VK_LBUTTON] &= ~0x80;
		if (msg->message == WM_RBUTTONDOWN) In_Keys[VK_RBUTTON] |= 0x80;
		if (msg->message == WM_RBUTTONUP) In_Keys[VK_RBUTTON] &= ~0x80;
		if (msg->message != WM_MOUSEMOVE) Log("input: %s mouse 0x%X at %d, %d\n", who, msg->message, In_Cursor.x, In_Cursor.y);
	} else if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) {
		if (!(In_Keys[msg->wParam & 0xFF] & 0x80)) In_Keys[msg->wParam & 0xFF] ^= 0x01;
		In_Keys[msg->wParam & 0xFF] |= 0x80;
		In_Side(msg, 0x80);
	} else if (msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP) {
		In_Keys[msg->wParam & 0xFF] &= ~0x80;
		In_Side(msg, 0);
	}
	In_Screen(&p);
	msg->pt = p;
}
static BOOL WINAPI In_GetMessageA(LPMSG msg, HWND w, UINT a, UINT b)
{
	BOOL r = Real_GetMessageA(msg, w, a, b);
	if (r > 0) In_Take(msg);
	return r;
}
static BOOL WINAPI In_PeekMessageA(LPMSG msg, HWND w, UINT a, UINT b, UINT f)
{
	BOOL r = Real_PeekMessageA(msg, w, a, b, f);
	if (r && (f & PM_REMOVE)) In_Take(msg);
	return r;
}
static BOOL WINAPI In_GetCursorPos(LPPOINT p) { In_Screen(p); return TRUE; }
static DWORD WINAPI In_GetMessagePos(void) { POINT p; In_Screen(&p); return MAKELONG((short)p.x, (short)p.y); }
static BOOL WINAPI In_SetCursorPos(int x, int y)
{
	POINT p; p.x = x; p.y = y;
	if (In_Window) ScreenToClient(In_Window, &p);
	In_Cursor = p;
	return TRUE;
}
static HWND WINAPI In_GetForegroundWindow(void) { return In_Window; }
static HWND WINAPI In_GetActiveWindow(void) { return In_Window; }
static SHORT WINAPI In_GetKeyState(int k) { BYTE s = In_Keys[k & 0xFF]; return (SHORT)(((s & 0x80) ? 0x8000 : 0) | (s & 1)); }
static SHORT WINAPI In_GetAsyncKeyState(int k) { return (SHORT)((In_Keys[k & 0xFF] & 0x80) ? 0x8000 : 0); }
static BOOL WINAPI In_ClipCursor(const RECT *r) { return TRUE; }
static BOOL WINAPI In_NoSetCursorPos(int x, int y) { return TRUE; }

static void Hook_Input(void)
{
	char v[8];
	unsigned char *base = (unsigned char *)GetModuleHandleA(NULL);
	IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
	IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	IMAGE_IMPORT_DESCRIPTOR *imp;
	int hooked = 0, guard = 0;
	/* D3D8SHOT_INPUT=1: the virtual cursor and keyboard. D3D8SHOT_CURSOR=off (without it): only SetCursorPos and
	   ClipCursor become no-ops, so several clients side by side do not fight over the real cursor (the client
	   warps it back each frame, 0x431C30) */
	if (!GetEnvironmentVariableA("D3D8SHOT_INPUT", v, sizeof(v)) || v[0] != '1') {
		if (!GetEnvironmentVariableA("D3D8SHOT_CURSOR", v, sizeof(v)) || stricmp(v, "off") != 0) return;
		guard = 1;
	}
	In_Tag_Only = GetEnvironmentVariableA("D3D8SHOT_INPUT_TAG", v, sizeof(v)) && v[0] == '1';
	if (!dir->VirtualAddress) return;
	for (imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir->VirtualAddress); imp->Name; imp++) {
		IMAGE_THUNK_DATA *names, *funcs;
		if (stricmp((const char *)(base + imp->Name), "user32.dll") != 0) continue;
		if (!imp->OriginalFirstThunk) continue;
		names = (IMAGE_THUNK_DATA *)(base + imp->OriginalFirstThunk);
		funcs = (IMAGE_THUNK_DATA *)(base + imp->FirstThunk);
		for (; names->u1.AddressOfData; names++, funcs++) {
			const char *n;
			void *to = NULL;
			if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
			n = (const char *)((IMAGE_IMPORT_BY_NAME *)(base + (DWORD)names->u1.AddressOfData))->Name;
			if (guard) {
				if (!strcmp(n, "SetCursorPos")) to = (void *)In_NoSetCursorPos;
				else if (!strcmp(n, "ClipCursor")) to = (void *)In_ClipCursor;
			}
			else if (!strcmp(n, "GetMessageA")) { Real_GetMessageA = (GetMessageType)funcs->u1.Function; to = (void *)In_GetMessageA; }
			else if (!strcmp(n, "PeekMessageA")) { Real_PeekMessageA = (PeekMessageType)funcs->u1.Function; to = (void *)In_PeekMessageA; }
			else if (!strcmp(n, "GetCursorPos")) to = (void *)In_GetCursorPos;
			else if (!strcmp(n, "GetMessagePos")) to = (void *)In_GetMessagePos;
			else if (!strcmp(n, "SetCursorPos")) to = (void *)In_SetCursorPos;
			else if (!strcmp(n, "GetForegroundWindow")) to = (void *)In_GetForegroundWindow;
			else if (!strcmp(n, "GetActiveWindow")) to = (void *)In_GetActiveWindow;
			else if (!strcmp(n, "GetKeyState")) to = (void *)In_GetKeyState;
			else if (!strcmp(n, "GetAsyncKeyState")) to = (void *)In_GetAsyncKeyState;
			else if (!strcmp(n, "ClipCursor")) to = (void *)In_ClipCursor;
			if (to) { Patch((void **)&funcs->u1.Function, to); Log("input: hooked %s\n", n); hooked++; }
		}
	}
	if (guard) Log("input: cursor guard, %d user32 imports redirected\n", hooked);
	else Log("input: virtual input, %d user32 imports redirected; tagged input read%s\n", hooked,
		In_Tag_Only ? ", untagged dropped" : "");
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
	if (!GetEnvironmentVariableA("D3D8SHOT_TRIGGER", Trigger, sizeof(Trigger))) Trigger[0] = 0;
	if (!GetEnvironmentVariableA("D3D8SHOT_CAMERA", Camera, sizeof(Camera))) Camera[0] = 0;
	if (Camera[0]) { strcpy(CameraTmp, Camera); strcat(CameraTmp, ".tmp"); }
	if (GetEnvironmentVariableA("D3D8SHOT_DUMP", frames, sizeof(frames))) {
		for (p = strtok(frames, ","); p && DumpCount < 64; p = strtok(NULL, ",")) {
			Dumps[DumpCount++] = atoi(p);
		}
	}
	DrawLog = GetEnvironmentVariableA("D3D8SHOT_DRAWLOG", frames, sizeof(frames)) && frames[0] == '1';
	if (DrawLog) Log("draw log on (D3D8SHOT_DRAWLOG): a trigger shot is taken one frame later, with its JSON\n");
	GetSystemDirectoryA(path, sizeof(path));
	strcat(path, "\\d3d8.dll");
	Real = LoadLibraryA(path);
	if (Real) RealCreate = (CreateType)GetProcAddress(Real, "Direct3DCreate8");
	Log("d3d8shot: real %s = %p, Direct3DCreate8 = %p; every %d, %d listed frames, max %d\n", path, Real,
		RealCreate, Every, FrameCount, Max);
	Hook_Imports();
	Hook_Input();
}

/* Safety (D69), installed at DLL_PROCESS_ATTACH (the program imports d3d8.dll, so before its WinMain): the
   program's DialogBoxParamA and MessageBoxA imports open nothing (a dialog answers IDOK, logged: the client's
   EULA / ROC text 0x847D20 and -MENU 0x846C90 dialogs go on; a message box's text is logged), unless
   D3D8SHOT_DIALOGS=1. And a first-chance vectored handler writes one 'CRASH:' line (the code, the faulting
   address, the exe's code addresses on the stack) and one 'CRASH regs:' line to d3d8shot.log before the
   program's own filter runs: tools/harness/symbolize.py names them with the exe's map. It only logs. */
typedef int (WINAPI *DialogBoxParamType)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
static int WINAPI Safe_DialogBoxParamA(HINSTANCE inst, LPCSTR tmpl, HWND parent, DLGPROC proc, LPARAM param)
{
	if (!Real) Init();
	if (((DWORD)tmpl >> 16) == 0) Log("DialogBoxParamA(resource %lu): not shown (D69), IDOK returned\n", (DWORD)tmpl);
	else Log("DialogBoxParamA(%s): not shown (D69), IDOK returned\n", tmpl);
	return IDOK;
}
static int WINAPI Safe_MessageBoxA(HWND wnd, LPCSTR text, LPCSTR caption, UINT type)
{
	if (!Real) Init();
	Log("MessageBoxA(\"%s\", \"%s\"): not shown (D69), IDOK returned\n", caption ? caption : "", text ? text : "");
	return IDOK;
}

static DWORD Exe_Lo = 0, Exe_Hi = 0;
static LONG CALLBACK Safe_Crash_Log(EXCEPTION_POINTERS *info)
{
	static int count = 0;
	DWORD code = info->ExceptionRecord->ExceptionCode;
	CONTEXT *c = info->ContextRecord;
	char line[1024];
	int n, i, found = 0, r;
	unsigned *sp;
	const char *names[7] = { "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp" };
	DWORD regs[7];
	if ((code != EXCEPTION_ACCESS_VIOLATION && code != EXCEPTION_ILLEGAL_INSTRUCTION && code != EXCEPTION_INT_DIVIDE_BY_ZERO
			&& code != EXCEPTION_STACK_OVERFLOW && code != EXCEPTION_PRIV_INSTRUCTION) || count >= 4 || !LogFile) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	count++;
	n = sprintf(line, "CRASH: code %08lX at %08lX (address %08lX) stack:", code, (DWORD)c->Eip,
		code == EXCEPTION_ACCESS_VIOLATION ? (DWORD)info->ExceptionRecord->ExceptionInformation[1] : 0);
	sp = (unsigned *)c->Esp;
	for (i = 0; i < 2048 && found < 24 && n < (int)sizeof(line) - 12; i++) {
		if (IsBadReadPtr(sp + i, 4)) break;
		if (sp[i] >= Exe_Lo && sp[i] < Exe_Hi) { n += sprintf(line + n, " %08X", sp[i]); found++; }
	}
	Log("%s\n", line);
	regs[0] = c->Eax; regs[1] = c->Ebx; regs[2] = c->Ecx; regs[3] = c->Edx; regs[4] = c->Esi; regs[5] = c->Edi; regs[6] = c->Ebp;
	n = sprintf(line, "CRASH regs:");
	for (r = 0; r < 7; r++) {
		unsigned *p = (unsigned *)regs[r];
		n += sprintf(line + n, " %s=%08lX", names[r], regs[r]);
		if (regs[r] >= 0x10000 && !IsBadReadPtr(p, 16)) n += sprintf(line + n, "[%08X %08X %08X %08X]", p[0], p[1], p[2], p[3]);
	}
	Log("%s\n", line);
	fflush(LogFile);
	return EXCEPTION_CONTINUE_SEARCH;
}

typedef PVOID (WINAPI *AddVectoredHandlerType)(ULONG, LONG (CALLBACK *)(EXCEPTION_POINTERS *));
static void Hook_Safety(void)
{
	char v[8];
	unsigned char *base = (unsigned char *)GetModuleHandleA(NULL);
	IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
	IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	IMAGE_IMPORT_DESCRIPTOR *imp;
	AddVectoredHandlerType add;
	Exe_Lo = (DWORD)base + nt->OptionalHeader.BaseOfCode;
	Exe_Hi = Exe_Lo + nt->OptionalHeader.SizeOfCode;
	add = (AddVectoredHandlerType)GetProcAddress(GetModuleHandleA("kernel32.dll"), "AddVectoredExceptionHandler");
	if (add) add(1, Safe_Crash_Log);
	if (GetEnvironmentVariableA("D3D8SHOT_DIALOGS", v, sizeof(v)) && v[0] == '1') return;
	if (!dir->VirtualAddress) return;
	for (imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir->VirtualAddress); imp->Name; imp++) {
		IMAGE_THUNK_DATA *names, *funcs;
		if (stricmp((const char *)(base + imp->Name), "user32.dll") != 0) continue;
		if (!imp->OriginalFirstThunk) continue;
		names = (IMAGE_THUNK_DATA *)(base + imp->OriginalFirstThunk);
		funcs = (IMAGE_THUNK_DATA *)(base + imp->FirstThunk);
		for (; names->u1.AddressOfData; names++, funcs++) {
			const char *n;
			if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
			n = (const char *)((IMAGE_IMPORT_BY_NAME *)(base + (DWORD)names->u1.AddressOfData))->Name;
			if (!strcmp(n, "DialogBoxParamA")) Patch((void **)&funcs->u1.Function, (void *)Safe_DialogBoxParamA);
			else if (!strcmp(n, "MessageBoxA")) Patch((void **)&funcs->u1.Function, (void *)Safe_MessageBoxA);
		}
	}
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

/* The frame being drawn, numbered as the shots are (shot_NAME_<frame>: Presented + 1, the frame whose draws a
** draw log records): the mss32log proxy (mss32log.c) reads it for each audio event's "f". Exported by name
** (run_reference.py's .def). */
unsigned long WINAPI D3d8shot_Frame(void)
{
	return (unsigned long)(Presented + 1);
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH) {
		/* Stay loaded: the hooked vtable slots point here, and a program may free its d3d8.dll
		   (WW3D::Shutdown) while D3D objects are still used */
		char self[MAX_PATH];
		if (GetModuleFileNameA(inst, self, sizeof(self))) LoadLibraryA(self);
		Hook_Safety();
	}
	if (reason == DLL_PROCESS_DETACH && LogFile) {
		Close_Dump();
		Log("detach: %d frames presented, %d saved\n", Presented, Saved);
		fclose(LogFile);
		LogFile = NULL;
	}
	return TRUE;
}
