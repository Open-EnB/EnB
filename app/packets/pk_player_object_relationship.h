// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\pk_player_object_relationship.h (the name from the
**	client's pk_player_object_relationship.cpp)
**
**	The packet classes of client 0x873AE0-0x8754A0 rebuilt so far (opcode 137, 138, 139); the file's other
**	class (138) is still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PK_PLAYER_OBJECT_RELATIONSHIP_H  // NAME 3 [pk_player_object_relationship.h]: after the client's .cpp (D57)
#define PK_PLAYER_OBJECT_RELATIONSHIP_H

#include "pk.h"
#include <vector>

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
	virtual bool Process_Packet(void *client);	// +0x08 0x7398E0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x873DF0
	virtual int Print(char *buffer, int len);	// +0x10 0x873E10
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x873BC0
	virtual int Get_Priority() { return(Priority); }	// +0x1C 0x873BA0

	int ObjectID;	// +0x0C (-1 by default)  // NAME 3: the client's Print text 'ObjectID='
	RelationshipClass Relationship;	// +0x10  // NAME 2: what it holds
	int Priority;	// +0x18 (-63 by default; not read from the message)  // NAME 2: Get_Priority returns it
	int Channel;	// +0x1C (PacketClass::Unknown_14 of ObjectID; -1 by default)  // NAME 2: Get_Channel returns it
};

// One reaction of opcode 138: 8 bytes (the id in network order, the reaction as its 4 bytes)
struct ReactionUpdateStruct  // NAME 1: neutral (D10): what it holds
{
	ReactionUpdateStruct() : ObjectID(-1), Reaction((MobReactionType)1) {}	// (the iterator's answer when done)
	ReactionUpdateStruct(int object_id, MobReactionType reaction) : ObjectID(object_id), Reaction(reaction) {}

	int ObjectID;	// +0x00  // NAME 2: as RELATIONSHIP 137's ObjectID (Print's ' %d=%s')
	MobReactionType Reaction;	// +0x04  // NAME 2: Print shows it through the reaction's Value_To_String
};

// The iterators Get_Iterator hands out (new'd; three virtuals, no destructor: vtables 0xB09B84, 0xB09BD4)
class ReactionUpdateIteratorClass  // NAME 1: neutral (D10): what it does
{
public:
	virtual bool Is_Done() = 0;	// NAME 3: the client's FAIL_IF text 'Is_Done()'
	virtual ReactionUpdateStruct Get() = 0;	// NAME 2: what it does
	virtual void Next() = 0;	// NAME 2: what it does
};

class AttackerUpdateIteratorClass  // NAME 1: neutral (D10): what it does
{
public:
	virtual bool Is_Done() = 0;	// NAME 3: the client's FAIL_IF text 'Is_Done()'
	virtual int Get() = 0;	// NAME 2: what it does
	virtual void Next() = 0;	// NAME 2: what it does
};

// opcode 138 (0x008A, ?): vtable 0xB09B44, constructor 0x873F70, 0x20 bytes
class PlayerObjectReactionUpdatePacket : public PacketClass
{
public:
	PlayerObjectReactionUpdatePacket(int priority);	// 0x873E90
	PlayerObjectReactionUpdatePacket(RawPacketClass *raw);	// 0x873F70
	virtual RawPacketClass *Build();	// +0x04 0x874270
	virtual bool Process_Packet(void *client);	// +0x08 0x7399D0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x874380
	virtual int Print(char *buffer, int len);	// +0x10 0x874530
	virtual int Get_Priority() { return(Priority); }	// +0x1C 0x873EE0

	void Add_Reaction(int object_id, MobReactionType reaction);	// 0x874790 (push_back)  // NAME 2: what it does
	ReactionUpdateIteratorClass *Get_Iterator();	// 0x8749E0 (new'd)  // NAME 2: what it returns

	std::vector<ReactionUpdateStruct> Reactions;	// +0x0C (a count and the reactions in the message)  // NAME 2: what it holds
	int Priority;	// +0x1C (not read from the message)  // NAME 2: Get_Priority returns it
};

// opcode 139 (0x008B, ATTACKER_UPDATES): vtable 0xB09B94, constructor 0x874BE0, 0x24 bytes
class PlayerObjectIsAttackingUpdatePacket : public PacketClass
{
public:
	PlayerObjectIsAttackingUpdatePacket(bool attacking, int priority);	// 0x874AE0
	PlayerObjectIsAttackingUpdatePacket(RawPacketClass *raw);	// 0x874BE0
	virtual RawPacketClass *Build();	// +0x04 0x874ED0
	virtual bool Process_Packet(void *client);	// +0x08 0x739AC0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x874FE0
	virtual int Print(char *buffer, int len);	// +0x10 0x875020
	virtual int Get_Priority() { return(Priority); }	// +0x1C 0x874B50

	void Add_Attacker(int id);	// 0x875060 (push_back)  // NAME 2: what it does
	AttackerUpdateIteratorClass *Get_Iterator();	// 0x875280 (new'd)  // NAME 2: what it returns

	bool Attacking;	// +0x0C  // NAME 3: the client's Print text 'Attacking='
	std::vector<int> Attackers;	// +0x10 (a count and the ids in the message: Print says '[data not displayed]')  // NAME 2: what it holds
	int Priority;	// +0x20 (not read from the message)  // NAME 2: Get_Priority returns it
};

#endif
