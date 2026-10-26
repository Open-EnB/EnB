// Earth & Beyond client (reconstruction): app\packets\pkcreate.cpp. Notes at the end of the file.
#include "pkcreate.h"
#include <stdio.h>


CreatePacketClass::CreatePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Scale, sizeof(Scale));
	raw->Get(&BaseAsset, sizeof(BaseAsset));
	raw->Get(&Type, sizeof(Type));
	raw->Get(HSV, sizeof(HSV));
}

RawPacketClass *CreatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(4, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Scale, sizeof(Scale));
	raw->Add(&BaseAsset, sizeof(BaseAsset));
	raw->Add(&Type, sizeof(Type));
	raw->Add(HSV, sizeof(HSV));
	return(raw);
}

int CreatePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Scale) + sizeof(BaseAsset) + sizeof(Type) + sizeof(HSV));
}

int CreatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Scale=%f BaseAsset=%hd Type=%d HSV=%f %f %f}", "Create", GameID, Scale, BaseAsset, Type, HSV[0], HSV[1], HSV[2]));
}


/*
**	Reconstruction notes (pkcreate.cpp)
**
**	* Client 0x87F920-0x87FCC0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
