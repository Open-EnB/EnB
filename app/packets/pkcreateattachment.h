// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkcreateattachment.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x876A20-0x876D00 rebuilt so far (opcode 74); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCREATEATTACHMENT_H  // NAME 1 [pkcreateattachment]: provisional, pk + its first named class's name
#define PKCREATEATTACHMENT_H

#include "pk.h"

// opcode 74 (0x004A, CREATE_ATTACHMENT): vtable 0xB09C74, constructor 0x876B30, 0x20 bytes
class CreateAttachmentPacketClass : public UnknownPacketBaseClass
{
public:
	CreateAttachmentPacketClass(int unknown0c, int parent_id, int child_id, int unknown18);	// 0x876A20  // NAME 2: the stores (no caller left)
	CreateAttachmentPacketClass(PacketClass *raw);	// 0x876B30
	virtual PacketClass *Encode();	// +0x04 0x876BD0  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x733BB0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x876CA0
	virtual int Print(char *buffer, int len);	// +0x10 0x876CC0
	virtual int Get_Channel() { return(Unknown1C); }	// +0x18 0x876AC0

	int Unknown0C;	// +0x0C (the sending constructor's first argument, Unknown1C's channel)
	int Parent_ID;	// +0x10  // NAME 3: the client's Print text 'Parent_ID='
	int Child_ID;	// +0x14  // NAME 3: the client's Print text 'Child_ID='
	int Unknown18;	// +0x18
	int Unknown1C;	// +0x1C
};

#endif
