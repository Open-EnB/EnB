// Earth & Beyond client (reconstruction): app\packets\pkgloballock.cpp. Notes at the end of the file.
#include "pkgloballock.h"
#include <stdio.h>

GlobalLockPacketClass::GlobalLockPacketClass(__int64 a, int b) : Unknown10(a), Unknown18(b)
{
}

GlobalLockPacketClass::GlobalLockPacketClass(RawPacketClass *raw)
{
	raw->Get(Unknown10);
	raw->Get(Unknown18);
}

void GlobalLockPacketClass::Unknown_28(RawPacketClass *raw)
{
	raw->Add(Unknown10);
	raw->Add(Unknown18);
}

int GlobalLockPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown10) + sizeof(Unknown18));
}

GlobalLockRequestPacketClass::GlobalLockRequestPacketClass(__int64 a, int b, int x) : GlobalLockPacketClass(a, b), Unknown20(x)
{
}

GlobalLockRequestPacketClass::GlobalLockRequestPacketClass(RawPacketClass *raw) : GlobalLockPacketClass(raw)
{
	raw->Get(Unknown20);
}

RawPacketClass *GlobalLockRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(227, Get_Data_Size());
	GlobalLockPacketClass::Unknown_28(raw);
	raw->Add(Unknown20);
	return(raw);
}

int GlobalLockRequestPacketClass::Get_Data_Size()
{
	return(GlobalLockPacketClass::Get_Data_Size() + sizeof(Unknown20));
}

int GlobalLockRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalLockRequest"));
}

GlobalLockResponsePacketClass::GlobalLockResponsePacketClass(int x, __int64 a, int b) : GlobalLockPacketClass(a, b), Unknown20(x)
{
}

GlobalLockResponsePacketClass::GlobalLockResponsePacketClass(RawPacketClass *raw) : GlobalLockPacketClass(raw)
{
	raw->Get(Unknown20);
}

RawPacketClass *GlobalLockResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(228, Get_Data_Size());
	GlobalLockPacketClass::Unknown_28(raw);
	raw->Add(Unknown20);
	return(raw);
}

int GlobalLockResponsePacketClass::Get_Data_Size()
{
	return(GlobalLockPacketClass::Get_Data_Size() + sizeof(Unknown20));
}

int GlobalLockResponsePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalLockResponse"));
}

GlobalLockReleasePacketClass::GlobalLockReleasePacketClass(__int64 a) : GlobalLockPacketClass(a, 0)
{
}

GlobalLockReleasePacketClass::GlobalLockReleasePacketClass(RawPacketClass *raw) : GlobalLockPacketClass(raw)
{
}

RawPacketClass *GlobalLockReleasePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(229, Get_Data_Size());
	GlobalLockPacketClass::Unknown_28(raw);
	return(raw);
}

int GlobalLockReleasePacketClass::Get_Data_Size()
{
	return(GlobalLockPacketClass::Get_Data_Size());
}

int GlobalLockReleasePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalLockRelease"));
}


/*
**	Reconstruction notes (pkgloballock.cpp)
**
**	* Client 0x8653F0-0x865D30 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand from the client (builder P, 2026-10-26): three packet
**	  classes over one base the factory never makes (vtable 0xB094F4: Build, Process_Packet and Print
**	  pure; +0x28 adds the base's members), as pkdirlock.cpp's. Every destructor is implicit (only
**	  PacketClass's vtable stored).
**	* The file name and the class names are provisional: the Print texts give only 'GlobalLockRequest',
**	  'GlobalLockResponse', 'GlobalLockRelease'.
*/
