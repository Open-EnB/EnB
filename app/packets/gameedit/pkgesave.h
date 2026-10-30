// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\gameedit\pkgesave.h (the .cpp named by its asserts)
**
**	The packet classes of client 0x861DB0-0x862320 (opcodes 30027, 30028).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGESAVE_H  // NAME 3 [pkgesave.h]: the header of the client's gameedit\pkgesave.cpp
#define PKGESAVE_H

#include "..\pk.h"

// opcode 30027 (0x754B, ?): vtable 0xB092A8, constructor 0x861E20, 0xC bytes
class GameEditorSaveSectorPacketClass : public PacketClass  // NAME 3: the client's Print text 'GameEditorSaveSector {' and the PacketClass convention
{
public:
	GameEditorSaveSectorPacketClass();	// 0x861DB0
	GameEditorSaveSectorPacketClass(RawPacketClass *raw);	// 0x861E20
	virtual RawPacketClass *Build();	// +0x04 0x861E80
	virtual bool Process_Packet(void *client);	// +0x08 0x868340 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x861F10
	virtual int Print(char *buffer, int len);	// +0x10 0x861F30
};

// opcode 30028 (0x754C, ?), sent by the server only (not in the packet switch: Process_Packet returns
// true here): vtable 0xB092D8, 0x18 bytes
class GameEditorSectorSavedPacketClass : public PacketClass  // NAME 3: the client's Print text 'GameEditorSectorSaved' and the PacketClass convention
{
public:
	GameEditorSectorSavedPacketClass(bool success, const char *message);	// 0x861F80
	GameEditorSectorSavedPacketClass(RawPacketClass *raw);	// 0x862070
	virtual ~GameEditorSectorSavedPacketClass();	// +0x00 0x862180 / 0x862040
	virtual RawPacketClass *Build();	// +0x04 0x8621B0
	virtual bool Process_Packet(void *client);	// +0x08 0x862300
	virtual int Get_Data_Size();	// +0x0C 0x8622A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8622C0

	bool Success;	// +0x0C  // NAME 3: the client's Print text 'Success='
	int Length;	// +0x10 (Message's bytes with its 0)  // NAME 2: what it holds
	char *Message;	// +0x14  // NAME 5: the client's assert 'Message' and Print text 'Message='
};

#endif
