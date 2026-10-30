// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkfire.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x887290-0x887560 (opcode 21; 22 derives from it, its inline
**	members compiled where the packet switch constructs it).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKFIRE_H  // NAME 1 [pkfire]: provisional, pk + its first class's name
#define PKFIRE_H

#include "pk.h"

// opcode 21 (0x0015, ?): vtable 0xB0A61C, constructor 0x887370, 0x14 bytes
class FirePacketClass : public PacketClass  // NAME 3: the client's Get_Name 'Fire'
{
public:
	FirePacketClass(int game_id, char slot);	// 0x887290
	FirePacketClass(RawPacketClass *raw);	// 0x887370
	virtual RawPacketClass *Build();	// +0x04 0x887410
	virtual bool Process_Packet(void *client);	// +0x08 0x867C50 (clientstub.cpp l.41)
	virtual int Get_Data_Size();	// +0x0C 0x8874E0
	virtual int Print(char *buffer, int len);	// +0x10 0x887500
	virtual int Get_Priority() { return(0x7F); }	// +0x1C 0x86E5B0
	virtual int Get_Type() { return(0x15); }	// +0x28 0x8872E0  // NAME 2: what the derived classes' return (their opcode)
	virtual bool Unknown_2C() { return(true); }	// +0x2C 0x887300  // NAME 1: neutral (D10)
	virtual const char *Get_Name() { return("Fire"); }	// +0x30 0x887320  // NAME 2: what the derived classes' return (a name)

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Slot;	// +0x10  // NAME 3: the client's Print text 'Slot='
};

// opcode 22 (0x0016, ?): vtable 0xB0979C, constructor 0x887370, 0x1C bytes
class FireEquippedPacketClass : public FirePacketClass  // NAME 3: the client's Get_Name 'FireEquipped'
{
public:
	FireEquippedPacketClass(RawPacketClass *raw) : FirePacketClass(raw) {}	// inline (the case stores the vtable)
	virtual int Get_Type() { return(0x16); }	// +0x28 0x86E5D0
	virtual bool Unknown_2C() { return(false); }	// +0x2C 0x86E5F0
	virtual const char *Get_Name() { return("FireEquipped"); }	// +0x30 0x86E610
	char Unknown14[0x8];	// +0x14 (never read or written by the client's code found so far)  // NAME 1: neutral (D10)
};

#endif
