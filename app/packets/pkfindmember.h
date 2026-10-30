/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Packets\pkfindmember.h (the .cpp named by its assert)
**
**	The packet class of client 0x886580-0x886C40 (opcode 83).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKFINDMEMBER_H  // NAME 3 [pkfindmember.h]: the header of the client's pkfindmember.cpp
#define PKFINDMEMBER_H

#include "pk.h"
#include <list>

// One player of FIND_MEMBER 83: 0x10 bytes, four ints sent in order (Add_Player takes the last two as chars)
struct FindMemberStruct  // NAME 1: neutral (D10): what it holds
{
	int Unknown00;	// +0x00  // NAME 1: neutral (D10)
	int Unknown04;	// +0x04  // NAME 1: neutral (D10)
	int Unknown08;	// +0x08 (a char widened)  // NAME 1: neutral (D10)
	int Unknown0C;	// +0x0C (a char widened)  // NAME 1: neutral (D10)
};

// opcode 83 (0x0053, FIND_MEMBER): vtable 0xB0A58C, constructor 0x886710, 0x1C bytes
class FindMemberPacketClass : public PacketClass  // NAME 3: the client's file pkfindmember.cpp and Net-7's opcode name FIND_MEMBER
{
public:
	FindMemberPacketClass();	// 0x886580
	FindMemberPacketClass(RawPacketClass *raw);	// 0x886710
	virtual ~FindMemberPacketClass();	// +0x00 0x886890 / 0x886640
	virtual RawPacketClass *Build();	// +0x04 0x8869F0
	virtual bool Process_Packet(void *client);	// +0x08 0x739500 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x886B00
	virtual int Print(char *buffer, int len);	// +0x10 0x886B30 ('%s {NumPlayers=%d}', the name not passed)
	virtual int Get_Priority() { return(-127); }	// +0x1C 0x886620
	void Add_Player(int unknown00, int unknown04, char unknown08, char unknown0c);	// 0x886670  // NAME 2: what it does

	int NumPlayers;	// +0x0C  // NAME 5: the client's assert 'NumPlayers == (int)PlayerList.size()'
	std::list<FindMemberStruct *> PlayerList;	// +0x10 (the allocator byte; +0x14 the head node, +0x18 the size)  // NAME 5: the same assert
};

#endif
