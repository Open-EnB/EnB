// Earth & Beyond client (reconstruction): app\packets\pkmessageline.cpp. Notes at the end of the file.
#include "pkmessageline.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

static int Clamp_Int(__int64 t);	// at the end of the file (the client's duration.h, line 205)  // NAME 2: describes what it does

MessageLinePacketClass::MessageLinePacketClass(char *string, char *ephemeral_type, __int64 time_ms, int display_priority) :
	String(nstrdup(string)),
	Ephemeral_Type(nstrdup(ephemeral_type)),
	TimeMS(Clamp_Int(time_ms)),
	Display_Priority(display_priority)
{
}

MessageLinePacketClass::MessageLinePacketClass(RawPacketClass *raw) :
	String(0),
	Ephemeral_Type(0),
	TimeMS(0),
	Display_Priority(0)
{
	String = raw->Get_String();
	Ephemeral_Type = raw->Get_String();
	raw->Get(&TimeMS, sizeof(TimeMS));
	raw->Get(&Display_Priority, sizeof(Display_Priority));
}

MessageLinePacketClass::~MessageLinePacketClass()
{
	if (String != 0) {
		delete [] String;
		String = 0;
	}
	if (Ephemeral_Type != 0) {
		delete [] Ephemeral_Type;
		Ephemeral_Type = 0;
	}
}

RawPacketClass *MessageLinePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add_String(String);
	raw->Add_String(Ephemeral_Type);
	raw->Add(&TimeMS, sizeof(TimeMS));
	raw->Add(&Display_Priority, sizeof(Display_Priority));
	return(raw);
}

int MessageLinePacketClass::Get_Data_Size()
{
	return(strlen(String) + 1 + strlen(Ephemeral_Type) + 1 + sizeof(TimeMS) + sizeof(Display_Priority));
}

int MessageLinePacketClass::Print(char *buffer, int)
{
	char type[128];
	char string[128];
	strncpy(string, String, sizeof(string));
	string[sizeof(string) - 1] = 0;
	strncpy(type, Ephemeral_Type, sizeof(type));
	type[sizeof(type) - 1] = 0;
	return(sprintf(buffer, "%s {String=%s Ephemeral_Type=%s TimeMS=%d Display_Priority=%d}", Get_Name(), string, type, TimeMS, Display_Priority));
}

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
**	  0xC416FC, atexit 0x42B320): MessageLinePacketClass (the base of opcodes 32-34; builder P4,
**	  2026-10-26, 6 of 6 exact: pkclasses.py no longer declares a base a rebuilt header declares),
**	  then opcodes 36 and 35 (builder P3, 2026-10-26, 11 of 11 exact in appfile.py match). SuperLine's
**	  value constructor nstrdup's in its initialiser list (the base vtable is stored before the call).
**	* The value constructor inlines a clamp of a 64-bit time whose FAIL_IF ('t > INT_MAX') the client
**	  reports from ..\shared\duration.h, line 205. It is defined here, at the end of the file on that
**	  line, not in duration.h: adding it there reorders the symbols of every object that includes
**	  duration.h and flips objmatch's tie-breaks in aux_roots.cpp (3 pairs gained, 2 lost), so the
**	  move waits for the user (P4's proposal). The string differs, the bytes match (a relocation).
**	* The file name is provisional (no assert names it).
*/



























































// duration.h's clamp of a 64-bit time to an int (the client's line 205: see the notes)
static int Clamp_Int(__int64 t)
{
	FAIL_IF(t > INT_MAX) return INT_MAX;
	if (t < INT_MIN) {
		return INT_MIN;
	}
	return (int)t;
}
