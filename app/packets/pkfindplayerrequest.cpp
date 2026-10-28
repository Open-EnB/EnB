// Earth & Beyond client (reconstruction): app\packets\pkfindplayerrequest.cpp. Notes at the end of the file.
#include "pkfindplayerrequest.h"
#include <stdio.h>

FindPlayerRequestPacketClass::FindPlayerRequestPacketClass(int unknown0c, const char *name, bool unknown51) :
	Unknown0C(unknown0c),
	Unknown51(unknown51)
{
	Unknown_4A1590(Name, sizeof(Name) - 1, name);
}

FindPlayerRequestPacketClass::FindPlayerRequestPacketClass(RawPacketClass *raw)
{
	int unknown51;
	raw->Get(Unknown0C);
	raw->Get(Name, sizeof(Name));
	Name[sizeof(Name) - 1] = 0;
	raw->Get(unknown51);
	Unknown51 = unknown51 != 0;
}

RawPacketClass *FindPlayerRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(181, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Name, sizeof(Name));
	int unknown51 = Unknown51;
	raw->Add(unknown51);
	return(raw);
}

int FindPlayerRequestPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Name) + sizeof(int));
}

int FindPlayerRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s", "FindPlayerRequest"));
}

FindPlayerResponsePacketClass::FindPlayerResponsePacketClass(int unknown0c, const char *name, bool unknown51, int unknown54) :
	Unknown0C(unknown0c),
	Unknown51(unknown51),
	Unknown54(unknown54)
{
	Unknown_4A1590(Name, sizeof(Name) - 1, name);
}

FindPlayerResponsePacketClass::FindPlayerResponsePacketClass(RawPacketClass *raw)
{
	int unknown51;
	raw->Get(Unknown0C);
	raw->Get(Name, sizeof(Name));
	Name[sizeof(Name) - 1] = 0;
	raw->Get(unknown51);
	Unknown51 = unknown51 != 0;
	raw->Get(Unknown54);
}

RawPacketClass *FindPlayerResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(182, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Name, sizeof(Name));
	int unknown51 = Unknown51;
	raw->Add(unknown51);
	raw->Add(Unknown54);
	return(raw);
}

int FindPlayerResponsePacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Name) + sizeof(int) + sizeof(Unknown54));
}

int FindPlayerResponsePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s", "FindPlayerResponse"));
}


/*
**	Reconstruction notes (pkfindplayerrequest.cpp)
**
**	* The client's object 0x886C70-0x887280 (after pkfindmember.cpp's guarded static 0x886C40; 0x887230's
**	  Print ends with this object's guarded static, 0xC416FC): opcodes 181 and 182 written by hand (builder
**	  P10, 2026-10-27). No assert names the file: pk + the first class's Print label, provisional (D10).
**	* Both classes carry a 0x41-byte name (Get reads 0x41 bytes and clears the last; the value constructors
**	  copy at most 0x40 with the bounded strcpy 0x4A1590) and a bool sent and received as an int.
**	* The implicit destructors: the object holds PacketClass's destructor copies 0x886E10 / 0x887100.
*/
