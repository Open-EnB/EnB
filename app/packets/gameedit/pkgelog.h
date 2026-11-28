// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\gameedit\pkgelog.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x85F250-0x85F840 rebuilt so far (opcode 30000, 30001); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGELOG_H  // NAME 3 [pkgelog.h]: by convention from its class GameEditorLogin / Logout / ProtocolVersion (thunk order) and its place in retail's object order (between pkgekv and pkgeobj)
#define PKGELOG_H

#include "..\pk.h"

// opcode 30000 (0x7530, ?): vtable 0xB08F78, constructor 0x85F2C0, 0x10 bytes
class GameEditorLoginPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GameEditorLoginPacketClass(int editor_protocol_version);	// 0x85F250
	GameEditorLoginPacketClass(PacketClass *raw);	// 0x85F2C0
	virtual PacketClass *Encode();	// +0x04 0x85F370  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x867F20 (clientstub.cpp l.86)
	virtual int Get_Data_Size();	// +0x0C 0x85F430
	virtual int Print(char *buffer, int len);	// +0x10 0x85F450

	int EditorProtocolVersion;	// +0x0C  // NAME 3: the client's Print text 'EditorProtocolVersion='
};

// opcode 30001 (0x7531, ?): vtable 0xB08FA8, constructor 0x85F480, 0xC bytes
class GameEditorLogoutPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GameEditorLogoutPacketClass(PacketClass *raw);	// 0x85F480
	virtual PacketClass *Encode();	// +0x04 0x85F510  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x868190 (clientstub.cpp l.125)
	virtual int Get_Data_Size();	// +0x0C 0x85F5A0
	virtual int Print(char *buffer, int len);	// +0x10 0x85F5C0

};
// 0x7543 (30019, not in the packet switch: sent only): vtable 0xB08FD8, constructor 0x85F660, 0x10 bytes
class GameEditorProtocolVersionPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	GameEditorProtocolVersionPacketClass(unsigned int server_protocol_version);	// 0x85F5F0
	GameEditorProtocolVersionPacketClass(PacketClass *raw);	// 0x85F660
	virtual PacketClass *Encode();	// +0x04 0x85F710  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x85F820
	virtual int Get_Data_Size();	// +0x0C 0x85F7D0
	virtual int Print(char *buffer, int len);	// +0x10 0x85F7F0

	unsigned int ServerProtocolVersion;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'ServerProtocolVersion='
};

#endif
