// Earth & Beyond client (reconstruction): e:\build\beta\G\app\Packets\pk_player_object_relationship.cpp. Notes at the end of the file.
#include "pk_player_object_relationship.h"
#include "..\shared\strconv_lib.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>
#include <string.h>

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

PlayerObjectReactionUpdatePacket::PlayerObjectReactionUpdatePacket(int priority) :
	Priority(priority)
{
}

PlayerObjectReactionUpdatePacket::PlayerObjectReactionUpdatePacket(RawPacketClass *raw)
{
	int count = 0;
	raw->Get(count);
	int id = 0;
	MobReactionType reaction = (MobReactionType)1;
	while (count--) {
		raw->Get(id);
		raw->Get(&reaction, sizeof(reaction));
		Add_Reaction(id, reaction);
	}
}

RawPacketClass *PlayerObjectReactionUpdatePacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(138, Get_Data_Size());
	int count = Reactions.size();
	raw->Add(count);
	for (std::vector<ReactionUpdateStruct>::iterator it = Reactions.begin(); it != Reactions.end(); ++it) {
		raw->Add(it->ObjectID);
		raw->Add(&it->Reaction, sizeof(it->Reaction));
	}
	return(raw);
}

int PlayerObjectReactionUpdatePacket::Get_Data_Size()
{
	return(sizeof(int) + Reactions.size() * sizeof(ReactionUpdateStruct));
}

// 0x8743C0: one reaction as Print shows it
std::string Unknown_8743C0(const ReactionUpdateStruct &reaction)  // NAME 1: neutral (D10)
{
	char name[256];
	char text[512];
	Value_To_String(name, sizeof(name), reaction.Reaction);
	sprintf(text, " %d=%s", reaction.ObjectID, name);
	return(std::string(text));
}

int PlayerObjectReactionUpdatePacket::Print(char *buffer, int len)
{
	std::string text("");
	for (std::vector<ReactionUpdateStruct>::iterator it = Reactions.begin(); it != Reactions.end(); ++it) {
		std::string s = Unknown_8743C0(*it);
		text += s;
	}
	_snprintf(buffer, len, "%s {%s }", "Player_Object_Reaction_Update", text.c_str());
	return(strlen(buffer));
}

void PlayerObjectReactionUpdatePacket::Add_Reaction(int id, MobReactionType reaction)
{
	Reactions.push_back(ReactionUpdateStruct(id, reaction));
}











































class ReactionUpdateVectorIteratorClass : public ReactionUpdateIteratorClass  // NAME 1: neutral (D10): what it does
{
public:
	ReactionUpdateVectorIteratorClass(std::vector<ReactionUpdateStruct> &list) : Cur(list.begin()), End(list.end()) {}
	virtual bool Is_Done() { return(Cur == End); }	// 0x874A20
	virtual ReactionUpdateStruct Get()	// 0x874A50
	{
		FAIL_IF(Is_Done()) {
			return(ReactionUpdateStruct());
		}
		return(*Cur);
	}
	virtual void Next() { Cur++; }	// 0x874AC0

	std::vector<ReactionUpdateStruct>::iterator Cur;  // NAME 2: what it holds
	std::vector<ReactionUpdateStruct>::iterator End;  // NAME 2: what it holds
};

ReactionUpdateIteratorClass *PlayerObjectReactionUpdatePacket::Get_Iterator()
{
	return(new ReactionUpdateVectorIteratorClass(Reactions));
}

PlayerObjectIsAttackingUpdatePacket::PlayerObjectIsAttackingUpdatePacket(bool attacking, int priority) :
	Attacking(attacking),
	Priority(priority)
{
}

PlayerObjectIsAttackingUpdatePacket::PlayerObjectIsAttackingUpdatePacket(RawPacketClass *raw)
{
	raw->Get(&Attacking, sizeof(Attacking));
	int count = 0;
	raw->Get(count);
	int id = 0;
	while (count--) {
		raw->Get(id);
		int value = id;
		Attackers.push_back(value);
	}
}

RawPacketClass *PlayerObjectIsAttackingUpdatePacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(139, Get_Data_Size());
	raw->Add(&Attacking, sizeof(Attacking));
	int count = Attackers.size();
	raw->Add(count);
	for (std::vector<int>::iterator it = Attackers.begin(); it != Attackers.end(); ++it) {
		raw->Add(*it);
	}
	return(raw);
}

int PlayerObjectIsAttackingUpdatePacket::Get_Data_Size()
{
	return(sizeof(Attacking) + sizeof(int) + Attackers.size() * sizeof(int));
}

int PlayerObjectIsAttackingUpdatePacket::Print(char *buffer, int)
{
	char attacking[16];
	Value_To_String(attacking, sizeof(attacking), Attacking);
	return(sprintf(buffer, "Attacker Updates. {Attacking=%s} [data not displayed]", attacking));
}

void PlayerObjectIsAttackingUpdatePacket::Add_Attacker(int id)
{
	Attackers.push_back(id);
}






















class AttackerUpdateVectorIteratorClass : public AttackerUpdateIteratorClass  // NAME 1: neutral (D10): what it does
{
public:
	AttackerUpdateVectorIteratorClass(std::vector<int> &list) : Cur(list.begin()), End(list.end()) {}
	virtual bool Is_Done() { return(Cur == End); }	// 0x8752C0
	virtual int Get()	// 0x8752F0
	{
		FAIL_IF(Is_Done()) {
			return(-1);
		}
		return(*Cur);
	}
	virtual void Next() { Cur++; }	// 0x875330

	std::vector<int>::iterator Cur;  // NAME 2: what it holds
	std::vector<int>::iterator End;  // NAME 2: what it holds
};

AttackerUpdateIteratorClass *PlayerObjectIsAttackingUpdatePacket::Get_Iterator()
{
	return(new AttackerUpdateVectorIteratorClass(Attackers));
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
**	* Opcode 139 (ATTACKER_UPDATES) written by hand (builder P3, 2026-10-26), 6 of 6 exact in
**	  appfile.py match: a std::vector<int> of attacker ids, a count before them in the message; its
**	  push_back helper 0x875060 is a member defined after Print. The raw constructor pushes a copy
**	  of the id read (`int value = id;`: the client's frame has that slot).
**	* Opcode 138 (Player_Object_Reaction_Update) and both packets' iterators written by hand (builder P6,
**	  2026-10-27): a std::vector of 8-byte reactions (an id in network order, the reaction as its 4 bytes);
**	  Print joins the reactions through the out-of-line Unknown_8743C0 into a std::string. Get_Iterator
**	  new's an iterator over the vector (three virtuals, no destructor); its Get's FAIL_IF(Is_Done()) sits
**	  on the client's line 159 (138's, returning id -1 and reaction 1) and 251 (139's, returning -1).
*/
