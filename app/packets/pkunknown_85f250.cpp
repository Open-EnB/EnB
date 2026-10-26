// Earth & Beyond client (reconstruction): app\packets\pkunknown_85f250.cpp. Notes at the end of the file.
#include "pkunknown_85f250.h"
#include <stdio.h>


Packet_7530Class::Packet_7530Class(int editor_protocol_version) : EditorProtocolVersion(editor_protocol_version)
{
}

Packet_7530Class::Packet_7530Class(RawPacketClass *raw)
{
	raw->Get(&EditorProtocolVersion, sizeof(EditorProtocolVersion));
}

RawPacketClass *Packet_7530Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30000, Get_Data_Size());
	raw->Add(&EditorProtocolVersion, sizeof(EditorProtocolVersion));
	return(raw);
}

int Packet_7530Class::Get_Data_Size()
{
	return(sizeof(EditorProtocolVersion));
}

int Packet_7530Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EditorProtocolVersion=%u}", "GameEditorLogin", EditorProtocolVersion));
}

Packet_7531Class::Packet_7531Class(RawPacketClass *)
{
}

RawPacketClass *Packet_7531Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30001, Get_Data_Size());
	return(raw);
}

int Packet_7531Class::Get_Data_Size()
{
	return(0);
}

int Packet_7531Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GameEditorLogout"));
}


/*
**	Reconstruction notes (pkunknown_85f250.cpp)
**
**	* Client 0x85F250-0x85F840 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
