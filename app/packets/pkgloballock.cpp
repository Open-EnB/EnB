// Earth & Beyond client (reconstruction): app\packets\pkgloballock.cpp. Notes at the end of the file.
#include "pkgloballock.h"
#include <stdio.h>

GlobalLockBasePacketClass::GlobalLockBasePacketClass(__int64 a, int b) : Unknown10(a), Unknown18(b)
{
}

GlobalLockBasePacketClass::GlobalLockBasePacketClass(PacketClass *raw)
{
	raw->Get(Unknown10);
	raw->Get(Unknown18);
}

void GlobalLockBasePacketClass::Unknown_28(PacketClass *raw)
{
	raw->Add(Unknown10);
	raw->Add(Unknown18);
}

int GlobalLockBasePacketClass::Get_Data_Size()
{
	return(sizeof(Unknown10) + sizeof(Unknown18));
}

GlobalLockRequestPacketClass::GlobalLockRequestPacketClass(__int64 a, int b, int x) : GlobalLockBasePacketClass(a, b), Unknown20(x)
{
}

GlobalLockRequestPacketClass::GlobalLockRequestPacketClass(PacketClass *raw) : GlobalLockBasePacketClass(raw)
{
	raw->Get(Unknown20);
}

PacketClass *GlobalLockRequestPacketClass::Build()
{
	PacketClass *raw = new PacketClass(227, Get_Data_Size());
	GlobalLockBasePacketClass::Unknown_28(raw);
	raw->Add(Unknown20);
	return(raw);
}

int GlobalLockRequestPacketClass::Get_Data_Size()
{
	return(GlobalLockBasePacketClass::Get_Data_Size() + sizeof(Unknown20));
}

int GlobalLockRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalLockRequest"));
}

GlobalLockResponsePacketClass::GlobalLockResponsePacketClass(int x, __int64 a, int b) : GlobalLockBasePacketClass(a, b), Unknown20(x)
{
}

GlobalLockResponsePacketClass::GlobalLockResponsePacketClass(PacketClass *raw) : GlobalLockBasePacketClass(raw)
{
	raw->Get(Unknown20);
}

PacketClass *GlobalLockResponsePacketClass::Build()
{
	PacketClass *raw = new PacketClass(228, Get_Data_Size());
	GlobalLockBasePacketClass::Unknown_28(raw);
	raw->Add(Unknown20);
	return(raw);
}

int GlobalLockResponsePacketClass::Get_Data_Size()
{
	return(GlobalLockBasePacketClass::Get_Data_Size() + sizeof(Unknown20));
}

int GlobalLockResponsePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalLockResponse"));
}

GlobalLockReleasePacketClass::GlobalLockReleasePacketClass(__int64 a) : GlobalLockBasePacketClass(a, 0)
{
}

GlobalLockReleasePacketClass::GlobalLockReleasePacketClass(PacketClass *raw) : GlobalLockBasePacketClass(raw)
{
}

PacketClass *GlobalLockReleasePacketClass::Build()
{
	PacketClass *raw = new PacketClass(229, Get_Data_Size());
	GlobalLockBasePacketClass::Unknown_28(raw);
	return(raw);
}

int GlobalLockReleasePacketClass::Get_Data_Size()
{
	return(GlobalLockBasePacketClass::Get_Data_Size());
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
**	  UnknownPacketBaseClass's vtable stored).
**	* The file name and the class names are provisional: the Print texts give only 'GlobalLockRequest',
**	  'GlobalLockResponse', 'GlobalLockRelease'.
*/
