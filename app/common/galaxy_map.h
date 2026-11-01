// Earth & Beyond client (reconstruction): app\common\galaxy_map.h. Notes at the end of the file.
/*
**	The galaxy map's data objects (systems, sectors, gates, starbases), reference counted and
**	written to / read from a LinearBufferClass. So far only the base and the system, as far as
**	galaxy_map.cpp's 0x837C10-0x837C70 and their vtables need them; the map itself (its AllData array and the
**	Systems / Sectors / Gates / Starbases hash maps, galaxy_map.cpp's asserts) is not declared yet.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_H
#define __GALAXY_MAP_H

#include <string>
#include "vector3.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\linearbu.h"

class LinearBufferClass;
class GalaxyMapSystemClass;































































































































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
	virtual void *Unknown_18() { return(NULL); }								// +0x18 0x837E10  // NAME 1: neutral (D10)
	virtual void *Unknown_1C() { return(NULL); }								// +0x1C 0x837E30  // NAME 1: neutral (D10)
	virtual void *Unknown_20() { return(NULL); }								// +0x20 0x837E50  // NAME 1: neutral (D10)
	virtual int Get_Hash_ID() = 0;												// +0x24 _purecall  // NAME 3: galaxy_map.cpp's 'Systems.find(system->Get_Hash_ID())'

	bool Is_In_Galaxy_Map() { return(GalaxyMapID != NOT_IN_GALAXY_MAP); }	// NAME 3: the destructor's G_ASSERT

	int GalaxyMapID;															// +0x08  // NAME 3: the asserts'
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
*/
