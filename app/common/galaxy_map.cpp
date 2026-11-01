// Earth & Beyond client (reconstruction): app\common\galaxy_map.cpp. Notes at the end of the file.
#include "galaxy_map.h"
#include "..\shared\auxdata\linearbu.h"

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
**	functions follow from 0x837D50. So far only its last three functions: the base's Read / Write
**	(out of line: they precede the factory, the header's inline virtuals follow it) and the system's
**	factory 0x837C70 (galaxy_map_client.cpp's 0x7A4FC0 reads the new system and adds it through
**	0x836F30). Not built yet, before them: Add_System 0x836F30 (FAIL_IF '!system' / the Systems.find
**	assert on lines 159 / 160), 0x837190, 0x8373F0, Add_Starbase 0x837650 (lines 189 / 190), the
**	lookups 0x8378B0-0x837B30; their asserts fix this file's line layout when they are added.
*/
