// Earth & Beyond client (reconstruction): app\packets\pkgroup.cpp. Notes at the end of the file.
#include "pkgroup.h"
#include <stdio.h>
#include <string.h>

GroupPacketClass::GroupPacketClass(unsigned char type, char *text) :
	Type(type),
	Owned(false)
{
	Size = (short)(strlen(text) + 1);
	String = text;
}

GroupPacketClass::GroupPacketClass(unsigned char type, const std::string &text) :
	Type(type),
	Owned(true)
{
	Size = (short)(strlen(text.c_str()) + 1);
	String = new char[Size];
	strcpy(String, text.c_str());
}

GroupPacketClass::GroupPacketClass(RawPacketClass *raw) :
	Owned(true)
{
	raw->Get(&Size, sizeof(Size));
	raw->Get(&Type, sizeof(Type));
	String = new char[Size];
	raw->Get(String, Size);
}

GroupPacketClass::~GroupPacketClass()
{
	if (String != 0 && Owned) {
		delete [] String;
	}
}

RawPacketClass *GroupPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(&Type, sizeof(Type));
	raw->Add(String, Size);
	return(raw);
}

int GroupPacketClass::Get_Data_Size()
{
	return(sizeof(Size) + sizeof(Type) + Size);
}

int GroupPacketClass::Print(char *buffer, int)
{
	char text[128];
	short len = Size;
	if (len >= 128) {
		len = 128;
	}
	strncpy(text, String, len);
	return(sprintf(buffer, "%s {String=%s Size=%hd Type=%d}", "Group_Action", text, Size, Type));
}

/*
**	Reconstruction notes (pkgroup.cpp)
**
**	* Client 0x888690-0x888B30 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P3, 2026-10-26), 7 of 7 exact in appfile.py
**	  match: a length-counted string (Size counts its 0) the packet owns unless the char * constructor
**	  made it; the same shape as opcode 29's.
**	* The file name is provisional (no assert names it).
*/
