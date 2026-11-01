// Earth & Beyond client (reconstruction): app\common\galaxy_map.h. Notes at the end of the file.
/*
**	The galaxy map's data objects (systems, sectors, gates, starbases), reference counted and
**	written to / read from a LinearBufferClass, and the map holding them (AllData and the Systems /
**	Sectors / Gates / Starbases hash maps of galaxy_map.cpp's asserts). Each data class has a
**	Create factory and a Set; galaxy_map_client.cpp (0x7A4830-0x7A5260) builds and fills the map.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_H
#define __GALAXY_MAP_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "vector3.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\linearbu.h"

class LinearBufferClass;
class GalaxyMapSystemClass;
class GalaxyMapSectorClass;
class GalaxyMapGateClass;
class GalaxyMapStarbaseClass;


























































































































// The base of the map's data objects (vtable 0xB08B74; destructor 0x837D50, deleting 0x837E70)
class GalaxyMapDataClass : public RefCountClass	// NAME 2: the map's 'AllData' (galaxy_map.h's asserts) holds them
{
public:
	enum { NOT_IN_GALAXY_MAP = -1 };	// NAME 3: the asserts' 'GalaxyMapID != NOT_IN_GALAXY_MAP'

	GalaxyMapDataClass() : GalaxyMapID(NOT_IN_GALAXY_MAP) {}	// (inline: 0x837C70)
	virtual ~GalaxyMapDataClass()
	{
		G_ASSERT(!Is_In_Galaxy_Map());
	}
	virtual int Get_Type() { return(-1); }										// +0x08 0x837DD0  // NAME 1: what it returns (the system's 5)
	virtual void Read(LinearBufferClass *buffer);								// +0x0C 0x837C10  // NAME 2 [Read, buffer]: LinearBufferClass::Read of GalaxyMapID
	virtual void Write(LinearBufferClass *buffer);								// +0x10 0x837C40  // NAME 2 [Write, buffer]: LinearBufferClass::Write of GalaxyMapID
	virtual GalaxyMapSystemClass *As_System() { return(NULL); }				// +0x14 0x837DF0 (the system's 0x837EE0 returns this)  // NAME 2: the system's override
	virtual GalaxyMapSectorClass *As_Sector() { return(NULL); }				// +0x18 0x837E10  // NAME 2: Remove_Data 0x836B70 calls the sector's Get_Hash_ID through it (Sectors)
	virtual GalaxyMapGateClass *As_Gate() { return(NULL); }					// +0x1C 0x837E30  // NAME 2: Remove_Data's Gates branch
	virtual GalaxyMapStarbaseClass *As_Starbase() { return(NULL); }			// +0x20 0x837E50  // NAME 2: Remove_Data's Starbases branch
	virtual int Get_Hash_ID() = 0;												// +0x24 _purecall  // NAME 3: galaxy_map.cpp's 'Systems.find(system->Get_Hash_ID())'

	bool Is_In_Galaxy_Map() { return(GalaxyMapID != NOT_IN_GALAXY_MAP); }	// NAME 3: the destructor's G_ASSERT

	int GalaxyMapID;															// +0x08  // NAME 3: the asserts'





	void Set_Galaxy_Map_ID(int id) { G_ASSERT(GalaxyMapID == NOT_IN_GALAXY_MAP); GalaxyMapID = id; }	// NAME 2: the assert's (line 180; Add_Data 0x836960)




	void Remove_From_Galaxy_Map() { G_ASSERT(GalaxyMapID != NOT_IN_GALAXY_MAP); GalaxyMapID = NOT_IN_GALAXY_MAP; }	// NAME 3: the assert's (line 185; Remove_Data, the map's destructor)
};

// A system (vtable 0xB08B44, 0x40 bytes; destructor 0x837F80, deleting 0x837F50)
class GalaxyMapSystemClass : public GalaxyMapDataClass	// NAME 2: 0x7A4FC0 hands it to the map's Add_System 0x836F30 (FAIL_IF '!system')
{
public:
	GalaxyMapSystemClass();	virtual ~GalaxyMapSystemClass() {}	// 0x837F00 (galaxy_map.cpp; Create 0x837C70 inlines it); the destructor 0x837F80 stores the vtable: explicit
	virtual int Get_Type() { return(5); }										// +0x08 0x837EC0
	virtual void Read(LinearBufferClass *buffer);								// +0x0C 0x8381B0
	virtual void Write(LinearBufferClass *buffer);								// +0x10 0x838320
	virtual GalaxyMapSystemClass *As_System() { return(this); }				// +0x14 0x837EE0
	virtual int Get_Hash_ID() { return(HashID); }								// +0x24 0x837EA0

	void Set(int hash_id, const std::string &name, const Vector3 &unknown20, const Vector3 &unknown2c, float unknown38, int unknown3c);	// 0x838050 (no caller)  // NAME 1 [Set, hash_id, name]: stores the members Read carries
	static RefCountPtr<GalaxyMapSystemClass> Create();						// 0x837C70 (galaxy_map_client.cpp's 0x7A4FC0)  // NAME 2: what it does

	int HashID;																	// +0x0C  // NAME 2: Get_Hash_ID's
	std::string Name;															// +0x10  // NAME 1 [Name]: a string on the wire (Read_String)
	Vector3 Unknown20;															// +0x20
	Vector3 Unknown2C;															// +0x2C
	float Unknown38;															// +0x38 (1.0f)
	int Unknown3C;																// +0x3C (3)
};

// A sector (vtable 0xB08BA4, 0x34 bytes; destructor 0x838520, deleting 0x8384F0)
class GalaxyMapSectorClass : public GalaxyMapDataClass	// NAME 2: galaxy_map.cpp's 'Sectors.find(sector->Get_Hash_ID())'
{
public:
	GalaxyMapSectorClass();	virtual ~GalaxyMapSectorClass() {}				// 0x8384A0 (galaxy_map.cpp; Create 0x8383B0 inlines it); the destructor stores the vtable: explicit
	virtual int Get_Type() { return(Unknown24); }								// +0x08 0x838480
	virtual void Read(LinearBufferClass *buffer);								// +0x0C 0x838780
	virtual void Write(LinearBufferClass *buffer);								// +0x10 0x8388E0
	virtual GalaxyMapSectorClass *As_Sector() { return(this); }				// +0x18 0x838440
	virtual int Get_Hash_ID() { return(HashID); }								// +0x24 0x838460

	void Set(int hash_id, int unknown10, const std::string &name, int unknown24, const Vector3 &unknown28, int unused);	// 0x8385F0 (no caller; ret 0x18: a sixth argument it never reads)  // NAME 1 [Set, hash_id, name]: as the system's
	static RefCountPtr<GalaxyMapSectorClass> Create();						// 0x8383B0 (galaxy_map_client.cpp's 0x7A50A0)  // NAME 2: as the system's

	int HashID;																	// +0x0C (-1)  // NAME 2: Get_Hash_ID's
	int Unknown10;																// +0x10 (-1)
	std::string Name;															// +0x14  // NAME 1 [Name]: a string on the wire, as the system's
	int Unknown24;																// +0x24 (-1; Get_Type returns it)
	Vector3 Unknown28;															// +0x28
};

// A gate (vtable 0xB08BD4, 0x18 bytes; destructor 0x838AD0 (implicit), deleting 0x838AA0)
class GalaxyMapGateClass : public GalaxyMapDataClass	// NAME 2: galaxy_map.cpp's 'Gates.find(gate->Get_Hash_ID())'
{
public:
	GalaxyMapGateClass();														// 0x838A60 (galaxy_map.cpp; Create 0x838970 inlines it)
	virtual int Get_Type() { return(9); }										// +0x08 0x838A40
	virtual void Read(LinearBufferClass *buffer);								// +0x0C 0x838B80
	virtual void Write(LinearBufferClass *buffer);								// +0x10 0x838BD0
	virtual GalaxyMapGateClass *As_Gate() { return(this); }					// +0x1C 0x838A00
	virtual int Get_Hash_ID() { return(GalaxyMapID); }							// +0x24 0x838A20 (the map id: a gate has no hash id of its own)

	void Set(int unknown0c, int unknown10, int unknown14);						// 0x838B50 (no caller)  // NAME 1 [Set]: as the system's
	static RefCountPtr<GalaxyMapGateClass> Create();							// 0x838970  // NAME 2: as the system's

	int Unknown0C;																// +0x0C (0)
	int Unknown10;																// +0x10 (0)
	int Unknown14;																// +0x14 (UniqueDesignIDLookupClass::INVALID_ID 0xAECC44: the constructor and Read set it, Write doesn't carry it)
};

// A starbase (vtable 0xB08C04, 0x28 bytes; destructor 0x838D80, deleting 0x838D50)
class GalaxyMapStarbaseClass : public GalaxyMapDataClass	// NAME 2: galaxy_map.cpp's 'starbase_it == Starbases.end()'
{
public:
	GalaxyMapStarbaseClass();	virtual ~GalaxyMapStarbaseClass() {}			// 0x838D10 (galaxy_map.cpp; Create 0x838C20 inlines it); the destructor stores the vtable: explicit
	virtual int Get_Type() { return(10); }										// +0x08 0x838CF0
	virtual void Read(LinearBufferClass *buffer);								// +0x0C 0x838F80
	virtual void Write(LinearBufferClass *buffer);								// +0x10 0x8390C0
	virtual GalaxyMapStarbaseClass *As_Starbase() { return(this); }			// +0x20 0x838CB0
	virtual int Get_Hash_ID() { return(HashID); }								// +0x24 0x838CD0

	void Set(int hash_id, int unknown10, const char *name, int unknown24);		// 0x838E50 (no caller)  // NAME 1 [Set, hash_id, name]: as the system's
	static RefCountPtr<GalaxyMapStarbaseClass> Create();						// 0x838C20  // NAME 2: as the system's

	int HashID;																	// +0x0C (not initialised)  // NAME 2: Get_Hash_ID's
	int Unknown10;																// +0x10 (0)
	std::string Name;															// +0x14  // NAME 1 [Name]: a string on the wire, as the system's
	int Unknown24;																// +0x24 (not initialised)
};

// The map (vtable 0xB08B38: the deleting destructor 0x835FA0, Clear 0x836740; 0x78 bytes)
class GalaxyMapClass	// NAME 2: holds AllData and the Systems / Sectors / Gates / Starbases of galaxy_map.cpp's asserts
{
public:
	GalaxyMapClass();																// 0x835DD0 (galaxy_map_client.cpp's 0x7A4830)
	virtual ~GalaxyMapClass();														// 0x836240 (calls Clear: the client inlines it)
	virtual void Clear();															// +0x04 0x836740  // NAME 1: empties AllData and the four maps

	void Write(LinearBufferClass *buffer);											// 0x836590  // NAME 2: LinearBufferClass::Write of MaxDataObjects, as the data's Write
	void Read(LinearBufferClass *buffer);											// 0x8365C0 (galaxy_map_client.cpp's 0x7A4C50)  // NAME 2: as the data's Read; grows AllData to MaxDataObjects

	void Add_System(const RefCountPtr<GalaxyMapSystemClass> &system);				// 0x836F30 (galaxy_map_client.cpp's 0x7A4FC0)  // NAME 2: FAIL_IF 'Systems.find(system->Get_Hash_ID()) != Systems.end()'
	void Add_Sector(const RefCountPtr<GalaxyMapSectorClass> &sector);				// 0x837190  // NAME 2: FAIL_IF 'Sectors.find(sector->Get_Hash_ID()) != Sectors.end()'
	void Add_Gate(const RefCountPtr<GalaxyMapGateClass> &gate);						// 0x8373F0  // NAME 2: FAIL_IF 'Gates.find(gate->Get_Hash_ID()) != Gates.end()'
	void Add_Starbase(const RefCountPtr<GalaxyMapStarbaseClass> &starbase);			// 0x837650  // NAME 2: FAIL_IF 'Starbases.find(starbase->Get_Hash_ID()) != Starbases.end()'
	void Remove_Data(unsigned int id);											// 0x836B70 (galaxy_map_client.cpp's 0x7A5260)  // NAME 2: FAIL_IF 'id >= AllData.size()', then the four maps' erase
	void Add_Data(const RefCountPtr<GalaxyMapDataClass> &data);						// 0x836960  // NAME 2: Add_System's first call, before Systems[]

	const RefCountPtr<GalaxyMapDataClass> &Get_Data(unsigned int id);				// 0x8378B0 (galaxy_map_client.cpp's 0x7A4C50)  // NAME 2: returns AllData[id]
	const RefCountPtr<GalaxyMapSystemClass> &Get_System(int hash_id);				// 0x837970  // NAME 2: Systems.find
	const RefCountPtr<GalaxyMapSectorClass> &Get_Sector(int hash_id);				// 0x837A50  // NAME 2: Sectors.find
	const RefCountPtr<GalaxyMapStarbaseClass> &Get_Starbase(int hash_id);			// 0x837B30  // NAME 2: Starbases.find

	std::vector<RefCountPtr<GalaxyMapDataClass> > AllData;							// +0x04  // NAME 3: the asserts'
	std::hash_map<int, RefCountPtr<GalaxyMapSystemClass> > Systems;					// +0x14  // NAME 3: the asserts'
	std::hash_map<int, RefCountPtr<GalaxyMapSectorClass> > Sectors;					// +0x2C  // NAME 3: the asserts'
	std::hash_map<int, RefCountPtr<GalaxyMapGateClass> > Gates;						// +0x44  // NAME 3: the asserts'
	std::hash_map<int, RefCountPtr<GalaxyMapStarbaseClass> > Starbases;				// +0x5C  // NAME 3: the asserts'
	int MaxDataObjects;																// +0x74  // NAME 3: Add_Data's FAIL_IF '(int)id >= MaxDataObjects'
};

// The map's Systems in turn (galaxy_map.cpp 0x839140-0x839310; 0x10 bytes)
class GalaxyMapSystemIteratorClass	// NAME 1: an iterator over GalaxyMapClass's Systems
{
public:
	GalaxyMapSystemIteratorClass(GalaxyMapClass *map);											// 0x839140  // NAME 1 [map]: what it walks
	bool Next();																		// 0x839220 (false at the end)  // NAME 2: what it does
	void First();																		// 0x839310 (no caller)  // NAME 2: what it does
	void Reset() { It = Map->Systems.begin(); if (It == Map->Systems.end()) { Current = NULL; } else { Current = (*It).second; } }	// (the constructor's: inline, it calls begin out of line)  // NAME 2: what it does

	GalaxyMapClass *Map;																// +0x00  // NAME 2: what it walks
	std::hash_map<int, RefCountPtr<GalaxyMapSystemClass> >::const_iterator It;				// +0x04  // NAME 1: the position
	RefCountPtr<GalaxyMapSystemClass> Current;										// +0x0C  // NAME 1: the entry at It, NULL at the end
};

// The map's Sectors in turn (galaxy_map.cpp 0x8393D0-0x8395A0; 0x10 bytes)
class GalaxyMapSectorIteratorClass	// NAME 1: an iterator over GalaxyMapClass's Sectors
{
public:
	GalaxyMapSectorIteratorClass(GalaxyMapClass *map);											// 0x8393D0  // NAME 1 [map]: what it walks
	bool Next();																		// 0x8394B0 (false at the end)  // NAME 2: what it does
	void First();																		// 0x8395A0 (no caller)  // NAME 2: what it does
	void Reset() { It = Map->Sectors.begin(); if (It == Map->Sectors.end()) { Current = NULL; } else { Current = (*It).second; } }	// (the constructor's: inline, it calls begin out of line)  // NAME 2: what it does

	GalaxyMapClass *Map;																// +0x00  // NAME 2: what it walks
	std::hash_map<int, RefCountPtr<GalaxyMapSectorClass> >::const_iterator It;				// +0x04  // NAME 1: the position
	RefCountPtr<GalaxyMapSectorClass> Current;										// +0x0C  // NAME 1: the entry at It, NULL at the end
};

// The map's Gates in turn (galaxy_map.cpp 0x839660-0x839830; 0x10 bytes)
class GalaxyMapGateIteratorClass	// NAME 1: an iterator over GalaxyMapClass's Gates
{
public:
	GalaxyMapGateIteratorClass(GalaxyMapClass *map);											// 0x839660  // NAME 1 [map]: what it walks
	bool Next();																		// 0x839740 (false at the end)  // NAME 2: what it does
	void First();																		// 0x839830 (no caller)  // NAME 2: what it does
	void Reset() { It = Map->Gates.begin(); if (It == Map->Gates.end()) { Current = NULL; } else { Current = (*It).second; } }	// (the constructor's: inline, it calls begin out of line)  // NAME 2: what it does

	GalaxyMapClass *Map;																// +0x00  // NAME 2: what it walks
	std::hash_map<int, RefCountPtr<GalaxyMapGateClass> >::const_iterator It;				// +0x04  // NAME 1: the position
	RefCountPtr<GalaxyMapGateClass> Current;										// +0x0C  // NAME 1: the entry at It, NULL at the end
};

// The map's Starbases in turn (galaxy_map.cpp 0x8398F0-0x839AC0; 0x10 bytes)
class GalaxyMapStarbaseIteratorClass	// NAME 1: an iterator over GalaxyMapClass's Starbases
{
public:
	GalaxyMapStarbaseIteratorClass(GalaxyMapClass *map);											// 0x8398F0  // NAME 1 [map]: what it walks
	bool Next();																		// 0x8399D0 (false at the end)  // NAME 2: what it does
	void First();																		// 0x839AC0 (no caller)  // NAME 2: what it does
	void Reset() { It = Map->Starbases.begin(); if (It == Map->Starbases.end()) { Current = NULL; } else { Current = (*It).second; } }	// (the constructor's: inline, it calls begin out of line)  // NAME 2: what it does

	GalaxyMapClass *Map;																// +0x00  // NAME 2: what it walks
	std::hash_map<int, RefCountPtr<GalaxyMapStarbaseClass> >::const_iterator It;				// +0x04  // NAME 1: the position
	RefCountPtr<GalaxyMapStarbaseClass> Current;										// +0x0C  // NAME 1: the entry at It, NULL at the end
};

#endif

/*
**	Reconstruction notes
**
**	client.exe: the base's destructor 0x837D50 asserts '!Is_In_Galaxy_Map()' on galaxy_map.h's line 161
**	(0xA1), the system's 0x837F80 inlines it. The base's vtable 0xB08B74: Delete_This, the deleting
**	destructor, Get_Type (-1), Read / Write (GalaxyMapID only; galaxy_map.cpp's 0x837C10 / 0x837C40),
**	four slots returning NULL (the system overrides the first with 'return this') and _purecall
**	(Get_Hash_ID). The system's Read / Write 0x8381B0 / 0x838320 (galaxy_map.cpp's, after Set)
**	carry GalaxyMapID, HashID, the string, two 12-byte members and two 4-byte ones; Read deletes the
**	Read_String buffer with scalar delete (operator delete, as the client). The system's destructor
**	0x837F80 stores its own vtable first: declared, not implicit; so do the sector's and the
**	starbase's (0x838520 / 0x838D80); the gate's 0x838AD0 doesn't (implicit). Each class's
**	constructor, Set, Read and Write are galaxy_map.cpp's (GM1); its other virtuals are inline here.
**
**	The map (TK16, GM1): its layout from the lookups 0x8378B0-0x837B30, its constructor 0x835DD0 and
**	Add_Data 0x836960 (vtable 0xB08B38 at +0x00; AllData's vector at +0x04; four SGI hash_maps of 0x18
**	bytes, bucket vectors' _First at +0x1C / +0x34 / +0x4C / +0x64, in the asserts' order;
**	MaxDataObjects at +0x74, zeroed by the constructor). Set_Galaxy_Map_ID (line 180) and
**	Remove_From_Galaxy_Map (line 185) are the data object's inlines that Add_Data, Remove_Data and
**	Clear expand. The class sits before the system in the client's header, but stays at the end here
**	(no asserts of its own pin it).
*/
