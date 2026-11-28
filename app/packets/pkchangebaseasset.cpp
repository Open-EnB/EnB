// Earth & Beyond client (reconstruction): app\packets\pkchangebaseasset.cpp. Notes at the end of the file.
#include "pkchangebaseasset.h"
#include <stdio.h>


ChangeBaseAssetPacketClass::ChangeBaseAssetPacketClass(int game_id, int base_asset, float scale, const Vector3 &hsv_shift) :
	GameID(game_id), BaseAsset(base_asset), Scale(scale), HSVShift(hsv_shift)
{
	Unknown24 = Unknown_14(GameID, -1);
}

ChangeBaseAssetPacketClass::ChangeBaseAssetPacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&BaseAsset, sizeof(BaseAsset));
	raw->Get(&Scale, sizeof(Scale));
	raw->Get(&HSVShift, sizeof(HSVShift));
}

PacketClass *ChangeBaseAssetPacketClass::Build()
{
	PacketClass *raw = new PacketClass(38, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&BaseAsset, sizeof(BaseAsset));
	raw->Add(&Scale, sizeof(Scale));
	raw->Add(&HSVShift, sizeof(HSVShift));
	return(raw);
}

int ChangeBaseAssetPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(BaseAsset) + sizeof(Scale) + sizeof(HSVShift));
}

int ChangeBaseAssetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d BaseAsset=%d Scale=%g HSVShift=(%g,%g,%g)}", "Change_Base_Asset", GameID, BaseAsset, Scale, HSVShift[0], HSVShift[1], HSVShift[2]));
}


/*
**	Reconstruction notes (pkchangebaseasset.cpp)
**
**	* Client 0x87E420-0x87E7A0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
