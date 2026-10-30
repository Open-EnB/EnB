// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgmresetmission.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x871A30-0x871F40 (opcode 237).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGMRESETMISSION_H  // NAME 1 [pkgmresetmission]: provisional, pk + its class's name
#define PKGMRESETMISSION_H

#include "pk.h"

// opcode 237 (0x00ED, ?): vtable 0xB099C4, constructor 0x871A30, 0x24 bytes
class GMResetMissionPacketClass : public PacketClass  // NAME 3: the client's Print text 'GM Reset Mission {'
{
public:
	GMResetMissionPacketClass(int unknown0c, const char *mission_name, bool unknown20);	// 0x871C10
	GMResetMissionPacketClass(RawPacketClass *raw);	// 0x871A30
	virtual ~GMResetMissionPacketClass();	// +0x00 0x871BE0 / 0x871D80
	virtual RawPacketClass *Build();	// +0x04 0x871DF0
	virtual bool Process_Packet(void *client);	// +0x08 0x8695A0 (clientstub.cpp l.446)
	virtual int Get_Data_Size();	// +0x0C 0x871EE0
	virtual int Print(char *buffer, int len);	// +0x10 0x871F00

	int Unknown0C;	// +0x0C (network order)
	std::string MissionName;	// +0x10 (sent as a string)  // NAME 3: the client's Print text 'Mission Name ='
	bool Unknown20;	// +0x20 (sent as a network-order int)
};

#endif
