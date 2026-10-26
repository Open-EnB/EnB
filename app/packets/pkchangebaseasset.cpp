// Earth & Beyond client (reconstruction): app\packets\pkchangebaseasset.cpp. Notes at the end of the file.
#include "pkchangebaseasset.h"
#include <stdio.h>


ChangeBaseAssetPacketClass::ChangeBaseAssetPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&BaseAsset, sizeof(BaseAsset));
	raw->Get(&Scale, sizeof(Scale));
	raw->Get(Unknown18, sizeof(Unknown18));
}

RawPacketClass *ChangeBaseAssetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(38, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&BaseAsset, sizeof(BaseAsset));
	raw->Add(&Scale, sizeof(Scale));
	raw->Add(Unknown18, sizeof(Unknown18));
	return(raw);
}

int ChangeBaseAssetPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(BaseAsset) + sizeof(Scale) + sizeof(Unknown18));
}

int ChangeBaseAssetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d BaseAsset=%d Scale=%g HSVShift=(%g,%g,%g)}", "Change_Base_Asset", GameID, BaseAsset, Scale, Unknown18[0], Unknown18[1], Unknown18[2]));
}


/*
**	Reconstruction notes (pkchangebaseasset.cpp)
**
**	* Client 0x87E420-0x87E7A0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
