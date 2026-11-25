// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): a mob's reaction and the relationship the RELATIONSHIP packet carries, out of
**	pk_player_object_relationship.h because the client's pal_mgr.obj and faction_and_reputation.obj have no
**	<string> initialiser: they never saw pk.h. pk_player_object_relationship.h includes this header where the
**	declarations stood (sclient.cpp sees them in the same order).
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RELATIONSHIP_H	// NAME 1 [relationship.h]: neutral, what it declares (D10)
#define RELATIONSHIP_H

// A mob's reaction to the player (app\Common\faction_and_reputation.cpp's FAIL_IFs: 'reaction < 0',
// 'reaction >= MOB_REACTION_COUNT'); only the count is known
enum MobReactionType  // NAME 2: what it holds, after the client's MOB_REACTION_COUNT
{
	MOB_REACTION_COUNT = 4
};

// faction_and_reputation.cpp 0x835890 (not rebuilt): the reaction's name ('BadReaction' out of range)
char *Value_To_String(char *buf, int size, MobReactionType reaction);  // NAME 2: as strconv_lib.h's pairs

// The relationship a RELATIONSHIP packet carries: 8 bytes, copied whole (its constructor
// 0x848C80 is out of line, Reaction 1 and IsAttacking false)
class RelationshipClass  // NAME 2: what it holds (the packet's Print names both members)
{
public:
	RelationshipClass();	// 0x848C80 (pal_mgr.cpp by position, D137)

	MobReactionType Reaction;	// +0x00  // NAME 3: the client's Print text 'Reaction='
	bool IsAttacking;	// +0x04  // NAME 3: the client's Print text 'IsAttacking='
};

#endif
