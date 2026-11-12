// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\faction_and_reputation.h (name assumed from
**	faction_and_reputation.cpp, the FAIL_IFs of 0x835890 and 0x835980; no assert names it)
**
**	How a mob or an NPC reacts to a player from the player's reputation with its faction: the
**	reactions and their names, the reputation limits between them, and the formation state's text
**	converters (faction_and_reputation.cpp 0x835860-0x835DC0).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FACTION_AND_REPUTATION_H  // NAME 3 [faction_and_reputation.h]: header name assumed from the client's faction_and_reputation.cpp (D57)
#define FACTION_AND_REPUTATION_H

// MobReactionType and its Value_To_String are declared in pk_player_object_relationship.h (interim: sclient.cpp
// sees that header, and a declaration moved out of it moves sclient.cpp's symbol pad; workflow.md 3.4)
#include "..\packets\pk_player_object_relationship.h"

// An NPC's reaction to the player (the FAIL_IF 'reaction >= NPC_REACTION_COUNT'; the names are the table's, 0xBA9624)
enum NPCReactionType  // NAME 2: after the client's NPC_REACTION_COUNT, as MobReactionType
{
	NPC_REACTION_RUDE,			// 'Rude'  // NAME 2: the client's text
	NPC_REACTION_SHUN,			// 'Shun'  // NAME 2: the client's text
	NPC_REACTION_FRIENDLY,		// 'Friendly'  // NAME 2: the client's text
	NPC_REACTION_ADORATION,		// 'Adoration'  // NAME 2: the client's text
	NPC_REACTION_COUNT
};

// Where a ship is in its group's formation (the names are the table's, 0xB08A88)
enum FormationState  // NAME 5: the client's Type_Name 'FormationState'
{
	FORMATION_STATE_NOT_FORMING,	// 'NotForming'  // NAME 2: the client's text
	FORMATION_STATE_JOINING,		// 'Joining'  // NAME 2: the client's text
	FORMATION_STATE_IN_FORMATION,	// 'InFormation'  // NAME 2: the client's text
	FORMATION_STATE_LEADER			// 'Leader'  // NAME 2: the client's text
};

// The mob reactions' names are 'Attack', 'Shun', 'Friendly', 'Adoration' (0xBA9614); 'BadReaction' out of range
bool String_To_Value(const char *text, MobReactionType *reaction);			// 0x835930
char *Value_To_String(char *buf, int size, NPCReactionType reaction);		// 0x835980
bool String_To_Value(const char *text, NPCReactionType *reaction);			// 0x835A20

const char *Type_Name(FormationState value);									// 0x835CF0
bool String_To_Value(const char *text, FormationState *value);				// 0x835D10
char *Value_To_String(char *buf, int size, FormationState value);			// 0x835D80 ('NotForming' for a value not in the table)

// A player's reputation with a faction, as the limits below judge it (8 bytes). Nothing in the client calls
// this file's classes: the server's code, compiled into the client with the file.
struct FactionReputationStruct  // NAME 1: what it holds
{
	FactionReputationStruct(int reputation, bool can_be_lowest);				// 0x835860

	int Reputation;				// +0x00  // NAME 1: what the limits compare
	bool CanBeLowest;			// +0x04: only then a reputation under the lowest limit gives the lowest reaction  // NAME 1: what it does
};

// The reputation limits between a mob's four reactions (0x0C bytes)
class MobReactionLimitsClass  // NAME 1: what it holds; that it is the mobs' is a guess (the first of three alike, as the converters' order)
{
public:
	MobReactionLimitsClass();													// 0x835A70 (-2000, 2000, 5000)
	MobReactionLimitsClass(int shun, int friendly, int adoration);				// 0x835AA0
	MobReactionType Get_Reaction(const FactionReputationStruct &reputation);	// 0x835AD0  // NAME 1: what it does

	int Shun;					// +0x00: under it Attack, when the reputation can be the lowest  // NAME 1: the reaction from it on
	int Friendly;				// +0x04  // NAME 1: the reaction from it on
	int Adoration;				// +0x08  // NAME 1: the reaction from it on
};

// The same for an NPC's four reactions (0x0C bytes)
class NPCReactionLimitsClass  // NAME 1: what it holds; that it is the NPCs' is a guess (the second of three alike)
{
public:
	NPCReactionLimitsClass();													// 0x835B20 (-2000, 2000, 5000)
	NPCReactionLimitsClass(int shun, int friendly, int adoration);				// 0x835B50
	NPCReactionType Get_Reaction(const FactionReputationStruct &reputation);	// 0x835B80  // NAME 1: what it does

	int Shun;					// +0x00: under it Rude, when the reputation can be the lowest  // NAME 1: the reaction from it on
	int Friendly;				// +0x04  // NAME 1: the reaction from it on
	int Adoration;				// +0x08  // NAME 1: the reaction from it on
};

// A third set of the same limits (0x0C bytes): whose four reactions they give is not known
class Unknown835BD0Class  // NAME 1: neutral (D10)
{
public:
	Unknown835BD0Class();														// 0x835BD0 (-2000, 2000, 5000)
	Unknown835BD0Class(int second, int third, int fourth);						// 0x835C00
	int Get_Reaction(const FactionReputationStruct &reputation);				// 0x835C30 (0 to 3)  // NAME 1: what it does

	int Second;					// +0x00: under it reaction 0, when the reputation can be the lowest  // NAME 1: the reaction from it on
	int Third;					// +0x04  // NAME 1: the reaction from it on
	int Fourth;					// +0x08  // NAME 1: the reaction from it on
};

// One limit (4 bytes): 0 for a reputation under it that can be the lowest, else 1
class Unknown835C80Class  // NAME 1: neutral (D10)
{
public:
	Unknown835C80Class();														// 0x835C80 (-2000)
	Unknown835C80Class(int limit);												// 0x835CA0
	int Get_Reaction(const FactionReputationStruct &reputation);				// 0x835CC0  // NAME 1: what it does

	int Limit;					// +0x00  // NAME 1: what it holds
};

#endif
