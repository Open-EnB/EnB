// Earth & Beyond client (reconstruction): app\common\galaxy_map.cpp. Notes at the end of the file.
#include "galaxy_map.h"
#include "..\shared\auxdata\linearbu.h"








































































































// NOT EXACT: 0.74, the client sends the six failure exits to one shared epilogue (jmp), this form repeats it after each; nesting the first two FAIL_IFs as {} else {} doesn't share it either
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

/*
**	Reconstruction notes
**
**	client.exe: galaxy_map.cpp's object (__FILE__ 0xBA97D4) runs to 0x837C70; its header's inline
**	functions follow from 0x837D50. Built: the lookups 0x8378B0-0x837B30 (AllData[id], Systems /
**	Sectors / Starbases find; each returns a function-local static null RefCountPtr when absent, the
**	static declared inside the AllData branch, at the top in the hash lookups: the client's block
**	order), the base's Read / Write (out of line: they precede the factory, the header's inline
**	virtuals follow it) and the system's factory 0x837C70 (galaxy_map_client.cpp's 0x7A4FC0 reads
**	the new system and adds it through Add_System 0x836F30, whose FAIL_IFs sit on the client's lines
**	159 / 160; the blank lines above it hold that layout); Add_Sector / Add_Gate / Add_Starbase
**	0x837190 / 0x8373F0 / 0x837650 the same, FAIL_IFs on 169 / 170, 179 / 180, 189 / 190. Remove_Data
**	0x836B70 is NOT EXACT (its FAIL_IFs on the client's lines 112 / 115 / 120 / 128 / 136 / 144; the
**	client shares one epilogue for every exit). Not built yet: 0x8360A0-0x836740 (the header's inline
**	functions, the map's destructor 0x836240 among them) and Add_Data 0x836960 (galaxy_map.h's line
**	180 'GalaxyMapID == NOT_IN_GALAXY_MAP', the FAIL_IF '(int)id >= MaxDataObjects' on line 98,
**	MaxDataObjects at +0x74).
*/
