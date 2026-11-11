// Earth & Beyond client (reconstruction): app\client\sbase_info.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\sbase_info.cpp', 0x6974F0-0x69ABC0). Notes at the end.
#include "sbase_info.h"
#include "..\shared\g_assert.h"
#include "..\shared\message.h"
#include "..\packets\pkloungenpc.h"

// @0x006974F0
StarbaseNPCClass::StarbaseNPCClass(int slot, int unknown04, int booth, int unknown1c, const AvatarDescClass &desc, const char *name) :
	Slot(slot),
	Unknown04(unknown04),
	Name(""),
	Booth(booth),
	Unknown1C(unknown1c),
	Desc(desc)
{
	if (name) {
		Name = name;
	}
}

// @0x006976C0
int StarbaseNPCClass::Get_Booth_Index()
{
	if (Booth >= BOOTH_WEAPON && Booth <= BOOTH_BLACKMKT) {
		return(Booth - BOOTH_WEAPON);
	}
	return(-1);
}

// @0x006976F0
StarbaseFacilityClass::StarbaseFacilityClass(int terminal, int type, int unknown08) :
	Terminal(terminal),
	Type(type),
	Unknown08(unknown08)
{
}

// @0x00697720
StarbaseRoomClass::StarbaseRoomClass(int type) :
	Type(type),
	Unknown04(false),
	Unknown05(false),
	Unknown06(false),
	Unknown07(false),
	Unknown08(false),
	Unknown09(false),
	Unknown0A(false),
	Unknown0B(false),
	Unknown0C(false),
	Unknown10(0),
	Unknown14(0),
	Unknown18(0.0f, 0.0f, 0.0f),
	Facilities(0),
	NPCs(0)
{
}

// @0x00697AA0
StarbaseRoomClass::~StarbaseRoomClass()
{
	std::hash_map<unsigned int, StarbaseFacilityClass *>::iterator facility;
	for (facility = Facilities.begin(); facility != Facilities.end(); ++facility) {
		delete facility->second;
	}
	Facilities.clear();

	std::hash_map<unsigned int, StarbaseNPCClass *>::iterator npc;
	for (npc = NPCs.begin(); npc != NPCs.end(); ++npc) {
		delete npc->second;
	}
	NPCs.clear();
}






















































































// @0x00697E00
void StarbaseRoomClass::Add_Facility(StarbaseFacilityClass *facility)
{
	FAIL_IF(!facility) return;

	if (Find_Facility(facility->Terminal)) {
		Message_Info("", "Starbase Info", __FILE__, __LINE__, "Duplicate facility type definition encountered");
		return;
	}
	Facilities[facility->Terminal] = facility;
}

// @0x00697F70
void StarbaseRoomClass::Add_NPC(StarbaseNPCClass *npc)
{
	FAIL_IF(!npc) return;

	if (Find_NPC(npc->Slot)) {
		Message_Info("", "Starbase Info", __FILE__, __LINE__, "Duplicate npc slot definition encountered");
		return;
	}
	NPCs[npc->Slot] = npc;
}

// @0x006980E0
StarbaseFacilityClass *StarbaseRoomClass::Find_Facility(unsigned int terminal)
{
	std::hash_map<unsigned int, StarbaseFacilityClass *>::iterator facility = Facilities.find(terminal);
	if (facility == Facilities.end()) {
		return(NULL);
	}
	return(facility->second);
}

// @0x00698150
StarbaseNPCClass *StarbaseRoomClass::Find_NPC(unsigned int slot)
{
	std::hash_map<unsigned int, StarbaseNPCClass *>::iterator npc = NPCs.find(slot);
	if (npc == NPCs.end()) {
		return(NULL);
	}
	return(npc->second);
}

// @0x006981C0
StarbaseInfoClass::StarbaseInfoClass(int style) :
	Rooms(0),
	Style(style)
{
}

// @0x00698370
StarbaseInfoClass::~StarbaseInfoClass()
{
	std::hash_map<unsigned int, StarbaseRoomClass *>::iterator room;
	for (room = Rooms.begin(); room != Rooms.end(); ++room) {
		delete room->second;
	}
	Rooms.clear();
}












// @0x006985C0
void StarbaseInfoClass::Add_Room(StarbaseRoomClass *room)
{
	FAIL_IF(!room) return;

	if (Find_Room(room->Type)) {
		Message_Info("", "Starbase Info", __FILE__, __LINE__, "Duplicate room type definition encountered");
		return;
	}
	Rooms[room->Type] = room;
}

// @0x00698730
StarbaseRoomClass *StarbaseInfoClass::Find_Room(unsigned int type)
{
	std::hash_map<unsigned int, StarbaseRoomClass *>::iterator room = Rooms.find(type);
	if (room == Rooms.end()) {
		return(NULL);
	}
	return(room->second);
}

// LOUNGE_NPC's room types as StarbaseRoomClass's (its 2 and 3 change places)
static inline int Convert_Room_Type(int type)	// NAME 2: what it does
{
	switch (type) {
		case 0:	return(StarbaseRoomClass::HANGAR);
		case 1:	return(StarbaseRoomClass::MAIN_DECK);
		case 2:	return(StarbaseRoomClass::MERCHANTS);
		case 3:	return(StarbaseRoomClass::NPC_LOUNGE);
		case 4:	return(StarbaseRoomClass::LOUNGE);
	}
	return(StarbaseRoomClass::NOT_ACTIVE);
}

// LOUNGE_NPC's facility types as StarbaseFacilityClass's (the same numbers)
static inline int Convert_Facility_Type(int type)	// NAME 2: what it does
{
	switch (type) {
		case 0:	return(StarbaseFacilityClass::REFINERY);
		case 1:	return(StarbaseFacilityClass::ANALYZE);
		case 2:	return(StarbaseFacilityClass::MANUFACTURE);
		case 3:	return(StarbaseFacilityClass::JOBS);
		case 4:	return(StarbaseFacilityClass::BROWSER);
		case 5:	return(StarbaseFacilityClass::SHIP_CUSTOMIZE);
		case 6:	return(StarbaseFacilityClass::AVATAR_CUSTOMIZE);
		case 7:	return(StarbaseFacilityClass::ENCYCLOPEDIA);
		case 9:	return(StarbaseFacilityClass::UNKNOWN_9);
	}
	return(StarbaseFacilityClass::INVALID);
}

// LOUNGE_NPC's booth types as StarbaseNPCClass's (two higher; anything else is no booth)
static inline int Convert_Booth(int booth)	// NAME 2: what it does
{
	switch (booth) {
		case 0:	return(StarbaseNPCClass::BOOTH_WEAPON);
		case 1:	return(StarbaseNPCClass::BOOTH_SYSTEM);
		case 2:	return(StarbaseNPCClass::BOOTH_CORE);
		case 3:	return(StarbaseNPCClass::BOOTH_CONSUMABLE);
		case 4:	return(StarbaseNPCClass::BOOTH_JUNK);
		case 5:	return(StarbaseNPCClass::BOOTH_COMPONENT);
		case 6:	return(StarbaseNPCClass::BOOTH_RESOURCE);
		case 7:	return(StarbaseNPCClass::BOOTH_BLACKMKT);
	}
	return(StarbaseNPCClass::BOOTH_NONE);
}

// LOUNGE_NPC's fifth NPC number as StarbaseNPCClass's +0x1C (1..3, anything else 0)
static inline int Convert_Unknown1C(int value)	// NAME 1: neutral (D10)
{
	switch (value) {
		case 0:	return(0);
		case 1:	return(1);
		case 2:	return(2);
		case 3:	return(3);
	}
	return(0);
}

// LOUNGE_NPC's room flags, the low byte's eight (0x6987A0 sets the ninth itself: with all nine here, or fewer than
// eight, VC6's inline budget leaves 0x6987A0's nested calls and its table's destructor differently from the client)
static inline void Set_Room_Flags(StarbaseRoomClass *room, int flags)	// NAME 2: what it does
{
	if (flags & 0x001) room->Unknown04 = true;
	if (flags & 0x002) room->Unknown05 = true;
	if (flags & 0x004) room->Unknown06 = true;
	if (flags & 0x008) room->Unknown07 = true;
	if (flags & 0x010) room->Unknown08 = true;
	if (flags & 0x020) room->Unknown09 = true;
	if (flags & 0x040) room->Unknown0A = true;
	if (flags & 0x080) room->Unknown0B = true;
}

// @0x006987A0
// The rooms by type first (a local table), then each room's facilities and NPCs, then the starbase with the rooms
StarbaseInfoClass *StarbaseInfoClass::Create(LoungeNPCPacketClass *packet)
{
	std::hash_map<unsigned int, StarbaseRoomClass *> rooms;












	for (std::vector<LoungeNPCRoomStruct>::iterator room_entry = packet->Rooms.begin(); room_entry != packet->Rooms.end(); ++room_entry) {
		int room_type = Convert_Room_Type(room_entry->RoomType);
		FAIL_IF(room_type == StarbaseRoomClass::NOT_ACTIVE) continue;

		StarbaseRoomClass *room = new StarbaseRoomClass(room_type);
		int flags = room_entry->Flags;
		Set_Room_Flags(room, flags);
		if (flags & 0x100) room->Unknown0C = true;
		room->Unknown10 = room_entry->Unknown08;
		room->Unknown14 = room_entry->Unknown0C;
		room->Unknown18 = room_entry->Unknown10;
		rooms[room_type] = room;
	}


	for (std::vector<LoungeNPCFacilityStruct>::iterator facility_entry = packet->Facilities.begin(); facility_entry != packet->Facilities.end(); ++facility_entry) {
		int facility_type = Convert_Facility_Type(facility_entry->FacilityType);
		FAIL_IF(facility_type == StarbaseFacilityClass::INVALID) continue;

		int room_type = Convert_Room_Type(facility_entry->RoomType);
		FAIL_IF(room_type == StarbaseRoomClass::NOT_ACTIVE) continue;

		StarbaseRoomClass *room = rooms[room_type];
		FAIL_IF(room == 0) continue;

		room->Add_Facility(new StarbaseFacilityClass(facility_entry->Terminal, facility_type, 0));
	}




	for (std::vector<LoungeNPCNPCStruct>::iterator npc_entry = packet->NPCs.begin(); npc_entry != packet->NPCs.end(); ++npc_entry) {
		int booth = Convert_Booth(npc_entry->Booth);
		int unknown1c = Convert_Unknown1C(npc_entry->Unknown10);

		int room_type = Convert_Room_Type(npc_entry->RoomType);
		FAIL_IF(room_type == StarbaseRoomClass::NOT_ACTIVE) continue;

		StarbaseRoomClass *room = rooms[room_type];
		FAIL_IF(room == 0) continue;

		room->Add_NPC(new StarbaseNPCClass(npc_entry->Slot, npc_entry->Unknown08, booth, unknown1c, npc_entry->Desc, npc_entry->Text.c_str()));
	}

	StarbaseInfoClass *info = new StarbaseInfoClass(packet->Style);
	for (std::hash_map<unsigned int, StarbaseRoomClass *>::iterator room = rooms.begin(); room != rooms.end(); ++room) {
		info->Add_Room(room->second);
	}
	return(info);
}

// Notes (reconstruction):
// - One object, 0x6974F0-0x69ABC0: the functions above in the client's order (0x6974F0-0x698E96), then the SGI hash_map /
//   hashtable / vector instances of the three tables (0x699090-0x69AB9F) and the guarded <string> static (0x69ABA0); the
//   static pair at 0x69ABD0 / 0x69AC00 opens the next object. The instance 0x699150 (hash_map(size_type)) is not exact.
// - The asserts sit on the client's lines (163-382); the three Add_ functions are also inlined in 0x6987A0, with their lines.
// - Both destructors declare the iterator before the loop: VC6 then keeps it in registers, as the client does (a
//   declaration in the for statement builds begin()'s result in place, in memory).
// - The lookups test 'not found' first (the client's block order).
// - Convert_Unknown1C lists case 0: the client's switch is a jump table (four cases).
