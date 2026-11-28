// Earth & Beyond client (reconstruction): app\packets\pkrequesttransformchange.cpp. Notes at the end of the file.
#include "pkrequesttransformchange.h"
#include <stdio.h>


RequestTransformChangePacketClass::RequestTransformChangePacketClass(int player_id, int game_id, const Vector3 &position, const Quaternion &orientation, const Vector3 &velocity) :
	PlayerID(player_id), GameID(game_id), Position(position), Orientation(orientation), Velocity(velocity)
{
}

RequestTransformChangePacketClass::RequestTransformChangePacketClass(PacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Position, sizeof(Position));
	raw->Get(&Orientation, sizeof(Orientation));
	raw->Get(&Velocity, sizeof(Velocity));
}

PacketClass *RequestTransformChangePacketClass::Encode()
{
	PacketClass *raw = new PacketClass(67, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Position, sizeof(Position));
	raw->Add(&Orientation, sizeof(Orientation));
	raw->Add(&Velocity, sizeof(Velocity));
	return(raw);
}

int RequestTransformChangePacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(GameID) + sizeof(Position) + sizeof(Orientation) + sizeof(Velocity));
}

int RequestTransformChangePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d GameID=%d Position={%f %f %f} Orientation={%f %f %f %f} Velocity={%f %f %f}}", "Request_Transform_Change", PlayerID, GameID, Position[0], Position[1], Position[2], Orientation[0], Orientation[1], Orientation[2], Orientation[3], Velocity[0], Velocity[1], Velocity[2]));
}


/*
**	Reconstruction notes (pkrequesttransformchange.cpp)
**
**	* Client 0x8A27B0-0x8A2B40 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
