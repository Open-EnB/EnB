// Earth & Beyond client (reconstruction): app\packets\pkmoderequest.cpp. Notes at the end of the file.
#include "pkmoderequest.h"
#include <stdio.h>

static char *ModeRequestNames[] = { "Cockpit", "Resource" };	// 0xBB56E8  // NAME 2: what it holds

Packet_004BClass::Packet_004BClass(int unknown0c, int mode) :
	Unknown0C(unknown0c),
	Mode(mode)
{
}

Packet_004BClass::Packet_004BClass(RawPacketClass *raw)
{
	raw->Get(&Unknown0C, sizeof(Unknown0C));
	raw->Get(&Mode, sizeof(Mode));
}

RawPacketClass *Packet_004BClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(75, Get_Data_Size());
	raw->Add(&Unknown0C, sizeof(Unknown0C));
	raw->Add(&Mode, sizeof(Mode));
	return(raw);
}

int Packet_004BClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Mode));
}

int Packet_004BClass::Print(char *buffer, int)
{
	char *mode = ModeRequestNames[Mode];
	return(sprintf(buffer, "%s {Mode=%s}", "Mode Request", mode));
}


/*
**	Reconstruction notes (pkmoderequest.cpp)
**
**	* Client 0x89CDB0-0x89D040 (0x89CD80 is the previous object's guarded static, 0xC416FC; this one's is 0x89D040):
**	  opcode 75, written by hand (builder P4, 2026-10-27).
**	* Not rebuilt: 0x89CE00, between the value constructor and the scalar deleting destructor, returns 0x7F
**	  and nothing refers to it (no vtable slot, no call).
**	* The file name is provisional (no assert names it).
*/
