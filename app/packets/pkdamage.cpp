// Earth & Beyond client (reconstruction): app\packets\pkdamage.cpp. Notes at the end of the file.
#include "pkdamage.h"
#include <stdio.h>


DamagePacketClass::DamagePacketClass(float damage, float modifer, int type, int inflicted, int source, int victim) : Damage(damage), Modifer(modifer), Type(type), Inflicted(inflicted), Source(source), Victim(victim)
{
}

DamagePacketClass::DamagePacketClass(RawPacketClass *raw)
{
	raw->Get(&Damage, sizeof(Damage));
	raw->Get(&Modifer, sizeof(Modifer));
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Inflicted, sizeof(Inflicted));
	raw->Get(&Source, sizeof(Source));
	raw->Get(&Victim, sizeof(Victim));
}

RawPacketClass *DamagePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(100, Get_Data_Size());
	raw->Add(&Damage, sizeof(Damage));
	raw->Add(&Modifer, sizeof(Modifer));
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Inflicted, sizeof(Inflicted));
	raw->Add(&Source, sizeof(Source));
	raw->Add(&Victim, sizeof(Victim));
	return(raw);
}

int DamagePacketClass::Get_Data_Size()
{
	return(sizeof(Damage) + sizeof(Modifer) + sizeof(Type) + sizeof(Inflicted) + sizeof(Source) + sizeof(Victim));
}

int DamagePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Client Damage Packet {Damage=%f Modifer=%f Type=%d Inflicted=%d Source=%d Victim=%d}", Damage, Modifer, Type, Inflicted, Source, Victim));
}


/*
**	Reconstruction notes (pkdamage.cpp)
**
**	* Client 0x87FF60-0x880290 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
