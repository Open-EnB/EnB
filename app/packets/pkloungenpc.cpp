// Earth & Beyond client (reconstruction): app\packets\pkloungenpc.cpp. Notes at the end of the file.
#include "pkloungenpc.h"
#include <stdio.h>

LoungeNPCPacketClass::LoungeNPCPacketClass(int unknown0c) :
	Unknown0C(unknown0c)
{
}

// NOT EXACT: inline budget (frame 0x170 against 0x168, one string destructor inlined that the client calls; attempts.csv)
LoungeNPCPacketClass::LoungeNPCPacketClass(RawPacketClass *raw)
{
	int count = 0;
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&count, sizeof(count));
	Unknown10.resize(count);
	for (int i = 0; i < count; i++) {
		raw->Get(&Unknown10[i].Unknown00, sizeof(Unknown10[i].Unknown00));
		raw->Get(&Unknown10[i].Unknown04, sizeof(Unknown10[i].Unknown04));
		raw->Get(&Unknown10[i].Unknown08, sizeof(Unknown10[i].Unknown08));
		raw->Get(&Unknown10[i].Unknown0C, sizeof(Unknown10[i].Unknown0C));
		raw->Get(&Unknown10[i].Unknown10, sizeof(Unknown10[i].Unknown10));
	}
	raw->Get(&count, sizeof(count));
	Unknown20.resize(count);
	for (i = 0; i < count; i++) {
		raw->Get(&Unknown20[i].Unknown00, sizeof(Unknown20[i].Unknown00));
		raw->Get(&Unknown20[i].Unknown04, sizeof(Unknown20[i].Unknown04));
		raw->Get(&Unknown20[i].Unknown08, sizeof(Unknown20[i].Unknown08));
		raw->Get(&Unknown20[i].Unknown0C, sizeof(Unknown20[i].Unknown0C));
	}
	raw->Get(&count, sizeof(count));
	Unknown30.resize(count);
	for (i = 0; i < count; i++) {
		LoungeNPCUnknown30Struct &entry = Unknown30[i];
		raw->Get(&entry.Unknown00, sizeof(entry.Unknown00));
		raw->Get(&entry.Unknown04, sizeof(entry.Unknown04));
		raw->Get(&entry.Unknown08, sizeof(entry.Unknown08));
		raw->Get(&entry.Unknown0C, sizeof(entry.Unknown0C));
		raw->Get(&entry.Unknown10, sizeof(entry.Unknown10));
		raw->Get(&entry.Length, sizeof(entry.Length));
		if (entry.Length != 0) {
			char text[256];
			raw->Get(text, entry.Length);
			text[entry.Length] = 0;
			entry.Text = text;
		}
		raw->Get((Unknown_PacketDataClass *)&entry.Desc);
	}
}

LoungeNPCPacketClass::~LoungeNPCPacketClass()
{
}

// NOT EXACT: shape (the client keeps each list's byte offset in a stack slot; attempts.csv)
RawPacketClass *LoungeNPCPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(82, Get_Data_Size());
	int count = 0;
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	count = Unknown10.size();
	raw->Add(&count, sizeof(count));
	for (int i = 0; i < count; i++) {
		raw->Add(&Unknown10[i].Unknown00, sizeof(Unknown10[i].Unknown00));
		raw->Add(&Unknown10[i].Unknown04, sizeof(Unknown10[i].Unknown04));
		raw->Add(&Unknown10[i].Unknown08, sizeof(Unknown10[i].Unknown08));
		raw->Add(&Unknown10[i].Unknown0C, sizeof(Unknown10[i].Unknown0C));
		raw->Add(&Unknown10[i].Unknown10, sizeof(Unknown10[i].Unknown10));
	}
	count = Unknown20.size();
	raw->Add(&count, sizeof(count));
	for (i = 0; i < count; i++) {
		raw->Add(&Unknown20[i].Unknown00, sizeof(Unknown20[i].Unknown00));
		raw->Add(&Unknown20[i].Unknown04, sizeof(Unknown20[i].Unknown04));
		raw->Add(&Unknown20[i].Unknown08, sizeof(Unknown20[i].Unknown08));
		raw->Add(&Unknown20[i].Unknown0C, sizeof(Unknown20[i].Unknown0C));
	}
	count = Unknown30.size();
	raw->Add(&count, sizeof(count));
	for (i = 0; i < count; i++) {
		LoungeNPCUnknown30Struct &entry = Unknown30[i];
		raw->Add(&entry.Unknown00, sizeof(entry.Unknown00));
		raw->Add(&entry.Unknown04, sizeof(entry.Unknown04));
		raw->Add(&entry.Unknown08, sizeof(entry.Unknown08));
		raw->Add(&entry.Unknown0C, sizeof(entry.Unknown0C));
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
	int count = Unknown10.size();
	int size = sizeof(Unknown0C) + sizeof(int);
	for (int i = 0; i < count; i++) {
		size += sizeof(LoungeNPCUnknown10Struct);
	}
	count = Unknown20.size();
	size += sizeof(int);
	for (i = 0; i < count; i++) {
		size += sizeof(LoungeNPCUnknown20Struct);
	}
	count = Unknown30.size();
	size += sizeof(int);
	for (i = 0; i < count; i++) {
		LoungeNPCUnknown30Struct &entry = Unknown30[i];
		size += sizeof(entry.Unknown00) + sizeof(entry.Unknown04) + sizeof(entry.Unknown08) + sizeof(entry.Unknown0C) +
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

void LoungeNPCPacketClass::Add_Unknown10(const LoungeNPCUnknown10Struct &entry)
{
	Unknown10.push_back(entry);
}

void LoungeNPCPacketClass::Add_Unknown20(const LoungeNPCUnknown20Struct &entry)
{
	Unknown20.push_back(entry);
}

void LoungeNPCPacketClass::Add_Unknown30(const LoungeNPCUnknown30Struct &entry)
{
	Unknown30.push_back(entry);
}


/*
**	Reconstruction notes (pkloungenpc.cpp)
**
**	* Client 0x897800-0x89A710 (0x8977D0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x89A710): LOUNGE_NPC 82, written by hand (builder P6, 2026-10-27): three std::vectors behind 4-byte
**	  counts, resized to the count and filled by index; the third's entries carry a text with its length
**	  and an AvatarDescClass through RawPacketClass's Get / Add of its wire interface.
**	* NOT EXACT: the raw constructor (inline budget: frame and one string destructor) and Build (shape:
**	  the client's per-list offset slot), two rounds each (.work/p6/v82.py; attempts.csv).
**	* The file name is provisional (no assert names it).
*/
