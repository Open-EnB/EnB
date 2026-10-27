// Earth & Beyond client (reconstruction): app\packets\pkunknown_877640.cpp. Notes at the end of the file.
#include "pkunknown_877640.h"
#include <stdio.h>

AvatarDescPacketClass::AvatarDescPacketClass(int game_id, const AvatarDescClass *desc) :
	GameID(game_id)
{
	Channel = Unknown_14(GameID, -1);
	if (desc != NULL) {
		Desc = *desc;
	}
}

AvatarDescPacketClass::AvatarDescPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get((Unknown_PacketDataClass *)&Desc);
}

RawPacketClass *AvatarDescPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(97, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add((Unknown_PacketDataClass &)Desc);
	return(raw);
}

int AvatarDescPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Desc));
}

int AvatarDescPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d AvatarName=%s}", "Avatar Description", GameID, Desc.First_Name));
}


/*
**	Reconstruction notes (pkunknown_877640.cpp)
**
**	* Client 0x877640-0x877C50 (0x877610 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x877C50): opcode 97, written by hand (builder P5, 2026-10-27), the value constructor first.
**	* Desc goes through RawPacketClass::Add / Get of pk.h's Unknown_PacketDataClass, the same three-slot
**	  interface as pk.h's Unknown_PacketDataClass (its first base, at offset 0): cast until the two
**	  declarations are one. Get_Data_Size is the class's size (0x110), not Desc's Get_Size.
**	* The file name is provisional (no assert names it).
*/
