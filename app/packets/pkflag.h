// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\pkflag.h (the name from the client's pkflag.cpp)
**
**	The flagged packets' base: a bit mask says which of the packet's fields the message carries
**	(the effects, opcodes 9 and 11-14, and ADVANCED_POSITIONAL_UPDATE 62 derive from it), and the
**	helpers that read and write a field by its bit (client 0x887570-0x8878D0, pkflag.cpp).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKFLAG_H  // NAME 3 [pkflag.h]: after the client's pkflag.cpp (D57)
#define PKFLAG_H

#include "pk.h"

// the base of the flagged packets: vtable 0xB0A3D4 (PacketClass's slots, +0x28 pure; its scalar
// deleting destructor 0x882950 and destructor 0x882910 are emitted in the effects' object)
class FlagPacketClass : public PacketClass  // NAME 3: after the client's file pkflag.cpp (D24)
{
public:
	virtual int Get_Flag_Count() = 0;	// +0x28 (the number of flag bits)  // NAME 2: what pkflag.cpp's asserts call it (flag_max)

	void Write_Flags(RawPacketClass *raw, unsigned int flags);	// 0x887680 (1, 2 or 4 bytes)  // NAME 2: what it does
	void Read_Flags(RawPacketClass *raw, unsigned int *flags);	// 0x8877B0  // NAME 2: what it does
	int Get_Flag_Size();	// 0x887880 (the bytes Write_Flags writes)  // NAME 2: what it returns
};

void Add_Flagged(RawPacketClass *raw, unsigned int flags, int flag, const void *data, int len);	// 0x887570  // NAME 2: what it does
void Get_Flagged(RawPacketClass *raw, unsigned int flags, int flag, void *data, int len);	// 0x8875B0  // NAME 2: what it does
int Flagged_Size(unsigned int flags, int flag, int len);	// 0x8875F0 (len when the bit is set, else 0)  // NAME 2: what it returns
void Set_Flag(unsigned int *flags, int flag, bool set);	// 0x887620  // NAME 2: what it does
bool Is_Flag_Set(unsigned int flags, int flag);	// 0x887650  // NAME 2: what it returns

#endif
