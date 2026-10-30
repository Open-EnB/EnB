/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmoderequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89CDB0-0x89D040 (opcode 75).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMODEREQUEST_H  // NAME 1 [pkmoderequest]: provisional, pk + its Print text 'Mode Request'
#define PKMODEREQUEST_H

#include "pk.h"

// opcode 75 (0x004B, ?): vtable 0xB0B314, constructor 0x89CE70, 0x14 bytes
class Packet_004BClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x004B
{
public:
	Packet_004BClass(int unknown0c, int mode);	// 0x89CDB0
	Packet_004BClass(RawPacketClass *raw);	// 0x89CE70
	virtual RawPacketClass *Build();	// +0x04 0x89CF10
	virtual bool Process_Packet(void *client);	// +0x08 0x738100 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C PKMODEREQUEST_HDS
	virtual int Print(char *buffer, int len);	// +0x10 0x89D000

	int Unknown0C;	// +0x0C (4 bytes as they are)
	int Mode;	// +0x10 (4 bytes as they are: an index into 0xBB56E8's names)  // NAME 3: the client's Print text 'Mode='
};

#endif
