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
**   D3D8SHOT_DUMP   dump every draw call of these frames, comma-separated (M2.4: the land's geometry)
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

static HRESULT STDMETHODCALLTYPE Hook_Set_Transform(void *dev, DWORD state, const float *m)
{
	int k = state == 256 ? 0 : state == 2 ? 1 : state == 3 ? 2 : -1;
	if (k >= 0 && m) memcpy(Matrices[k], m, sizeof(Matrices[k]));
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

static HRESULT STDMETHODCALLTYPE Hook_Draw_Indexed(void *dev, UINT type, UINT minidx, UINT count, UINT start, UINT prims)
{
	int frame = Presented + 1;
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
		DumpDraws++;
	} else if (DumpFile) {
		Close_Dump();
	}
	return RealDrawIndexed(dev, type, minidx, count, start, prims);
}

static HRESULT STDMETHODCALLTYPE Hook_Draw_Primitive(void *dev, UINT type, UINT start, UINT prims)
{
	if (Dumping(Presented + 1)) { Open_Dump(Presented + 1); DumpOther++; }
	return RealDrawPrimitive(dev, type, start, prims);
}

static HRESULT STDMETHODCALLTYPE Hook_Draw_Primitive_UP(void *dev, UINT type, UINT prims, const void *data, UINT stride)
{
	if (Dumping(Presented + 1)) { Open_Dump(Presented + 1); DumpOther++; }
	return RealDrawPrimitiveUP(dev, type, prims, data, stride);
}

static HRESULT STDMETHODCALLTYPE Hook_Draw_Indexed_UP(void *dev, UINT type, UINT minidx, UINT count, UINT prims,
	const void *idx, UINT fmt, const void *data, UINT stride)
{
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
	if (DumpFile && !Dumping(Presented + 1)) Close_Dump();
	if (DumpCount > 0) Hook_Dump(dev);		/* again: the runtime rewrites slots of its vtable copy */
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
		if (DumpCount > 0) Hook_Dump(*device);
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
	if (GetEnvironmentVariableA("D3D8SHOT_DUMP", frames, sizeof(frames))) {
		for (p = strtok(frames, ","); p && DumpCount < 64; p = strtok(NULL, ",")) {
			Dumps[DumpCount++] = atoi(p);
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
	if (reason == DLL_PROCESS_ATTACH) {
		/* Stay loaded: the hooked vtable slots point here, and a program may free its d3d8.dll
		   (WW3D::Shutdown) while D3D objects are still used */
		char self[MAX_PATH];
		if (GetModuleFileNameA(inst, self, sizeof(self))) LoadLibraryA(self);
	}
	if (reason == DLL_PROCESS_DETACH && LogFile) {
		Close_Dump();
		Log("detach: %d frames presented, %d saved\n", Presented, Saved);
		fclose(LogFile);
		LogFile = NULL;
	}
	return TRUE;
}
