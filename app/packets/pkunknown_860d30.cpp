// Earth & Beyond client (reconstruction): app\packets\pkunknown_860d30.cpp. Notes at the end of the file.
#include "pkunknown_860d30.h"
#include <stdio.h>


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


/*
**	Reconstruction notes (pkunknown_860d30.cpp)
**
**	* Client 0x860D30-0x861730 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Not rebuilt yet: opcode 30023 (declared in pk_classes.h).
*/
