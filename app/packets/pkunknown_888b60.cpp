// Earth & Beyond client (reconstruction): app\packets\pkunknown_888b60.cpp. Notes at the end of the file.
#include "pkunknown_888b60.h"
#include <stdio.h>


// NOT EXACT: shape (the string assignment's tails: 9 calls against the client's 10; attempts.csv)
Packet_00ADClass::Packet_00ADClass(int unknown0c, const std::string &unknown10, __int64 unknown20, __int64 unknown28) :
	Unknown0C(unknown0c),
	Unknown10(""),
	Unknown20(0),
	Unknown28(0)
{
	Unknown10 = unknown10;
	Unknown20 = unknown20;
	Unknown28 = unknown28;
}

// NOT EXACT: inline budget (the client calls the string's _Grow out of line for "", ours inlines it; attempts.csv)
Packet_00ADClass::Packet_00ADClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(""),
	Unknown20(0),
	Unknown28(0)
{
	raw->Get(Unknown0C);
	raw->Get(&Unknown10);
	raw->Get(Unknown20);
	raw->Get(Unknown28);
}

// NOT EXACT: shape (the client advances this through the members: add edi, 0x10 / 0x18; attempts.csv)
RawPacketClass *Packet_00ADClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(173, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Unknown10);
	raw->Add(Unknown20);
	raw->Add(Unknown28);
	return(raw);
}

int Packet_00ADClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Unknown10) + sizeof(Unknown0C) + sizeof(Unknown20) + sizeof(Unknown28));
}

int Packet_00ADClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerLoginPacketClass"));
}

Packet_00AEClass::Packet_00AEClass(int unknown0c, int unknown10, __int64 unknown18, __int64 unknown20) :
	Unknown0C(unknown0c),
	Unknown10(unknown10),
	Unknown18(unknown18),
	Unknown20(unknown20)
{
}

Packet_00AEClass::Packet_00AEClass(RawPacketClass *raw) :
	Unknown0C(0),
	Unknown10(0),
	Unknown18(0),
	Unknown20(0)
{
	raw->Get(Unknown0C);
	raw->Get(&Unknown10, sizeof(Unknown10));
	raw->Get(Unknown18);
	raw->Get(Unknown20);
}

RawPacketClass *Packet_00AEClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(174, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(&Unknown10, sizeof(Unknown10));
	raw->Add(Unknown18);
	raw->Add(Unknown20);
	return(raw);
}

int Packet_00AEClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10) + sizeof(Unknown18) + sizeof(Unknown20));
}

int Packet_00AEClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerRequestPacketClass"));
}

Packet_00AFClass::Packet_00AFClass(int unknown0_c, int unknown10) : Unknown0C(unknown0_c), Unknown10(unknown10)
{
}

Packet_00AFClass::Packet_00AFClass(RawPacketClass *raw) : Unknown0C(0), Unknown10(0)
{
	raw->Get(Unknown0C);
	raw->Get(&Unknown10, sizeof(Unknown10));
}

RawPacketClass *Packet_00AFClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(175, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(&Unknown10, sizeof(Unknown10));
	return(raw);
}

int Packet_00AFClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10));
}

int Packet_00AFClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GroupServerResponsePacketClass"));
}


/*
**	Reconstruction notes (pkunknown_888b60.cpp)
**
**	* Client 0x888B60-0x88BD10 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
**	* Opcode 174 (its Print: GroupServerRequestPacketClass) written by hand (builder P3, 2026-10-26),
**	  5 of 5 exact in appfile.py match: an int and two __int64 in network order around a raw int.
**	* Opcode 173 (Print 'GroupServerLoginPacketClass') written by hand (builder P5, 2026-10-27) over an empty
**	  base (vtable 0xB0A794) that 176 / 177's objects share: Get_Data_Size and Print exact, the constructors
**	  and Build NOT EXACT (attempts.csv).
**	* Not rebuilt yet: opcode 176, 177 (declared in pk_classes.h).
*/
