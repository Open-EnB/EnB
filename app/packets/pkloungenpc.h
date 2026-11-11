// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkloungenpc.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x897800-0x89A710 (opcode 82).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKLOUNGENPC_H  // NAME 1 [pkloungenpc]: provisional, pk + its first named class's name
#define PKLOUNGENPC_H

#include "pk.h"
#include "..\shared\avatar_desc.h"
#include "vector3.h"
#include <string>
#include <vector>

// LOUNGE_NPC's first list: 0x1C bytes (four ints, a Vector3), sent as they are
struct LoungeNPCRoomStruct  // NAME 2: what 0x6987A0 (sbase_info.cpp) makes of an entry
{
	int RoomType;	// +0x00 the room's type (0x6987A0: 0 hangar, 1 main deck, 2 merchants, 3 NPC lounge, 4 lounge)  // NAME 2: what 0x6987A0 reads it as
	int Flags;	// +0x04 the room's flags, nine bits (0x6987A0: StarbaseRoomClass +0x04..+0x0C)  // NAME 2: what 0x6987A0 reads it as
	int Unknown08;	// +0x08  // NAME 1: neutral (D10)
	int Unknown0C;	// +0x0C  // NAME 1: neutral (D10)
	Vector3 Unknown10;	// +0x10 (12 bytes as they are; its copy constructor makes the entry's copy member by member)  // NAME 1: neutral (D10)
};

// LOUNGE_NPC's second list: 0x10 bytes, sent as they are
struct LoungeNPCFacilityStruct  // NAME 2: what 0x6987A0 (sbase_info.cpp) makes of an entry
{
	int RoomType;	// +0x00 the room's type, as the first list's  // NAME 2: what 0x6987A0 reads it as
	int Terminal;	// +0x04 the terminal's index in the room (the key of the room's facility table)  // NAME 2: what 0x6987A0 reads it as
	int FacilityType;	// +0x08 the facility's type (0..7, 9: StarbaseFacilityClass's)  // NAME 2: what 0x6987A0 reads it as
	int Unknown0C;	// +0x0C  // NAME 1: neutral (D10)
};

// LOUNGE_NPC's third list: 0x134 bytes (five ints, a text with its length, an avatar)
struct LoungeNPCNPCStruct  // NAME 2: what 0x6987A0 (sbase_info.cpp) makes of an entry
{
	int RoomType;	// +0x00 the room's type, as the first list's  // NAME 2: what 0x6987A0 reads it as
	int Slot;	// +0x04 the NPC's slot in the room (the key of the room's NPC table)  // NAME 2: what 0x6987A0 reads it as
	int Unknown08;	// +0x08  // NAME 1: neutral (D10)
	int Booth;	// +0x0C the NPC's booth (0..7: StarbaseNPCClass's BOOTH_WEAPON..BOOTH_BLACKMKT; anything else none)  // NAME 2: what 0x6987A0 reads it as
	int Unknown10;	// +0x10  // NAME 1: neutral (D10)
	std::string Text;	// +0x14 (Length bytes in the message, no terminator)  // NAME 2: what it holds
	int Length;	// +0x24  // NAME 2: what it holds
	AvatarDescClass Desc;	// +0x28 (through RawPacketClass's Add / Get of its wire interface)  // NAME 2: what it holds
};

// opcode 82 (0x0052, LOUNGE_NPC): vtable 0xB0AFE4, constructor 0x8978D0, 0x40 bytes
class LoungeNPCPacketClass : public PacketClass  // NAME 2: Net-7's opcode name LOUNGE_NPC (the Net-7 opcode table), the client's Print text 'LoungeNPC'
{
public:
	LoungeNPCPacketClass(int style);	// 0x897800
	LoungeNPCPacketClass(RawPacketClass *raw);	// 0x8978D0
	virtual ~LoungeNPCPacketClass();	// +0x00 0x8978A0 / 0x897FE0
	virtual RawPacketClass *Build();	// +0x04 0x898080
	virtual bool Process_Packet(void *client);	// +0x08 0x7385B0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8983D0
	virtual int Print(char *buffer, int len);	// +0x10 0x8984C0
	virtual int Get_Priority() { return(-127); }	// +0x1C 0x897880

	void Add_Room(const LoungeNPCRoomStruct &entry);	// 0x8984F0 (push_back)  // NAME 2: what it does
	void Add_Facility(const LoungeNPCFacilityStruct &entry);	// 0x898790 (push_back)  // NAME 2: what it does
	void Add_NPC(const LoungeNPCNPCStruct &entry);	// 0x898A30 (push_back)  // NAME 2: what it does

	int Style;	// +0x0C (4 bytes as they are): StarbaseInfoClass's Style, the column of the starbase's model names  // NAME 2: what it picks
	std::vector<LoungeNPCRoomStruct> Rooms;	// +0x10 (a 4-byte count, then the entries)  // NAME 2: what 0x6987A0 makes of them
	std::vector<LoungeNPCFacilityStruct> Facilities;	// +0x20 (a 4-byte count, then the entries)  // NAME 2: what 0x6987A0 makes of them
	std::vector<LoungeNPCNPCStruct> NPCs;	// +0x30 (a 4-byte count, then the entries)  // NAME 2: what 0x6987A0 makes of them
};

#endif
