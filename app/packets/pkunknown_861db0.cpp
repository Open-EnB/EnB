// Earth & Beyond client (reconstruction): app\packets\pkunknown_861db0.cpp. Notes at the end of the file.
#include "pkunknown_861db0.h"
#include <string.h>

GameEditorSaveSectorPacketClass::GameEditorSaveSectorPacketClass()
{
}

GameEditorSaveSectorPacketClass::GameEditorSaveSectorPacketClass(RawPacketClass *)
{
}

RawPacketClass *GameEditorSaveSectorPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30027, Get_Data_Size());
	return(raw);
}

int GameEditorSaveSectorPacketClass::Get_Data_Size()
{
	return(0);
}

int GameEditorSaveSectorPacketClass::Print(char *buffer, int)
{
	strcpy(buffer, "GameEditorSaveSector {NO DATA}");
	return(strlen(buffer));
}


/*
**	Reconstruction notes (pkunknown_861db0.cpp)
**
**	* Client 0x861DB0-0x862320 (0x861D80 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x862320): opcode 30027 written by hand (builder P5, 2026-10-27), as 30023's (pkunknown_860d30.cpp).
**	  The object's other functions (0x861F80-0x8622C0) are not rebuilt: no class of the packet switch
**	  declares them (a game-editor packet the client only sends?).
**	* The file name is provisional (no assert names it).
*/
