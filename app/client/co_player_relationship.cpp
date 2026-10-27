// Earth & Beyond client (reconstruction): app\Client\co_player_relationship.cpp. Notes at the end of the file.
#include "sclient.h"
#include "co.h"
#include "..\packets\pk_player_object_relationship.h"
#include "..\shared\unknown_debug.h"

void Unknown_70CC40(SClient *sclient, PlayerObjectRelationshipPacket *packet);	// NAME 1 [Unknown_70CC40, sclient, packet]: cpacket.cpp's RELATIONSHIP 137 handler
void Unknown_70CC90(SClient *sclient, PlayerObjectReactionUpdatePacket *packet);	// NAME 1 [Unknown_70CC90, sclient, packet]: cpacket.cpp's 138 handler
void Unknown_70CD50(SClient *sclient, PlayerObjectIsAttackingUpdatePacket *packet);	// NAME 1 [Unknown_70CD50, sclient, packet]: cpacket.cpp's ATTACKER_UPDATES 139 handler









































































































































































































































































































void Unknown_70CC40(SClient *sclient, PlayerObjectRelationshipPacket *packet)
{
	ClientObjectClass *object = sclient->Find_Object(packet->ObjectID);
	FAIL_IF(!object) return;
	object->Unknown_6FEEE0(packet->Relationship);
}

void Unknown_70CC90(SClient *sclient, PlayerObjectReactionUpdatePacket *packet)
{
	ReactionUpdateIteratorClass *it = packet->Get_Iterator();
	while (!it->Is_Done()) {
		ReactionUpdateStruct update = it->Get();
		it->Next();
		ClientObjectClass *object = sclient->Find_Object(update.ObjectID);
		if (object == NULL) return;
		RelationshipClass relationship = *(RelationshipClass *)object->Relationship;
		relationship.Reaction = update.Reaction;
		object->Unknown_6FEEE0(relationship);
	}
	delete it;
}

void Unknown_70CD50(SClient *sclient, PlayerObjectIsAttackingUpdatePacket *packet)
{
	AttackerUpdateIteratorClass *it = packet->Get_Iterator();
	while (!it->Is_Done()) {
		int id = it->Get();
		it->Next();
		ClientObjectClass *object = sclient->Find_Object(id);
		if (object == NULL) return;
		RelationshipClass relationship = *(RelationshipClass *)object->Relationship;
		relationship.IsAttacking = packet->Attacking;
		object->Unknown_6FEEE0(relationship);
	}
	delete it;
}

/*
**	Reconstruction notes
**
**	The object 0x709EA0-0x70DC00 (asserts' __FILE__ 'e:\build\beta\G\app\Client\co_player_relationship.cpp'):
**	the static initialisers 0x709EA0 (a PermStringInitClass at 0xBE7078) and 0x709EF0 (an SGI hash_map at
**	0xBE70E8, constructor 0x70CE50, destructor 0x709F40), the gutter colours read from an ini (0x70A010-0x70CAF0,
**	asserts on lines 63-301, not rebuilt yet), the three packet handlers cpacket.cpp calls (0x70CC40, 0x70CC90,
**	0x70CD50) and the table's out-of-line helpers (0x70CDF0-0x70DBC0). The next object is extrap.cpp (0x70F1D0).
**	The handlers read the object's relationship at +0x12C / +0x130 (co.h's Relationship) through a cast, as
**	co.cpp's 0x6FEEE0 does.
*/
