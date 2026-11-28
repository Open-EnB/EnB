// Earth & Beyond client (reconstruction): app\packets\pkglobalcontainer.cpp. Notes at the end of the file.
#include "pkglobalcontainer.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\auth_product.h"
GlobalContainerRequestPacketClass::GlobalContainerRequestPacketClass(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, UnknownPacketBaseClass *packet) :
	Data(NULL),
	DataSize(0),
	Unknown18(unknown18),
	String(NULL),
	Unknown28(unknown28),
	Unknown30(unknown30),
	Unknown34(NULL)
{
	String = nstrdup(string);
	PacketClass *raw = packet->Build();
	DataSize = raw->Data.Get_Size() + sizeof(PacketHeaderClass);
	if (DataSize != 0) {
		Data = new char[DataSize];
		*(PacketHeaderClass *)Data = raw->Header;
		char *buffer = raw->Data.Buffer;
		memcpy(Data + sizeof(PacketHeaderClass), buffer, raw->Data.Get_Size());
	}
	delete raw;
}

GlobalContainerRequestPacketClass::GlobalContainerRequestPacketClass(PacketClass *raw) :
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

GlobalContainerRequestPacketClass::~GlobalContainerRequestPacketClass()
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

PacketClass *GlobalContainerRequestPacketClass::Build()
{
	PacketClass *raw = new PacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown18);
	raw->Add_String(String);
	raw->Add(Unknown28);
	raw->Add(Unknown30);
	raw->Add(DataSize);
	raw->Add(Data, DataSize);
	return(raw);
}

int GlobalContainerRequestPacketClass::Get_Type()
{
	return(230);
}

int GlobalContainerRequestPacketClass::Get_Data_Size()
{
	unsigned int size = PacketClass::Get_Size(String) + sizeof(Unknown18) + sizeof(Unknown28) + sizeof(Unknown30) + sizeof(DataSize);
	return(size + DataSize);
}

int GlobalContainerRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalContainerRequest"));
}

GlobalContainerResponsePacketClass::GlobalContainerResponsePacketClass(__int64 unknown18, const char *string, __int64 unknown28, int unknown30, UnknownPacketBaseClass *packet) :
	Data(NULL),
	DataSize(0),
	Unknown18(unknown18),
	String(NULL),
	Unknown28(unknown28),
	Unknown30(unknown30)
{
	String = nstrdup(string);
	PacketClass *raw = packet->Build();
	DataSize = raw->Data.Get_Size() + sizeof(PacketHeaderClass);
	if (DataSize != 0) {
		Data = new char[DataSize];
		*(PacketHeaderClass *)Data = raw->Header;
		char *buffer = raw->Data.Buffer;
		memcpy(Data + sizeof(PacketHeaderClass), buffer, raw->Data.Get_Size());
	}
	delete raw;
}

GlobalContainerResponsePacketClass::GlobalContainerResponsePacketClass(PacketClass *raw) :
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

GlobalContainerResponsePacketClass::~GlobalContainerResponsePacketClass()
{
	delete [] String;
	String = NULL;
	delete [] Data;
	Data = NULL;
}

PacketClass *GlobalContainerResponsePacketClass::Build()
{
	PacketClass *raw = new PacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown18);
	raw->Add_String(String);
	raw->Add(Unknown28);
	raw->Add(Unknown30);
	raw->Add(DataSize);
	raw->Add(Data, DataSize);
	return(raw);
}

int GlobalContainerResponsePacketClass::Get_Type()
{
	return(231);
}

int GlobalContainerResponsePacketClass::Get_Data_Size()
{
	unsigned int size = PacketClass::Get_Size(String) + sizeof(Unknown18) + sizeof(Unknown28) + sizeof(Unknown30) + sizeof(DataSize);
	return(size + DataSize);
}

int GlobalContainerResponsePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalContainerResponse"));
}


/*
**	Reconstruction notes (pkglobalcontainer.cpp)
**
**	* Client 0x863720-0x864090 (0x8636F0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x864060): opcodes 230 and 231 (Print 'GlobalContainerRequest' / 'GlobalContainerResponse'),
**	  written by hand (builder P5, 2026-10-27), each value constructor first. Both carry another
**	  packet as its message bytes (header and payload, from the packet's Build); 230 also owns a
**	  packet pointer at +0x34 that its destructor deletes (set by neither constructor).
**	* The file name is by convention from its class GlobalContainerRequest / Response (thunk order) and its place in retail's object order (between pkglobalconnect and pkglobalcreatecharacter), NAME 3; no assert names it.
*/
