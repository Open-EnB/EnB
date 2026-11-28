// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeplan.cpp. Notes at the end of the file.
#include "pkgeplan.h"
#include "..\..\shared\g_assert.h"
#include <stdio.h>
#include <string.h>


GameEditorRequestPlanetUnitTypesPacketClass::GameEditorRequestPlanetUnitTypesPacketClass()
{
}

GameEditorRequestPlanetUnitTypesPacketClass::GameEditorRequestPlanetUnitTypesPacketClass(PacketClass *)
{
}

PacketClass *GameEditorRequestPlanetUnitTypesPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(30023, Get_Data_Size());
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

GameEditorPlanetUnitTypesPacketClass::GameEditorPlanetUnitTypesPacketClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

GameEditorPlanetUnitTypesPacketClass::GameEditorPlanetUnitTypesPacketClass(PacketClass *raw) :
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

GameEditorPlanetUnitTypesPacketClass::~GameEditorPlanetUnitTypesPacketClass()
{
	if (Owned) {
		delete [] Data;
	}
}

PacketClass *GameEditorPlanetUnitTypesPacketClass::Encode()
{
	PacketClass *packet = new PacketClass(30024, Get_Data_Size());
	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int GameEditorPlanetUnitTypesPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorPlanetUnitTypesPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Types=%s}", "GameEditorPlanetUnitTypes", Data));
}

bool GameEditorPlanetUnitTypesPacketClass::Process_Packet(void *)
{
	return(true);
}


GameEditorRequestPlanetUnitDetailsPacketClass::GameEditorRequestPlanetUnitDetailsPacketClass(int unit_type) : UnitType(unit_type)
{
}

GameEditorRequestPlanetUnitDetailsPacketClass::GameEditorRequestPlanetUnitDetailsPacketClass(PacketClass *raw)
{
	raw->Get(&UnitType, sizeof(UnitType));
}

PacketClass *GameEditorRequestPlanetUnitDetailsPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(30025, Get_Data_Size());
	raw->Add(&UnitType, sizeof(UnitType));
	return(raw);
}

int GameEditorRequestPlanetUnitDetailsPacketClass::Get_Data_Size()
{
	return(sizeof(UnitType));
}

int GameEditorRequestPlanetUnitDetailsPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {UnitType=%d}", "GameEditorRequestPlanetUnitDetails", UnitType));
}

GameEditorPlanetUnitDetailsPacketClass::GameEditorPlanetUnitDetailsPacketClass(char *data, int size) :
	Data(data),
	Size(size),
	Owned(false)
{
}

GameEditorPlanetUnitDetailsPacketClass::GameEditorPlanetUnitDetailsPacketClass(PacketClass *raw) :
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

GameEditorPlanetUnitDetailsPacketClass::~GameEditorPlanetUnitDetailsPacketClass()
{
	if (Owned) {
		delete [] Data;
	}
}

PacketClass *GameEditorPlanetUnitDetailsPacketClass::Encode()
{
	PacketClass *packet = new PacketClass(30026, Get_Data_Size());
	G_ASSERT(packet);
	packet->Add(&Size, sizeof(Size));
	packet->Add(Data, Size);
	return(packet);
}

int GameEditorPlanetUnitDetailsPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size));
}

int GameEditorPlanetUnitDetailsPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Details=%s}", "GameEditorPlanetUnitDetails", Data));
}

bool GameEditorPlanetUnitDetailsPacketClass::Process_Packet(void *)
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
