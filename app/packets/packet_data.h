// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the wire interface of an object a raw message adds or gets, out of pk.h: the
**	client's unknown_galaxy object has no <string> initialiser, so unknown_packetdata.h, its only path to pk.h,
**	did not include pk.h. pk.h includes this header where the class stood.
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PACKET_DATA_H	// NAME 1 [packet_data.h]: neutral, what it declares (D10)
#define PACKET_DATA_H

class LinearBufferClass;

// What the raw message's Add / Get of an object call (slot 0 writes, 4 reads, 8 sizes it): MoneyPacketDataClass
class Unknown_PacketDataClass	// NAME 1: neutral placeholder (D10)
{
public:
	virtual bool Write(LinearBufferClass *buffer) = 0;	// +0x00 NAME 2: what RawPacketClass::Add calls it for (the money wrapper's returns true)
	virtual bool Read(LinearBufferClass *buffer) = 0;	// +0x04 NAME 2: what RawPacketClass::Get calls it for
	virtual int Get_Size() = 0;							// +0x08 NAME 2: the bytes Write writes (the money wrapper's 0x873A90)
};

#endif
