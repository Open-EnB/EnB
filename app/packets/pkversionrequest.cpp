// Earth & Beyond client (reconstruction): app\packets\pkversionrequest.cpp. Notes at the end of the file.
#include "pkversionrequest.h"
#include <stdio.h>


VersionRequestPacketClass::VersionRequestPacketClass(int major, int minor) : Major(major), Minor(minor)
{
}

VersionRequestPacketClass::VersionRequestPacketClass(RawPacketClass *raw)
{
	raw->Get(Major);
	raw->Get(Minor);
}

RawPacketClass *VersionRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(0, Get_Data_Size());
	raw->Add(Major);
	raw->Add(Minor);
	return(raw);
}

int VersionRequestPacketClass::Get_Data_Size()
{
	return(sizeof(Major) + sizeof(Minor));
}

int VersionRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "VersionRequest { Major=%d Minor=%d }", Major, Minor));
}

VersionResponsePacketClass::VersionResponsePacketClass(int status) : Status(status)
{
}

VersionResponsePacketClass::VersionResponsePacketClass(RawPacketClass *raw)
{
	raw->Get(&Status, sizeof(Status));
}

RawPacketClass *VersionResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(1, Get_Data_Size());
	raw->Add(&Status, sizeof(Status));
	return(raw);
}

int VersionResponsePacketClass::Get_Data_Size()
{
	return(sizeof(Status));
}

int VersionResponsePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "VersionResponse { Status=%d }", Status));
}


/*
**	Reconstruction notes (pkversionrequest.cpp)
**
**	* Client 0x8B15F0-0x8B1A70 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
