// Earth & Beyond client (reconstruction): app\shared\spln_db.cpp. Notes at the end of the file.
// NOT EXACT, the whole file: functional, from VIDTEST's 2002 copy (spln_db.h).
#include "spln_db.h"
#include "curve.h"
#include "hermitespline.h"
#include "chunkio.h"
#include "rawfile.h"
#include <string.h>
#include <stdlib.h>
#include "unknown_debug.h"

SplineDatabaseClass::SplineDatabaseClass(const char *database_filename) :
	Filename(0),
	Loaded(false)
{
	if (database_filename != 0) {
		Load(database_filename);
	}
}

SplineDatabaseClass::~SplineDatabaseClass(void)
{
	Free();
	delete [] Filename;
	Filename = 0;
}

void SplineDatabaseClass::Free(void)
{
	for (int i = 0; i < EntryArray.Count(); i++) {
		EntryStruct *entry = EntryArray[i];
		delete entry->Curve;
		delete [] entry->Name;
		delete entry;
	}
	EntryArray.Delete_All();
	Loaded = false;
}

SplineDatabaseClass::EntryStruct *SplineDatabaseClass::Find(const char *spline_name) const
{
	G_ASSERT(spline_name != NULL);
	if (spline_name == NULL) {
		return NULL;
	}
	for (int i = 0; i < EntryArray.Count(); i++) {
		if (stricmp(EntryArray[i]->Name, spline_name) == 0) {
			return EntryArray[i];
		}
	}
	return NULL;
}

Curve1DClass *SplineDatabaseClass::Get_Spline(const char *spline_name) const
{
	EntryStruct *entry = Find(spline_name);
	return entry != NULL ? entry->Curve : NULL;
}

bool SplineDatabaseClass::Get_Color_Splines(const char *base_name, Curve1DClass **r, Curve1DClass **g,
	Curve1DClass **b) const
{
	G_ASSERT(base_name);
	char name[256];
	strncpy(name, base_name, sizeof(name) - 3);
	name[sizeof(name) - 3] = 0;
	size_t len = strlen(name);
	name[len] = '_';
	name[len + 2] = 0;
	name[len + 1] = 'R';
	*r = Get_Spline(name);
	name[len + 1] = 'G';
	*g = Get_Spline(name);
	name[len + 1] = 'B';
	*b = Get_Spline(name);
	return *r != NULL && *g != NULL && *b != NULL;
}

bool SplineDatabaseClass::Evaluate_Color(const char *base_name, float t, float *r, float *g, float *b) const
{
	Curve1DClass *cr, *cg, *cb;
	if (!Get_Color_Splines(base_name, &cr, &cg, &cb)) {
		return false;
	}
	cr->Evaluate(t, r);
	cg->Evaluate(t, g);
	cb->Evaluate(t, b);
	return true;
}

static bool Read_String(ChunkLoadClass &cload, char **string)	// NAME 2: VIDTEST 0x420100
{
	unsigned int length;
	if (cload.Read(&length, 4) < 4) {
		return false;
	}
	if (length != 0) {
		char *buffer = new char[length + 1];
		if (cload.Read(buffer, length) < length) {
			delete [] buffer;
			return false;
		}
		buffer[length] = 0;
		if (string != NULL) {
			*string = buffer;
		} else {
			delete [] buffer;
		}
	}
	return true;
}

static bool Read_Curve(ChunkLoadClass &cload, Curve1DClass *curve, bool hermite)	// NAME 2: VIDTEST 0x41FCB0 / 0x41FDA0
{
	unsigned char looping;
	int count;
	if (cload.Read(&looping, 1) < 1) return false;
	curve->Set_Looping(looping != 0);
	if (cload.Read(&count, 4) < 4) return false;
	for (int i = 0; i < count; i++) {
		float point, t;
		unsigned int extra;
		if (cload.Read(&point, 4) < 4) return false;
		if (cload.Read(&t, 4) < 4) return false;
		if (cload.Read(&extra, 4) < 4) return false;
		curve->Add_Key(point, t, extra);
		if (hermite) {
			float in_tan, out_tan;
			if (cload.Read(&in_tan, 4) < 4) return false;
			if (cload.Read(&out_tan, 4) < 4) return false;
			((HermiteSpline1DClass *)curve)->Set_Tangents(i, in_tan, out_tan);
		}
	}
	return true;
}

bool SplineDatabaseClass::Load_Entry(ChunkLoadClass &cload)
{
	if (!cload.Open_Chunk()) {
		return false;
	}
	unsigned int id = cload.Cur_Chunk_ID();
	if (id != 0x810 && id != 0x811) {
		cload.Close_Chunk();
		return false;
	}
	struct {
		int				Type;
		unsigned int	NameLength;
		unsigned char	Flag;
		unsigned char	Pad[3];
		float				Params[4];
		unsigned char	Unused[8];
	} header;
	char name[0x81];
	if (cload.Read(&header, 0x24) < 0x24 || header.NameLength > 0x80
		|| cload.Read(name, header.NameLength) < header.NameLength) {
		cload.Close_Chunk();
		return false;
	}
	name[header.NameLength] = 0;
	char *extra = NULL;
	if (id == 0x811 && !Read_String(cload, &extra)) {
		cload.Close_Chunk();
		return false;
	}
	delete [] extra;

	EntryStruct *entry = new EntryStruct;
	entry->Name = new char[strlen(name) + 1];
	strcpy(entry->Name, name);
	entry->Type = header.Type;
	entry->Flag = header.Flag != 0;
	for (int i = 0; i < 4; i++) {
		entry->Params[i] = header.Params[i];
	}
	entry->Curve = NULL;

	bool ok = true;
	switch (header.Type) {
		case TYPE_COLOR:
			break;
		case TYPE_LINEAR:
			entry->Curve = new LinearCurve1DClass;
			ok = Read_Curve(cload, entry->Curve, false);
			break;
		case TYPE_HERMITE:
		case TYPE_BLEND:
			entry->Curve = new HermiteSpline1DClass;
			ok = Read_Curve(cload, entry->Curve, true);
			if (ok && header.Type == TYPE_BLEND) {
				if (id == 0x811) {
					unsigned int alpha_clr, beta_clr;
					ok = cload.Read(&alpha_clr, 4) == 4 && cload.Read(&beta_clr, 4) == 4;
				} else {
					ok = Read_String(cload, NULL) && Read_String(cload, NULL);
				}
			}
			break;
		default:
			ok = false;
			break;
	}
	cload.Close_Chunk();
	if (!ok) {
		delete entry->Curve;
		delete [] entry->Name;
		delete entry;
		return false;
	}
	EntryStruct *old = Find(entry->Name);
	if (old != NULL) {
		EntryArray.Delete(old);
		delete old->Curve;
		delete [] old->Name;
		delete old;
	}
	EntryArray.Add(entry);
	return true;
}

bool SplineDatabaseClass::Load(const char *database_filename)
{
	G_ASSERT(database_filename);
	if (database_filename == NULL) {
		return false;
	}
	if (Filename != database_filename) {
		delete [] Filename;
		Filename = new char[strlen(database_filename) + 1];
		strcpy(Filename, database_filename);
	}

	RawFileClass file;
	if (!file.Open(Filename, FileClass::READ)) {
		return false;
	}
	Free();
	ChunkLoadClass cload(&file);

	// the header
	unsigned int header[4];
	if (!cload.Open_Chunk() || cload.Cur_Chunk_ID() != 0x800 || cload.Read(header, 0x10) < 0x10) {
		return false;
	}
	cload.Close_Chunk();
	if (header[0] != 0x100) {
		return false;
	}

	// the key names
	if (!cload.Open_Chunk() || cload.Cur_Chunk_ID() != 0x808) {
		return false;
	}
	cload.Close_Chunk();

	for (unsigned int i = 0; i < header[1]; i++) {
		if (!Load_Entry(cload)) {
			return false;
		}
	}
	Loaded = EntryArray.Count() > 0;
	return true;
}

/*
**	Reconstruction notes
**
**	client.exe 0x485000-0x48A000 (spln_db.cpp, 39 asserts: spline_name, base_name, alpha_clr,
**	beta_clr, resolution, database_filename, EntryArray[i], ...), VIDTEST.EXE 0x41C660-0x420F50
**	(ctor 0x41C660 ~ client 0x4845F0, dtor 0x41C880 ~ 0x489C70, Load 0x41CB20, the chunk readers
**	0x41F620-0x420100, Get_Color_Splines 0x41D030 with the name lookup 0x41CFF0).
**
**	NOT EXACT: functional, written from VIDTEST's loader (D42) for VIDTEST's lighting (milestones.md
**	M2.4). Not the client's layout or code: the client's name table (0x41EA50 turns a name into an
**	id), list heads, the type-1 entries' R/G/B ids, the type-2 blend (two more entries, alpha_clr and
**	beta_clr), the reload of an existing entry (Curve1DClass::operator=), the mailslot live update
**	(0x486AD0, never opened by VIDTEST) and the save code are not rebuilt. splines.wsd has types 1
**	and 4 only. As VIDTEST, the file is opened with RawFileClass (the working directory, not the
**	file factory).
*/
