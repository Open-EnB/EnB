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
**	Get_Vertex 0x463E70, Get_Height(position) 0x464600. Not rebuilt: the collision code 0x463F00-
**	0x465D30 (CollisionMath::Collide over the field's triangles). The functions after the notes'
**	"Ours" are VIDTEST's land's (functional, D42). The decorations keep the bilinear Sample(u, v)
**	for now: the client's Sample 0x463BF0 (two triangles per cell) moves a tree 1.8 units on average
**	over the river map (52 at most), and vidtest_sbs's run-to-run spread (about 1.5% of the pixels)
**	hides whether VIDTEST.EXE's trees follow it; the draw dumps' tree positions can settle it.
*/
