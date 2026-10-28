// Earth & Beyond client (reconstruction): app\packets\pkunknown_8a5510.cpp. Notes at the end of the file.
#include "pkunknown_8a5510.h"
#include <stdio.h>


Packet_00E8Class::Packet_00E8Class(int unknown0_c) : Unknown0C(unknown0_c), Unknown10(0)
{
}

Packet_00E8Class::~Packet_00E8Class()
{
}

Packet_00E8Class::Packet_00E8Class(RawPacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(Unknown0C);
}

RawPacketClass *Packet_00E8Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown0C);
	return(raw);
}

int Packet_00E8Class::Get_Type()
{
	return(232);
}

int Packet_00E8Class::Get_Data_Size()
{
	return(sizeof(Unknown0C));
}

int Packet_00E8Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "SpendReferralPoint"));
}


/*
**	Reconstruction notes (pkunknown_8a5510.cpp)
**
**	* Client 0x8A5510-0x8A5770 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
