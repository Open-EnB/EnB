// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\pk_sub_parts.h (by convention from the client's
**	pk_sub_parts.cpp, D24)
**
**	The packet classes of client 0x8754D0-0x876330 (opcode 180).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PK_SUB_PARTS_H  // NAME 3 [pk_sub_parts.h]: by convention from the client's pk_sub_parts.cpp (D24)
#define PK_SUB_PARTS_H

#include "pk.h"
#include <string>
#include <vector>

// One sub part: a name and a value (0x14 bytes)
struct SubPartStruct  // NAME 2: what it holds (the assert's 'SubParts')
{
	SubPartStruct() {}

	std::string Name;	// +0x00 (sent as a string)  // NAME 2: what it holds
	int Value;			// +0x10 (network order)  // NAME 2: what it holds
};

// opcode 180 (0x00B4, SUBPARTS): vtable 0xB09BE4, constructor 0x8756B0, 0x24 bytes
class SubPartsPacket : public PacketClass
{
public:
	SubPartsPacket(int game_id);	// 0x8754D0
	SubPartsPacket(RawPacketClass *raw);	// 0x8756B0
	virtual RawPacketClass *Build();	// +0x04 0x875B70
	virtual bool Process_Packet(void *client);	// +0x08 0x733A50 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x875CA0
	virtual int Print(char *buffer, int len);	// +0x10 0x875CF0
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x875570

	void Add(const std::string &name, int value);	// 0x875910  // NAME 2: what it does
	int Get_Count();	// 0x875A40  // NAME 2: what it returns
	const SubPartStruct &Get(unsigned int index);	// 0x875A80 (an empty one out of range)  // NAME 2: what it returns

	int GameID;	// +0x0C (network order)  // NAME 3: the client's Print text 'GameID='
	int Channel;	// +0x10 (PacketClass::Unknown_14 of the game id; -1 when received)  // NAME 2: Get_Channel returns it
	std::vector<SubPartStruct> SubParts;	// +0x14  // NAME 5: the client's assert 'index >= SubParts.size()'
};

#endif
