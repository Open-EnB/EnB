// Earth & Beyond client (reconstruction): app\shared\spln_db.cpp. Notes at the end of the file.
#include "spln_db.h"
#include "curve.h"
#include "hermitespline.h"
#include "catmullromspline.h"
#include "chunkio.h"
#include "ffactory.h"
#include "realcrc.h"
#include <windows.h>
#include <string.h>
#include <time.h>
#include "unknown_debug.h"

// NAME 2: chunk 0x800 (version 0x100, entry count, 0, 0), read by Load_Header (0xBE34A8)
struct SplineFileHeaderStruct
{
	unsigned int	Version;
	unsigned int	Count;
	unsigned int	Reserved[2];
};

static SplineFileHeaderStruct _Header;

// NAME 2: an inline copy for the entry's strings (its inlining leaves VectorClass's constructor out of line, as the client's 0x489050)
static inline void Copy_String(char *&copy, const char *string)
{
	copy = new char[strlen(string) + 1];
	strcpy(copy, string);
}

// NAME 2: the 0x24 bytes before an entry's name (chunks 0x810 / 0x811)
struct SplineEntryHeaderStruct
{
	int						Type;
	unsigned int			NameLength;
	bool						Flag;
	SplineParamsStruct	Params;
	unsigned int			Reserved[2];
};

SplineDatabaseClass::SplineDatabaseClass(const char *database_filename) :
	ShowHidden(false),
	Iterator(NULL),
	HasSplines(false),
	Mailslot(INVALID_HANDLE_VALUE)
{
	if (database_filename != NULL) {
		Filename = new char[strlen(database_filename) + 1];
		strcpy(Filename, database_filename);
	} else {
		Filename = NULL;
	}
	KeyNames.Add(NULL);
}

// NOT EXACT: shape (the inlined Free_Splines loop: registers and the next-node test)
SplineDatabaseClass::~SplineDatabaseClass(void)
{
	if (Mailslot != INVALID_HANDLE_VALUE) {
		CloseHandle(Mailslot);
	}
	if (Filename != NULL) {
		delete [] Filename;
	}
	Free_Splines();
	Free_Key_Names();
}

// NOT EXACT: shape (the inlined Free_Splines and Free_Key_Names loops)
bool SplineDatabaseClass::Load(const char *database_filename)
{
	clock();
	if (Filename != NULL) {
		delete [] Filename;
	}
	Filename = new char[strlen(database_filename) + 1];
	strcpy(Filename, database_filename);
	file_auto_ptr file(_TheFileFactory, Filename);
	if (file->Open(FileClass::READ)) {
		Free_Splines();
		Free_Key_Names();
		ChunkLoadClass cload(file);
		if (Load_Chunks(cload) && Resolve()) {
			clock();
			return true;
		}
	}
	return false;
}




















































































































































































































































































































bool SplineDatabaseClass::Get_Type(const char *spline_name, int *type)
{
	G_ASSERT(spline_name);
	SplineEntryClass *entry = Find(spline_name);
	if (entry == NULL) {
		return false;
	}
	*type = entry->Type;
	return true;
}




































































bool SplineDatabaseClass::Get_Curve(const char *spline_name, Curve1DClass **curve)
{
	G_ASSERT(spline_name);
	SplineEntryClass *entry = Find(spline_name);
	if (entry == NULL) {
		return false;
	}
	*curve = entry->Curve;
	return true;
}

// NOT EXACT: inline-budget (the client calls HashListClass::Find out of line for _G only)
bool SplineDatabaseClass::Get_Color_Curves(const char *base_name, Curve1DClass **r, Curve1DClass **g, Curve1DClass **b)
{
	char *name = new char[strlen(base_name) + 3];
	if (name == NULL) {
		return false;
	}
	strcpy(name, base_name);
	strcat(name, "_R");
	int len = strlen(name);
	SplineEntryClass *entry = Find(name);
	if (entry != NULL) {
		*r = entry->Curve;
		name[len - 1] = 'G';
		entry = Find(name);
		if (entry != NULL) {
			*g = entry->Curve;
			name[len - 1] = 'B';
			entry = Find(name);
			if (entry != NULL) {
				*b = entry->Curve;
				delete [] name;
				return true;
			}
		}
	}
	delete [] name;
	return false;
}

































































































































































































































































































































































































































































































































































































































































































































SplineEntryClass *SplineDatabaseClass::Find(const char *spline_name, int unused)
{
	G_ASSERT(spline_name != NULL);
	HashNodeClass<SplineEntryClass *> *node = Splines.Find(CRC_String(spline_name));
	if (node == NULL) {
		return NULL;
	}
	return node->Get();
}

// NOT EXACT: shape (the client tests the first node before the loop; no zero register)
void SplineDatabaseClass::Free_Splines(void)
{
	HashNodeClass<SplineEntryClass *> *node = Splines.First_Valid();
	while (node != NULL) {
		HashNodeClass<SplineEntryClass *> *next = node->Next_Valid();
		SplineEntryClass *entry = node->Get();
		if (entry != NULL) {
			delete entry;
		}
		Splines.Remove(node);
		node = next;
	}
	HasSplines = false;
}

void SplineDatabaseClass::Free_Key_Names(void)
{
	int i = KeyNames.Count();
	while (--i > 0) {
		if (KeyNames[i] != NULL) {
			delete [] KeyNames[i];
			KeyNames.Delete(i);
		}
	}
}

// NOT EXACT: inline-budget (HashListClass::Find out of line at some lookups)
bool SplineDatabaseClass::Resolve(void)
{
	bool ok = true;
	for (HashNodeClass<SplineEntryClass *> *node = Splines.First_Valid(); node != NULL; node = node->Next_Valid()) {
		SplineEntryClass *entry = node->Get();
		if (entry->Type == TYPE_COLOR) {
			char name[128];
			strcpy(name, entry->Name);
			strcat(name, "_R");
			int len = strlen(name);
			SplineEntryClass *component = Find(name);
			if (component != NULL) {
				component->Hidden = true;
			} else {
				ok = false;
			}
			name[len - 1] = 'G';
			component = Find(name);
			if (component != NULL) {
				component->Hidden = true;
			} else {
				ok = false;
			}
			name[len - 1] = 'B';
			component = Find(name);
			if (component != NULL) {
				component->Hidden = true;
			} else {
				ok = false;
			}
		} else if (entry->Type == TYPE_BLEND) {
			HashNodeClass<SplineEntryClass *> *found = Splines.Find(entry->AlphaKey);
			if (found == NULL || found->Get() == NULL) {
				ok = false;
			}
			found = Splines.Find(entry->BetaKey);
			if (found == NULL || found->Get() == NULL) {
				ok = false;
			}
		}
	}
	return ok;
}




































































































































































































































































































































































// NOT EXACT: shape (the inlined Free_Key_Names loop)
bool SplineDatabaseClass::Reload(void)
{
	const char *database_filename = Filename;
	G_ASSERT(database_filename);
	clock();
	file_auto_ptr file(_TheFileFactory, database_filename);
	if (!file->Open(FileClass::READ)) {
		return false;
	}
	Free_Key_Names();
	ChunkLoadClass cload(file);
	if (!Load_Chunks(cload)) {
		return false;
	}
	if (!Resolve()) {
		return false;
	}
	clock();
	return true;
}

// NOT EXACT: field-offset / shape (KeyNames kept in a register, the header test)
bool SplineDatabaseClass::Load_Chunks(ChunkLoadClass &cload)
{
	SplineFileHeaderStruct *header = Load_Header(cload);
	if (header == NULL) {
		return false;
	}
	if (header->Version != 0x100) {
		return false;
	}
	unsigned int count;
	if (!cload.Open_Chunk() || cload.Cur_Chunk_ID() != 0x808 || cload.Read(&count, 4) < 4) {
		return false;
	}
	char *name = NULL;
	KeyNames.Resize(count + 1);
	for (unsigned int i = 0; i < count; i++) {
		if (!Read_String(cload, &name)) {
			return false;
		}
		KeyNames.Add(name);
		name = NULL;
	}
	if (!cload.Close_Chunk()) {
		return false;
	}
	for (unsigned int index = 0; index < header->Count; index++) {
		SplineEntryClass *entry = Load_Entry(cload);
		if (entry == NULL) {
			return false;
		}
		unsigned int key = CRC_String(entry->Name);
		HashNodeClass<SplineEntryClass *> *node = Splines.Find(key);
		SplineEntryClass *existing;
		if (node != NULL && (existing = node->Get()) != NULL) {
			if (existing->Curve != NULL && entry->Curve != NULL) {
				*existing->Curve = *entry->Curve;
				if (existing->Type == TYPE_HERMITE) {
					((CatmullRomSpline1DClass *)existing->Curve)->Update_Tangents();
				}
			}
			delete entry;
		} else if (Splines.Add(entry, key) == NULL) {
			delete entry;
			return false;
		}
	}
	if (Splines.Num_Records() > 0) {
		HasSplines = true;
	}
	return true;
}

SplineFileHeaderStruct *SplineDatabaseClass::Load_Header(ChunkLoadClass &cload)
{
	if (!cload.Open_Chunk()) {
		return NULL;
	}
	if (cload.Cur_Chunk_ID() != 0x800) {
		return NULL;
	}
	if (cload.Read(&_Header, sizeof(_Header)) < sizeof(_Header)) {
		return NULL;
	}
	return cload.Close_Chunk() ? &_Header : NULL;
}

// NOT EXACT: shape (the switch's layout; the TYPE_BLEND strings)
SplineEntryClass *SplineDatabaseClass::Load_Entry(ChunkLoadClass &cload)
{
	int i = 0;
	if (!cload.Open_Chunk()) {
		return NULL;
	}
	if (cload.Cur_Chunk_ID() != 0x810 && cload.Cur_Chunk_ID() != 0x811) {
		return NULL;
	}
	SplineEntryHeaderStruct header;
	if (cload.Read(&header, sizeof(header)) < sizeof(header)) {
		return NULL;
	}
	if (header.NameLength > 0x80) {
		return NULL;
	}
	char name[128];
	if (cload.Read(name, header.NameLength) < header.NameLength) {
		return NULL;
	}
	char *description = NULL;
	if (cload.Cur_Chunk_ID() == 0x811 && !Read_String(cload, &description)) {
		return NULL;
	}
	SplineEntryClass *entry = new SplineEntryClass(header.Type, name, description, false, NULL, NULL);
	if (entry == NULL) {
		return NULL;
	}
	entry->FileFlag = header.Flag;
	entry->Params = header.Params;
	if (description != NULL) {
		delete [] description;
	}
	bool ok;
	switch (entry->Type) {
		case TYPE_COLOR:
		{
			char component[128];
			strcpy(component, entry->Name);
			strcat(component, "_R");
			int len = strlen(component);
			entry->RedKey = CRC_String(component);
			component[len - 1] = 'G';
			entry->GreenKey = CRC_String(component);
			component[len - 1] = 'B';
			entry->BlueKey = CRC_String(component);
			ok = true;
			break;
		}
		case TYPE_BLEND:
			ok = Load_Hermite(cload, entry);
			if (ok) {
				if (cload.Cur_Chunk_ID() == 0x811) {
					ok = cload.Read(&entry->AlphaKey, 4) >= 4 && cload.Read(&entry->BetaKey, 4) >= 4;
				} else if (cload.Cur_Chunk_ID() == 0x810) {
					char *alpha_clr = NULL;
					if (!Read_String(cload, &alpha_clr)) {
						ok = false;
					} else {
						entry->AlphaKey = CRC_String(alpha_clr);
						delete [] alpha_clr;
						char *beta_clr = NULL;
						if (!Read_String(cload, &beta_clr)) {
							ok = false;
						} else {
							entry->BetaKey = CRC_String(beta_clr);
							delete [] beta_clr;
						}
					}
				}
			}
			break;
		case TYPE_LINEAR:
		{
			Curve1DClass *curve = entry->Curve;
			bool looping;
			if (cload.Read(&looping, 1) < 1) {
				ok = false;
				break;
			}
			curve->Set_Looping(looping);
			int count;
			if (cload.Read(&count, 4) < 4) {
				ok = false;
				break;
			}
			ok = true;
			for (i = 0; i < count; i++) {
				float point;
				float t;
				unsigned int extra;
				if (cload.Read(&point, 4) < 4 || cload.Read(&t, 4) < 4 || cload.Read(&extra, 4) < 4) {
					ok = false;
					break;
				}
				curve->Add_Key(point, t, extra);
			}
			break;
		}
		case TYPE_HERMITE:
			ok = Load_Hermite(cload, entry);
			break;
		default:
			return NULL;
	}
	if (!ok) {
		return NULL;
	}
	return cload.Close_Chunk() ? entry : NULL;
}

bool SplineDatabaseClass::Load_Hermite(ChunkLoadClass &cload, SplineEntryClass *entry)
{
	HermiteSpline1DClass *curve = (HermiteSpline1DClass *)entry->Curve;
	bool looping;
	if (cload.Read(&looping, 1) < 1) {
		return false;
	}
	curve->Set_Looping(looping);
	int count;
	if (cload.Read(&count, 4) < 4) {
		return false;
	}
	for (int i = 0; i < count; i++) {
		float point;
		float t;
		unsigned int extra;
		if (cload.Read(&point, 4) < 4) {
			return false;
		}
		if (cload.Read(&t, 4) < 4) {
			return false;
		}
		if (cload.Read(&extra, 4) < 4) {
			return false;
		}
		curve->Add_Key(point, t, extra);
		float in_tan;
		float out_tan;
		if (cload.Read(&in_tan, 4) < 4) {
			return false;
		}
		if (cload.Read(&out_tan, 4) < 4) {
			return false;
		}
		curve->Set_Tangents(i, in_tan, out_tan);
	}
	return true;
}

bool SplineDatabaseClass::Read_String(ChunkLoadClass &cload, char **string)
{
	unsigned int length;
	if (cload.Read(&length, 4) < 4) {
		return false;
	}
	if (length == 0) {
		*string = NULL;
		return true;
	}
	if (*string == NULL) {
		*string = new char[length];
		if (*string == NULL) {
			return false;
		}
	}
	if (cload.Read(*string, length) >= length) {
		return true;
	}
	return false;
}































































































































































































































































































































































































































































































































































































































































































































































































































































































SplineEntryClass::SplineEntryClass(int type, const char *name, const char *description, bool flag,
	const char *alpha_clr, const char *beta_clr) :
	Alpha(NULL),
	Beta(NULL),
	AlphaKey(0),
	BetaKey(0),
	Red(NULL),
	Green(NULL),
	Blue(NULL),
	RedKey(0),
	GreenKey(0),
	BlueKey(0)
{
	G_ASSERT(name != NULL);
	FileFlag = false;
	Hidden = flag;
	switch (type) {
		case SplineDatabaseClass::TYPE_LINEAR:
			Curve = new LinearCurve1DClass;
			break;
		case SplineDatabaseClass::TYPE_HERMITE:
			Curve = new CatmullRomSpline1DClass;
			break;
		case SplineDatabaseClass::TYPE_COLOR:
			Curve = NULL;
			break;
		case SplineDatabaseClass::TYPE_BLEND:
			Curve = new CatmullRomSpline1DClass;
			if (alpha_clr != NULL) {
				AlphaKey = CRC_String(alpha_clr);
			}
			if (beta_clr != NULL) {
				BetaKey = CRC_String(beta_clr);
			}
			break;

































		case 0:
			G_ASSERT(false);
			break;




		default:
			G_ASSERT(false);
			break;
	}
	Type = type;
	Copy_String(Name, name);
	if (description != NULL) {
		Copy_String(Description, description);
	} else {
		Description = NULL;
	}
}

SplineEntryClass::~SplineEntryClass(void)
{
	if (Name != NULL) {
		delete [] Name;
	}
	if (Description != NULL) {
		delete [] Description;
	}
	if (Curve != NULL) {
		delete Curve;
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4845F0-0x489C70 (spln_db.cpp, 44 asserts: spline_name, base_name, alpha_clr,
**	beta_clr, resolution, database_filename, EntryArray[i], ...), the file's code in the client's
**	order; the asserts sit on the client's lines (D25). VIDTEST.EXE's 2002 copy is 0x41C660-0x420F50.
**	The entries live in wwlib's HashListClass (512 hash values) keyed by CRC_String of the name
**	(0x9D4840); the find helper (0x486BB0, line 1223) and HashListClass::Find are inlined in most
**	callers.
**
**	  0x4845F0 SplineDatabaseClass             0x486BB0 Find (1223)
**	  0x484810 ~SplineDatabaseClass            0x487190 Free_Splines, 0x487360 Free_Key_Names
**	  0x484AB0 Load                            0x487660 Resolve, 0x487900 Reload (1662)
**	  0x485110 Get_Type (400)                  0x488500 Load_Chunks, 0x4887F0 Load_Header
**	  0x485380 Get_Curve (478)                 0x488870 Load_Entry, 0x488DC0 Load_Hermite
**	  0x485440 Get_Color_Curves                0x488F20 Read_String
**	                                           0x489050 SplineEntryClass (2795), 0x489590 ~
**
**	Not rebuilt yet (the client has them; nothing in VIDTEST calls them): the save code (0x484D00,
**	0x487B20-0x4883A0, 0x488FB0, 0x486E10-0x486F60, 0x489660, 0x489830), Print 0x485020, the blend
**	and evaluation helpers 0x4851D0, 0x485620, 0x485790, the editing functions 0x485990-0x4864F0 and
**	0x487400, the key-name and iteration helpers 0x4867B0-0x486A40, the mailslot update 0x486AD0, the
**	key lookups 0x486C40-0x486D80, 0x4873E0, the entry's second constructor 0x489480.
*/
