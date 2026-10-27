// Earth & Beyond client (reconstruction): app\packets\pkmessagestring.cpp. Notes at the end of the file.
#include "pkmessagestring.h"
#include <stdio.h>
#include <string.h>

MessageStringPacketClass::MessageStringPacketClass(unsigned char color, char *text) :
	Color(color),
	Owned(false)
{
	Size = (short)(strlen(text) + 1);
	String = text;
}

MessageStringPacketClass::MessageStringPacketClass(unsigned char color, const std::string &text) :
	Color(color),
	Owned(true)
{
	Size = (short)(strlen(text.c_str()) + 1);
	String = new char[Size];
	strcpy(String, text.c_str());
}

MessageStringPacketClass::MessageStringPacketClass(RawPacketClass *raw) :
	Owned(true)
{
	raw->Get(&Size, sizeof(Size));
	raw->Get(&Color, sizeof(Color));
	String = new char[Size];
	raw->Get(String, Size);
}

MessageStringPacketClass::~MessageStringPacketClass()
{
	if (String != 0 && Owned) {
		delete [] String;
	}
}

RawPacketClass *MessageStringPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(29, Get_Data_Size());
	raw->Add(&Size, sizeof(Size));
	raw->Add(&Color, sizeof(Color));
	raw->Add(String, Size);
	return(raw);
}

int MessageStringPacketClass::Get_Data_Size()
{
	return(sizeof(Size) + sizeof(Color) + Size);
}

int MessageStringPacketClass::Print(char *buffer, int)
{
	char text[128];
	short len = Size;
	if (len >= 128) {
		len = 128;
	}
	strncpy(text, String, len);
	return(sprintf(buffer, "%s {String=%s Size=%hd Color=%d}", "Message_String", text, Size, Color));
}

/*
**	Reconstruction notes (pkmessagestring.cpp)
**
**	* Client 0x89E110-0x89E5B0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P3, 2026-10-26), 7 of 7 exact in appfile.py
**	  match: a length-counted string (Size counts its 0) the packet owns unless the char * constructor
**	  made it; the same shape as opcode 30's.
**	* The file name is provisional (no assert names it).
*/
