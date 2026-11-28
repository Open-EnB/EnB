// Earth & Beyond client (reconstruction): app\packets\pkcreate.cpp. Notes at the end of the file.
#include "pkcreate.h"
#include <stdio.h>

CreatePacketClass::CreatePacketClass(int game_id, short base_asset, char type, float scale, int, const Vector3 &hsv, int channel) :
	GameID(game_id), Scale(scale), BaseAsset(base_asset), Type(type), HSV(hsv)
{
	if (channel == -1) {
		Unknown24 = Unknown_14(GameID, Type);
	} else {
		Unknown24 = Unknown_14(channel, Type);
	}
}

CreatePacketClass::CreatePacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Scale, sizeof(Scale));
	raw->Get(&BaseAsset, sizeof(BaseAsset));
	raw->Get(&Type, sizeof(Type));
	raw->Get(&HSV, sizeof(HSV));
}

PacketClass *CreatePacketClass::Encode()
{
	PacketClass *raw = new PacketClass(4, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Scale, sizeof(Scale));
	raw->Add(&BaseAsset, sizeof(BaseAsset));
	raw->Add(&Type, sizeof(Type));
	raw->Add(&HSV, sizeof(HSV));
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
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The sending constructor 0x87F920 (no caller left): HSV a Vector3 (the client copies it as one
**	  struct after the initialiser list's stores), its fifth argument unused, the channel if / else around
**	  two Unknown_14 calls (a ?: form loads the argument before the push).
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
