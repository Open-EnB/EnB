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
**	* Not rebuilt yet: opcode 169, 171, 172 (declared in pk_classes.h).
*/
