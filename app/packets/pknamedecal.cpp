// Earth & Beyond client (reconstruction): app\packets\pknamedecal.cpp. Notes at the end of the file.
#include "pknamedecal.h"
#include <stdio.h>

DecalPacketClass::DecalPacketClass(int game_id, const std::vector<DecalStruct> &decals) :
	GameID(game_id),
	Decals(decals)
{
	Channel = Unknown_14(GameID, -1);
}

DecalPacketClass::DecalPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	short count = 0;
	raw->Get(&count, sizeof(count));
	Decals.reserve(count);
	for (int i = 0; i < count; i++) {
		DecalStruct decal;
		raw->Get(&decal.Unknown00, sizeof(decal.Unknown00));
		raw->Get(&decal.Unknown04, sizeof(decal.Unknown04));
		raw->Get(&decal.Unknown08, sizeof(decal.Unknown08));
		Decals.push_back(decal);
	}
}

RawPacketClass *DecalPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(16, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	int count = Decals.size();
	raw->Add(&count, sizeof(short));
	for (std::vector<DecalStruct>::iterator it = Decals.begin(); it != Decals.end(); ++it) {
		raw->Add(&it->Unknown00, sizeof(it->Unknown00));
		raw->Add(&it->Unknown04, sizeof(it->Unknown04));
		raw->Add(&it->Unknown08, sizeof(it->Unknown08));
	}
	return(raw);
}

int DecalPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(short) + Decals.size() * sizeof(DecalStruct));
}

int DecalPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d NumDecals=%d}", "Decal", GameID, Decals.size()));
}

NameDecalPacketClass::NameDecalPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(Name, sizeof(Name));
	raw->Get(RGB, sizeof(RGB));
}

RawPacketClass *NameDecalPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(178, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(Name, sizeof(Name));
	raw->Add(RGB, sizeof(RGB));
	return(raw);
}

int NameDecalPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Name) + sizeof(RGB));
}

int NameDecalPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Name=%s RGB=%f %f %f}", "NameDecal", GameID, Name, RGB[0], RGB[1], RGB[2]));
}


/*
**	Reconstruction notes (pknamedecal.cpp)
**
**	* Client 0x8805C0-0x8812D0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* DECAL 16's constructors 0x8805C0 / 0x8807B0 exact once DecalStruct's 16-byte field is a Vector4
**	  (builder PK3, 2026-10-29): with float[4] the struct is trivially copied (rep movsd) and push_back's
**	  insert stays out of line; with Vector4's copy constructor the client's member-by-member copy
**	  (one pointer at +0x10) and the inlined insert both follow.
*/
