// Earth & Beyond client (reconstruction): app\common\manufacturing_defs.cpp
// Client 0x845200-0x846253 and the guarded <string> static 0x846270: the whole object (the
// manufacturing lab's value converters). Notes at the end of the file.
#include "manufacturing_defs.h"
#include <string.h>
#include "..\shared\g_assert.h"
#include "..\shared\unknown_4a1590.h"
#include "..\shared\unknown_4fac20.h"

// 0x845200, 0x845240: a byte buffer's two constructors, uncalled (the first functions of the object).
class UnknownBufferClass	// NAME 1: what it holds (no client string names it)
{
public:
	UnknownBufferClass(int size);
	UnknownBufferClass(void *buffer, int size, bool copy);

	char *Buffer;	// +0x00  // NAME 1: what it holds
	bool Owned;		// +0x04  // NAME 1: what it says
	int Size;		// +0x08  // NAME 1: what it holds
	char *Cursor;	// +0x0C  // NAME 1: what it holds
};

// 0x845200
UnknownBufferClass::UnknownBufferClass(int size)
{
	Owned = true;
	Size = size;
	Buffer = new char[size];
	Cursor = Buffer;
}

// 0x845240
UnknownBufferClass::UnknownBufferClass(void *buffer, int size, bool copy)
{
	Owned = copy;
	Size = size;
	if (copy) {
		Buffer = new char[size];
		Cursor = Buffer;
		memcpy(Buffer, buffer, Size);
	} else {
		Buffer = (char *)buffer;
		Cursor = Buffer;
	}
}

struct MfgTypeStruct
{
	char *Name;
	MfgTypeEnum Value;
};

struct MfgCategoryStruct
{
	char *Name;
	MfgCategoryEnum Value;
};

struct MfgSubCategoryStruct
{
	char *Name;
	UnknownB0122CEnum Value;
};

static MfgTypeStruct _TypeDef[] = {
	{ "None", (MfgTypeEnum)-1 },
	{ "Items", (MfgTypeEnum)0 },
	{ "Components", (MfgTypeEnum)1 },
	{ NULL, (MfgTypeEnum)-1 }
};

static MfgCategoryStruct _CategoryDef[] = {
	{ "None", (MfgCategoryEnum)-1 },
	{ "Weapons", (MfgCategoryEnum)10 },
	{ "Systems", (MfgCategoryEnum)11 },
	{ "Core", (MfgCategoryEnum)12 },
	{ "Consumables", (MfgCategoryEnum)13 },
	{ "Computer / Electronics", (MfgCategoryEnum)50 },
	{ "Power", (MfgCategoryEnum)51 },
	{ "Fabricated", (MfgCategoryEnum)52 },
	{ "Weapon Components", (MfgCategoryEnum)53 },
	{ "Ammo", (MfgCategoryEnum)54 },
	{ "Refined Resources", (MfgCategoryEnum)80 },
	{ "Trade Goods", (MfgCategoryEnum)90 },
	{ NULL, (MfgCategoryEnum)-1 }
};

static MfgSubCategoryStruct _SubCategoryDef[] = {
	{ "None", (UnknownB0122CEnum)-1 },
	{ "Beam", (UnknownB0122CEnum)100 },
	{ "Projectile", (UnknownB0122CEnum)101 },
	{ "Missile", (UnknownB0122CEnum)102 },
	{ "Ammo", (UnknownB0122CEnum)103 },
	{ "Basic", (UnknownB0122CEnum)110 },
	{ "Drone", (UnknownB0122CEnum)111 },
	{ "Reactor", (UnknownB0122CEnum)120 },
	{ "Engine", (UnknownB0122CEnum)121 },
	{ "Shield", (UnknownB0122CEnum)122 },
	{ "Consumable X", (UnknownB0122CEnum)130 },
	{ "Software", (UnknownB0122CEnum)140 },
	{ "Electronics", (UnknownB0122CEnum)141 },
	{ "Computer", (UnknownB0122CEnum)142 },
	{ "Power Converter", (UnknownB0122CEnum)150 },
	{ "Power Coupling", (UnknownB0122CEnum)151 },
	{ "Power Core", (UnknownB0122CEnum)152 },
	{ "Generator", (UnknownB0122CEnum)153 },
	{ "Shielded Casing", (UnknownB0122CEnum)160 },
	{ "Engine Frame", (UnknownB0122CEnum)161 },
	{ "Drone Frame", (UnknownB0122CEnum)162 },
	{ "Mounts", (UnknownB0122CEnum)163 },
	{ "Firing Mechanism", (UnknownB0122CEnum)170 },
	{ "Ammo Feeder / Loader", (UnknownB0122CEnum)171 },
	{ "Optics", (UnknownB0122CEnum)172 },
	{ "Barrel / Launch Tube", (UnknownB0122CEnum)173 },
	{ "Slug / Rail", (UnknownB0122CEnum)180 },
	{ "Warhead", (UnknownB0122CEnum)181 },
	{ "Propellant", (UnknownB0122CEnum)182 },
	{ "Shell Casing", (UnknownB0122CEnum)183 },
	{ "Refined Optic Gems", (UnknownB0122CEnum)200 },
	{ "Refined Metals", (UnknownB0122CEnum)201 },
	{ "Refined Conductors", (UnknownB0122CEnum)202 },
	{ "Refined Alloys", (UnknownB0122CEnum)203 },
	{ "Refined Radioactives", (UnknownB0122CEnum)204 },
	{ "Refined Catalysts", (UnknownB0122CEnum)205 },
	{ "Refined Hydrocarbons", (UnknownB0122CEnum)206 },
	{ "Refined Silicates", (UnknownB0122CEnum)207 },
	{ "Refined Magnetics", (UnknownB0122CEnum)208 },
	{ "Refined Trade Gemstones", (UnknownB0122CEnum)209 },
	{ "Refined Gases", (UnknownB0122CEnum)210 },
	{ "Refined Engine COre Mtls", (UnknownB0122CEnum)211 },
	{ "Refined Engine Core Metals", (UnknownB0122CEnum)211 },
	{ NULL, (UnknownB0122CEnum)-1 }
};

static char *_ModeNames[] = { "None", "Manufacture", "Analyze", "Dismantle", "Refining" };	// 0xBAEF30  // NAME 1: what it holds
static char *_ValidityNames[] = {	// 0xBAEF44  // NAME 1: what it holds
	"NoTarget", "NotManufacturable", "NotDismantleable", "RecipeNotKnown", "InsufficientCredits",
	"MissingComponents", "InsufficientCargo", "NotQualified", "TooDifficult", "LowQualitiy", "BadStackSize",
	"PlayerMade", "DuplicateUniqueItem", "Valid", "AttemptTotalFailure", "AttemptNearSuccess",
	"AttemptNormalSuccess", "AttemptCriticalSuccess"
};
static char *_DifficultyNames[] = { "Invalid", "Automatic", "Easy", "Moderate", "Hard", "VeryHard", "Impossible" };	// 0xBAEF8C  // NAME 1: what it holds

// 0x8452C0
bool String_To_Value(const char *buf, MfgTypeEnum *value)
{


























	FAIL_IF(!buf) return(false);
	int i = 0;
	while (_TypeDef[i].Name != NULL) {




		FAIL_IF(i >= sizeof(_TypeDef) / sizeof(MfgTypeStruct)) return(false);
		if (stricmp(_TypeDef[i].Name, buf) == 0) {
			*value = _TypeDef[i].Value;
			return(true);
		}
		i++;
	}
	return(false);
}

// 0x845380
char *Value_To_String(char *buf, int size, MfgTypeEnum value)
{






	FAIL_IF(!buf) return(NULL);
	FAIL_IF(size < 0) return(NULL);
	int i = 0;
	while (_TypeDef[i].Name != NULL) {




		FAIL_IF(i >= sizeof(_TypeDef) / sizeof(MfgTypeStruct)) return(NULL);
		if (_TypeDef[i].Value == value) {
			if (strlen(_TypeDef[i].Name) < (unsigned)size) {
				strcpy(buf, _TypeDef[i].Name);
				return(buf);
			}




			G_ASSERT(false);
			return(NULL);
		}
		i++;
	}
	return(NULL);
}

// 0x8454C0
bool String_To_Value(const char *buf, MfgCategoryEnum *value)
{




	FAIL_IF(!buf) return(false);
	int i = 0;
	while (_CategoryDef[i].Name != NULL) {




		FAIL_IF(i >= sizeof(_CategoryDef) / sizeof(MfgCategoryStruct)) return(false);
		if (stricmp(_CategoryDef[i].Name, buf) == 0) {
			*value = _CategoryDef[i].Value;
			return(true);
		}
		i++;
	}
	return(false);
}

// 0x845580
char *Value_To_String(char *buf, int size, MfgCategoryEnum value)
{






	FAIL_IF(!buf) return(NULL);
	FAIL_IF(size < 0) return(NULL);
	int i = 0;
	while (_CategoryDef[i].Name != NULL) {




		FAIL_IF(i >= sizeof(_CategoryDef) / sizeof(MfgCategoryStruct)) return(NULL);
		if (_CategoryDef[i].Value == value) {
			if (strlen(_CategoryDef[i].Name) < (unsigned)size) {
				strcpy(buf, _CategoryDef[i].Name);
				return(buf);
			}




			G_ASSERT(false);
			return(NULL);
		}
		i++;
	}
	return(NULL);
}

// 0x8456C0
bool String_To_Value(const char *buf, UnknownB0122CEnum *value)
{




	FAIL_IF(!buf) return(false);
	int i = 0;
	while (_SubCategoryDef[i].Name != NULL) {




		FAIL_IF(i >= sizeof(_SubCategoryDef) / sizeof(MfgSubCategoryStruct)) return(false);
		if (stricmp(_SubCategoryDef[i].Name, buf) == 0) {
			*value = _SubCategoryDef[i].Value;
			return(true);
		}
		i++;
	}
	return(false);
}

// 0x845780
char *Value_To_String(char *buf, int size, UnknownB0122CEnum value)
{






	FAIL_IF(!buf) return(NULL);
	FAIL_IF(size < 0) return(NULL);
	int i = 0;
	while (_SubCategoryDef[i].Name != NULL) {




		FAIL_IF(i >= sizeof(_SubCategoryDef) / sizeof(MfgSubCategoryStruct)) return(NULL);
		if (_SubCategoryDef[i].Value == value) {
			if (strlen(_SubCategoryDef[i].Name) < (unsigned)size) {
				strcpy(buf, _SubCategoryDef[i].Name);
				return(buf);
			}




			G_ASSERT(false);
			return(NULL);
		}
		i++;
	}
	return(NULL);
}

// 0x8458C0
bool String_To_Value(const char *text, UnknownB013ECEnum *value)
{
	for (int i = MODE_NONE; i <= MODE_COUNT; i++) {
		if (stricmp(_ModeNames[i], text) == 0) {
			*value = (UnknownB013ECEnum)i;
			return(true);
		}
	}
	return(false);
}

// 0x845910
char *Value_To_String(char *buf, int size, UnknownB013ECEnum mode)
{














	FAIL_IF(mode < MODE_NONE || mode > MODE_DISMANTLE) return(strncpy(buf, "error", size));
	return(strncpy(buf, _ModeNames[mode], size));
}

// 0x845990
ManufacturingModeBitFlag Mode_To_Bit_Flag(UnknownB013ECEnum mode)
{
	switch (mode) {
		case MODE_NONE:			return((ManufacturingModeBitFlag)0);
		case MODE_MANUFACTURE:	return((ManufacturingModeBitFlag)1);
		case MODE_ANALYZE:		return((ManufacturingModeBitFlag)2);
		case MODE_DISMANTLE:	return((ManufacturingModeBitFlag)4);
		case MODE_REFINE:		return((ManufacturingModeBitFlag)8);
		default:


			G_ASSERT(0);
	}
	return((ManufacturingModeBitFlag)0);
}

// 0x845A10
UnknownB013ECEnum Bit_Flag_To_Mode(ManufacturingModeBitFlag flag)
{
	switch (flag) {
		case 0:	return(MODE_NONE);
		case 1:	return(MODE_MANUFACTURE);
		case 2:	return(MODE_ANALYZE);
		case 4:	return(MODE_DISMANTLE);
		case 8:	return(MODE_REFINE);
		default:
			G_ASSERT(0);
	}
	return(MODE_NONE);
}

// NOT EXACT: the failed lookup's path destroys the list's strings through the inlined _Tidy(true), the client's through ??_G(0) (inline budget, D89)
// 0x845AA0
bool String_To_Value(const char *text, ManufacturingModeBitFlag *value)
{
	unsigned int flags = 0;
	std::vector<std::string> list;
	String_Tokenize(text, "|", list);
	if (list.size() == 0) return(false);
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++) {
		UnknownB013ECEnum mode;
		bool ok = String_To_Value(it->c_str(), &mode);






		FAIL_IF(!ok) return(false);
		flags |= Mode_To_Bit_Flag(mode);
	}
	*value = (ManufacturingModeBitFlag)flags;
	return(true);
}

// NOT EXACT: one exit (the client shares the string's destructor between both paths), but this compile inlines text += name's append, the client calls it (inline budget, D89)
// 0x845D40
char *Value_To_String(char *buf, int size, ManufacturingModeBitFlag value)
{
	std::string text;
	for (unsigned int flag = 8; flag != 0; flag <<= 1) {
		if (value & flag) {
			char name[64];
			Value_To_String(name, sizeof(name) - 1, Bit_Flag_To_Mode((ManufacturingModeBitFlag)flag));
			if (text.length() != 0) {
				text += std::string("|");
			}
			text += name;
		}
	}
	char *result;
	if (text.length() == 0) {
		result = Value_To_String(buf, size, MODE_NONE);
	} else {
		Bounded_Strcpy(buf, size, text.c_str());
		result = buf;
	}
	return(result);
}

// 0x846040
bool String_To_Value(const char *text, UnknownB01394Enum *value)
{
	for (int i = MANUFACTURING_VALIDITY_START; i <= MANUFACTURING_VALIDITY_COUNT; i++) {
		if (stricmp(_ModeNames[i], text) == 0) {
			*value = (UnknownB01394Enum)i;
			return(true);
		}
	}
	return(false);
}

// 0x846090
char *Value_To_String(char *buf, int size, UnknownB01394Enum validity)
{






























	FAIL_IF(validity < MANUFACTURING_VALIDITY_START || validity > MANUFACTURING_VALIDITY_COUNT) return(strncpy(buf, "error", size));
	return(strncpy(buf, _ValidityNames[validity], size));
}

// 0x846110
bool String_To_Value(const char *text, UnknownB0133CEnum *value)
{
	for (int i = MANUFACTURING_DIFFICULTY_INVALID; i <= NUM_MANUFACTURING_DIFFICULTY; i++) {
		if (stricmp(_DifficultyNames[i], text) == 0) {
			*value = (UnknownB0133CEnum)i;
			return(true);
		}
	}
	return(false);
}

// 0x846160
char *Value_To_String(char *buf, int size, UnknownB0133CEnum difficulty)
{













	FAIL_IF(difficulty < MANUFACTURING_DIFFICULTY_INVALID || difficulty >= NUM_MANUFACTURING_DIFFICULTY) return(strncpy(buf, "error", size));
	return(strncpy(buf, _DifficultyNames[difficulty], size));
}

// 0x8461E0
bool Is_Validity_Failure(UnknownB01394Enum validity)
{
	if (validity >= MANUFACTURING_VALIDITY_START && validity <= 12) return(true);
	return(false);
}

// 0x846210
bool Is_Validity_Attempt(UnknownB01394Enum validity)
{
	if (validity >= 14 && validity <= 17) return(true);
	return(false);
}

// 0x846240
bool Is_Validity_Success(UnknownB01394Enum validity)
{
	if (validity >= 16 && validity <= 17) return(true);
	return(false);
}

/*
**	Notes (reconstruction)
**
**	* The object: 0x845200 (right after lego_identifier.cpp's guarded <string> static 0x8451D0) to
**	  its own guarded <string> static 0x846270 (unknown_navlist.cpp starts at 0x8462A0). The asserts
**	  sit on the client's lines (173 ... 534); the blank lines keep them there.
**	* UnknownBufferClass's two constructors (0x845200, 0x845240) have no caller and no client name;
**	  they open the object, so they are placed here (provisional: no string ties them to the file).
**	* The tables (0xBAED48 _TypeDef, 0xBAED68 _CategoryDef, 0xBAEDD0 _SubCategoryDef, then the mode,
**	  validity and difficulty names) are the client's, typos included ('Refined Engine COre Mtls',
**	  'LowQualitiy'). The three name lookups bound their loops with '<=' and read one entry past
**	  their table; the validity lookup searches the mode names (the client's code does both).
**	* The mode, validity and difficulty value types keep aux_roots.h's tags (UnknownB013ECEnum,
**	  UnknownB01394Enum, UnknownB0133CEnum; UnknownB0122CEnum for the subcategory): their
**	  converters' mangled names are what aux_roots.h's KVPair views call.
**	* Value_To_String(ManufacturingModeBitFlag) starts its bit loop at 8, as the client does (bits
**	  1, 2 and 4 are never printed).
*/
