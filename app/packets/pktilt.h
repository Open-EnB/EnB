// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pktilt.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AAAB0-0x8AAD20 rebuilt so far (opcode 19); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKTILT_H  // NAME 1 [pktilt]: provisional, pk + its first named class's name
#define PKTILT_H

#include "pk.h"

// opcode 19 (0x0013, TILT): vtable 0xB0BF88, constructor 0x8AAB50, 0x14 bytes
class TiltPacketClass : public PacketClass  // NAME 2: Net-7's opcode name TILT (the Net-7 opcode table; no structure check)
{
public:
	TiltPacketClass(int game_id, float intensity);	// 0x8AAAB0
	TiltPacketClass(RawPacketClass *raw);	// 0x8AAB50
	virtual RawPacketClass *Build();	// +0x04 0x8AABF0
	virtual bool Process_Packet(void *client);	// +0x08 0x867BF0 (clientstub.cpp l.35)
	virtual int Get_Data_Size();	// +0x0C 0x8AACC0
	virtual int Print(char *buffer, int len);	// +0x10 0x8AACE0
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8AAB00

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	float Intensity;	// +0x10  // NAME 3: the client's Print text 'Intensity='
};

#endif
