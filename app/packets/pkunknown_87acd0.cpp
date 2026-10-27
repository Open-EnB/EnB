// Earth & Beyond client (reconstruction): app\packets\pkunknown_87acd0.cpp. Notes at the end of the file.
#include "pkunknown_87acd0.h"
#include <stdio.h>

Packet_00A8Class::Packet_00A8Class() :
	Unknown0C(0),
	Unknown10(""),
	Unknown20(""),
	Unknown30(0),
	Unknown34(0),
	Unknown38(0),
	Unknown40(0)
{
}

static void Read_Strings(std::vector<std::string> &list, RawPacketClass *raw)
{
	unsigned int count = 0;
	raw->Get(&count, sizeof(count));
	list.reserve(count);
	for (unsigned int i = 0; i < count; i++) {
		std::string string;
		raw->Get(&string);
		list.push_back(string);
	}
}

Packet_00A8Class::Packet_00A8Class(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(""),
	Unknown20(""),
	Unknown30(0),
	Unknown34(0),
	Unknown38(0),
	Unknown40(0)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10);
	raw->Get(&Unknown20);
	raw->Get(Unknown30);
	raw->Get(&Unknown34, sizeof(Unknown34));
	raw->Get(Unknown38);
	raw->Get(Unknown40);
	Read_Strings(Unknown48, raw);
	Read_Strings(Unknown58, raw);
}

Packet_00A8Class::~Packet_00A8Class()
{
}

static void Write_Strings(std::vector<std::string> &list, RawPacketClass *raw)
{
	int count = list.size();
	raw->Add(&count, sizeof(count));
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++) {
		raw->Add(*it);
	}
}

RawPacketClass *Packet_00A8Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(168, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(Unknown10);
	raw->Add(Unknown20);
	raw->Add(Unknown30);
	raw->Add(&Unknown34, sizeof(Unknown34));
	raw->Add(Unknown38);
	raw->Add(Unknown40);
	Write_Strings(Unknown48, raw);
	Write_Strings(Unknown58, raw);
	return(raw);
}

static int Get_Size_Strings(std::vector<std::string> &list)
{
	int size = sizeof(int);
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++) {
		size += RawPacketClass::Get_Size(*it);
	}
	return(size);
}

int Packet_00A8Class::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown10) + sizeof(Unknown0C);
	size += RawPacketClass::Get_Size(Unknown20) + sizeof(Unknown30) + sizeof(Unknown34) + sizeof(Unknown38) + sizeof(Unknown40);
	size += Get_Size_Strings(Unknown48);
	return(Get_Size_Strings(Unknown58) + size);
}

int Packet_00A8Class::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatProxyLoginPacketClass"));
}

Packet_00A9Class::Packet_00A9Class() :
	Unknown0C(0),
	Unknown10(-1),
	Data(0),
	Size(0),
	Owned(false)
{
}

Packet_00A9Class::Packet_00A9Class(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(-1),
	Data(0),
	Size(0),
	Owned(false)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(&Unknown14);
	raw->Get(&Unknown24);
	raw->Get(&Unknown34);
	raw->Get(&Unknown44);
	raw->Get(&Unknown54);
	unsigned int count = 0;
	raw->Get(&count, sizeof(count));
	Unknown64.reserve(count);
	for (unsigned int i = 0; i < count; i++) {
		std::string string;
		raw->Get(&string);
		Unknown64.push_back(string);
	}
	raw->Get(&Size, sizeof(Size));
	if (Size != 0) {
		Owned = true;
		Data = new char[Size];
		raw->Get(Data, Size);
	}
}

Packet_00A9Class::~Packet_00A9Class()
{
	if (Owned && Data != 0) {
		delete [] Data;
		Data = 0;
	}
}

RawPacketClass *Packet_00A9Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(169, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown14);
	raw->Add(Unknown24);
	raw->Add(Unknown34);
	raw->Add(Unknown44);
	raw->Add(Unknown54);
	int count = Unknown64.size();
	raw->Add(&count, sizeof(count));
	for (std::vector<std::string>::iterator it = Unknown64.begin(); it != Unknown64.end(); it++) {
		raw->Add(*it);
	}
	raw->Add(&Size, sizeof(Size));
	if (Size != 0) {
		raw->Add(Data, Size);
	}
	return(raw);
}

int Packet_00A9Class::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown14) + sizeof(Unknown0C) + sizeof(Unknown10);
	size += RawPacketClass::Get_Size(Unknown24);
	size += RawPacketClass::Get_Size(Unknown34);
	size += RawPacketClass::Get_Size(Unknown44);
	size += RawPacketClass::Get_Size(Unknown54) + sizeof(int);
	for (std::vector<std::string>::iterator it = Unknown64.begin(); it != Unknown64.end(); it++) {
		size += RawPacketClass::Get_Size(*it);
	}
	return(Size + size + sizeof(Size));
}

void Packet_00A9Class::Add_Unknown64(const std::string &string)
{
	Unknown64.push_back(string);
}

int Packet_00A9Class::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatProxyRequestPacketClass"));
}

Packet_00AAClass::Packet_00AAClass(int unknown0_c, int unknown10) : Unknown0C(unknown0_c), Unknown10(unknown10)
{
}

Packet_00AAClass::Packet_00AAClass(RawPacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_00AAClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(170, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_00AAClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10));
}

int Packet_00AAClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatProxyResponsePacketClass"));
}

Packet_00ABClass::Packet_00ABClass(const std::string &unknown0c) :
	Unknown0C(unknown0c),
	Unknown1C(""),
	Unknown2C(-1)
{
}

Packet_00ABClass::Packet_00ABClass(RawPacketClass *raw) :
	Unknown0C(""),
	Unknown1C(""),
	Unknown2C(-1)
{
	unsigned int count = 0;
	raw->Get(&Unknown0C);
	raw->Get(&Unknown1C);
	raw->Get(&Unknown2C, sizeof(Unknown2C));
	raw->Get(&count, sizeof(count));
	Unknown30.reserve(count);
	for (unsigned int i = 0; i < count; i++) {
		std::string string;
		raw->Get(&string);
		Unknown30.push_back(string);
	}
	raw->Get(&count, sizeof(count));
	Unknown40.reserve(count);
	for (i = 0; i < count; i++) {
		std::string string;
		raw->Get(&string);
		Unknown40.push_back(string);
	}
}

RawPacketClass *Packet_00ABClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(171, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Unknown1C);
	raw->Add(&Unknown2C, sizeof(Unknown2C));
	int count = Unknown30.size();
	raw->Add(&count, sizeof(count));
	for (std::vector<std::string>::iterator it = Unknown30.begin(); it != Unknown30.end(); it++) {
		raw->Add(*it);
	}
	count = Unknown40.size();
	raw->Add(&count, sizeof(count));
	for (it = Unknown40.begin(); it != Unknown40.end(); it++) {
		raw->Add(*it);
	}
	return(raw);
}

int Packet_00ABClass::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown0C);
	size += sizeof(Unknown2C) + sizeof(int) + RawPacketClass::Get_Size(Unknown1C);
	for (std::vector<std::string>::iterator it = Unknown30.begin(); it != Unknown30.end(); it++) {
		size += RawPacketClass::Get_Size(*it);
	}
	size += sizeof(int);
	for (it = Unknown40.begin(); it != Unknown40.end(); it++) {
		size += RawPacketClass::Get_Size(*it);
	}
	return(size);
}

void Packet_00ABClass::Add_Unknown30(const std::string &string)
{
	Unknown30.push_back(string);
}

void Packet_00ABClass::Add_Unknown40(const std::string &string)
{
	Unknown40.push_back(string);
}

int Packet_00ABClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "ChatProxyResponseListPacketClass"));
}


/*
**	Reconstruction notes (pkunknown_87acd0.cpp)
**
**	* Client 0x87ACD0-0x87DE90 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Opcode 168 (Print 'ChatProxyLoginPacketClass') written by hand (builder P6, 2026-10-27): two strings,
**	  numbers and two std::vectors of strings behind 4-byte counts.
**	* Opcode 169 (Print 'ChatProxyRequestPacketClass') written by hand (builder P6, 2026-10-27): five strings,
**	  a string list and a sized data block it owns when read.
**	* Opcode 171 (Print 'ChatProxyResponseListPacketClass') written by hand (builder P6, 2026-10-27).
**	* Not rebuilt yet: opcode 172 (declared in pk_classes.h).
*/
