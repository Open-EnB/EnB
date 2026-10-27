// Earth & Beyond client (reconstruction): app\packets\pkmodechange.cpp. Notes at the end of the file.
#include "pkmodechange.h"
#include <stdio.h>

static char *ModeChangeNames[] = { "Cockpit", "Resource" };	// 0xBB56B0  // NAME 2: what it holds

ModeChangePacketClass::ModeChangePacketClass(int unknown0c, int mode) :
	Unknown0C(unknown0c),
	Mode(mode)
{
}

ModeChangePacketClass::ModeChangePacketClass(RawPacketClass *raw)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Mode, sizeof(Mode));
}

RawPacketClass *ModeChangePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(76, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Mode, sizeof(Mode));
	return(raw);
}

int ModeChangePacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Mode));
}

int ModeChangePacketClass::Print(char *buffer, int)
{
	char *mode = ModeChangeNames[Mode];
	return(sprintf(buffer, "%s {Mode=%s}", "Mode Request", mode));
}


/*
**	Reconstruction notes (pkmodechange.cpp)
**
**	* Client 0x89CAF0-0x89CD80 (0x89CAC0 is the previous object's guarded static, 0xC416FC; this one's is 0x89CD80):
**	  opcode 76, written by hand (builder P4, 2026-10-27).
**	* Not rebuilt: 0x89CB40, between the value constructor and the scalar deleting destructor, returns 0x7F
**	  and nothing refers to it (no vtable slot, no call).
**	* The file name is provisional (no assert names it).
*/
