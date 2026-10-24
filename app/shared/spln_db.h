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
**	("\\.\mailslot\G\SplineDatabaseUpdate"). The client's asserts give spline_name, base_name,
**	alpha_clr, beta_clr, resolution, database_filename, EntryArray.
**
**	NOT THE CLIENT'S LAYOUT: VIDTEST's copy (ctor 0x41C660) is 0x850 bytes with a 0x200-entry table
**	and list heads; here the entries are a plain DynamicVectorClass, searched by name.
**
**	The file (VIDTEST 0x41F620, 0x41F7B0, 0x41F830, 0x41F960, 0x41FC20): chunk 0x800, 16 bytes
**	(version 0x100, entry count, 0, 0); chunk 0x808, the key names (count, then length-prefixed
**	strings: Dawn, Noon, ...); then one chunk 0x810 or 0x811 per entry: a 0x24-byte header (type,
**	name length, a flag byte at +8, four floats at +0x0C), the name, in 0x811 a length-prefixed
**	string, and the curve by type: 1 a colour (the entries name_R, name_G, name_B), 3 a linear
**	curve, 4 a Hermite spline, 2 a Hermite spline and two more names (0x811: two ids). A curve is a
**	looping byte, a key count, then per key point, time and extra (and for Hermite the in and out
**	tangents).
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 3 [spln_db.h]: header of the client's app\shared\spln_db.cpp (assert __FILE__)
#ifndef __SPLN_DB_H
#define __SPLN_DB_H

#include "vector.h"

class Curve1DClass;
class ChunkLoadClass;

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

	SplineDatabaseClass(const char *database_filename = 0);
	~SplineDatabaseClass(void);

// NAME 2: loads a .wsd (assert 'database_filename' is the client's argument name)
	bool						Load(const char *database_filename);
	void						Free(void);
	bool						Is_Loaded(void) const		{ return Loaded; }
	int						Get_Count(void) const		{ return EntryArray.Count(); }

// NAME 2: lookup by name (assert 'spline_name')
	Curve1DClass *			Get_Spline(const char *spline_name) const;
// NAME 2: name_R / name_G / name_B (VIDTEST 0x41D030; assert 'base_name')
	bool						Get_Color_Splines(const char *base_name, Curve1DClass **r, Curve1DClass **g,
									Curve1DClass **b) const;
// NAME 2: ours, the three curves at t
	bool						Evaluate_Color(const char *base_name, float t, float *r, float *g, float *b) const;

protected:
// NAME 2: one spline (the client's assert 'EntryArray[i]' names the array)
	struct EntryStruct
	{
		char *				Name;
		int					Type;
		bool					Flag;
		float					Params[4];
		Curve1DClass *		Curve;
	};

// NAME 2: reads chunk 0x810 / 0x811 (VIDTEST 0x41F960)
	bool						Load_Entry(ChunkLoadClass &cload);
// NAME 2: name lookup (VIDTEST 0x41CFF0)
	EntryStruct *			Find(const char *spline_name) const;

	DynamicVectorClass<EntryStruct *>	EntryArray;
	char *					Filename;
	bool						Loaded;
};

#endif //__SPLN_DB_H
