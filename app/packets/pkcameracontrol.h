// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkcameracontrol.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x878280-0x8785A0 rebuilt so far (opcode 146); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCAMERACONTROL_H  // NAME 1 [pkcameracontrol]: provisional, pk + its first named class's name
#define PKCAMERACONTROL_H

#include "pk.h"

// opcode 146 (0x0092, CAMERA_CONTROL): vtable 0xB09DDC, constructor 0x878390, 0x18 bytes
class CameraControlPacketClass : public PacketClass
{
public:
	CameraControlPacketClass(RawPacketClass *raw);	// 0x878390
	virtual RawPacketClass *Build();	// +0x04 0x878450
	virtual bool Process_Packet(void *client);	// +0x08 0x739FF0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x878540
	virtual int Print(char *buffer, int len);	// +0x10 0x878560
	virtual int Get_Channel() { return(Unknown14); }	// +0x18 0x878320
	virtual int Get_Type();	// +0x28 0x878520  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Message;	// +0x0C  // NAME 3: the client's Print text 'Message='
	int GameID;	// +0x10  // NAME 3: the client's Print text 'GameID='
	int Unknown14;	// +0x14
};

#endif
