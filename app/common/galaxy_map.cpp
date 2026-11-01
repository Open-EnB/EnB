// Earth & Beyond client (reconstruction): app\common\galaxy_map.cpp. Notes at the end of the file.
#include "galaxy_map.h"
#include "..\shared\auxdata\linearbu.h"
#include "..\shared\unique_design_id_lookup.h"

// 0x835DD0
GalaxyMapClass::GalaxyMapClass() :
	MaxDataObjects(0)
{
}





// 0x836240
GalaxyMapClass::~GalaxyMapClass()
{
	Clear();
}

// 0x836590
void GalaxyMapClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&MaxDataObjects, 4);
}

// 0x8365C0
void GalaxyMapClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&MaxDataObjects, 4);
	if ((int)AllData.size() < MaxDataObjects) {
		AllData.resize(MaxDataObjects);
	}
}

// 0x836740
void GalaxyMapClass::Clear()
{
	for (std::vector<RefCountPtr<GalaxyMapDataClass> >::iterator it = AllData.begin(); it != AllData.end(); ++it) {
		if (*it) {
			(*it)->Remove_From_Galaxy_Map();
			*it = NULL;
		}
	}
	AllData.clear();
	Systems.clear();
	Sectors.clear();
	Gates.clear();
	Starbases.clear();
}






































// 0x836960
void GalaxyMapClass::Add_Data(const RefCountPtr<GalaxyMapDataClass> &data)
{
	unsigned int id = data->GalaxyMapID;
	if (id == GalaxyMapDataClass::NOT_IN_GALAXY_MAP) {
		id = MaxDataObjects++;
		data->Set_Galaxy_Map_ID(id);
	}
	FAIL_IF((int)id >= MaxDataObjects) {
		MaxDataObjects = id + 1;
	}

	if (AllData.size() <= id) {
		AllData.resize(id * 2 + 2);
	}

	G_ASSERT(AllData[id] == 0);
	AllData[id] = data;
}
// 0x836B70
void GalaxyMapClass::Remove_Data(unsigned int id)
{
	FAIL_IF(id >= AllData.size()) return;
	RefCountPtr<GalaxyMapDataClass> data = AllData[id];

	FAIL_IF(!data) return;

	GalaxyMapSystemClass *system = data->As_System();
	if (system != NULL) {
		std::hash_map<int, RefCountPtr<GalaxyMapSystemClass> >::iterator sys_it = Systems.find(system->Get_Hash_ID());
		FAIL_IF(sys_it == Systems.end()) return;
		Systems.erase(sys_it);
	} else {
		GalaxyMapSectorClass *sector = data->As_Sector();
		if (sector != NULL) {
			std::hash_map<int, RefCountPtr<GalaxyMapSectorClass> >::iterator sect_it = Sectors.find(sector->Get_Hash_ID());


			FAIL_IF(sect_it == Sectors.end()) return;
			Sectors.erase(sect_it);
		} else if (data->As_Gate() != NULL) {
			std::hash_map<int, RefCountPtr<GalaxyMapGateClass> >::iterator gate_it = Gates.find(data->Get_Hash_ID());




			FAIL_IF(gate_it == Gates.end()) return;
			Gates.erase(gate_it);
		} else if (data->As_Starbase() != NULL) {
			std::hash_map<int, RefCountPtr<GalaxyMapStarbaseClass> >::iterator starbase_it = Starbases.find(data->Get_Hash_ID());




			FAIL_IF(starbase_it == Starbases.end()) return;
			Starbases.erase(starbase_it);
		}
	}
	AllData[id] = NULL;
	data->Remove_From_Galaxy_Map();
}





// 0x836F30
void GalaxyMapClass::Add_System(const RefCountPtr<GalaxyMapSystemClass> &system)
{
	FAIL_IF(!system) return;
	FAIL_IF(Systems.find(system->Get_Hash_ID()) != Systems.end()) return;
	Add_Data(system);
	Systems[system->Get_Hash_ID()] = system;
}


// 0x837190
void GalaxyMapClass::Add_Sector(const RefCountPtr<GalaxyMapSectorClass> &sector)
{
	FAIL_IF(!sector) return;
	FAIL_IF(Sectors.find(sector->Get_Hash_ID()) != Sectors.end()) return;
	Add_Data(sector);
	Sectors[sector->Get_Hash_ID()] = sector;
}


// 0x8373F0
void GalaxyMapClass::Add_Gate(const RefCountPtr<GalaxyMapGateClass> &gate)
{
	FAIL_IF(!gate) return;
	FAIL_IF(Gates.find(gate->Get_Hash_ID()) != Gates.end()) return;
	Add_Data(gate);
	Gates[gate->Get_Hash_ID()] = gate;
}


// 0x837650
void GalaxyMapClass::Add_Starbase(const RefCountPtr<GalaxyMapStarbaseClass> &starbase)
{
	FAIL_IF(!starbase) return;
	FAIL_IF(Starbases.find(starbase->Get_Hash_ID()) != Starbases.end()) return;
	Add_Data(starbase);
	Starbases[starbase->Get_Hash_ID()] = starbase;
}

// 0x8378B0
const RefCountPtr<GalaxyMapDataClass> &GalaxyMapClass::Get_Data(unsigned int id)
{
	if (id >= AllData.size()) {
		static RefCountPtr<GalaxyMapDataClass> null_data;
		return(null_data);
	}
	return(AllData[id]);
}

// 0x837970
const RefCountPtr<GalaxyMapSystemClass> &GalaxyMapClass::Get_System(int hash_id)
{
	static RefCountPtr<GalaxyMapSystemClass> null_system;
	std::hash_map<int, RefCountPtr<GalaxyMapSystemClass> >::iterator sys_it = Systems.find(hash_id);
	if (sys_it == Systems.end()) {
		return(null_system);
	}
	return((*sys_it).second);
}

// 0x837A50
const RefCountPtr<GalaxyMapSectorClass> &GalaxyMapClass::Get_Sector(int hash_id)
{
	static RefCountPtr<GalaxyMapSectorClass> null_sector;
	std::hash_map<int, RefCountPtr<GalaxyMapSectorClass> >::iterator sect_it = Sectors.find(hash_id);
	if (sect_it == Sectors.end()) {
		return(null_sector);
	}
	return((*sect_it).second);
}

// 0x837B30
const RefCountPtr<GalaxyMapStarbaseClass> &GalaxyMapClass::Get_Starbase(int hash_id)
{
	static RefCountPtr<GalaxyMapStarbaseClass> null_starbase;
	std::hash_map<int, RefCountPtr<GalaxyMapStarbaseClass> >::iterator starbase_it = Starbases.find(hash_id);
	if (starbase_it == Starbases.end()) {
		return(null_starbase);
	}
	return((*starbase_it).second);
}

// 0x837C10
void GalaxyMapDataClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&GalaxyMapID, 4);
}

// 0x837C40
void GalaxyMapDataClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&GalaxyMapID, 4);
}

// 0x837C70
RefCountPtr<GalaxyMapSystemClass> GalaxyMapSystemClass::Create()
{
	return Create_NEW(new GalaxyMapSystemClass);
}

// 0x837F00
GalaxyMapSystemClass::GalaxyMapSystemClass() :
	HashID(-1),
	Unknown38(1.0f),
	Unknown3C(3)
{
}

// 0x838050
void GalaxyMapSystemClass::Set(int hash_id, const std::string &name, const Vector3 &unknown20, const Vector3 &unknown2c, float unknown38, int unknown3c)
{
	HashID = hash_id;
	Name = name;
	Unknown20 = unknown20;
	Unknown2C = unknown2c;
	Unknown38 = unknown38;
	Unknown3C = unknown3c;
}

// 0x8381B0
void GalaxyMapSystemClass::Read(LinearBufferClass *buffer)
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

// 0x838320
void GalaxyMapSystemClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&GalaxyMapID, 4);
	buffer->Write(&HashID, 4);
	buffer->Write_String(Name.c_str());
	buffer->Write(&Unknown20, 12);
	buffer->Write(&Unknown2C, 12);
	buffer->Write(&Unknown38, 4);
	buffer->Write(&Unknown3C, 4);
}

// 0x8383B0
RefCountPtr<GalaxyMapSectorClass> GalaxyMapSectorClass::Create()
{
	return Create_NEW(new GalaxyMapSectorClass);
}

// 0x8384A0
GalaxyMapSectorClass::GalaxyMapSectorClass() :
	HashID(-1),
	Unknown10(-1),
	Unknown24(-1)
{
}

// 0x8385F0
void GalaxyMapSectorClass::Set(int hash_id, int unknown10, const std::string &name, int unknown24, const Vector3 &unknown28, int unused)
{
	HashID = hash_id;
	Unknown10 = unknown10;
	Name = name;
	Unknown24 = unknown24;
	Unknown28 = unknown28;
}

// 0x838780
void GalaxyMapSectorClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&GalaxyMapID, 4);
	buffer->Read(&HashID, 4);
	buffer->Read(&Unknown10, 4);
	char *name = buffer->Read_String();
	Name = name;
	delete name;
	buffer->Read(&Unknown24, 4);
	buffer->Read(&Unknown28, 12);
}

// 0x8388E0
void GalaxyMapSectorClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&GalaxyMapID, 4);
	buffer->Write(&HashID, 4);
	buffer->Write(&Unknown10, 4);
	buffer->Write_String(Name.c_str());
	buffer->Write(&Unknown24, 4);
	buffer->Write(&Unknown28, 12);
}

// 0x838970
RefCountPtr<GalaxyMapGateClass> GalaxyMapGateClass::Create()
{
	return Create_NEW(new GalaxyMapGateClass);
}

// 0x838A60
GalaxyMapGateClass::GalaxyMapGateClass() :
	Unknown0C(0),
	Unknown10(0),
	Unknown14(UniqueDesignIDLookupClass::INVALID_ID)
{
}

// 0x838B50
void GalaxyMapGateClass::Set(int unknown0c, int unknown10, int unknown14)
{
	Unknown0C = unknown0c;
	Unknown10 = unknown10;
	Unknown14 = unknown14;
}

// 0x838B80
void GalaxyMapGateClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&GalaxyMapID, 4);
	buffer->Read(&Unknown0C, 4);
	buffer->Read(&Unknown10, 4);
	Unknown14 = UniqueDesignIDLookupClass::INVALID_ID;
}

// 0x838BD0
void GalaxyMapGateClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&GalaxyMapID, 4);
	buffer->Write(&Unknown0C, 4);
	buffer->Write(&Unknown10, 4);
}

// 0x838C20
RefCountPtr<GalaxyMapStarbaseClass> GalaxyMapStarbaseClass::Create()
{
	return Create_NEW(new GalaxyMapStarbaseClass);
}

// 0x838D10
GalaxyMapStarbaseClass::GalaxyMapStarbaseClass() :
	Unknown10(0)
{
}

// 0x838E50
void GalaxyMapStarbaseClass::Set(int hash_id, int unknown10, const char *name, int unknown24)
{
	HashID = hash_id;
	Name = name;
	Unknown10 = unknown10;
	Unknown24 = unknown24;
}

// 0x838F80
void GalaxyMapStarbaseClass::Read(LinearBufferClass *buffer)
{
	buffer->Read(&GalaxyMapID, 4);
	buffer->Read(&HashID, 4);
	char *name = buffer->Read_String();
	Name = name;
	delete name;
	buffer->Read(&Unknown10, 4);
	buffer->Read(&Unknown24, 4);
}

// 0x8390C0
void GalaxyMapStarbaseClass::Write(LinearBufferClass *buffer)
{
	buffer->Write(&GalaxyMapID, 4);
	buffer->Write(&HashID, 4);
	buffer->Write_String(Name.c_str());
	buffer->Write(&Unknown10, 4);
	buffer->Write(&Unknown24, 4);
}

/*
**	Reconstruction notes
**
**	client.exe: galaxy_map.cpp's object starts with the map's constructor 0x835DD0 (galaxy_map_client.cpp's
**	0x7A4830 calls it; the compiler's deleting destructor 0x835FA0 and the hash maps' destructors for its
**	unwinding, 0x835FD0 / 0x8360A0 / 0x836170, follow it; the Starbases one, 0x8364C0, follows the
**	destructor) and runs to 0x8390C0 (__FILE__ 0xBA97D4; the data classes' part below). The
**	constructor leaves the hash maps to their default constructors (SGI's 100 buckets):
**	writing Systems(100) ... inlines _M_initialize_buckets into Starbases instead of the client's Gates.
**	The destructor calls Clear (vtable 0xB08B38's second slot), which the client inlines there with
**	AllData.clear() and the maps' clear() out of line (inlined in Clear 0x836740 itself). Write / Read
**	0x836590 / 0x8365C0 carry MaxDataObjects only; Read grows AllData to it.
**
**	Add_Data 0x836960: an object not in the map yet gets the next id (galaxy_map.h's line-180 assert in
**	Set_Galaxy_Map_ID), FAIL_IF on line 98, AllData grown to id * 2 + 2, the assert on line 106. Its
**	shared epilogue (and Remove_Data's 0x836B70, FAIL_IFs on 112 / 115 / 120 / 128 / 136 / 144) came
**	with the constructor, destructor, Read / Write and Clear above it: with them missing both
**	functions repeated the epilogue after each exit (0.67 / 0.74; TK16, GM1). The size test is
**	written 'AllData.size() <= id' (cmp size, id; ja).
**
**	The lookups 0x8378B0-0x837B30 (AllData[id], Systems / Sectors / Starbases find; each returns a
**	function-local static null RefCountPtr when absent, the static declared inside the AllData branch,
**	at the top in the hash lookups: the client's block order), the base's Read / Write (out of line:
**	they precede the factory, the header's inline virtuals follow it) and the system's factory
**	0x837C70 (galaxy_map_client.cpp's 0x7A4FC0 reads the new system and adds it through Add_System
**	0x836F30, whose FAIL_IFs sit on the client's lines 159 / 160; the blank lines above it hold that
**	layout); Add_Sector / Add_Gate / Add_Starbase 0x837190 / 0x8373F0 / 0x837650 the same, FAIL_IFs on
**	169 / 170, 179 / 180, 189 / 190.
**
**	After the map, each data class in turn (GM1): Create, the constructor (inlined into Create,
**	which precedes it, yet kept: no caller), Set (no caller) and Read / Write, the class's inline
**	virtuals emitted between them: the system 0x837C70-0x838320, the sector 0x8383B0-0x8388E0, the
**	gate 0x838970-0x838BD0 and the starbase 0x838C20-0x8390C0; galaxy_map.cpp's own functions run
**	to 0x8390C0. The gate's Unknown14 starts as UniqueDesignIDLookupClass::INVALID_ID (0xAECC44, read
**	from memory: hence that header); the sector's Set takes a sixth argument it never reads.
*/
