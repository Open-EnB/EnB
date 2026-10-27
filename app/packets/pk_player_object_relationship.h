/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\pk_player_object_relationship.h (the name from the
**	client's pk_player_object_relationship.cpp)
**
**	The packet classes of client 0x873AE0-0x8754A0 rebuilt so far (opcode 137); the file's other classes
**	(138, 139) are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PK_PLAYER_OBJECT_RELATIONSHIP_H  // NAME 3 [pk_player_object_relationship.h]: after the client's .cpp (D57)
#define PK_PLAYER_OBJECT_RELATIONSHIP_H

#include "pk.h"

// A mob's reaction to the player (app\Common\faction_and_reputation.cpp's FAIL_IFs: 'reaction < 0',
// 'reaction >= MOB_REACTION_COUNT'); only the count is known
enum MobReactionType  // NAME 2: what it holds, after the client's MOB_REACTION_COUNT
{
	MOB_REACTION_COUNT = 4
};

// faction_and_reputation.cpp 0x835890 (not rebuilt): the reaction's name ('BadReaction' out of range)
char *Value_To_String(char *buf, int size, MobReactionType reaction);  // NAME 2: as strconv_lib.h's pairs

// The relationship a RELATIONSHIP packet carries: 8 bytes, copied whole (client class not rebuilt:
// its constructor 0x848C80 is out of line, Reaction 1 and IsAttacking false)
class RelationshipClass  // NAME 2: what it holds (the packet's Print names both members)
{
public:
	RelationshipClass();	// 0x848C80 (not rebuilt)

	MobReactionType Reaction;	// +0x00  // NAME 3: the client's Print text 'Reaction='
	bool IsAttacking;	// +0x04  // NAME 3: the client's Print text 'IsAttacking='
};

// opcode 137 (0x0089, RELATIONSHIP): vtable 0xB09B14, constructor 0x873C30, 0x20 bytes
class PlayerObjectRelationshipPacket : public PacketClass
{
public:
	PlayerObjectRelationshipPacket(int object_id, int channel, const RelationshipClass &relationship, int priority);	// 0x873AE0
	PlayerObjectRelationshipPacket(RawPacketClass *raw);	// 0x873C30
	virtual RawPacketClass *Build();	// +0x04 0x873D10
	virtual bool Process_Packet(void *client);	// +0x08 0x7398E0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x873DF0
	virtual int Print(char *buffer, int len);	// +0x10 0x873E10
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x873BC0
	virtual int Get_Priority() { return(Priority); }	// +0x1C 0x873BA0

	int ObjectID;	// +0x0C (-1 by default)  // NAME 3: the client's Print text 'ObjectID='
	RelationshipClass Relationship;	// +0x10  // NAME 2: what it holds
	int Priority;	// +0x18 (-63 by default; not read from the message)  // NAME 2: Get_Priority returns it
	int Channel;	// +0x1C (PacketClass::Unknown_14 of ObjectID; -1 by default)  // NAME 2: Get_Channel returns it
};

#endif
