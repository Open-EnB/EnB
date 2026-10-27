/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgmviewmissionlog.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x86FD00-0x8703A0 (opcode 235).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGMVIEWMISSIONLOG_H  // NAME 1 [pkgmviewmissionlog]: provisional, pk + its class's name
#define PKGMVIEWMISSIONLOG_H

#include "pk.h"
#include <vector>

// opcode 235 (0x00EB, ?): vtable 0xB0986C, constructor 0x86FD00, 0x20 bytes
class GMViewMissionLogPacketClass : public PacketClass  // NAME 3: the client's Print text 'GM View Mission Log'
{
public:
	GMViewMissionLogPacketClass(int unknown0c, const std::vector<std::string> &strings);	// 0x86FF40
	GMViewMissionLogPacketClass(RawPacketClass *raw);	// 0x86FD00
	virtual ~GMViewMissionLogPacketClass();	// +0x00 0x86FF10 / 0x870170
	virtual RawPacketClass *Build();	// +0x04 0x870200
	virtual bool Process_Packet(void *client);	// +0x08 0x73F1F0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C PKGMVIEWMISSIONLOG_HDS
	virtual int Print(char *buffer, int len);	// +0x10 0x870370

	int Unknown0C;	// +0x0C (network order)
	std::vector<std::string> Strings;	// +0x10 (a network-order count, then the strings)  // NAME 2: what it holds
};

#endif
