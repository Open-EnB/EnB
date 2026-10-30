// Earth & Beyond client (reconstruction): app\packets\pkdialog.cpp. Notes at the end of the file.
#include "pkdialog.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

DialogPacketClass::DialogPacketClass(int type, char *text) :
	Type(type)
{
	Size = (short)(strlen(text) + 1);
	String = nstrdup(text);
}

DialogPacketClass::DialogPacketClass(RawPacketClass *raw)
{
	raw->Get(&Size, sizeof(Size));
	raw->Get(&Type, sizeof(Type));
	String = new char[Size];
	raw->Get(String, Size);
}

DialogPacketClass::~DialogPacketClass()
{
	delete [] String;
	String = 0;
}

RawPacketClass *DialogPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(98, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(&Type, sizeof(Type));
	raw->Add(String, Size);
	return(raw);
}

int DialogPacketClass::Get_Data_Size()
{
	return(sizeof(Size) + sizeof(Type) + Size);
}

int DialogPacketClass::Print(char *buffer, int)
{
	char text[128];
	short len = Size;
	if (len >= 128) {
		len = 128;
	}
	strncpy(text, String, len);
	return(sprintf(buffer, "%s {String=%s Size=%hd Type=%d}", "Dialog_Packet", text, Size, Type));
}

/*
**	Reconstruction notes (pkdialog.cpp)
**
**	* Client 0x8812D0-0x8816A0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P3, 2026-10-26), 6 of 6 exact in appfile.py
**	  match: a length-counted string (Size counts its 0), nstrdup'd by the value constructor, new[]'d by
**	  the raw one, deleted by the destructor either way; the same shape as opcode 99's.
**	* The file name is provisional (no assert names it).
*/
