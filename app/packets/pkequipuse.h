/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkequipuse.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AE810-0x8AEAB0 rebuilt so far (opcode 93); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKEQUIPUSE_H  // NAME 1 [pkequipuse]: provisional, pk + its first named class's name
#define PKEQUIPUSE_H

#include "pk.h"

// opcode 93 (0x005D, EQUIP_USE): vtable 0xB0C204, constructor 0x8AE8B0, 0x14 bytes
class EquipUsePacketClass : public PacketClass  // NAME 2: Net-7's opcode name EQUIP_USE (the Net-7 opcode table; no structure check)
{
public:
	EquipUsePacketClass(RawPacketClass *raw);	// 0x8AE8B0
	virtual RawPacketClass *Build();	// +0x04 0x8AE970
	virtual bool Process_Packet(void *client);	// +0x08 0x867C80 (clientstub.cpp l.44)
	virtual int Get_Data_Size();	// +0x0C 0x8AEA50
	virtual int Print(char *buffer, int len);	// +0x10 0x8AEA70
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8AE860

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char Inventory;	// +0x10  // NAME 3: the client's Print text 'Inventory='
	char Slot;	// +0x11  // NAME 3: the client's Print text 'Slot='
	char Unknown12[0x2];	// +0x12
};

#endif
