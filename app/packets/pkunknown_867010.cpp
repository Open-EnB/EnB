// Earth & Beyond client (reconstruction): app\packets\pkunknown_867010.cpp. Notes at the end of the file.
#include "pkunknown_867010.h"
#include <stdio.h>


// NOT EXACT: the inline std::string assign's terminating store indexes [ebx + ecx], the client's [ecx + ebx] (otherwise the same bytes)
Packet_00B3Class::Packet_00B3Class(RawPacketClass *raw) : Unknown1C(0)
{
	char *string = raw->Get_String();
	Name = string;
	delete [] string;
}

RawPacketClass *Packet_00B3Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(179, Get_Data_Size());
	raw->Add_String(Name.c_str());
	return(raw);
}

int Packet_00B3Class::Get_Data_Size()
{
	return(Name.length() + 1);
}

int Packet_00B3Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Name=%s}", "ShipName_Validate", Name.c_str()));
}


/*
**	Reconstruction notes (pkunknown_867010.cpp)
**
**	* Client 0x867010-0x8674F0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
