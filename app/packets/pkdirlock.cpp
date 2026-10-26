// Earth & Beyond client (reconstruction): app\packets\pkdirlock.cpp. Notes at the end of the file.
#include "pkdirlock.h"
#include "..\..\wwlib\nstrdup.h"
#include <stdio.h>

DirLockPacketClass::DirLockPacketClass(int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d) : Unknown0C(a), Unknown10(0), Unknown18(b), Unknown20(c), Unknown28(d)
{
	if (name != 0) {
		Unknown10 = nstrdup(name);
	}
}

DirLockPacketClass::DirLockPacketClass(RawPacketClass *raw)
{
	raw->Get(Unknown0C);
	Unknown10 = raw->Get_String();
	raw->Get(Unknown18);
	raw->Get(&Unknown20, sizeof(Unknown20));
	raw->Get(Unknown28);
}

DirLockPacketClass::~DirLockPacketClass()
{
	delete [] Unknown10;
	Unknown10 = 0;
}

void DirLockPacketClass::Unknown_28(RawPacketClass *raw)
{
	raw->Add(Unknown0C);
	raw->Add_String(Unknown10);
	raw->Add(Unknown18);
	raw->Add(&Unknown20, sizeof(Unknown20));
	raw->Add(Unknown28);
}

int DirLockPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Unknown10) + sizeof(Unknown0C) + sizeof(Unknown18) + sizeof(Unknown20) + sizeof(Unknown28));
}

DirLockRequestPacketClass::DirLockRequestPacketClass(int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d) : DirLockPacketClass(a, name, b, c, d)
{
}

DirLockRequestPacketClass::DirLockRequestPacketClass(RawPacketClass *raw) : DirLockPacketClass(raw)
{
}

RawPacketClass *DirLockRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(223, Get_Data_Size());
	DirLockPacketClass::Unknown_28(raw);
	return(raw);
}

int DirLockRequestPacketClass::Get_Data_Size()
{
	return(DirLockPacketClass::Get_Data_Size());
}

int DirLockRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "DirLockRequest"));
}

DirLockResponseSectorPacketClass::DirLockResponseSectorPacketClass(int x, int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d) : DirLockPacketClass(a, name, b, c, d)
{
	Unknown30 = x;
}

DirLockResponseSectorPacketClass::DirLockResponseSectorPacketClass(RawPacketClass *raw) : DirLockPacketClass(raw)
{
	raw->Get(Unknown30);
}

RawPacketClass *DirLockResponseSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(225, Get_Data_Size());
	DirLockPacketClass::Unknown_28(raw);
	raw->Add(Unknown30);
	return(raw);
}

int DirLockResponseSectorPacketClass::Get_Data_Size()
{
	return(DirLockPacketClass::Get_Data_Size() + sizeof(Unknown30));
}

int DirLockResponseSectorPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "DirLockResponseSector"));
}

DirLockResponseGlobalPacketClass::DirLockResponseGlobalPacketClass(int x, int a, const char *name, __int64 b, Unknown_DirLockStruct c, int y, int d) : DirLockPacketClass(a, name, b, c, d), Unknown30(x), Unknown34(y)
{
}

DirLockResponseGlobalPacketClass::DirLockResponseGlobalPacketClass(RawPacketClass *raw) : DirLockPacketClass(raw)
{
	raw->Get(Unknown30);
	raw->Get(Unknown34);
}

RawPacketClass *DirLockResponseGlobalPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(224, Get_Data_Size());
	DirLockPacketClass::Unknown_28(raw);
	raw->Add(Unknown30);
	raw->Add(Unknown34);
	return(raw);
}

int DirLockResponseGlobalPacketClass::Get_Data_Size()
{
	return(DirLockPacketClass::Get_Data_Size() + sizeof(Unknown30) + sizeof(Unknown34));
}

int DirLockResponseGlobalPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "DirLockResponseGlobal"));
}

DirLockReleasePacketClass::DirLockReleasePacketClass(int a, const char *name, __int64 b, Unknown_DirLockStruct c, int d) : DirLockPacketClass(a, name, b, c, d)
{
}

DirLockReleasePacketClass::DirLockReleasePacketClass(RawPacketClass *raw) : DirLockPacketClass(raw)
{
}

RawPacketClass *DirLockReleasePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(226, Get_Data_Size());
	DirLockPacketClass::Unknown_28(raw);
	return(raw);
}

int DirLockReleasePacketClass::Get_Data_Size()
{
	return(DirLockPacketClass::Get_Data_Size());
}

int DirLockReleasePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "DirLockRelease"));
}


/*
**	Reconstruction notes (pkdirlock.cpp)
**
**	* Client 0x8816D0-0x882680 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand from the client (builder P, 2026-10-26): four packet
**	  classes over one base the factory never makes (vtable 0xB0A290: Build, Process_Packet and Print
**	  pure; +0x28 adds the base's members). The derived constructors inline the base's; their Build
**	  calls the +0x28 function qualified (inlined); their destructors are implicit (the base's inlined:
**	  no own vtable store).
**	* Opcode 225 assigns its member in the body (stored after the vtable), opcode 224 in the
**	  initialiser list (before it).
**	* The 8 bytes at +0x20 are read and written raw and passed by value as two dwords: a struct of two
**	  ints here (Unknown_DirLockStruct); a double would be copied through the FPU.
**	* The file name and the class names are provisional: the Print texts give only 'DirLockRequest',
**	  'DirLockResponseSector', 'DirLockResponseGlobal', 'DirLockRelease'.
*/
