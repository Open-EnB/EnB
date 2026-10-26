/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkauxdata.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x876D30-0x877340 (opcodes 27, 28): the aux data message and the
**	player-variable one derived from it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKAUXDATA_H  // NAME 1 [pkauxdata]: provisional, pk + its first named class's name
#define PKAUXDATA_H

#include "pk.h"

// opcode 27 (0x001B, AUX_DATA): vtable 0xB09CA4, constructor 0x876E90, 0x1C bytes
class AuxDataPacketClass : public PacketClass
{
public:
	AuxDataPacketClass(int game_id, const void *data, int size);	// 0x876D30
	AuxDataPacketClass(RawPacketClass *raw);	// 0x876E90
	virtual ~AuxDataPacketClass();	// +0x00 0x876F60 / 0x876E60
	virtual RawPacketClass *Build();	// +0x04 0x876FA0
	virtual bool Process_Packet(void *client);	// +0x08 0x7364C0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x877080
	virtual int Print(char *buffer, int len);	// +0x10 0x8770A0
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x876E00
	virtual int Get_Type() { return(27); }	// +0x28 0x876E20  // NAME 2: what it returns (the opcode), as MessageLinePacketClass's
	virtual const char *Get_Name() { return("Aux_Data"); }	// +0x2C 0x876E40  // NAME 2: what it returns (Print's '%s {')

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	short Size;	// +0x10  // NAME 3: the client's Print text 'Size='
	char *Data;	// +0x14 (new[] Size bytes; Print shows its first dword as 'Flags=')  // NAME 2: what it holds
	int Channel;	// +0x18 (PacketClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
};

// opcode 28 (0x001C, PLAYER_VAR_AUX_DATA): vtable 0xB09CE0, constructor 0x877230, 0x1C bytes
class AuxDataPlayerVarPacketClass : public AuxDataPacketClass
{
public:
	AuxDataPlayerVarPacketClass(int game_id, const void *data, int size);	// 0x8770E0
	AuxDataPlayerVarPacketClass(RawPacketClass *raw);	// 0x877230
	virtual bool Process_Packet(void *client);	// +0x08 0x736650 (not rebuilt)
	virtual int Get_Type() { return(28); }	// +0x28 0x8771C0
	virtual const char *Get_Name() { return("PlayerVar_Aux_Data"); }	// +0x2C 0x8771E0
};

#endif
