// Earth & Beyond client (reconstruction): app\packets\pkcolorization.cpp. Notes at the end of the file.
#include "pkcolorization.h"
#include <stdio.h>

ColorizationPacketClass::ColorizationPacketClass(int game_id, const std::vector<ColorizationRegionStruct> &regions) :
	GameID(game_id),
	Regions(regions)
{
	Channel = Unknown_14(GameID, -1);
}

ColorizationPacketClass::ColorizationPacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	short count = 0;
	raw->Get(&count, sizeof(count));
	Regions.reserve(count);
	for (int i = 0; i < count; i++) {
		ColorizationRegionStruct region;
		raw->Get(&region.first.Unknown00, sizeof(region.first.Unknown00));
		raw->Get(&region.first.Unknown04, sizeof(region.first.Unknown04));
		raw->Get(&region.second.Unknown00, sizeof(region.second.Unknown00));
		raw->Get(&region.second.Unknown04, sizeof(region.second.Unknown04));
		Regions.push_back(region);
	}
}

PacketClass *ColorizationPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(17, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	int count = Regions.size();
	raw->Add(&count, sizeof(short));
	for (std::vector<ColorizationRegionStruct>::iterator it = Regions.begin(); it != Regions.end(); ++it) {
		raw->Add(&it->first.Unknown00, sizeof(it->first.Unknown00));
		raw->Add(&it->first.Unknown04, sizeof(it->first.Unknown04));
		raw->Add(&it->second.Unknown00, sizeof(it->second.Unknown00));
		raw->Add(&it->second.Unknown04, sizeof(it->second.Unknown04));
	}
	return(raw);
}

int ColorizationPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(short) + Regions.size() * sizeof(ColorizationRegionStruct));
}

int ColorizationPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d NumRegions=%d}", "Colorization", GameID, Regions.size()));
}


/*
**	Reconstruction notes (pkcolorization.cpp)
**
**	* Client 0x87F2F0-0x87F8F0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P3, 2026-10-26), 4 of 5 exact in appfile.py
**	  match: a std::vector of 32-byte regions (Vector3 members: the client copies them member by
**	  member); the raw constructor reserves the count first. The value constructor 0x87F320 exact
**	  (builder PK3, 2026-10-29) once a region is two {int, Vector3} halves: the nested copy
**	  constructors put the inline copy's pointer on the region's last dword, as the client's.
**	* The file name is provisional (no assert names it).
*/
