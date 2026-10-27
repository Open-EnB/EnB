/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\spln_db.h (name assumed from spln_db.cpp; no
**	assert names it)
**
**	The spline database: named 1D curves loaded from a chunk file (splines.wsd), which the
**	lighting evaluates by time of day ("LightColor.Earth", "SkyIntensity.Sun", ...). The client's
**	class name is not known (D24): SplineDatabaseClass after its mailslot
**	("\\.\mailslot\G\SplineDatabaseUpdate").
**
**	The client's layout (ctor 0x4845F0, dtor 0x484810; 0x850 bytes as VIDTEST's 0x41C660):
**	  +0x000 a bool, +0x004 Splines: wwlib's HashListClass of entries keyed by CRC_String(name),
**	  512 hash values (List +0x004, HashTable +0x020, NumRecords +0x820, UsedValues +0x824),
**	  +0x828 KeyNames (DynamicVectorClass<char *>, entry 0 NULL), +0x840 Filename, +0x844 an int,
**	  +0x848 HasSplines, +0x84C the mailslot handle (INVALID_HANDLE_VALUE).
**	An entry (0x4C bytes, ctor 0x489050): name, description, type (1 colour: entries name_R, _G, _B;
**	2 a blend of two more entries, alpha_clr and beta_clr; 3 LinearCurve1DClass; 4
**	CatmullRomSpline1DClass), two flags, four parameters from the file and the curve.
**
**	The file (VIDTEST 0x41F620, 0x41F7B0, 0x41F830, 0x41F960; client 0x488500, 0x4887F0, 0x488870):
**	chunk 0x800, 16 bytes (version 0x100, entry count, 0, 0); chunk 0x808, the key names (count,
**	then length-prefixed strings: Dawn, Noon, ...); then one chunk 0x810 or 0x811 per entry: a
**	0x24-byte header (type, name length, a flag byte at +8, four parameters at +0x0C), the name,
**	in 0x811 a length-prefixed description, and the curve by type.
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 3 [spln_db.h]: header of the client's app\shared\spln_db.cpp (assert __FILE__)
#ifndef __SPLN_DB_H
#define __SPLN_DB_H

#include "vector.h"
#include "hashlist.h"

class Curve1DClass;
class ChunkLoadClass;
struct SplineFileHeaderStruct;

// NAME 2: the four parameters an entry carries from the file (+0x10, copied as one)
struct SplineParamsStruct
{
	float	Value[4];
};

// NAME 2: one named spline (0x4C bytes, ctor 0x489050); the client names no class
class SplineEntryClass
{
public:
	SplineEntryClass(int type, const char *name, const char *description, bool flag,
		const char *alpha_clr, const char *beta_clr);
	~SplineEntryClass(void);

// NAME 2: after the assert 'name != NULL' and the file's header
	char *					Name;					// +0x00
	char *					Description;		// +0x04 (chunk 0x811's string)
	int						Type;					// +0x08 (SplineDatabaseClass::TYPE_*)
	bool						Hidden;				// +0x0C NAME 2: Resolve sets it on colour components; the name iteration (0x4869C0) skips them
	bool						FileFlag;			// +0x0D NAME 1: the header's byte at +8
	SplineParamsStruct	Params;				// +0x10
	Curve1DClass *			Curve;				// +0x20
	SplineEntryClass *	Alpha;				// +0x24 (resolved from AlphaKey)
	SplineEntryClass *	Beta;					// +0x28 (resolved from BetaKey)
	unsigned int			AlphaKey;			// +0x2C CRC_String(alpha_clr)
	unsigned int			BetaKey;				// +0x30 CRC_String(beta_clr)
	SplineEntryClass *	Red;					// +0x34 NAME 1: (not yet seen read)
	SplineEntryClass *	Green;				// +0x38 NAME 1
	SplineEntryClass *	Blue;					// +0x3C NAME 1
	unsigned int			RedKey;				// +0x40 CRC_String(name + "_R")
	unsigned int			GreenKey;			// +0x44
	unsigned int			BlueKey;				// +0x48
};

// NAME 2: after the mailslot "\\.\mailslot\G\SplineDatabaseUpdate"; the class name is not in the client
class SplineDatabaseClass
{
public:
	enum {
		TYPE_COLOR = 1,
		TYPE_BLEND = 2,
		TYPE_LINEAR = 3,
		TYPE_HERMITE = 4,
	};

	SplineDatabaseClass(const char *database_filename);
	~SplineDatabaseClass(void);

// NAME 2: loads a .wsd through the file factory (0x484AB0)
	bool						Load(const char *database_filename);
// NAME 2: loads Filename again (0x487900, assert 'database_filename'; the mailslot's reload)
	bool						Reload(void);
	bool						Has_Splines(void) const		{ return HasSplines; }
	int						Get_Count(void)				{ return Splines.Num_Records(); }

// NAME 2: an entry's type (0x485110, assert 'spline_name')
	bool						Get_Type(const char *spline_name, int *type);
// NAME 2: an entry's curve (0x485380, assert 'spline_name')
	bool						Get_Curve(const char *spline_name, Curve1DClass **curve);
// NAME 2: the curves of name_R, name_G, name_B (0x485440; the client's lighting 0x4F2960, 0x4F34C0)
	bool						Get_Color_Curves(const char *base_name, Curve1DClass **r, Curve1DClass **g,
									Curve1DClass **b);
// NAME 2: lookup by name (0x486BB0, assert 'spline_name != NULL')
	SplineEntryClass *	Find(const char *spline_name, int unused = 0);	// NAME 1 [unused]: the client's copy takes a second argument (ret 8) it never reads

protected:
// NAME 2: deletes the entries (0x487190)
	void						Free_Splines(void);
// NAME 2: deletes the key names but entry 0 (0x487360)
	void						Free_Key_Names(void);
// NAME 2: checks the colour and blend entries' components and hides the colour components (0x487660)
	bool						Resolve(void);
// NAME 2: the chunks after the file's opening (0x488500)
	bool						Load_Chunks(ChunkLoadClass &cload);
// NAME 2: chunk 0x800 into a static record (0x4887F0)
	SplineFileHeaderStruct *	Load_Header(ChunkLoadClass &cload);
// NAME 2: one chunk 0x810 / 0x811 (0x488870)
	static SplineEntryClass *	Load_Entry(ChunkLoadClass &cload);
// NAME 2: a Hermite curve's keys and tangents (0x488DC0)
	static bool				Load_Hermite(ChunkLoadClass &cload, SplineEntryClass *entry);
// NAME 2: a length-prefixed string (0x488F20)
	static bool				Read_String(ChunkLoadClass &cload, char **string);

// NAME 2: +0x000, the name iteration (0x4869C0) returns hidden entries too when set
	bool																		ShowHidden;
// NAME 2: the entries by CRC_String(name)
	HashListClass<SplineEntryClass *, HashDefaultUserClass, 512>	Splines;
// NAME 2: chunk 0x808's names; entry 0 is NULL
	DynamicVectorClass<char *>											KeyNames;
// NAME 2: the assert 'database_filename' names the argument kept here
	char *																	Filename;
// NAME 2: +0x844, the name iteration's node (0x4869C0, 0x486A40)
	HashNodeClass<SplineEntryClass *> *								Iterator;
// NAME 2: set when an entry is added
	bool																		HasSplines;
// NAME 2: the mailslot "\\.\mailslot\G\SplineDatabaseUpdate" (0x486AD0)
	void *																	Mailslot;
};

#endif //__SPLN_DB_H
