// Earth & Beyond client (reconstruction): app\packets\pkgadget.cpp. Notes at the end of the file.
#include "pkgadget.h"
#include <stdio.h>
#include <string.h>

GadgetPacketClass::GadgetPacketClass(int function, char *name) :
	Function(function),
	Owned(false)
{
	Size = strlen(name) + 1;
	Name = name;
}

GadgetPacketClass::GadgetPacketClass(RawPacketClass *raw) :
	Owned(true)
{
	raw->Get(&Size, sizeof(Size));
	raw->Get(&Function, sizeof(Function));
	Name = new char[Size];
	raw->Get(Name, Size);
}

GadgetPacketClass::~GadgetPacketClass()
{
	if (Name != NULL && Owned) {
		delete [] Name;
	}
}

RawPacketClass *GadgetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(&Function, sizeof(Function));
	raw->Add(Name, Size);
	return(raw);
}

int GadgetPacketClass::Get_Data_Size()
{
	return(Size + sizeof(Size) + sizeof(Function));
}

int GadgetPacketClass::Print(char *buffer, int)
{
	char name[128];
	short count = Size;
	if (count >= 128) {
		count = 128;
	}
	strncpy(name, Name, count);
	return(sprintf(buffer, "%s {Name=%s Size=%hd Function=%d}", Get_Name(), name, Size, Function));
}

Packet_0068Class::Packet_0068Class(int unknown18, int function, char *name) :
	GadgetPacketClass(function, name),
	Unknown18(unknown18)
{
}

Packet_0068Class::Packet_0068Class(RawPacketClass *raw) :
	GadgetPacketClass(raw),
	Unknown18(0)
{
	raw->Get(&Unknown18, sizeof(Unknown18));
}

int Packet_0068Class::Get_Data_Size()
{
	return(GadgetPacketClass::Get_Data_Size() + sizeof(Unknown18));
}

RawPacketClass *Packet_0068Class::Build()
{
	RawPacketClass *raw = GadgetPacketClass::Build();
	raw->Add(&Unknown18, sizeof(Unknown18));
	return(raw);
}


/*
**	Reconstruction notes (pkgadget.cpp)
**
**	* Client 0x887900-0x887EF0 (0x8878D0 is the previous object's guarded static, 0xC416FC; this one's is 0x887EF0):
**	  opcode 103, written by hand (builder P4, 2026-10-27). The value constructor comes first.
**	* The file name is provisional (no assert names it).
*/
