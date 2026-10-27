// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeplan.cpp. Notes at the end of the file.
#include "pkgeplan.h"
#include "..\..\shared\unknown_debug.h"
#include <stdio.h>
#include <string.h>


GameEditorRequestPlanetUnitTypesPacketClass::GameEditorRequestPlanetUnitTypesPacketClass()
{
}

GameEditorRequestPlanetUnitTypesPacketClass::GameEditorRequestPlanetUnitTypesPacketClass(RawPacketClass *)
{
}

RawPacketClass *GameEditorRequestPlanetUnitTypesPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30023, Get_Data_Size());
	return(raw);
}

int GameEditorRequestPlanetUnitTypesPacketClass::Get_Data_Size()
{
	return(0);
}

int GameEditorRequestPlanetUnitTypesPacketClass::Print(char *buffer, int)
{
	strcpy(buffer, "GameEditorRequestPlanetUnitTypes {NO DATA}");
	return(strlen(buffer));
}

Packet_7548Class::Packet_7548Class(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_7548Class::Packet_7548Class(RawPacketClass *raw) :
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

Packet_7548Class::~Packet_7548Class()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_7548Class::Build()
{
	RawPacketClass *packet = new RawPacketClass(30024, Get_Data_Size());
	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int Packet_7548Class::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_7548Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Types=%s}", "GameEditorPlanetUnitTypes", Data));
}

bool Packet_7548Class::Process_Packet(void *)
{
	return(true);
}


Packet_7549Class::Packet_7549Class(int unit_type) : UnitType(unit_type)
{
}

Packet_7549Class::Packet_7549Class(RawPacketClass *raw)
{
	raw->Get(&UnitType, sizeof(UnitType));
}

RawPacketClass *Packet_7549Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30025, Get_Data_Size());
	raw->Add(&UnitType, sizeof(UnitType));
	return(raw);
}

int Packet_7549Class::Get_Data_Size()
{
	return(sizeof(UnitType));
}

int Packet_7549Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {UnitType=%d}", "GameEditorRequestPlanetUnitDetails", UnitType));
}

Packet_754AClass::Packet_754AClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

Packet_754AClass::Packet_754AClass(RawPacketClass *raw) :
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

Packet_754AClass::~Packet_754AClass()
{
	if (Owned) {
		delete [] Data;
	}
}

RawPacketClass *Packet_754AClass::Build()
{
	RawPacketClass *packet = new RawPacketClass(30026, Get_Data_Size());
	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int Packet_754AClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int Packet_754AClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Details=%s}", "GameEditorPlanetUnitDetails", Data));
}

bool Packet_754AClass::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (gameedit\pkgeplan.cpp)
**
**	* Client 0x860D30-0x861730 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* Named pkunknown_860d30.cpp until the rename (D95(c), builder P6, 2026-10-27).
**	* Opcode 30023 written by hand (builder P5, 2026-10-27). The object is the client's
**	  app\Packets\gameedit\pkgeplan.cpp: the raw constructor at 0x860F80 between 30023 and 30025 asserts
**	  on its line 69. The send-only 30024 and 30026 (not in the packet switch; Process_Packet true) written
**	  by hand (builder P5, 2026-10-27): G_ASSERT(Data) on lines 69 and 182, Build's G_ASSERT(packet) on 85
**	  and 198 (blank lines above them).
*/
