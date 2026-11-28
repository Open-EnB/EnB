// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgelog.cpp. Notes at the end of the file.
#include "pkgelog.h"
#include <stdio.h>


GameEditorLoginPacketClass::GameEditorLoginPacketClass(int editor_protocol_version) : EditorProtocolVersion(editor_protocol_version)
{
}

GameEditorLoginPacketClass::GameEditorLoginPacketClass(PacketClass *raw)
{
	raw->Get(&EditorProtocolVersion, sizeof(EditorProtocolVersion));
}

PacketClass *GameEditorLoginPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(30000, Get_Data_Size());
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

GameEditorLogoutPacketClass::GameEditorLogoutPacketClass(PacketClass *)
{
}

PacketClass *GameEditorLogoutPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(30001, Get_Data_Size());
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

GameEditorProtocolVersionPacketClass::GameEditorProtocolVersionPacketClass(PacketClass *raw)
{
	raw->Get(&ServerProtocolVersion, sizeof(ServerProtocolVersion));
}

PacketClass *GameEditorProtocolVersionPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(30019, Get_Data_Size());
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
**	Reconstruction notes (pkgelog.cpp)
**
**	* Client 0x85F250-0x85F840 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The send-only 30019 (not in the packet switch; Process_Packet true) written by hand (builder P5, 2026-10-27).
**	* The file name is by convention from its class GameEditorLogin / Logout / ProtocolVersion (thunk order) and its place in retail's object order (between pkgekv and pkgeobj), NAME 3; no assert names it.
*/
