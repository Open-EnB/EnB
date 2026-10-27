/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_861db0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x861DB0-0x862320 rebuilt so far (opcode 30027).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_861DB0_H  // NAME 1 [pkunknown_861db0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_861DB0_H

#include "pk.h"

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

#endif
