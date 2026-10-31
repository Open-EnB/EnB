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
#include "obbox.h"

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

// The PGM reader 0x4637C0 (the same as VIDTEST's 0x4026B0): every token through one variable
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
	char *token = strtok(text, delimiters);
	if (strstr(token, "P2") == NULL) {
		delete [] text;
		return false;
	}
	unsigned int width;
	do {
		token = strtok(NULL, delimiters);
		width = atoi(token);
	} while (width == 0);
	token = strtok(NULL, delimiters);
	unsigned int height = atoi(token);
	token = strtok(NULL, delimiters);
	atoi(token);
	if (width != height) {
		delete [] text;
		return false;
	}
	Size = width;
	Count = width * width;
	Buffer = new char[width * width * 2 + 0x3E];
	Heights = (unsigned short *)(((unsigned int)Buffer + 0x3E) & ~0x3F);
	unsigned short *p = Heights;
	unsigned int n = Count;
	MaxValue = 0;
	while (n--) {
		unsigned short value = (unsigned short)atoi(strtok(NULL, delimiters));
		MaxValue = (value > MaxValue) ? value : MaxValue;
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

	if (Size > max_size) {
		unsigned int count = max_size * max_size;
		void *buffer = new char[count * 2 + 0x3E];
		unsigned short *heights = (unsigned short *)(((unsigned int)buffer + 0x3E) & ~0x3F);
		unsigned int step = Size / max_size;
		unsigned int area = step * step;
		for (unsigned int j = 0; j < max_size; j++) {
			for (unsigned int i = 0; i < max_size; i++) {
				unsigned int sum = 0;
				for (unsigned int y = 0; y < step; y++) {
					for (unsigned int x = 0; x < step; x++) {
						sum += Heights[(j * step + y) * Size + i * step + x];
					}
				}
				heights[j * max_size + i] = (unsigned short)(sum / area);
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

Vector3 HeightFieldClass::Sample(const Vector2 &uv)
{
	Vector3 pos;
	pos.X = (float)((uv.X - 0.5) * XSpacing * Size);
	pos.Y = (float)((uv.Y - 0.5) * YSpacing * Size);
	pos.Z = Get_Height(pos);
	return pos;
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

// NOT EXACT: shape (0x463F00, 27 masked rows: the client pops edi before the last height and doesn't merge case 2's tail into case 3's; attempts.csv)
void HeightFieldClass::Get_Downhill(Vector3 *direction, const Vector2 &pos)
{
	float fx = pos.X / XSpacing + HalfSize;
	float fy = pos.Y / YSpacing + HalfSize;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	float tx = fx - i;
	float ty = fy - j;
	float h01 = Get_Height(i, j + 1);
	float h10 = Get_Height(i + 1, j);
	float h00 = Get_Height(i, j);
	float h11 = Get_Height(i + 1, j + 1);
	float h;
	if (tx < ty) {
		h = (h11 - h01) * tx + (1.0f - ty) * (h00 - h01) + h01;
	} else {
		h = (h11 - h10) * ty + (1.0f - tx) * (h00 - h10) + h10;
	}
	int corner = -1;
	float lowest = h;
	if (lowest > h00) {
		corner = 0;
		lowest = h00;
	}
	if (lowest > h10) {
		corner = 1;
		lowest = h10;
	}
	if (lowest > h11) {
		corner = 2;
		lowest = h11;
	}
	if (lowest > h01) {
		corner = 3;
	}
	Vector3 d(0.0f, 0.0f, 0.0f);
	switch (corner) {
		case 0:
			d.X = -tx;
			d.Y = -ty;
			break;
		case 1:
			d.X = 1.0f - tx;
			d.Y = -ty;
			break;
		case 2:
			d.X = 1.0f - tx;
			d.Y = 1.0f - ty;
			break;
		case 3:
			d.X = -tx;
			d.Y = 1.0f - ty;
			break;
	}
	if (corner >= 0) {
		d.Normalize();
	}
	*direction = d;
}

// NOT EXACT: frame (0x464290: 0x34, the client 0x40; it keeps h10 on the FPU, spills h01 and copies one edge component; attempts.csv)
void HeightFieldClass::Get_Normal(Vector3 *normal, const Vector2 &pos)
{
	float fx = pos.X / XSpacing + HalfSize;
	float fy = pos.Y / YSpacing + HalfSize;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	float h10 = Get_Height(i + 1, j);
	float h00 = Get_Height(i, j);
	float h11 = Get_Height(i + 1, j + 1);
	Vector3 n;
	if (fx - i < fy - j) {
		float h01 = Get_Height(i, j + 1);
		Vector3 a(XSpacing, YSpacing, h11 - h00);
		Vector3 b(0.0f, YSpacing, h01 - h00);
		Vector3::Cross_Product(a, b, &n);
		n.Normalize();
	} else {
		Vector3 a(XSpacing, 0.0f, h10 - h00);
		Vector3 b(XSpacing, YSpacing, h11 - h00);
		Vector3::Cross_Product(a, b, &n);
		n.Normalize();
	}
	*normal = n;
}

// 0x464600: the cell's two triangles, the heights through the inlined Get_Height(i, j)
float HeightFieldClass::Get_Height(const Vector3 &pos)
{
	float fx = pos.X / XSpacing + HalfSize;
	float fy = pos.Y / YSpacing + HalfSize;
	int i = (int)floor(fx);
	int j = (int)floor(fy);
	float tx = fx - i;
	float ty = fy - j;
	float h00 = Get_Height(i, j);
	float h11 = Get_Height(i + 1, j + 1);
	if (tx < ty) {
		float h01 = Get_Height(i, j + 1);
		return (h11 - h01) * tx + (1.0f - ty) * (h00 - h01) + h01;
	}
	float h10 = Get_Height(i + 1, j);
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

// NOT EXACT: frame (0xAC, the client 0xB0: it keeps tmax and the second slab's values on the FPU and swaps through a register; 90 masked rows; attempts.csv 0x4647C0)
bool HeightFieldClass::Cast_Ray(RayCollisionTestClass &raytest)
{
	bool hit = false;
	Vector3 start = raytest.Ray.Get_P0();
	Vector3 end = raytest.Ray.Get_P1();
	if (start.Z < Get_Height(start)) {
		raytest.Result->StartBad = true;
		return hit;
	}
	float half = (float)HalfSize;
	float x0 = start.X / XSpacing + half;
	float y0 = start.Y / YSpacing + half;
	float x1 = end.X / XSpacing + half;
	float y1 = end.Y / YSpacing + half;
	if (!Wrapping) {
		float tmin_x = 0.0f;
		float tmax_x = 1.0f;
		float tmin_y = 0.0f;
		float tmax_y = 1.0f;
		float limit = (float)(Size - 1);
		float dx = x1 - x0;
		float dy = y1 - y0;
		if (dx == 0.0f) {
			if (x0 < 0.0f || x0 > limit) {
				return false;
			}
		} else {
			tmin_x = -x0 / dx;
			tmax_x = (limit - x0) / dx;
			if (tmin_x > tmax_x) {
				float swap = tmin_x;
				tmin_x = tmax_x;
				tmax_x = swap;
			}
		}
		if (dy == 0.0f) {
			if (y0 < 0.0f || y0 > limit) {
				return false;
			}
		} else {
			tmin_y = -y0 / dy;
			tmax_y = (limit - y0) / dy;
			if (tmin_y > tmax_y) {
				float swap = tmin_y;
				tmin_y = tmax_y;
				tmax_y = swap;
			}
		}
		float tmin = WWMath::Max(tmin_x, tmin_y);
		float tmax = WWMath::Min(tmax_x, tmax_y);
		if (tmin > 1.0f) {
			return false;
		}
		if (tmax < 0.0f) {
			return false;
		}
		if (tmin < 0.0f) {
			tmin = 0.0f;
		}
		if (tmax > 1.0f) {
			tmax = 1.0f;
		}
		x1 = tmax * dx + x0;
		y1 = tmax * dy + y0;
		x0 = tmin * dx + x0;
		y0 = tmin * dy + y0;
	}
	float dist_x;
	float dist_y;
	int step_x;
	int step_y;
	if (x0 < x1) {
		dist_x = x1 - x0;
		step_x = 1;
	} else {
		dist_x = x0 - x1;
		step_x = -1;
	}
	if (y0 < y1) {
		dist_y = y1 - y0;
		step_y = 1;
	} else {
		dist_y = y0 - y1;
		step_y = -1;
	}
	float err_x;
	if (x0 < x1) {
		err_x = (float)(ceil(x0) - x0);
	} else {
		err_x = (float)(x0 - floor(x0));
	}
	err_x *= dist_y;
	float err_y;
	if (y0 < y1) {
		err_y = (float)(ceil(y0) - y0);
	} else {
		err_y = (float)(y0 - floor(y0));
	}
	err_y *= dist_x;
	int i = WWMath::Float_To_Long(floor(x0));
	int j = WWMath::Float_To_Long(floor(y0));
	int count = WWMath::Float_To_Long(ceil(dist_y) + ceil(dist_x));
	if (count == 0) {
		count = 1;
	}
	for (; count > 0; count--) {
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
		hit |= CollisionMath::Collide(raytest.Ray, tri, raytest.Result);
		tri.V[0] = &verts[0];
		tri.V[1] = &verts[1];
		tri.V[2] = &verts[2];
		tri.Compute_Normal();
		hit |= CollisionMath::Collide(raytest.Ray, tri, raytest.Result);
		if (hit) {
			break;
		}
		if (err_x < err_y) {
			err_x += dist_y;
			i += step_x;
		} else {
			err_y += dist_x;
			j += step_y;
		}
	}
	return hit;
}

// NOT EXACT: frame (0x168, the client 0x164: a 4-byte local too many; 12 masked rows; attempts.csv 0x4654E0)
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
			Get_Vertex(i, j, &verts[0]);
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

// NOT EXACT: shape (0x465D30: the second push_back reads end() after pushing 1, and v3.Y is stored after v3.X; the first corner through Get_Vertex puts the first push_back's insert out of line as the client, 0x466180; attempts.csv)
void HeightFieldClass::Get_Triangles(const OBBoxClass &box, std::vector<Vector3> &verts)
{
	Vector3 extent;
	box.Compute_Axis_Aligned_Extent(&extent);
	Vector3 min = box.Center - extent;
	float half = (float)HalfSize;
	float x0 = min.X / XSpacing + half;
	float y0 = min.Y / YSpacing + half;
	Vector3 max = box.Center + extent;
	float x1 = max.X / XSpacing + half;
	float y1 = max.Y / YSpacing + half;
	if (x0 > x1) {
		float swap = x0;
		x0 = x1;
		x1 = swap;
	}
	if (y0 > y1) {
		float swap = y0;
		y0 = y1;
		y1 = swap;
	}
	int i0 = (int)(x0 - 1.0f);
	int i1 = (int)(x1 + 1.0f);
	int j1 = (int)(y1 + 1.0f);
	int j0 = (int)(y0 - 1.0f);
	for (int j = j0; j <= j1; j++) {
		for (int i = i0; i <= i1; i++) {
			Vector3 verts4[4];
			Get_Vertex(i, j, &verts4[0]);
			verts4[1].X = verts4[0].X + XSpacing;
			verts4[1].Y = verts4[0].Y;
			verts4[1].Z = Get_Height(i + 1, j);
			verts4[2].X = verts4[1].X;
			verts4[3].Y = verts4[2].Y = verts4[0].Y + YSpacing;
			verts4[2].Z = Get_Height(i + 1, j + 1);
			verts4[3].X = verts4[0].X;
			verts4[3].Z = Get_Height(i, j + 1);
			verts.push_back(verts4[0]);
			verts.push_back(verts4[2]);
			verts.push_back(verts4[3]);
			verts.push_back(verts4[0]);
			verts.push_back(verts4[1]);
			verts.push_back(verts4[2]);
		}
	}
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
**	Cast_AABox 0x465030 (exact), Cast_OBBox 0x4654E0 and Cast_Ray 0x4647C0 (NOT EXACT: frame): two triangles
**	per cell over the sweep's bounds (LandClass's Cast_* forward to them through 0x46C680-0x46C6E0). Get_Triangles 0x465D30 (NOT
**	EXACT, for the land's render) with vector<Vector3>::insert 0x4663E0. Cast_Ray clips the ray to the grid unless it wraps, then
**	walks the cells it crosses (a 2D DDA) until a triangle is hit. The functions after the notes'
**	"Ours" are VIDTEST's land's (functional, D42). The decorations keep the bilinear Sample(u, v)
**	for now: the client's Sample 0x463BF0 (two triangles per cell) moves a tree 1.8 units on average
**	over the river map (52 at most), and vidtest_sbs's run-to-run spread (about 1.5% of the pixels)
**	hides whether VIDTEST.EXE's trees follow it; the draw dumps' tree positions can settle it.
*/
