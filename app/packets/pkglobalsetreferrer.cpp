// Earth & Beyond client (reconstruction): app\packets\pkglobalsetreferrer.cpp. Notes at the end of the file.
#include "pkglobalsetreferrer.h"
#include <stdio.h>

GlobalSetReferrerPacketClass::GlobalSetReferrerPacketClass(const char *name) :
	Name(name),
	Unknown1C(0)
{
}

GlobalSetReferrerPacketClass::GlobalSetReferrerPacketClass(PacketClass *raw) :
	Unknown1C(0)
{
	raw->Get(&Name);
}

PacketClass *GlobalSetReferrerPacketClass::Build()
{
	PacketClass *raw = new PacketClass(119, Get_Data_Size());
	raw->Add(Name);
	return(raw);
}

int GlobalSetReferrerPacketClass::Get_Data_Size()
{
	return(PacketClass::Get_Size(Name));
}

// The client hands sprintf the string object itself (its four words), not its text
int GlobalSetReferrerPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s : ReferrerName=%s", "Global SetReferrer", Name));
}


GlobalSetReferrerResponsePacketClass::GlobalSetReferrerResponsePacketClass(PacketClass *raw)
{
	raw->Get(Result);
}

PacketClass *GlobalSetReferrerResponsePacketClass::Build()
{
	PacketClass *raw = new PacketClass(120, Get_Data_Size());
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
**	Reconstruction notes (pkglobalsetreferrer.cpp)
**
**	* Client 0x866AC0-0x866FE0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class GlobalSetReferrer / Response (thunk order) and its place in retail's object order (before pkglobalticket), NAME 3; no assert names it.
**	* Opcode 119 written by hand (builder P9, 2026-10-27): the referrer's name (LoginTaskClass::Send_Referrer);
**	  Print passes the std::string by value to sprintf, as the client does (four words pushed).
*/
