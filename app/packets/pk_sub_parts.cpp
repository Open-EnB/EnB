// Earth & Beyond client (reconstruction): app\Packets\pk_sub_parts.cpp (the client's __FILE__).
// SUBPARTS 180: a game id and a list of named values. Notes at the end of the file.
//
// The message (numbers in network byte order):
//	GameID		4 bytes
//	count		4 bytes
//	count times:
//		Name	a string (RawPacketClass::Add_String / Get_String)
//		Value	4 bytes
//
// Layout (0x24 bytes, vtable 0xB09BE4):
//	+0x0C	GameID (-1 when the raw constructor starts)
//	+0x10	Channel: PacketClass::Unknown_14 of the game id for a packet made here, -1 for a
//			received one (Get_Channel returns it)
//	+0x14	SubParts, a std::vector<SubPartStruct> (0x14 bytes each: a std::string and an int)
//
// The raw constructor adds each received part through Add (inlined there). Get returns an empty
// part (a function-local static, 0xBF4130) for an index out of range.
//
// Function order is the client's (0x8754D0-0x875CF0); the vector's insert (0x4408F0) and the
// part's constructor and destructor are copies from earlier objects or this one's (COMDAT).
//
#include "pk_sub_parts.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>

SubPartsPacket::SubPartsPacket(int game_id) :
	GameID(game_id),
	Channel(Unknown_14(game_id, -1))
{
}

SubPartsPacket::SubPartsPacket(RawPacketClass *raw) :
	GameID(-1),
	Channel(-1)
{
	raw->Get(GameID);
	int count = 0;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		char *name = raw->Get_String();
		int value = 0;
		raw->Get(value);
		Add(name, value);
		delete [] name;
	}
}

void SubPartsPacket::Add(const std::string &name, int value)
{
	SubPartStruct part;
	part.Name = name;
	part.Value = value;
	SubParts.push_back(part);
}

int SubPartsPacket::Get_Count()
{
	return(SubParts.size());
}

const SubPartStruct &SubPartsPacket::Get(unsigned int index)
{
	FAIL_IF(index >= SubParts.size()) {
		static SubPartStruct empty;
		return(empty);
	}
	return(SubParts[index]);
}

RawPacketClass *SubPartsPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(180, Get_Data_Size());
	raw->Add(GameID);
	int count = SubParts.size();
	raw->Add(count);
	for (std::vector<SubPartStruct>::iterator it = SubParts.begin(); it != SubParts.end(); ++it) {
		raw->Add_String(it->Name.c_str());
		raw->Add(it->Value);
	}
	return(raw);
}

int SubPartsPacket::Get_Data_Size()
{
	int size = sizeof(GameID) + sizeof(int);
	for (std::vector<SubPartStruct>::iterator it = SubParts.begin(); it != SubParts.end(); ++it) {
		size += sizeof(it->Value) + RawPacketClass::Get_Size(it->Name.c_str());
	}
	return(size);
}

int SubPartsPacket::Print(char *buffer, int)
{
	return(sprintf(buffer, "Sub Parts {GameID=%d Count=%d}", GameID, SubParts.size()));
}


/*
**	Reconstruction notes (pk_sub_parts.cpp)
**
**	* Client 0x8754D0-0x876330 (0x8754A0 is the previous object's guarded static); written by hand
**	  (builder P4, 2026-10-26). Get's FAIL_IF is on the client's line 64.
*/
