// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgespwn.cpp. Notes at the end of the file.
#include "pkgespwn.h"
#include "..\..\shared\unknown_debug.h"
#include <stdio.h>
#include <string.h>

Packet_7541Class::Packet_7541Class(char *s) :
	Size(0),
	Data(NULL),
	Owned(false)
{





	G_ASSERT(s);
	Data = s;
	Size = strlen(s) + 1;
}

Packet_7541Class::Packet_7541Class(RawPacketClass *raw) :
	Size(0),
	Data(NULL),
	Owned(false)
{
	raw->Get(&Size, sizeof(Size));
	if (Size > 0) {
		Data = new char[Size];



		G_ASSERT(Data);
		Owned = true;
		raw->Get(Data, Size);
	}
}

Packet_7541Class::~Packet_7541Class()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_7541Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30017, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	if (Size > 0) {
		raw->Add(Data, Size);
	}
	return(raw);
}

int Packet_7541Class::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_7541Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {SpawnLocationTemplateName=%s}", "GameEditorRequestSpawnDefaults", Data));
}

Packet_7542Class::Packet_7542Class(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_7542Class::Packet_7542Class(RawPacketClass *raw) :
	Data(NULL),
	Size(0),
	Owned(false)
{
	raw->Get(&Size, sizeof(Size));
	if (Size > 0) {
		Data = new char[Size];












		G_ASSERT(Data);
		Owned = true;
		raw->Get(Data, Size);
	}
}

Packet_7542Class::~Packet_7542Class()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_7542Class::Build()
{
	RawPacketClass *packet = new RawPacketClass(30018, Get_Data_Size());
	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int Packet_7542Class::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_7542Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {DefaultValues=%s}", "GameEditorSpawnDefaults", Data));
}

bool Packet_7542Class::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgespwn.cpp)
**
**	* The client's app\Packets\gameedit\pkgespwn.cpp (its asserts' __FILE__), 0x862350-0x862970 (0x862320 is
**	  the previous object's guarded static, 0xC416FC; this one's is 0x862970). Written by hand (builder P5,
**	  2026-10-27): opcode 30017 (a text the value constructor keeps, G_ASSERT(s) on line 17, the raw one's
**	  G_ASSERT(Data) on line 33) and the send-only 30018 (not in the packet switch: its Process_Packet
**	  returns true here; G_ASSERT(Data) on line 93 and Build's G_ASSERT(packet) on line 109).
*/
