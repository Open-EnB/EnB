/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkhullupgrade.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x894A00-0x894C70 rebuilt so far (opcode 89); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKHULLUPGRADE_H  // NAME 1 [pkhullupgrade]: provisional, pk + its first named class's name
#define PKHULLUPGRADE_H

#include "pk.h"

// opcode 89 (0x0059, ?): vtable 0xB0AE04, constructor 0x894AA0, 0x14 bytes
class HullUpgradePacketClass : public PacketClass
{
public:
	HullUpgradePacketClass(int ship_id, int hull_upgrade_level);	// 0x894A00
	HullUpgradePacketClass(RawPacketClass *raw);	// 0x894AA0
	virtual RawPacketClass *Build();	// +0x04 0x894B40
	virtual bool Process_Packet(void *client);	// +0x08 0x7396C0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x894C10
	virtual int Print(char *buffer, int len);	// +0x10 0x894C30

	int ShipID;	// +0x0C  // NAME 3: the client's Print text 'ShipID='
	int HullUpgradeLevel;	// +0x10  // NAME 3: the client's Print text 'HullUpgradeLevel='
};

#endif
