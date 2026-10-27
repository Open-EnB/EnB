// Earth & Beyond client (reconstruction): app\packets\pkmessagetime.cpp. Notes at the end of the file.
#include "pkmessagetime.h"
#include <stdio.h>

MessageTimePacketClass::MessageTimePacketClass(unsigned char color, time_t time) :
	Time(time),
	Color(color)
{
}

MessageTimePacketClass::MessageTimePacketClass(RawPacketClass *raw)
{
	raw->Get(&Color, sizeof(Color));
	raw->Get(&Time, sizeof(Time));
}

RawPacketClass *MessageTimePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(96, Get_Data_Size());
	raw->Add(&Color, sizeof(Color));
	raw->Add(&Time, sizeof(Time));
	return(raw);
}

int MessageTimePacketClass::Get_Data_Size()
{
	return(sizeof(Color) + sizeof(Time));
}

int MessageTimePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Time=%s Color=%d}", "Message_Time", ctime(&Time), Color));
}


/*
**	Reconstruction notes (pkmessagetime.cpp)
**
**	* Client 0x89E5E0-0x89E890 (0x89E5B0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x89E890): opcode 96, written by hand (builder P4, 2026-10-27). The value constructor comes
**	  first, then the inline Get_Priority and Get_Channel the vtable emits.
**	* The file name is provisional (no assert names it).
*/
