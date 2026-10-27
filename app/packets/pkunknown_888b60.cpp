// Earth & Beyond client (reconstruction): app\packets\pkunknown_888b60.cpp. Notes at the end of the file.
#include "pkunknown_888b60.h"
#include <stdio.h>

GroupServerEntryStruct::GroupServerEntryStruct() :
	Unknown00(""),
	Unknown10(0),
	Unknown18(0)
{
}

GroupServerEntryStruct::GroupServerEntryStruct(const std::string &unknown00, __int64 unknown10, __int64 unknown18) :
	Unknown00(unknown00),
	Unknown10(unknown10),
	Unknown18(unknown18)
{
}

int Unknown_B0A794PacketClass::Get_Size(const GroupServerEntryStruct &entry)
{
	return(RawPacketClass::Get_Size(entry.Unknown00) + sizeof(entry.Unknown10) + sizeof(entry.Unknown18));
}

bool Unknown_B0A794PacketClass::Write(const GroupServerEntryStruct &entry, RawPacketClass *raw)
{
	raw->Add(entry.Unknown00);
	raw->Add(entry.Unknown10);
	raw->Add(entry.Unknown18);
	return(true);
}

bool Unknown_B0A794PacketClass::Read(GroupServerEntryStruct &entry, RawPacketClass *raw)
{
	raw->Get(&entry.Unknown00);
	raw->Get(entry.Unknown10);
	raw->Get(entry.Unknown18);
	return(true);
}

int Unknown_B0A794PacketClass::Get_Size(const std::vector<GroupServerEntryStruct> &list)
{
	int size = sizeof(unsigned char);
	for (std::vector<GroupServerEntryStruct>::const_iterator it = list.begin(); it != list.end(); it++) {
		size += Get_Size(*it);
	}
	return(size);
}

bool Unknown_B0A794PacketClass::Write(const std::vector<GroupServerEntryStruct> &list, RawPacketClass *raw)
{
	unsigned char count = list.size();
	raw->Add(&count, sizeof(count));
	for (std::vector<GroupServerEntryStruct>::const_iterator it = list.begin(); it != list.end(); it++) {
		Write(*it, raw);
	}
	return(true);
}

bool Unknown_B0A794PacketClass::Read(std::vector<GroupServerEntryStruct> &list, RawPacketClass *raw)
{
	unsigned char count = 0;
	raw->Get(&count, sizeof(count));
	list.reserve(count);
	for (unsigned char i = 0; i < count; i++) {
		GroupServerEntryStruct entry;
		Read(entry, raw);
		list.push_back(entry);
	}
	return(true);
}

GroupServerEntryStruct::~GroupServerEntryStruct()
{
}

Packet_00ADClass::Packet_00ADClass(int unknown0c, const std::string &unknown10, __int64 unknown20, __int64 unknown28) :
	Unknown0C(unknown0c)
{
	Unknown10.Unknown00 = unknown10;
	Unknown10.Unknown10 = unknown20;
	Unknown10.Unknown18 = unknown28;
}

Packet_00ADClass::Packet_00ADClass(RawPacketClass *raw) :
	Unknown0C(0)
{
	raw->Get(Unknown0C);
	Read(Unknown10, raw);
}

RawPacketClass *Packet_00ADClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(173, Get_Data_Size());
	raw->Add(Unknown0C);
	Write(Unknown10, raw);
	return(raw);
}

int Packet_00ADClass::Get_Data_Size()
{
	return(Get_Size(Unknown10) + sizeof(Unknown0C));
}

int Packet_00ADClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerLoginPacketClass"));
}

Packet_00AEClass::Packet_00AEClass(int unknown0c, int unknown10, __int64 unknown18, __int64 unknown20) :
	Unknown0C(unknown0c),
	Unknown10(unknown10),
	Unknown18(unknown18),
	Unknown20(unknown20)
{
}

Packet_00AEClass::Packet_00AEClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(0),
	Unknown18(0),
	Unknown20(0)
{
	raw->Get(Unknown0C);
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(Unknown18);
	raw->Get(Unknown20);
}

RawPacketClass *Packet_00AEClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(174, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown18);
	raw->Add(Unknown20);
	return(raw);
}

int Packet_00AEClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10) + sizeof(Unknown18) + sizeof(Unknown20));
}

int Packet_00AEClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerRequestPacketClass"));
}

Packet_00AFClass::Packet_00AFClass(int unknown0_c, int unknown10) : Unknown0C(unknown0_c), Unknown10(unknown10)
{
}

Packet_00AFClass::Packet_00AFClass(RawPacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(Unknown0C);
	raw->Get(&Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_00AFClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(175, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(&Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_00AFClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10));
}

int Packet_00AFClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerResponsePacketClass"));
}

Packet_00B0Class::Packet_00B0Class(RawPacketClass *raw) :
	Unknown10(0),
	Unknown38(true),
	Unknown3C(0)
{
	raw->Get(Unknown10);
	Read(Unknown18, raw);
	Read(Unknown28, raw);
	raw->Get(&Unknown38, sizeof(Unknown38));
	raw->Get(Unknown3C);
}

RawPacketClass *Packet_00B0Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(176, Get_Data_Size());
	raw->Add(Unknown10);
	Write(Unknown18, raw);
	Write(Unknown28, raw);
	raw->Add(&Unknown38, sizeof(Unknown38));
	raw->Add(Unknown3C);
	return(raw);
}

int Packet_00B0Class::Get_Data_Size()
{
	return(Get_Size(Unknown18) + Get_Size(Unknown28) + sizeof(Unknown10) + sizeof(Unknown38) + sizeof(Unknown3C));
}

int Packet_00B0Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerCreatePacketClass"));
}

void Packet_00B0Class::Add_Unknown18(const GroupServerEntryStruct &entry)
{
	Unknown18.push_back(entry);
}

void Packet_00B0Class::Add_Unknown28(const GroupServerEntryStruct &entry)
{
	Unknown28.push_back(entry);
}

Packet_00B1Class::Packet_00B1Class(__int64 unknown10, int unknown38) :
	Unknown10(unknown10),
	Unknown38(unknown38),
	Unknown3C(0)
{
}

Packet_00B1Class::Packet_00B1Class(RawPacketClass *raw) :
	Unknown10(0),
	Unknown38(0),
	Unknown3C(0)
{
	raw->Get(Unknown10);
	raw->Get(&Unknown38, sizeof(Unknown38));
	Read(Unknown18, raw);
	raw->Get(Unknown3C);
}

RawPacketClass *Packet_00B1Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(177, Get_Data_Size());
	raw->Add(Unknown10);
	raw->Add(&Unknown38, sizeof(Unknown38));
	Write(Unknown18, raw);
	raw->Add(Unknown3C);
	return(raw);
}

int Packet_00B1Class::Get_Data_Size()
{
	return(Get_Size(Unknown18) + sizeof(Unknown10) + sizeof(Unknown38) + sizeof(Unknown3C));
}

int Packet_00B1Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerUpdatePacketClass"));
}


/*
**	Reconstruction notes (pkunknown_888b60.cpp)
**
**	* Client 0x888B60-0x88BD10 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Opcode 174 (its Print: GroupServerRequestPacketClass) written by hand (builder P3, 2026-10-26),
**	  5 of 5 exact in appfile.py match: an int and two __int64 in network order around a raw int.
**	* Opcode 173 (Print 'GroupServerLoginPacketClass') written by hand (builder P5, 2026-10-27) over an empty
**	  base (vtable 0xB0A794) that 176 / 177's objects share; its constructors and Build exact once its string
**	  and two __int64 became the entry below (builder P6).
**	* Builder P6 (2026-10-27): the object opens with a 0x20-byte entry (a string, two __int64) and the base's
**	  helpers that read and write one entry or a vector of them (0x888B60-0x889230: they ignore this, and
**	  the packets inline them; 176's constructor calls Read for its second vector). 173 holds one entry at
**	  +0x10, 177 (Print 'GroupServerUpdatePacketClass') one at +0x18 (the client stores the entry's members
**	  through a pointer to it), 176 (Print 'GroupServerCreatePacketClass') two vectors of them.
*/
