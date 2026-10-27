// Earth & Beyond client (reconstruction): app\packets\pkunknown_863720.cpp. Notes at the end of the file.
#include "pkunknown_863720.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

Packet_00E6Class::Packet_00E6Class(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, PacketClass *packet) :
	Data(NULL),
	DataSize(0),
	Unknown18(unknown18),
	String(NULL),
	Unknown28(unknown28),
	Unknown30(unknown30),
	Unknown34(NULL)
{
	String = nstrdup(string);
	RawPacketClass *raw = packet->Build();
	DataSize = raw->Data.Get_Size() + sizeof(PacketHeaderClass);
	if (DataSize != 0) {
		Data = new char[DataSize];
		*(PacketHeaderClass *)Data = raw->Header;
		char *buffer = raw->Data.Buffer;
		memcpy(Data + sizeof(PacketHeaderClass), buffer, raw->Data.Get_Size());
	}
	delete raw;
}

Packet_00E6Class::Packet_00E6Class(RawPacketClass *raw) :
	Data(NULL),
	DataSize(0),
	Unknown18(0),
	String(NULL),
	Unknown28(0),
	Unknown30(0),
	Unknown34(NULL)
{
	raw->Get(Unknown18);
	String = raw->Get_String();
	raw->Get(Unknown28);
	raw->Get(Unknown30);
	raw->Get(DataSize);
	Data = new char[DataSize];
	raw->Get(Data, DataSize);
}

Packet_00E6Class::~Packet_00E6Class()
{
	delete [] String;
	String = NULL;
	delete [] Data;
	Data = NULL;
	if (Unknown34 != NULL) {
		delete Unknown34;
	}
	Unknown34 = NULL;
}

RawPacketClass *Packet_00E6Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown18);
	raw->Add_String(String);
	raw->Add(Unknown28);
	raw->Add(Unknown30);
	raw->Add(DataSize);
	raw->Add(Data, DataSize);
	return(raw);
}

int Packet_00E6Class::Get_Type()
{
	return(230);
}

// NOT EXACT: shape (the client adds the string's size and 0x18 first, then DataSize; attempts.csv)
int Packet_00E6Class::Get_Data_Size()
{
	return(DataSize + RawPacketClass::Get_Size(String) + sizeof(Unknown18) + sizeof(Unknown28) + sizeof(Unknown30) + sizeof(DataSize));
}

int Packet_00E6Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalContainerRequest"));
}

Packet_00E7Class::Packet_00E7Class(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, PacketClass *packet) :
	Data(NULL),
	DataSize(0),
	Unknown18(unknown18),
	String(NULL),
	Unknown28(unknown28),
	Unknown30(unknown30)
{
	String = nstrdup(string);
	RawPacketClass *raw = packet->Build();
	DataSize = raw->Data.Get_Size() + sizeof(PacketHeaderClass);
	if (DataSize != 0) {
		Data = new char[DataSize];
		*(PacketHeaderClass *)Data = raw->Header;
		char *buffer = raw->Data.Buffer;
		memcpy(Data + sizeof(PacketHeaderClass), buffer, raw->Data.Get_Size());
	}
	delete raw;
}

Packet_00E7Class::Packet_00E7Class(RawPacketClass *raw) :
	Data(NULL),
	DataSize(0),
	Unknown18(0),
	String(NULL),
	Unknown28(0),
	Unknown30(0)
{
	raw->Get(Unknown18);
	String = raw->Get_String();
	raw->Get(Unknown28);
	raw->Get(Unknown30);
	raw->Get(DataSize);
	Data = new char[DataSize];
	raw->Get(Data, DataSize);
}

Packet_00E7Class::~Packet_00E7Class()
{
	delete [] String;
	String = NULL;
	delete [] Data;
	Data = NULL;
}

RawPacketClass *Packet_00E7Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown18);
	raw->Add_String(String);
	raw->Add(Unknown28);
	raw->Add(Unknown30);
	raw->Add(DataSize);
	raw->Add(Data, DataSize);
	return(raw);
}

int Packet_00E7Class::Get_Type()
{
	return(231);
}

// NOT EXACT: shape (as opcode 230's; attempts.csv)
int Packet_00E7Class::Get_Data_Size()
{
	return(DataSize + RawPacketClass::Get_Size(String) + sizeof(Unknown18) + sizeof(Unknown28) + sizeof(Unknown30) + sizeof(DataSize));
}

int Packet_00E7Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalContainerResponse"));
}


/*
**	Reconstruction notes (pkunknown_863720.cpp)
**
**	* Client 0x863720-0x864090 (0x8636F0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x864060): opcodes 230 and 231 (Print 'GlobalContainerRequest' / 'GlobalContainerResponse'),
**	  written by hand (builder P5, 2026-10-27), each value constructor first. Both carry another
**	  packet as its message bytes (header and payload, from the packet's Build); 230 also owns a
**	  packet pointer at +0x34 that its destructor deletes (set by neither constructor).
**	* The file name is provisional (no assert names it).
*/
