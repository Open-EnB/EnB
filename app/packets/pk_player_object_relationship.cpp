// Earth & Beyond client (reconstruction): e:\build\beta\G\app\Packets\pk_player_object_relationship.cpp. Notes at the end of the file.
#include "pk_player_object_relationship.h"
#include "..\shared\strconv_lib.h"
#include <stdio.h>

PlayerObjectRelationshipPacket::PlayerObjectRelationshipPacket(int object_id, int channel, const RelationshipClass &relationship, int priority) :
	ObjectID(object_id),
	Relationship(relationship),
	Priority(priority),
	Channel(Unknown_14(object_id, channel))
{
}

PlayerObjectRelationshipPacket::PlayerObjectRelationshipPacket(RawPacketClass *raw) :
	ObjectID(-1),
	Priority(-63),
	Channel(-1)
{
	raw->Get(ObjectID);
	raw->Get(&Relationship.Reaction, sizeof(Relationship.Reaction));
	raw->Get(&Relationship.IsAttacking, sizeof(Relationship.IsAttacking));
}

RawPacketClass *PlayerObjectRelationshipPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(137, Get_Data_Size());
	raw->Add(ObjectID);
	raw->Add(&Relationship.Reaction, sizeof(Relationship.Reaction));
	raw->Add(&Relationship.IsAttacking, sizeof(Relationship.IsAttacking));
	return(raw);
}

int PlayerObjectRelationshipPacket::Get_Data_Size()
{
	return(sizeof(ObjectID) + sizeof(Relationship.Reaction) + sizeof(Relationship.IsAttacking));
}

int PlayerObjectRelationshipPacket::Print(char *buffer, int)
{
	char reaction[128];
	char is_attacking[32];
	Value_To_String(reaction, sizeof(reaction), Relationship.Reaction);
	Value_To_String(is_attacking, sizeof(is_attacking), Relationship.IsAttacking);
	return(sprintf(buffer, "Relationship {ObjectID=%d, Reaction=%s, IsAttacking=%s}", ObjectID, reaction, is_attacking));
}


/*
**	Reconstruction notes (pk_player_object_relationship.cpp)
**
**	* The client's file (its FAIL_IFs on lines 159 and 251 name it), 0x873AE0-0x8754A0 (the object ends
**	  with the guarded static every <string> includer has, 0xC416FC). Opcode 137 (RELATIONSHIP) written
**	  by hand (builder P3, 2026-10-26), 5 of 5 exact in appfile.py match: the value constructor calls
**	  PacketClass::Unknown_14 in its initialiser list (the own vtable is stored after the call), the raw
**	  one constructs RelationshipClass out of line (0x848C80) and reads ObjectID through Get(int &).
**	  Print's conversions are faction_and_reputation.cpp's (0x835890) and strconv_lib.cpp's (0x4A30A0),
**	  both not rebuilt: stubs/app_stubs.cpp stands in for them and for RelationshipClass's constructor.
**	* Not rebuilt yet: opcode 138, 139 (declared in pk_classes.h; the FAIL_IFs on lines 159 and 251 are
**	  theirs).
*/
