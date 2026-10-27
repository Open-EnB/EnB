// Earth & Beyond client (reconstruction): app\shared\ height field (file name unknown). Notes at the end.
#include "heightfield.h"
#include "ini.h"
#include "ffactory.h"
#include "wwfile.h"
#include "wwmath.h"
#include "vector2.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pot.h"
#include "coltest.h"
#include "colmath.h"
#include "tri.h"

HeightFieldClass::HeightFieldClass(void) :
	Heights(NULL),
	Buffer(NULL),
	MaxValue(0),
	XSpacing(1.0f),
	YSpacing(1.0f),
	VScale(1.0f),
	Wrapping(false),
	Log2(0),
	Size(0),
	Count(0),
	Mask(0),
	CountMask(0),
	HalfSize(0)
{
}

// NOT EXACT: order-only (the client loads Size before Count - 1 and copies it for HalfSize), parked
HeightFieldClass *HeightFieldClass::Load(const char *ini_name, unsigned int max_size)
{
	INIClass ini;
	if (!ini.Load(ini_name)) {
		return NULL;
	}
	HeightFieldClass *field = new HeightFieldClass;
	ini.Get_String("HEIGHTMAP", "HEIGHT_MAP", "", field->Name, 100);
	float hscale = ini.Get_Float("HEIGHTMAP", "HSCALE", 1.0f);
	float vscale = ini.Get_Float("HEIGHTMAP", "VSCALE", 1.0f);
	field->Wrapping = ini.Get_Bool("HEIGHTMAP", "WRAPPING", false);
	bool flip = ini.Get_Bool("HEIGHTMAP", "FLIP_PGM", true);
	if (!field->Load_PGM(field->Name, flip, max_size)) {
		delete field;
		return NULL;
	}
	field->Mask = field->Size - 1;
	field->CountMask = field->Count - 1;
	field->Log2 = Find_POT_Log2(field->Size);
	float spacing = hscale / (float)field->Size;
	field->HalfSize = field->Size >> 1;
	vscale = vscale / (float)field->MaxValue;
	field->XSpacing = spacing;
	field->YSpacing = spacing;
	field->VScale = vscale;
	return field;
}

HeightFieldClass::~HeightFieldClass(void)
{
	if (Buffer != NULL) {
		delete [] Buffer;
		Buffer = NULL;
	}
}

// NOT EXACT: written from the pseudocode (the PGM reader 0x4637C0, the same as VIDTEST's 0x4026B0)
bool HeightFieldClass::Load_PGM(const char *filename, bool flip, unsigned int max_size)
{
	char delimiters[] = " \r\n\t";
	FileClass *file = _TheFileFactory->Get_File(filename);
	if (!file->Is_Available(0)) {
		return false;
	}
	file->Open(FileClass::READ);
	char *text = new char[file->Size() + 1];
	file->Read(text, file->Size());
	text[file->Size()] = 0;
	if (strstr(strtok(text, delimiters), "P2") == NULL) {
		delete [] text;
		return false;
	}
	unsigned int width;
	do {
		width = atoi(strtok(NULL, delimiters));
	} while (width == 0);
	unsigned int height = atoi(strtok(NULL, delimiters));
	atoi(strtok(NULL, delimiters));
	if (width != height) {
		delete [] text;
		return false;
	}
	Size = width;
	Count = width * width;
	Buffer = new char[width * width * 2 + 0x3E];
	unsigned short *p = (unsigned short *)(((unsigned int)Buffer + 0x3E) & ~0x3F);
	Heights = p;
	MaxValue = 0;
	for (unsigned int n = Count; n != 0; n--) {
		unsigned short value = (unsigned short)atoi(strtok(NULL, delimiters));
		if (value > MaxValue) {
			MaxValue = value;
		}
		*p++ = value;
	}
	_TheFileFactory->Return_File(file);
	delete [] text;

	if (flip) {
		unsigned short *row = new unsigned short[Size];
		for (unsigned int j = 0; j < Size / 2; j++) {
			memcpy(row, &Heights[Size * j], Size * 2);
			memcpy(&Heights[Size * j], &Heights[Size * (Size - j - 1)], Size * 2);
			memcpy(&Heights[Size * (Size - j - 1)], row, Size * 2);
		}
		delete [] row;
	}

	if (max_size < Size) {
		unsigned int count = max_size * max_size;
		void *buffer = new char[count * 2 + 0x3E];
		unsigned short *heights = (unsigned short *)(((unsigned int)buffer + 0x3E) & ~0x3F);
		unsigned int step = Size / max_size;
		for (unsigned int j = 0; j < max_size; j++) {
			for (unsigned int i = 0; i < max_size; i++) {
				unsigned int sum = 0;
				for (unsigned int y = 0; y < step; y++) {
					unsigned short *src = &Heights[(j * step + y) * Size + i * step];
					for (unsigned int x = step; x != 0; x--) {
						sum += *src++;
					}
				}
				heights[j * max_size + i] = (unsigned short)(sum / (step * step));
			}
		}
		delete [] Buffer;
		Buffer = buffer;
		Heights = heights;
		Size = max_size;
		Count = count;
	}
	return true;
}

// NOT EXACT: frame (0xC in the client: one more local) and the y copy through a register
void HeightFieldClass::Sample(Vector3 *out, const Vector2 &uv)
{
	Vector2 pos((float)((uv.X - 0.5) * XSpacing * Size), (float)((uv.Y - 0.5) * YSpacing * Size));
	*out = Vector3(pos.X, pos.Y, Get_Height(pos));
}

float HeightFieldClass::Get_Height(unsigned int i, unsigned int j)
{
	return (float)Heights[((j & Mask) << Log2) + (i & Mask)] * VScale;
}

float HeightFieldClass::Get_Height_2(unsigned int i, unsigned int j)
{
	return (float)Heights[((j & Mask) << Log2) + (i & Mask)] * VScale;
}

void HeightFieldClass::Set_Height(float height, unsigned int i, unsigned int j)
{
	float value = height / VScale;
	if (value > 65535.0f) {
		value = 65535.0f;
	}
	Heights[((j & Mask) << Log2) + (i & Mask)] = (unsigned short)value;
}

void HeightFieldClass::Set_Height_2(float height, unsigned int i, unsigned int j)
{
	float value = height / VScale;
	if (value > 65535.0f) {
		value = 65535.0f;
	}
	Heights[((j & Mask) << Log2) + (i & Mask)] = (unsigned short)value;
}

// NOT EXACT: order-only (the client loads HalfSize before the position)
void HeightFieldClass::World_To_Grid(const Vector2 &pos, float *i, float *j)
{
	*i = HalfSize + pos.X / XSpacing;
	*j = HalfSize + pos.Y / YSpacing;
}

void HeightFieldClass::Get_Vertex(unsigned int i, unsigned int j, Vector3 *out)
{
	out->X = (float)(int)(i - HalfSize) * XSpacing;
	out->Y = (float)(int)(j - HalfSize) * YSpacing;
	out->Z = Get_Height(i, j);
}

// NOT EXACT: written from the pseudocode (0x463F00: frame 0x1C in the client, the corner tests' scheduling)
void HeightFieldClass::Get_Downhill(Vector3 *direction, const Vector2 &pos)
{
	float fx = pos.X / XSpacing + HalfSize;
	float fy = pos.Y / YSpacing + HalfSize;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	unsigned int row1 = ((j + 1) & Mask) << Log2;
	float tx = fx - i;
	float ty = fy - j;
	unsigned int row0 = (j & Mask) << Log2;
	unsigned int i1 = (i + 1) & Mask;
	float h01 = Heights[row1 + (i & Mask)] * VScale;
	float h10 = Heights[i1 + row0] * VScale;
	float h00 = Heights[row0 + (i & Mask)] * VScale;
	float h11 = Heights[i1 + row1] * VScale;
	float h;
	if (ty <= tx) {
		h = (h11 - h10) * ty + (1.0f - tx) * (h00 - h10) + h10;
	} else {
		h = (h11 - h01) * tx + (1.0f - ty) * (h00 - h01) + h01;
	}
	int corner = -1;
	if (h00 < h) {
		corner = 0;
		h = h00;
	}
	if (h10 < h) {
		corner = 1;
		h = h10;
	}
	if (h11 < h) {
		corner = 2;
		h = h11;
	}
	if (h01 < h) {
		corner = 3;
	}
	Vector2 d(0.0f, 0.0f);
	switch (corner) {
		case 0:
			d.Set(-tx, -ty);
			break;
		case 1:
			d.Set(1.0f - tx, -ty);
			break;
		case 2:
			d.Set(1.0f - tx, 1.0f - ty);
			break;
		case 3:
			d.Set(-tx, 1.0f - ty);
			break;
	}
	if (corner >= 0) {
		d.Normalize();
	}
	direction->Set(d.X, d.Y, 0.0f);
}

// NOT EXACT: written from the pseudocode (0x464290: the cross products and Normalize as Vector3's)
void HeightFieldClass::Get_Normal(Vector3 *normal, const Vector2 &pos)
{
	float fx = pos.X / XSpacing + HalfSize;
	float fy = pos.Y / YSpacing + HalfSize;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	unsigned int i1 = (i + 1) & Mask;
	unsigned int row0 = (j & Mask) << Log2;
	unsigned int row1 = ((j + 1) & Mask) << Log2;
	float h00 = Heights[row0 + (i & Mask)] * VScale;
	float h11 = Heights[i1 + row1] * VScale;
	Vector3 n;
	if (fy - j <= fx - i) {
		float h10 = Heights[i1 + row0] * VScale;
		n = Vector3::Cross_Product(Vector3(XSpacing, 0.0f, h10 - h00), Vector3(XSpacing, YSpacing, h11 - h00));
	} else {
		float h01 = Heights[row1 + (i & Mask)] * VScale;
		n = Vector3::Cross_Product(Vector3(XSpacing, YSpacing, h11 - h00), Vector3(0.0f, YSpacing, h01 - h00));
	}
	n.Normalize();
	*normal = n;
}

// NOT EXACT: written from the pseudocode (0x464600: the cell's two triangles)
float HeightFieldClass::Get_Height(const Vector2 &pos)
{
	float fx = pos.X / XSpacing + HalfSize;
	float fy = pos.Y / YSpacing + HalfSize;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	float tx = fx - i;
	float ty = fy - j;
	unsigned int row0 = (j & Mask) << Log2;
	unsigned int row1 = ((j + 1) & Mask) << Log2;
	unsigned int i1 = (i + 1) & Mask;
	float h00 = Heights[(i & Mask) + row0] * VScale;
	float h11 = Heights[i1 + row1] * VScale;
	if (tx < ty) {
		float h01 = Heights[row1 + (i & Mask)] * VScale;
		return (h11 - h01) * tx + (1.0f - ty) * (h00 - h01) + h01;
	}
	float h10 = Heights[i1 + row0] * VScale;
	return (h11 - h10) * ty + (1.0f - tx) * (h00 - h10) + h10;
}

/*
** Ours (VIDTEST's land, functional): the grid clamped at the edges (the client's accessors wrap with
** Mask), the vertex at world coordinates centred on the origin, the normal from central differences.
*/
float HeightFieldClass::Get_Vertex_Height(int i, int j) const
{
	if (Wrapping) {
		i &= Size - 1;
		j &= Size - 1;
	} else {
		if (i < 0) i = 0;
		if (i >= (int)Size) i = Size - 1;
		if (j < 0) j = 0;
		if (j >= (int)Size) j = Size - 1;
	}
	return (float)Heights[j * Size + i] * VScale;
}

Vector3 HeightFieldClass::Get_Vertex(int i, int j) const
{
	return Vector3((i - (int)Size / 2) * XSpacing, (j - (int)Size / 2) * XSpacing, Get_Vertex_Height(i, j));
}

Vector3 HeightFieldClass::Get_Normal(int i, int j) const
{
	float dx = Get_Vertex_Height(i + 1, j) - Get_Vertex_Height(i - 1, j);
	float dy = Get_Vertex_Height(i, j + 1) - Get_Vertex_Height(i, j - 1);
	Vector3 n(-dx, -dy, 2.0f * XSpacing);
	n.Normalize();
	return n;
}

bool HeightFieldClass::Cast_AABox(AABoxCollisionTestClass &boxtest)
{
	const Vector3 &min = boxtest.SweepMin;
	const Vector3 &max = boxtest.SweepMax;
	bool hit = false;
	float half = (float)HalfSize;
	float x0 = min.X / XSpacing + half;
	float y0 = min.Y / YSpacing + half;
	float x1 = max.X / XSpacing + half;
	float y1 = max.Y / YSpacing + half;
	int i0 = WWMath::Float_To_Long(floor(x0));
	int j0 = WWMath::Float_To_Long(floor(y0));
	int i1 = WWMath::Float_To_Long(ceil(x1));
	int j1 = WWMath::Float_To_Long(ceil(y1));
	for (int j = j0; j <= j1; j++) {
		for (int i = i0; i <= i1; i++) {
			Vector3 verts[4];
			Vector3 normal;
			TriClass tri;
			tri.N = &normal;
			verts[0].X = (float)(int)(i - HalfSize) * XSpacing;
			verts[0].Y = (float)(int)(j - HalfSize) * YSpacing;
			verts[0].Z = Get_Height(i, j);
			verts[1].X = verts[0].X + XSpacing;
			verts[1].Y = verts[0].Y;
			verts[1].Z = Get_Height(i + 1, j);
			verts[2].X = verts[0].X + XSpacing;
			verts[2].Y = verts[0].Y + YSpacing;
			verts[2].Z = Get_Height(i + 1, j + 1);
			verts[3].X = verts[0].X;
			verts[3].Y = verts[0].Y + YSpacing;
			verts[3].Z = Get_Height(i, j + 1);
			tri.V[0] = &verts[0];
			tri.V[1] = &verts[2];
			tri.V[2] = &verts[3];
			tri.Compute_Normal();
			hit |= CollisionMath::Collide(boxtest.Box, boxtest.Move, tri, boxtest.Result);
			tri.V[0] = &verts[0];
			tri.V[1] = &verts[1];
			tri.V[2] = &verts[2];
			tri.Compute_Normal();
			hit |= CollisionMath::Collide(boxtest.Box, boxtest.Move, tri, boxtest.Result);
		}
	}
	return hit;
}

// NOT EXACT: frame (0x168, the client 0x164: a 4-byte local too many; 22 masked rows; 10 forms, attempts.csv 0x4654E0)
bool HeightFieldClass::Cast_OBBox(OBBoxCollisionTestClass &boxtest)
{
	bool hit = false;
	Vector3 corners[16];
	int n = 0;
	int i, j, k;
	for (i = -1; i <= 1; i += 2) {
		for (j = -1; j <= 1; j += 2) {
			for (k = -1; k <= 1; k += 2) {
				float params[3];
				params[0] = (float)i;
				params[1] = (float)j;
				params[2] = (float)k;
				boxtest.Box.Compute_Point(params, &corners[n]);
				n++;
			}
		}
	}
	for (i = 0; i < 8; i++) {
		corners[i + 8] = corners[i] + boxtest.Move;
	}
	Vector3 min = corners[0];
	Vector3 max = corners[0];
	for (i = 1; i < 16; i++) {
		min.X = WWMath::Min(min.X, corners[i].X);
		max.X = WWMath::Max(max.X, corners[i].X);
		min.Y = WWMath::Min(min.Y, corners[i].Y);
		max.Y = WWMath::Max(max.Y, corners[i].Y);
	}
	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	int j0 = (int)(min.Y / YSpacing + half - 1.0f);
	for (j = j0; j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
			Vector3 verts[4];
			Vector3 normal;
			TriClass tri;
			tri.N = &normal;
			verts[0].X = (float)(int)(i - HalfSize) * XSpacing;
			verts[0].Y = (float)(int)(j - HalfSize) * YSpacing;
			verts[0].Z = Get_Height(i, j);
			verts[1].X = verts[0].X + XSpacing;
			verts[1].Y = verts[0].Y;
			verts[1].Z = Get_Height(i + 1, j);
			verts[2].X = verts[0].X + XSpacing;
			verts[2].Y = verts[0].Y + YSpacing;
			verts[2].Z = Get_Height(i + 1, j + 1);
			verts[3].X = verts[0].X;
			verts[3].Y = verts[0].Y + YSpacing;
			verts[3].Z = Get_Height(i, j + 1);
			tri.V[0] = &verts[0];
			tri.V[1] = &verts[2];
			tri.V[2] = &verts[3];
			tri.Compute_Normal();
			hit |= CollisionMath::Collide(boxtest.Box, boxtest.Move, tri, Vector3(0, 0, 0), boxtest.Result);
			tri.V[0] = &verts[0];
			tri.V[1] = &verts[1];
			tri.V[2] = &verts[2];
			tri.Compute_Normal();
			hit |= CollisionMath::Collide(boxtest.Box, boxtest.Move, tri, Vector3(0, 0, 0), boxtest.Result);
		}
	}
	return hit;
}

float HeightFieldClass::Get_Height(float x, float y) const
{
	float fx = x / XSpacing + (int)Size / 2;
	float fy = y / XSpacing + (int)Size / 2;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	float tx = fx - i;
	float ty = fy - j;
	float h00 = Get_Vertex_Height(i, j);
	float h10 = Get_Vertex_Height(i + 1, j);
	float h01 = Get_Vertex_Height(i, j + 1);
	float h11 = Get_Vertex_Height(i + 1, j + 1);
	return (h00 * (1.0f - tx) + h10 * tx) * (1.0f - ty) + (h01 * (1.0f - tx) + h11 * tx) * ty;
}

void HeightFieldClass::Sample(Vector3 *out, float u, float v) const
{
	out->X = (u - 0.5f) * XSpacing * Size;
	out->Y = (v - 0.5f) * XSpacing * Size;
	out->Z = Get_Height(out->X, out->Y);
}

/*
**	Reconstruction notes
**
**	The client's height field, 0x4634A0-0x465D30 (no asserts, no __FILE__: the file name is unknown;
**	heightfield.cpp is a proposal), and VIDTEST's 2002 copy 0x4023D0-0x404B30, in the client's order:
**	the constructor 0x4634A0, Load 0x4634F0, the destructor 0x463790, the PGM reader 0x4637C0, Sample
**	0x463BF0, Get_Height 0x463C80 / 0x463CD0, Set_Height 0x463D20 / 0x463D90, World_To_Grid 0x463E00,
**	Get_Vertex 0x463E70, Get_Downhill 0x463F00, Get_Normal 0x464290, Get_Height(position) 0x464600,
**	Cast_AABox 0x465030 and Cast_OBBox 0x4654E0 (NOT EXACT: frame): two triangles per cell over the
**	sweep's bounds (LandClass's Cast_* forward to them through 0x46C680-0x46C6E0). Not rebuilt:
**	Cast_Ray 0x4647C0 and 0x465D30 (called by the land's render). The functions after the notes'
**	"Ours" are VIDTEST's land's (functional, D42). The decorations keep the bilinear Sample(u, v)
**	for now: the client's Sample 0x463BF0 (two triangles per cell) moves a tree 1.8 units on average
**	over the river map (52 at most), and vidtest_sbs's run-to-run spread (about 1.5% of the pixels)
**	hides whether VIDTEST.EXE's trees follow it; the draw dumps' tree positions can settle it.
*/
