// Earth & Beyond client (reconstruction): app\packets\pkunknown_85f250.cpp. Notes at the end of the file.
#include "pkunknown_85f250.h"
#include <stdio.h>


GameEditorLoginPacketClass::GameEditorLoginPacketClass(int editor_protocol_version) : EditorProtocolVersion(editor_protocol_version)
{
}

GameEditorLoginPacketClass::GameEditorLoginPacketClass(RawPacketClass *raw)
{
	raw->Get(&EditorProtocolVersion, sizeof(EditorProtocolVersion));
}

RawPacketClass *GameEditorLoginPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30000, Get_Data_Size());
	raw->Add(&EditorProtocolVersion, sizeof(EditorProtocolVersion));
	return(raw);
}

int GameEditorLoginPacketClass::Get_Data_Size()
{
	return(sizeof(EditorProtocolVersion));
}

int GameEditorLoginPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EditorProtocolVersion=%u}", "GameEditorLogin", EditorProtocolVersion));
}

GameEditorLogoutPacketClass::GameEditorLogoutPacketClass(RawPacketClass *)
{
}

RawPacketClass *GameEditorLogoutPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30001, Get_Data_Size());
	return(raw);
}

int GameEditorLogoutPacketClass::Get_Data_Size()
{
	return(0);
}

int GameEditorLogoutPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GameEditorLogout"));
}

GameEditorProtocolVersionPacketClass::GameEditorProtocolVersionPacketClass(unsigned int server_protocol_version) : ServerProtocolVersion(server_protocol_version)
{
}

GameEditorProtocolVersionPacketClass::GameEditorProtocolVersionPacketClass(RawPacketClass *raw)
{
	raw->Get(&ServerProtocolVersion, sizeof(ServerProtocolVersion));
}

RawPacketClass *GameEditorProtocolVersionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(30019, Get_Data_Size());
	raw->Add(&ServerProtocolVersion, sizeof(ServerProtocolVersion));
	return(raw);
}

int GameEditorProtocolVersionPacketClass::Get_Data_Size()
{
	return(sizeof(ServerProtocolVersion));
}

int GameEditorProtocolVersionPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ServerProtocolVersion=%u}", "GameEditorProtocolVersion", ServerProtocolVersion));
}

bool GameEditorProtocolVersionPacketClass::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkunknown_85f250.cpp)
**
**	* Client 0x85F250-0x85F840 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The send-only 30019 (not in the packet switch; Process_Packet true) written by hand (builder P5, 2026-10-27).
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
