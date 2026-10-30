// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\gameedit\pkgeclon.h (the .cpp named by its asserts)
**
**	The packet classes of client 0x85DE30-0x85E610 (opcodes 30021, 30022).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGECLON_H  // NAME 3 [pkgeclon.h]: the header of the client's gameedit\pkgeclon.cpp
#define PKGECLON_H

#include "..\pk.h"
#include "matrix3d.h"

// opcode 30021 (0x7545, ?): vtable 0xB08E58, constructor 0x85DFB0, 0x48 bytes
class Packet_7545Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7545 (Print 'GameEditorClone')
{
public:
	Packet_7545Class(int game_id, const char *new_name, const Matrix3D &transform);	// 0x85DE30
	Packet_7545Class(RawPacketClass *raw);	// 0x85DFB0
	virtual RawPacketClass *Build();	// +0x04 0x85E080
	virtual bool Process_Packet(void *client);	// +0x08 0x8680D0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85E170
	virtual int Print(char *buffer, int len);	// +0x10 0x85E190

	int GameID;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'GameID='
	Matrix3D Transform;	// +0x10 (0x30 bytes as they are)  // NAME 2: what it holds (twelve floats, copied whole)
	int NewNameSize;	// +0x40 (the name's length without its 0)  // NAME 2: what it holds
	char *NewName;	// +0x44 (nstrdup'd by the value constructor; not deleted)  // NAME 3: the client's assert 'new_name' and Print text 'NewName='
};

// 0x7546 (30022, not in the packet switch: sent only): vtable 0xB08E88, constructor 0x85E2D0, 0x20 bytes
class Packet_7546Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7546 (Print 'GameEditorCloneResponse')
{
public:
	Packet_7546Class(int original_game_id, int clone_game_id, const char *error_msg);	// 0x85E1D0
	Packet_7546Class(RawPacketClass *raw);	// 0x85E2D0
	virtual ~Packet_7546Class();	// +0x00 0x85E400 / 0x85E2A0
	virtual RawPacketClass *Build();	// +0x04 0x85E440
	virtual bool Process_Packet(void *client);	// +0x08 0x85E5F0
	virtual int Get_Data_Size();	// +0x0C 0x85E560
	virtual int Print(char *buffer, int len);	// +0x10 0x85E580

	bool CloneSuccess;	// +0x0C  // NAME 3: the client's Print text 'CloneSuccess='
	int OriginalGameID;	// +0x10  // NAME 3: the client's Print text 'OriginalGameID='
	int CloneGameID;	// +0x14  // NAME 3: the client's Print text 'CloneGameID='
	int Size;	// +0x18 (ErrorMsg's bytes with its 0)  // NAME 3: the client's Print text 'Size='
	char *ErrorMsg;	// +0x1C  // NAME 3: the client's assert 'ErrorMsg' and Print text 'ErrorMsg='
};

#endif
