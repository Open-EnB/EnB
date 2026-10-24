// Earth & Beyond client (reconstruction): app\shared\ height field (file name unknown). Notes at the end.
// NOT EXACT, the whole file: functional, from VIDTEST's 2002 copy (heightfield.h).
#ifndef NDEBUG
#define NDEBUG
#endif
#include "heightfield.h"
#include "ini.h"
#include "ffactory.h"
#include "wwfile.h"
#include "wwmath.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

HeightFieldClass::HeightFieldClass(void) :
	Heights(0),
	MaxValue(0),
	Spacing(1.0f),
	HeightScale(1.0f),
	Wrapping(false),
	Width(0),
	Height(0)
{
	Name[0] = 0;
}

HeightFieldClass::~HeightFieldClass(void)
{
	delete [] Heights;
	Heights = 0;
}

HeightFieldClass *HeightFieldClass::Load(const char *ini_name)
{
	INIClass ini;
	if (!ini.Load(ini_name)) {
		return 0;
	}
	HeightFieldClass *field = new HeightFieldClass;
	ini.Get_String("HEIGHTMAP", "HEIGHT_MAP", "", field->Name, sizeof(field->Name));
	float hscale = ini.Get_Float("HEIGHTMAP", "HSCALE", 1.0f);
	float vscale = ini.Get_Float("HEIGHTMAP", "VSCALE", 1.0f);
	field->Wrapping = ini.Get_Bool("HEIGHTMAP", "WRAPPING", false);
	bool flip = ini.Get_Bool("HEIGHTMAP", "FLIP_PGM", true);
	if (!field->Load_PGM(field->Name, flip)) {
		delete field;
		return 0;
	}
	field->Spacing = hscale / (float)field->Width;
	field->HeightScale = field->MaxValue != 0 ? vscale / (float)field->MaxValue : 0.0f;
	for (int i = 0; i < field->Width * field->Height; i++) {
		field->Heights[i] *= field->HeightScale;
	}
	return field;
}

/*
** The PGM reader (VIDTEST 0x4026B0, client 0x4637C0): ASCII "P2", width, height, maxval, then the
** values; '#' starts a comment. FLIP_PGM turns the rows over.
*/
static bool Next_Token(const char *&p, const char *end, char *token, int size)	// NAME 2: ours, the PGM tokenizer
{
	for (;;) {
		while (p < end && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')) p++;
		if (p < end && *p == '#') {
			while (p < end && *p != '\n') p++;
			continue;
		}
		break;
	}
	if (p >= end) return false;
	int n = 0;
	while (p < end && !(*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')) {
		if (n < size - 1) token[n++] = *p;
		p++;
	}
	token[n] = 0;
	return true;
}

bool HeightFieldClass::Load_PGM(const char *filename, bool flip)
{
	FileClass *file = _TheFileFactory->Get_File(filename);
	if (file == 0) {
		return false;
	}
	if (!file->Open(FileClass::READ)) {
		_TheFileFactory->Return_File(file);
		return false;
	}
	int size = file->Size();
	char *text = new char[size + 1];
	int got = file->Read(text, size);
	file->Close();
	_TheFileFactory->Return_File(file);
	text[got] = 0;

	const char *p = text;
	const char *end = text + got;
	char token[32];
	bool ok = Next_Token(p, end, token, sizeof(token)) && strcmp(token, "P2") == 0;
	int w = 0, h = 0, maxval = 0;
	if (ok) ok = Next_Token(p, end, token, sizeof(token)) && (w = atoi(token)) > 0;
	if (ok) ok = Next_Token(p, end, token, sizeof(token)) && (h = atoi(token)) > 0;
	if (ok) ok = Next_Token(p, end, token, sizeof(token)) && (maxval = atoi(token)) > 0;
	if (ok) {
		Width = w;
		Height = h;
		MaxValue = 0;			// VIDTEST scales by the largest value read, not the header's maxval
		delete [] Heights;
		Heights = new float[w * h];
		for (int j = 0; j < h && ok; j++) {
			int row = flip ? h - 1 - j : j;
			for (int i = 0; i < w; i++) {
				if (!Next_Token(p, end, token, sizeof(token))) {
					ok = false;
					break;
				}
				Heights[row * w + i] = (float)atoi(token);
				if (Heights[row * w + i] > MaxValue) MaxValue = (unsigned short)Heights[row * w + i];
			}
		}
	}
	delete [] text;
	return ok;
}

float HeightFieldClass::Get_Vertex_Height(int i, int j) const
{
	if (Wrapping) {
		i &= Width - 1;
		j &= Height - 1;
	} else {
		if (i < 0) i = 0;
		if (i >= Width) i = Width - 1;
		if (j < 0) j = 0;
		if (j >= Height) j = Height - 1;
	}
	return Heights[j * Width + i];
}

Vector3 HeightFieldClass::Get_Vertex(int i, int j) const
{
	return Vector3((i - Width / 2) * Spacing, (j - Height / 2) * Spacing, Get_Vertex_Height(i, j));
}

Vector3 HeightFieldClass::Get_Normal(int i, int j) const
{
	float dx = Get_Vertex_Height(i + 1, j) - Get_Vertex_Height(i - 1, j);
	float dy = Get_Vertex_Height(i, j + 1) - Get_Vertex_Height(i, j - 1);
	Vector3 n(-dx, -dy, 2.0f * Spacing);
	n.Normalize();
	return n;
}

float HeightFieldClass::Get_Height(float x, float y) const
{
	float fx = x / Spacing + Width / 2;
	float fy = y / Spacing + Height / 2;
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
	out->X = (u - 0.5f) * Spacing * Width;
	out->Y = (v - 0.5f) * Spacing * Height;
	out->Z = Get_Height(out->X, out->Y);
}

/*
**	Reconstruction notes
**
**	The client's height field, 0x4634A0-0x465D30 (no asserts, no __FILE__: the file name is unknown;
**	heightfield.cpp is a proposal), and VIDTEST's 2002 copy 0x4023D0-0x404B30. NOT EXACT: written
**	functionally (D42) for VIDTEST's land (milestones.md M2.4). Not rebuilt: the collision code
**	(CollisionMath::Collide over the field's triangles) and the client's 16-bit height storage.
**	The PGM is read through the file factory (VIDTEST reads it with the same factory as its art).
*/
