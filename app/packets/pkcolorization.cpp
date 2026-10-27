// Earth & Beyond client (reconstruction): app\packets\pkcolorization.cpp. Notes at the end of the file.
#include "pkcolorization.h"
#include <stdio.h>

// NOT EXACT: 0x87F320, the inline vector copy's loop addresses each region from its last dword in
// the client (shape; attempts.py show 0x87F320)
ColorizationPacketClass::ColorizationPacketClass(int game_id, const std::vector<ColorizationRegionStruct> &regions) :
	GameID(game_id),
	Regions(regions)
{
	Channel = Unknown_14(GameID, -1);
}

ColorizationPacketClass::ColorizationPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	short count = 0;
	raw->Get(&count, sizeof(count));
	Regions.reserve(count);
	for (int i = 0; i < count; i++) {
		ColorizationRegionStruct region;
		raw->Get(&region.Unknown00, sizeof(region.Unknown00));
		raw->Get(&region.Unknown04, sizeof(region.Unknown04));
		raw->Get(&region.Unknown10, sizeof(region.Unknown10));
		raw->Get(&region.Unknown14, sizeof(region.Unknown14));
		Regions.push_back(region);
	}
}

RawPacketClass *ColorizationPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(17, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	int count = Regions.size();
	raw->Add(&count, sizeof(short));
	for (std::vector<ColorizationRegionStruct>::iterator it = Regions.begin(); it != Regions.end(); ++it) {
		raw->Add(&it->Unknown00, sizeof(it->Unknown00));
		raw->Add(&it->Unknown04, sizeof(it->Unknown04));
		raw->Add(&it->Unknown10, sizeof(it->Unknown10));
		raw->Add(&it->Unknown14, sizeof(it->Unknown14));
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
**	  member); the raw constructor reserves the count first. The value constructor is NOT EXACT
**	  (parked, attempts.csv).
**	* The file name is provisional (no assert names it).
*/
