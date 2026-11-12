// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\faction_and_reputation.cpp (the client's __FILE__:
**	the FAIL_IFs of 0x835890, lines 52 and 53, and of 0x835980, lines 72 and 73)
**
**	How a mob or an NPC reacts to a player from the player's reputation with its faction, and the
**	formation state's text converters. Client 0x835860-0x835DC0, the whole object:
**
**	  0x835860            FactionReputationStruct's constructor
**	  0x835890, 0x835930  the mob reaction's Value_To_String / String_To_Value (the names 0xBA9614)
**	  0x835980, 0x835A20  the NPC reaction's (the names 0xBA9624)
**	  0x835A70-0x835C30   three classes of three reputation limits, alike: the constructors (-2000,
**	                      2000, 5000 by default) and the reaction a reputation gives
**	  0x835C80-0x835CC0   a class of one limit
**	  0x835CF0-0x835D80   FormationState's Type_Name / String_To_Value / Value_To_String (the table
**	                      0xB08A88), which KVPairTemplateClass<FormationState> calls (0x759E80, 0x75A200,
**	                      0x759EA0)
**
**	Its start: 0x835830 before it is the closing <string> initialiser (guard 0xC416FC, atexit 0x42B320)
**	of the object before, which so holds the four functions of an 8-byte bit field 0x835770-0x8357F0
**	(after EffectGeneratorUsageBF's converters 0x835370-0x835590). Its end: GalaxyMapClass's constructor
**	0x835DD0 (galaxy_map.cpp). No <string> initialiser closes this object: the client's file saw no
**	header that includes <string>. That the formation state's converters are this file's and not a
**	small object's of their own between the two is not shown by anything (interim, D137).
**
**	Only the mob reaction's Value_To_String is called (the relationship packets' Print, 0x873E10 and
**	0x8743C0) besides the formation state's three: the classes and the other converters are the
**	server's side of the file.
*/


#include "faction_and_reputation.h"
#include <string.h>
#include "..\shared\g_assert.h"

#include "..\shared\unknown_4a1590.h"

static const char * const MobReactionNames[MOB_REACTION_COUNT] = { "Attack", "Shun", "Friendly", "Adoration" };	// 0xBA9614  // NAME 1: what it holds
static const char * const NPCReactionNames[NPC_REACTION_COUNT] = { "Rude", "Shun", "Friendly", "Adoration" };	// 0xBA9624  // NAME 1: what it holds

// @0x835860
FactionReputationStruct::FactionReputationStruct(int reputation, bool can_be_lowest) :
	Reputation(reputation),
	CanBeLowest(can_be_lowest)
{
}

// @0x835890
char *Value_To_String(char *buf, int size, MobReactionType reaction)
{
	FAIL_IF(reaction < 0) return(Bounded_Strcpy(buf, size, "BadReaction"));
	FAIL_IF(reaction >= MOB_REACTION_COUNT) return(Bounded_Strcpy(buf, size, "BadReaction"));
	return(Bounded_Strcpy(buf, size, MobReactionNames[reaction]));
}

// @0x835930
bool String_To_Value(const char *text, MobReactionType *reaction)
{
	for (int i = 0; i < MOB_REACTION_COUNT; i++) {
		if (stricmp(text, MobReactionNames[i]) == 0) {
			*reaction = (MobReactionType)i;
			return(true);
		}
	}
	return(false);
}

// @0x835980
char *Value_To_String(char *buf, int size, NPCReactionType reaction)
{
	FAIL_IF(reaction < 0) return(Bounded_Strcpy(buf, size, "BadReaction"));
	FAIL_IF(reaction >= NPC_REACTION_COUNT) return(Bounded_Strcpy(buf, size, "BadReaction"));
	return(Bounded_Strcpy(buf, size, NPCReactionNames[reaction]));
}

// @0x835A20
bool String_To_Value(const char *text, NPCReactionType *reaction)
{
	for (int i = 0; i < NPC_REACTION_COUNT; i++) {
		if (stricmp(text, NPCReactionNames[i]) == 0) {
			*reaction = (NPCReactionType)i;
			return(true);
		}
	}
	return(false);
}

// @0x835A70
MobReactionLimitsClass::MobReactionLimitsClass() :
	Shun(-2000),
	Friendly(2000),
	Adoration(5000)
{
}

// @0x835AA0
MobReactionLimitsClass::MobReactionLimitsClass(int shun, int friendly, int adoration) :
	Shun(shun),
	Friendly(friendly),
	Adoration(adoration)
{
}

// @0x835AD0
MobReactionType MobReactionLimitsClass::Get_Reaction(const FactionReputationStruct &reputation)
{
	if (reputation.CanBeLowest && reputation.Reputation < Shun) {
		return((MobReactionType)0);
	}
	if (reputation.Reputation < Friendly) {
		return((MobReactionType)1);
	}
	return((reputation.Reputation < Adoration) ? (MobReactionType)2 : (MobReactionType)3);
}

// @0x835B20
NPCReactionLimitsClass::NPCReactionLimitsClass() :
	Shun(-2000),
	Friendly(2000),
	Adoration(5000)
{
}

// @0x835B50
NPCReactionLimitsClass::NPCReactionLimitsClass(int shun, int friendly, int adoration) :
	Shun(shun),
	Friendly(friendly),
	Adoration(adoration)
{
}

// @0x835B80
NPCReactionType NPCReactionLimitsClass::Get_Reaction(const FactionReputationStruct &reputation)
{
	if (reputation.CanBeLowest && reputation.Reputation < Shun) {
		return(NPC_REACTION_RUDE);
	}
	if (reputation.Reputation < Friendly) {
		return(NPC_REACTION_SHUN);
	}
	return((reputation.Reputation < Adoration) ? NPC_REACTION_FRIENDLY : NPC_REACTION_ADORATION);
}

// @0x835BD0
Unknown835BD0Class::Unknown835BD0Class() :
	Second(-2000),
	Third(2000),
	Fourth(5000)
{
}

// @0x835C00
Unknown835BD0Class::Unknown835BD0Class(int second, int third, int fourth) :
	Second(second),
	Third(third),
	Fourth(fourth)
{
}

// @0x835C30
int Unknown835BD0Class::Get_Reaction(const FactionReputationStruct &reputation)
{
	if (reputation.CanBeLowest && reputation.Reputation < Second) {
		return(0);
	}
	if (reputation.Reputation < Third) {
		return(1);
	}
	return((reputation.Reputation < Fourth) ? 2 : 3);
}

// @0x835C80
Unknown835C80Class::Unknown835C80Class() :
	Limit(-2000)
{
}

// @0x835CA0
Unknown835C80Class::Unknown835C80Class(int limit) :
	Limit(limit)
{
}

// @0x835CC0
int Unknown835C80Class::Get_Reaction(const FactionReputationStruct &reputation)
{
	if (reputation.CanBeLowest && reputation.Reputation < Limit) {
		return(0);
	}
	return(1);
}

struct FormationStateNameStruct  // NAME 1: what it holds
{
	FormationState State;		// +0x00  // NAME 1: what it holds
	const char *Name;			// +0x04  // NAME 1: what it holds
};

static FormationStateNameStruct FormationStateNames[] = {	// 0xB08A88  // NAME 1: what it holds
	{ FORMATION_STATE_NOT_FORMING, "NotForming" },
	{ FORMATION_STATE_JOINING, "Joining" },
	{ FORMATION_STATE_IN_FORMATION, "InFormation" },
	{ FORMATION_STATE_LEADER, "Leader" },
};

// @0x835CF0
const char *Type_Name(FormationState)
{
	return("FormationState");
}

// @0x835D10
bool String_To_Value(const char *text, FormationState *value)
{
	for (int i = 0; i < (int)(sizeof(FormationStateNames) / sizeof(FormationStateNames[0])); i++) {
		if (stricmp(FormationStateNames[i].Name, text) == 0) {
			*value = FormationStateNames[i].State;
			return(true);
		}
	}
	return(false);
}

// @0x835D80
char *Value_To_String(char *buf, int size, FormationState value)
{
	for (int i = 0; i < (int)(sizeof(FormationStateNames) / sizeof(FormationStateNames[0])); i++) {
		if (FormationStateNames[i].State == value) {
			return(Bounded_Strcpy(buf, size, FormationStateNames[i].Name));
		}
	}
	return(Bounded_Strcpy(buf, size, FormationStateNames[0].Name));
}
