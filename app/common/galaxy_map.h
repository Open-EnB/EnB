// Earth & Beyond client (reconstruction): app\common\galaxy_map.h. Notes at the end of the file.
/*
**	The galaxy map's data objects (systems, sectors, gates, starbases), reference counted and
**	written to / read from a LinearBufferClass, and the map holding them (AllData and the Systems /
**	Sectors / Gates / Starbases hash maps of galaxy_map.cpp's asserts). The sector, gate and starbase
**	classes are only named so far.
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
	GalaxyMapSystemClass() : HashID(-1), Unknown38(1.0f), Unknown3C(3) {}	virtual ~GalaxyMapSystemClass() {}	// (inline: 0x837C70); the destructor 0x837F80 stores the vtable: explicit
	virtual int Get_Type() { return(5); }										// +0x08 0x837EC0
	virtual void Read(LinearBufferClass *buffer)
	{
		buffer->Read(&GalaxyMapID, 4);
		buffer->Read(&HashID, 4);
		char *name = buffer->Read_String();
		Name = name;
		delete name;
		buffer->Read(&Unknown20, 12);
		buffer->Read(&Unknown2C, 12);
		buffer->Read(&Unknown38, 4);
		buffer->Read(&Unknown3C, 4);
	}
	virtual void Write(LinearBufferClass *buffer)
	{
		buffer->Write(&GalaxyMapID, 4);
		buffer->Write(&HashID, 4);
		buffer->Write_String(Name.c_str());
		buffer->Write(&Unknown20, 12);
		buffer->Write(&Unknown2C, 12);
		buffer->Write(&Unknown38, 4);
		buffer->Write(&Unknown3C, 4);
	}
	virtual GalaxyMapSystemClass *As_System() { return(this); }				// +0x14 0x837EE0
	virtual int Get_Hash_ID() { return(HashID); }								// +0x24 0x837EA0

	static RefCountPtr<GalaxyMapSystemClass> Create();						// 0x837C70 (galaxy_map_client.cpp's 0x7A4FC0)  // NAME 2: what it does

	int HashID;																	// +0x0C  // NAME 2: Get_Hash_ID's
	std::string Name;															// +0x10  // NAME 1 [Name]: a string on the wire (Read_String)
	Vector3 Unknown20;															// +0x20
	Vector3 Unknown2C;															// +0x2C
	float Unknown38;															// +0x38 (1.0f)
	int Unknown3C;																// +0x3C (3)
};

// The sector, gate and starbase objects: only named so far (the map's Sectors / Gates / Starbases hold them)
class GalaxyMapSectorClass : public GalaxyMapDataClass {};	// NAME 2: galaxy_map.cpp's 'Sectors.find(sector->Get_Hash_ID())'
class GalaxyMapGateClass : public GalaxyMapDataClass {};	// NAME 2: galaxy_map.cpp's 'Gates.find(gate->Get_Hash_ID())'
class GalaxyMapStarbaseClass : public GalaxyMapDataClass {};	// NAME 2: galaxy_map.cpp's 'starbase_it == Starbases.end()'

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

#endif

/*
**	Reconstruction notes
**
**	client.exe: the base's destructor 0x837D50 asserts '!Is_In_Galaxy_Map()' on galaxy_map.h's line 161
**	(0xA1), the system's 0x837F80 inlines it. The base's vtable 0xB08B74: Delete_This, the deleting
**	destructor, Get_Type (-1), Read / Write (GalaxyMapID only; galaxy_map.cpp's 0x837C10 / 0x837C40),
**	four slots returning NULL (the system overrides the first with 'return this') and _purecall
**	(Get_Hash_ID). The system's Read / Write 0x8381B0 / 0x838320 (inline here, emitted after 0x837F80)
**	carry GalaxyMapID, HashID, the string, two 12-byte members and two 4-byte ones; Read deletes the
**	Read_String buffer with scalar delete (operator delete, as the client). The system's destructor
**	0x837F80 stores its own vtable first: declared, not implicit.
**
**	The map (TK16, GM1): its layout from the lookups 0x8378B0-0x837B30, its constructor 0x835DD0 and
**	Add_Data 0x836960 (vtable 0xB08B38 at +0x00; AllData's vector at +0x04; four SGI hash_maps of 0x18
**	bytes, bucket vectors' _First at +0x1C / +0x34 / +0x4C / +0x64, in the asserts' order;
**	MaxDataObjects at +0x74, zeroed by the constructor). Set_Galaxy_Map_ID (line 180) and
**	Remove_From_Galaxy_Map (line 185) are the data object's inlines that Add_Data, Remove_Data and
**	Clear expand. The class sits before the system in the client's header, but stays at the end here
**	(no asserts of its own pin it). The sector, gate and starbase classes are empty shells (their
**	RefCountPtr statics need complete types).
*/
