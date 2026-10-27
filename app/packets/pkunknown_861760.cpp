// Earth & Beyond client (reconstruction): app\packets\pkunknown_861760.cpp. Notes at the end of the file.
#include "pkunknown_861760.h"
#include "quat.h"
#include <stdio.h>

Packet_7532Class::Packet_7532Class(int game_id, const Matrix3D &transform) :
	GameID(game_id),
	Transform(transform)
{
}

Packet_7532Class::Packet_7532Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	Quaternion rotation;
	raw->Get(&rotation, sizeof(rotation));
	Vector3 position;
	raw->Get(&position, sizeof(position));
	Transform = Matrix3D(rotation, position);
}

RawPacketClass *Packet_7532Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30002, Get_Data_Size());
	Quaternion rotation = Build_Quaternion(Transform);
	Vector3 position = Transform.Get_Translation();
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&rotation, sizeof(rotation));
	raw->Add(&position, sizeof(position));
	return(raw);
}

int Packet_7532Class::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Quaternion) + sizeof(Vector3));
}

int Packet_7532Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorPositionalUpdate", GameID));
}

bool Packet_7532Class::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkunknown_861760.cpp)
**
**	* Client 0x861760-0x861B00 (0x861730 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x861B00): the send-only game-editor packet 30002 (not in the packet switch: its Process_Packet
**	  returns true here), written by hand (builder P5, 2026-10-27): a transform sent as a quaternion and
**	  a position (Build_Quaternion, Matrix3D::Set).
**	* The file name is provisional (no assert names it).
*/
