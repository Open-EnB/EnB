// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pksetzband.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A4250-0x8A4760 rebuilt so far (opcode 42, 43); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSETZBAND_H  // NAME 1 [pksetzband]: provisional, pk + its first named class's name
#define PKSETZBAND_H

#include "pk.h"

// opcode 42 (0x002A, SET_ZBAND): vtable 0xB0B9B8, constructor 0x8A42F0, 0x14 bytes
class SetZBandPacketClass : public PacketClass
{
public:
	SetZBandPacketClass(float min, float max);	// 0x8A4250
	SetZBandPacketClass(RawPacketClass *raw);	// 0x8A42F0
	virtual RawPacketClass *Build();	// +0x04 0x8A4390
	virtual bool Process_Packet(void *client);	// +0x08 0x737520 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A4460
	virtual int Print(char *buffer, int len);	// +0x10 0x8A4480

	float Min;	// +0x0C  // NAME 3: the client's Print text 'Min='
	float Max;	// +0x10  // NAME 3: the client's Print text 'Max='
};

// opcode 43 (0x002B, SET_BBOX): vtable 0xB0B9E8, constructor 0x8A4580, 0x1C bytes
class SetBBoxPacketClass : public PacketClass
{
public:
	SetBBoxPacketClass(RawPacketClass *raw);	// 0x8A4580
	virtual RawPacketClass *Build();	// +0x04 0x8A4620
	virtual bool Process_Packet(void *client);	// +0x08 0x737590 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A46F0
	virtual int Print(char *buffer, int len);	// +0x10 0x8A4710

	float XMin[2];	// +0x0C  // NAME 3: the client's Print text 'XMin='
	float XMax[2];	// +0x14  // NAME 3: the client's Print text 'XMax='
};

#endif
