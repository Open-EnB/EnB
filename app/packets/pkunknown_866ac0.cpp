// Earth & Beyond client (reconstruction): app\packets\pkunknown_866ac0.cpp. Notes at the end of the file.
#include "pkunknown_866ac0.h"
#include <stdio.h>

GlobalSetReferrerPacketClass::GlobalSetReferrerPacketClass(const char *name) :
	Name(name),
	Unknown1C(0)
{
}

GlobalSetReferrerPacketClass::GlobalSetReferrerPacketClass(RawPacketClass *raw) :
	Unknown1C(0)
{
	raw->Get(&Name);
}

RawPacketClass *GlobalSetReferrerPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(119, Get_Data_Size());
	raw->Add(Name);
	return(raw);
}

int GlobalSetReferrerPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Name));
}

// The client hands sprintf the string object itself (its four words), not its text
int GlobalSetReferrerPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s : ReferrerName=%s", "Global SetReferrer", Name));
}


GlobalSetReferrerResponsePacketClass::GlobalSetReferrerResponsePacketClass(RawPacketClass *raw)
{
	raw->Get(Result);
}

RawPacketClass *GlobalSetReferrerResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(120, Get_Data_Size());
	raw->Add(Result);
	return(raw);
}

int GlobalSetReferrerResponsePacketClass::Get_Data_Size()
{
	return(sizeof(Result));
}

int GlobalSetReferrerResponsePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s : Result=%d", "Global SetReferrerResponse", Result));
}


/*
**	Reconstruction notes (pkunknown_866ac0.cpp)
**
**	* Client 0x866AC0-0x866FE0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Opcode 119 written by hand (builder P9, 2026-10-27): the referrer's name (LoginTaskClass::Send_Referrer);
**	  Print passes the std::string by value to sprintf, as the client does (four words pushed).
*/
