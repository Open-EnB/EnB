// Earth & Beyond client (reconstruction): app\packets\pkloungenpc.cpp. Notes at the end of the file.
#include "pkloungenpc.h"
#include <stdio.h>

LoungeNPCPacketClass::LoungeNPCPacketClass(int style) :
	Style(style)
{
}

LoungeNPCPacketClass::LoungeNPCPacketClass(RawPacketClass *raw)
{
	int count = 0;
	raw->Get(&Style, sizeof(Style));
	raw->Get(&count, sizeof(count));
	Rooms.resize(count);
	for (int i = 0; i < count; i++) {
		LoungeNPCRoomStruct &entry = Rooms[i];
		raw->Get(&entry.RoomType, sizeof(entry.RoomType));
		raw->Get(&entry.Flags, sizeof(entry.Flags));
		raw->Get(&entry.Unknown08, sizeof(entry.Unknown08));
		raw->Get(&entry.Unknown0C, sizeof(entry.Unknown0C));
		raw->Get(&entry.Unknown10, sizeof(entry.Unknown10));
	}
	raw->Get(&count, sizeof(count));
	Facilities.resize(count);
	for (i = 0; i < count; i++) {
		LoungeNPCFacilityStruct &entry = Facilities[i];
		raw->Get(&entry.RoomType, sizeof(entry.RoomType));
		raw->Get(&entry.Terminal, sizeof(entry.Terminal));
		raw->Get(&entry.FacilityType, sizeof(entry.FacilityType));
		raw->Get(&entry.Unknown0C, sizeof(entry.Unknown0C));
	}
	raw->Get(&count, sizeof(count));
	NPCs.resize(count);
	for (i = 0; i < count; i++) {
		LoungeNPCNPCStruct &entry = NPCs[i];
		raw->Get(&entry.RoomType, sizeof(entry.RoomType));
		raw->Get(&entry.Slot, sizeof(entry.Slot));
		raw->Get(&entry.Unknown08, sizeof(entry.Unknown08));
		raw->Get(&entry.Booth, sizeof(entry.Booth));
		raw->Get(&entry.Unknown10, sizeof(entry.Unknown10));
		raw->Get(&entry.Length, sizeof(entry.Length));
		if (entry.Length != 0) {
			char text[256];
			raw->Get(text, entry.Length);
			text[entry.Length] = 0;
			if (text != NULL) entry.Text = text;	// the client tests the buffer
		}
		raw->Get((Unknown_PacketDataClass *)&entry.Desc);
	}
}

LoungeNPCPacketClass::~LoungeNPCPacketClass()
{
}

RawPacketClass *LoungeNPCPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(82, Get_Data_Size());
	int count = 0;
	raw->Add(&Style, sizeof(Style));
	count = Rooms.size();
	raw->Add(&count, sizeof(count));
	for (int i = 0; i < count; i++) {
		LoungeNPCRoomStruct &entry = Rooms[i];
		raw->Add(&entry.RoomType, sizeof(entry.RoomType));
		raw->Add(&entry.Flags, sizeof(entry.Flags));
		raw->Add(&entry.Unknown08, sizeof(entry.Unknown08));
		raw->Add(&entry.Unknown0C, sizeof(entry.Unknown0C));
		raw->Add(&entry.Unknown10, sizeof(entry.Unknown10));
	}
	count = Facilities.size();
	raw->Add(&count, sizeof(count));
	for (i = 0; i < count; i++) {
		LoungeNPCFacilityStruct &entry = Facilities[i];
		raw->Add(&entry.RoomType, sizeof(entry.RoomType));
		raw->Add(&entry.Terminal, sizeof(entry.Terminal));
		raw->Add(&entry.FacilityType, sizeof(entry.FacilityType));
		raw->Add(&entry.Unknown0C, sizeof(entry.Unknown0C));
	}
	count = NPCs.size();
	raw->Add(&count, sizeof(count));
	for (i = 0; i < count; i++) {
		LoungeNPCNPCStruct &entry = NPCs[i];
		raw->Add(&entry.RoomType, sizeof(entry.RoomType));
		raw->Add(&entry.Slot, sizeof(entry.Slot));
		raw->Add(&entry.Unknown08, sizeof(entry.Unknown08));
		raw->Add(&entry.Booth, sizeof(entry.Booth));
		raw->Add(&entry.Unknown10, sizeof(entry.Unknown10));
		raw->Add(&entry.Length, sizeof(entry.Length));
		if (entry.Length != 0) {
			raw->Add(entry.Text.c_str(), entry.Length);
		}
		raw->Add((Unknown_PacketDataClass &)entry.Desc);
	}
	return(raw);
}

int LoungeNPCPacketClass::Get_Data_Size()
{
	int count = Rooms.size();
	int size = sizeof(Style) + sizeof(int);
	for (int i = 0; i < count; i++) {
		size += sizeof(LoungeNPCRoomStruct);
	}
	count = Facilities.size();
	size += sizeof(int);
	for (i = 0; i < count; i++) {
		size += sizeof(LoungeNPCFacilityStruct);
	}
	count = NPCs.size();
	size += sizeof(int);
	for (i = 0; i < count; i++) {
		LoungeNPCNPCStruct &entry = NPCs[i];
		size += sizeof(entry.RoomType) + sizeof(entry.Slot) + sizeof(entry.Unknown08) + sizeof(entry.Booth) +
			sizeof(entry.Unknown10) + sizeof(entry.Length);
		if (entry.Length != 0) {
			size += entry.Length;
		}
		size += ((Unknown_PacketDataClass &)entry.Desc).Get_Size();
	}
	return(size);
}

int LoungeNPCPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "LoungeNPC"));
}

void LoungeNPCPacketClass::Add_Room(const LoungeNPCRoomStruct &entry)
{
	Rooms.push_back(entry);
}

void LoungeNPCPacketClass::Add_Facility(const LoungeNPCFacilityStruct &entry)
{
	Facilities.push_back(entry);
}

void LoungeNPCPacketClass::Add_NPC(const LoungeNPCNPCStruct &entry)
{
	NPCs.push_back(entry);
}


/*
**	Reconstruction notes (pkloungenpc.cpp)
**
**	* Client 0x897800-0x89A710 (0x8977D0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x89A710): LOUNGE_NPC 82, written by hand (builder P6, 2026-10-27): three std::vectors behind 4-byte
**	  counts, resized to the count and filled by index; the third's entries carry a text with its length
**	  and an AvatarDescClass through RawPacketClass's Get / Add of its wire interface.
**	* The raw constructor and Build are exact (builder PK1, 2026-10-28): every list's entries through a
**	  reference, and the constructor tests the text buffer before the assignment (P6's two near misses).
**	* The file name is provisional (no assert names it).
*/
