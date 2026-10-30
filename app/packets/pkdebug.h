/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkdebug.h (by convention from the client's
**	pkdebug.cpp, D24)
**
**	The packet classes of client 0x8802C0-0x880590 (opcode 26).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKDEBUG_H  // NAME 3 [pkdebug.h]: by convention from the client's pkdebug.cpp (D24)
#define PKDEBUG_H

#include "pk.h"

// opcode 26 (0x001A, DEBUG): vtable 0xB0A1D0, constructor 0x880340, 0x18 bytes
class DebugPacketClass : public PacketClass  // NAME 2: Net-7's opcode name DEBUG (the Net-7 opcode table; no structure check)
{
public:
	DebugPacketClass(int game_id, int debug_idx, int user_data);	// 0x8802C0
	DebugPacketClass(RawPacketClass *raw);	// 0x880340
	virtual RawPacketClass *Build();	// +0x04 0x880400
	virtual bool Process_Packet(void *client);	// +0x08 0x867CB0 (clientstub.cpp l.47)
	virtual int Get_Data_Size();	// +0x0C 0x8804E0
	virtual int Print(char *buffer, int len);	// +0x10 0x880500

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int DebugIdx;	// +0x10 (DebugLabels' index: the Print text's 'Command')  // NAME 5: the client's assert 'DebugIdx >= 0 && DebugIdx < label_count'
	int UserData;	// +0x14  // NAME 3: the client's Print text 'UserData='
};

#endif
