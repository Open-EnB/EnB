// Earth & Beyond client (reconstruction)
/*
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
class FileClass;
class ChunkLoadClass;
class ChunkSaveClass;
struct SplineFileHeaderStruct;
class SplineFloatArrayClass;
class SplineColorArrayClass;

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

// NAME 2: the entry packed for a message (0x4895E0 size, 0x489660 pack; asserts 'false' 2959, 3074)
	int						Get_Packed_Size(void);
	char *					Pack(char *buffer);
// NAME 2: the inverse: a packed entry read into this one (0x489830), or a new entry from one (0x489480)
	void						Unpack(char **buffer);
	SplineEntryClass(char **buffer);

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
// NAME 2: writes the database to database_filename, or Filename (0x484D00)
	bool						Save(const char *database_filename);
// NAME 2: loads Filename again (0x487900, assert 'database_filename'; the mailslot's reload)
	bool						Reload(void);
	bool						Has_Splines(void) const		{ return HasSplines; }
	int						Get_Count(void)				{ return Splines.Num_Records(); }

// NAME 2: prints "name description" or "name <not present>" (0x485020, assert 'SplineName')
	void						Print(FileClass *file, unsigned int depth, const char *SplineName);
// NAME 2: an entry's type (0x485110, assert 'spline_name')
	bool						Get_Type(const char *spline_name, int *type);
// NAME 2: a blend entry's two entries' names, resolved once (0x4851D0, assert 'spline_name')
	bool						Get_Blend_Names(const char *spline_name, char **alpha_clr, char **beta_clr);
// NAME 2: an entry's curve (0x485380, assert 'spline_name')
	bool						Get_Curve(const char *spline_name, Curve1DClass **curve);
// NAME 2: the curves of name_R, name_G, name_B (0x485440; the client's lighting 0x4F2960, 0x4F34C0)
	bool						Get_Color_Curves(const char *base_name, Curve1DClass **r, Curve1DClass **g,
									Curve1DClass **b);
// NAME 2: an entry's curve sampled at resolution evenly spaced times in [0, 1) (0x485620), and the
// colour curves name_R, _G, _B the same way (0x485790); asserts 'resolution >= 0'
	bool						Evaluate_Curve(const char *spline_name, SplineFloatArrayClass &values, int resolution);
	bool						Evaluate_Color(const char *base_name, SplineColorArrayClass &colors, int resolution);
// NAME 2: new entries for an editor: a linear (0x485990) or Catmull-Rom (0x485C10) curve, the three
// hidden name_R/_G/_B curves and their colour entry (0x485E80, 0x486040, asserts 'base_name'), a colour
// entry over three keys (0x487400, assert 'name')
	bool						Create_Linear(const char *spline_name, const char *description, Curve1DClass **curve, bool hidden);
	bool						Create_Hermite(const char *spline_name, const char *description, Curve1DClass **curve, bool hidden);
	bool						Create_Linear_Color(const char *base_name, const char *description, Curve1DClass **r, Curve1DClass **g, Curve1DClass **b);
	bool						Create_Hermite_Color(const char *base_name, const char *description, Curve1DClass **r, Curve1DClass **g, Curve1DClass **b);
	bool						Create_Color(const char *name, const char *description, unsigned int red_key, unsigned int green_key, unsigned int blue_key);
// NAME 2: a blend entry over two others (0x486200, asserts 'spline_name', 'alpha_clr', 'beta_clr'); deletes an entry and a colour's components (0x4864F0)
	bool						Create_Blend(const char *spline_name, const char *description, const char *alpha_clr, const char *beta_clr, Curve1DClass **curve);
	bool						Delete(const char *spline_name);
// NAME 2: the key names (chunk 0x808): add 0x4867B0, find 0x4868A0, get 0x486930, remove 0x486970
	int						Add_Key_Name(const char *name);
	int						Find_Key_Name(const char *name);
	bool						Get_Key_Name(unsigned int index, char **name);
	bool						Remove_Key_Name(unsigned int index);
// NAME 2: the entries' names in list order, hidden ones only with ShowHidden (0x4869C0, 0x486A40)
	const char *			Get_First_Name(void);
	const char *			Get_Next_Name(void);
// NAME 2: polls the mailslot and reloads when an editor wrote to it (0x486AD0)
	bool						Update(void);
// NAME 2: lookups by key (0x486C40, 0x486CE0), by curve (0x486C90)
	SplineEntryClass *	Find(unsigned int key);
	SplineEntryClass *	Find(Curve1DClass *curve);
	Curve1DClass *			Get_Curve(unsigned int key);
// NAME 2: names to keys (0x486D30), keys to entries (0x486D80), a name's key (0x4873E0)
	void						Get_Keys(unsigned int count, const char **names, unsigned int *keys);
	void						Get_Entries(unsigned int count, const unsigned int *keys, SplineEntryClass **entries);
	static unsigned int	Get_Key(const char *name);
// NAME 2: entries packed for a message (0x486E10 size, assert 'entry_array[count]'; 0x486EF0, assert 'EntryArray[i]')
	static int				Get_Packed_Size(unsigned int count, SplineEntryClass **entry_array);
	static void				Pack(int count, SplineEntryClass **EntryArray, char *buffer);
// NAME 2: packed entries added or merged (0x486F60, assert 'false' 1409)
	void						Unpack(char *buffer);
// NAME 2: lookup by name (0x486BB0, assert 'spline_name != NULL')
	SplineEntryClass *	Find(const char *spline_name, int unused = 0);	// NAME 1 [unused]: the client's copy takes a second argument (ret 8) it never reads

protected:
// NAME 2: deletes the entries (0x487190)
	void						Free_Splines(void);
// NAME 2: deletes the key names but entry 0 (0x487360)
	void						Free_Key_Names(void);
// NAME 2: the save side (0x487B20, 0x487E70, 0x487F00, 0x4881F0, 0x4883A0, 0x488FB0): chunk 0x800, 0x808, one 0x811 per entry
	bool						Save_Chunks(ChunkSaveClass &csave);
	bool						Save_Header(ChunkSaveClass &csave);
	static bool				Save_Entry(ChunkSaveClass &csave, SplineEntryClass *entry);
	static bool				Save_Curve(ChunkSaveClass &csave, SplineEntryClass *entry);
	static bool				Save_Hermite(ChunkSaveClass &csave, SplineEntryClass *entry);
	static bool				Write_String(ChunkSaveClass &csave, const char *string);
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
