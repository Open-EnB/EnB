// Earth & Beyond client (reconstruction): app\packets\pkmodechange.cpp. Notes at the end of the file.
#include "pkmodechange.h"
#include <stdio.h>

static char *ModeChangeNames[] = { "Cockpit", "Resource" };	// 0xBB56B0  // NAME 2: what it holds

ModeChangePacketClass::ModeChangePacketClass(int unknown0c, int mode) :
	Unknown0C(unknown0c),
	Mode(mode)
{
}

ModeChangePacketClass::ModeChangePacketClass(PacketClass *raw)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Mode, sizeof(Mode));
}

PacketClass *ModeChangePacketClass::Encode()
{
	PacketClass *raw = new PacketClass(76, Get_Data_Size());
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
**	* 0x89CB40 is the header's inline Get_Priority (returns 0x7F): the vtable 0xB0B2E4's slot +0x1C, through the
**	  thunk 0x401AEB (symbols.json named it UnknownPacketBaseClass::Get_Priority until QE's symbols_named.json row).
**	* The file name is provisional (no assert names it).
*/
