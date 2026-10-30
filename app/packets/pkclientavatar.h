// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkclientavatar.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x877370-0x877610 rebuilt so far (opcode 55); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCLIENTAVATAR_H  // NAME 1 [pkclientavatar]: provisional, pk + its first named class's name
#define PKCLIENTAVATAR_H

#include "pk.h"

// opcode 55 (0x0037, CLIENT_AVATAR): vtable 0xB09D1C, constructor 0x877470, 0x14 bytes
class ClientAvatarPacketClass : public PacketClass
{
public:
	ClientAvatarPacketClass(RawPacketClass *raw);	// 0x877470
	virtual RawPacketClass *Build();	// +0x04 0x877500
	virtual bool Process_Packet(void *client);	// +0x08 0x733590 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8775C0
	virtual int Print(char *buffer, int len);	// +0x10 0x8775E0
	virtual int Get_Channel() { return(Unknown10); }	// +0x18 0x877400

	int AvatarID;	// +0x0C  // NAME 3: the client's Print text 'AvatarID='
	int Unknown10;	// +0x10
};

#endif
