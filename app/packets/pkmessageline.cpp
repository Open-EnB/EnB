// Earth & Beyond client (reconstruction): app\packets\pkmessageline.cpp. Notes at the end of the file.
#include "pkmessageline.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

ClearMessageLinePacketClass::ClearMessageLinePacketClass()
{
}

ClearMessageLinePacketClass::ClearMessageLinePacketClass(RawPacketClass *)
{
}

RawPacketClass *ClearMessageLinePacketClass::Build()
{
	return(new RawPacketClass(36, 0));
}

int ClearMessageLinePacketClass::Get_Data_Size()
{
	return(0);
}

int ClearMessageLinePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "ClearMessageLine {}"));
}

SuperLinePacketClass::SuperLinePacketClass(char *string) : String(nstrdup(string))
{
}

SuperLinePacketClass::SuperLinePacketClass(RawPacketClass *raw) : String(0)
{
	String = raw->Get_String();
}

SuperLinePacketClass::~SuperLinePacketClass()
{
	if (String != 0) {
		delete [] String;
		String = 0;
	}
}

RawPacketClass *SuperLinePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add_String(String);
	return(raw);
}

int SuperLinePacketClass::Get_Data_Size()
{
	return(strlen(String) + 1);
}

int SuperLinePacketClass::Print(char *buffer, int)
{
	char string[128];
	strncpy(string, String, sizeof(string));
	string[sizeof(string) - 1] = 0;
	return(sprintf(buffer, "%s {String=%s}", "SuperLine", string));
}


/*
**	Reconstruction notes (pkmessageline.cpp)
**
**	* Client 0x89D730-0x89E0F0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320): MessageLinePacketClass (the base of opcodes 32-34, still generated
**	  in pk_classes.h by tools/re/pkclasses.py: 0x89D760-0x89DAF0), then opcodes 36 and 35. Written by
**	  hand (builder P3, 2026-10-26), 11 of 11 exact in appfile.py match. SuperLine's value constructor
**	  nstrdup's in its initialiser list (the base vtable is stored before the call).
**	* The file name is provisional (no assert names it).
*/
