// Earth & Beyond client (reconstruction): app\packets\pkmoderequest.cpp. Notes at the end of the file.
#include "pkmoderequest.h"
#include <stdio.h>

static char *ModeRequestNames[] = { "Cockpit", "Resource" };	// 0xBB56E8  // NAME 2: what it holds

ModeRequestPacketClass::ModeRequestPacketClass(int unknown0c, int mode) :
	Unknown0C(unknown0c),
	Mode(mode)
{
}

ModeRequestPacketClass::ModeRequestPacketClass(PacketClass *raw)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Mode, sizeof(Mode));
}

PacketClass *ModeRequestPacketClass::Build()
{
	PacketClass *raw = new PacketClass(75, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Mode, sizeof(Mode));
	return(raw);
}

int ModeRequestPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Mode));
}

int ModeRequestPacketClass::Print(char *buffer, int)
{
	char *mode = ModeRequestNames[Mode];
	return(sprintf(buffer, "%s {Mode=%s}", "Mode Request", mode));
}


/*
**	Reconstruction notes (pkmoderequest.cpp)
**
**	* Client 0x89CDB0-0x89D040 (0x89CD80 is the previous object's guarded static, 0xC416FC; this one's is 0x89D040):
**	  opcode 75, written by hand (builder P4, 2026-10-27).
**	* 0x89CE00 is the header's inline Get_Priority (returns 0x7F): the vtable 0xB0B314's slot +0x1C, through the
**	  thunk 0x40B988 (symbols.json named it UnknownPacketBaseClass::Get_Priority until QE's symbols_named.json row).
**	* The file name is provisional (no assert names it).
*/
