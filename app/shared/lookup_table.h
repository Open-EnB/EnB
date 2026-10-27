/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\lookup_table.h (name assumed from
**	lookup_table.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\lookup_table.cpp')
**
**	Named lookup tables read from an ini file: each section is a table, its Type entry says how it
**	maps an index to a value. 'continuous' tables hold a list of values (Values=a,b,c) indexed by the
**	whole part of the index; 'floor', 'ceiling', 'closest' and 'interpolated' tables hold samples
**	(index=value entries) and take the sample at or below, at or above, nearest to, or interpolate
**	between the two around the index. The tables are kept by section name in an SGI hash_map
**	(libs/sgistl, D87). Every class is reference counted (wwlib's RefCountClass, E&B's ref_ptr.h).
**	The client names the members through its asserts (_LookupTables, Tables, the self-test's
**	floor / ceiling / closest / interpolate); the classes are named by convention (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __LOOKUP_TABLE_H  // NAME 3 [lookup_table.h]: header name assumed from the client's lookup_table.cpp
#define __LOOKUP_TABLE_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"

struct INISection;

// One table: maps an index to a value (vtable 0xAEC798, its Lookup a pure virtual)
class LookupTableClass : public RefCountClass  // NAME 3: by convention (D24) from the client's file lookup_table.cpp
{
public:
	virtual float Lookup(float index) = 0;													// slot 2  // NAME 2: what it does (the self-test's floor->Lookup(index))

	static RefCountPtr<LookupTableClass> Create(const std::string &type, INISection *section);	// 0x4ED390: by the section's Type  // NAME 2: what it does
};

// Type=continuous: Values=v0,v1,...; Lookup(i) is Values[(int)i] (vtable 0xAEC788, 0x18 bytes)
class ContinuousLookupTableClass : public LookupTableClass  // NAME 2: the ini's type name
{
public:
	virtual ~ContinuousLookupTableClass() {}												// 0x4EDA70
	virtual float Lookup(float index);														// 0x4ED9A0

	static RefCountPtr<LookupTableClass> Create(const std::string &values);			// 0x4ED660  // NAME 2: what it does

	std::vector<float> Values;																	// +0x08  // NAME 2: the ini key
};

// One index=value entry of a sampled table
struct LookupSampleStruct  // NAME 2: what it holds (8 bytes)
{
	float Index;																						// +0x00  // NAME 2: what it holds
	float Value;																						// +0x04  // NAME 2: what it holds
};

// The sampled tables' base: the section's index=value entries sorted by index, duplicates removed
// (vtable 0xAEC7B8, its Lookup a pure virtual; 0x18 bytes)
class SampledLookupTableClass : public LookupTableClass  // NAME 2: what it holds
{
public:
	SampledLookupTableClass(INISection *section);											// 0x4EDBE0
	virtual ~SampledLookupTableClass() {}													// 0x4EDE90

protected:
	void Find_Samples(float index, LookupSampleStruct &lower, LookupSampleStruct &upper);	// inline  // NAME 2: what it does

	std::vector<LookupSampleStruct> Samples;													// +0x08  // NAME 2: what it holds
};

// Type=floor: the sample at or below the index (vtable 0xAEC7A8)
class FloorLookupTableClass : public SampledLookupTableClass  // NAME 2: the ini's type name
{
public:
	FloorLookupTableClass(INISection *section) : SampledLookupTableClass(section) {}
	virtual float Lookup(float index);														// 0x4EDED0

	static RefCountPtr<LookupTableClass> Create(INISection *section);					// 0x4EDB00  // NAME 2: what it does
};

// Type=ceiling: the sample at or above the index (vtable 0xAEC7C8)
class CeilingLookupTableClass : public SampledLookupTableClass  // NAME 2: the ini's type name
{
public:
	CeilingLookupTableClass(INISection *section) : SampledLookupTableClass(section) {}
	virtual float Lookup(float index);														// 0x4EE100

	static RefCountPtr<LookupTableClass> Create(INISection *section);					// 0x4EE020  // NAME 2: what it does
};

// Type=closest: the sample nearest to the index (vtable 0xAEC7D8)
class ClosestLookupTableClass : public SampledLookupTableClass  // NAME 2: the ini's type name
{
public:
	ClosestLookupTableClass(INISection *section) : SampledLookupTableClass(section) {}
	virtual float Lookup(float index);														// 0x4EE380

	static RefCountPtr<LookupTableClass> Create(INISection *section);					// 0x4EE2A0  // NAME 2: what it does
};

// Type=interpolated: linear between the samples around the index (vtable 0xAEC7E8)
class InterpolatedLookupTableClass : public SampledLookupTableClass  // NAME 2: the ini's type name
{
public:
	InterpolatedLookupTableClass(INISection *section) : SampledLookupTableClass(section) {}
	virtual float Lookup(float index);														// 0x4EE630

	static RefCountPtr<LookupTableClass> Create(INISection *section);					// 0x4EE550  // NAME 2: what it does
};

// The tables of one ini file by section name (vtable 0xAEC77C, 0x20 bytes)
class LookupTableSetClass : public RefCountClass  // NAME 2: what it holds
{
public:
	LookupTableSetClass();																			// 0x4ECC10
	virtual ~LookupTableSetClass();																// 0x4ECE50

	float Lookup(const std::string &name, float index);									// 0x4ED0B0  // NAME 2: what it does
	RefCountPtr<LookupTableClass> Get(const std::string &name);							// 0x4ED220  // NAME 2: what it does

	static RefCountPtr<LookupTableSetClass> Load(const std::string &filename);		// 0x4EC7D0  // NAME 2: what it does

	std::hash_map<std::string, RefCountPtr<LookupTableClass> > Tables;					// +0x08
};

// The loaded tables (lookup_table.cpp's _LookupTables, 0xBE566C) and their access
class LookupTableManagerClass  // NAME 2: what it does
{
public:
	static void Init(const std::string &filename);											// 0x4EC260: loads, then runs the self-test  // NAME 2: what it does
	static void Shutdown();																			// 0x4EC6B0  // NAME 2: what it does
	static float Lookup(const std::string &name, float index);							// 0x4EC710  // NAME 2: what it does
	static RefCountPtr<LookupTableClass> Get_Table(const std::string &name);			// 0x4EC760  // NAME 2: what it does
};

#endif //__LOOKUP_TABLE_H
