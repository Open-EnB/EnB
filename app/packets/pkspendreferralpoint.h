// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkspendreferralpoint.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x8A5510-0x8A5770 rebuilt so far (opcode 232); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSPENDREFERRALPOINT_H  // NAME 3 [pkspendreferralpoint.h]: by convention from its class SpendReferralPointPacketClass (thunk order) and its place in retail's object order (between pksoundcommand and pkspline)
#define PKSPENDREFERRALPOINT_H

#include "pk.h"

// opcode 232 (0x00E8, ?): vtable 0xB0BB08, constructor 0x8A55A0, 0x14 bytes
class SpendReferralPointPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	SpendReferralPointPacketClass(int unknown0_c);	// 0x8A5510
	SpendReferralPointPacketClass(PacketClass *raw);	// 0x8A55A0
	virtual ~SpendReferralPointPacketClass();	// +0x00 0x8A5580 / 0x8A5550 (out of line: SpaceClientClass 0x751660 calls it after Send_Packet)
	virtual PacketClass *Encode();	// +0x04 0x8A5630  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x869600 (clientstub.cpp l.452)
	virtual int Get_Data_Size();	// +0x0C 0x8A5720
	virtual int Print(char *buffer, int len);	// +0x10 0x8A5740
	virtual int Get_Type();	// +0x28 0x8A5700  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

#endif
