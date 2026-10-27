/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkinventorymove.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x894CA0-0x8952D0 rebuilt so far (opcode 39); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKINVENTORYMOVE_H  // NAME 1 [pkinventorymove]: provisional, pk + its first class's name
#define PKINVENTORYMOVE_H

#include "pk.h"

// An inventory (the packet reads it into an int and stores it: a 4-byte enum)
enum InventoryEnum  // NAME 1: neutral placeholder (D10)
{
	INVENTORY_NONE = 0	// NAME 1: neutral placeholder (D10)
};

// opcode 39 (0x0027, INVENTORY_MOVE): vtable 0xB0AE34, constructor 0x894D50, 0x24 bytes
class InventoryMovePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name INVENTORY_MOVE (the Net-7 opcode table; no structure check)
{
public:
	InventoryMovePacketClass(int id, InventoryEnum from_inv, int from_slot, InventoryEnum to_inv, int to_slot, int num);	// 0x894CA0
	InventoryMovePacketClass(RawPacketClass *raw);	// 0x894D50
	virtual RawPacketClass *Build();	// +0x04 0x894E30
	virtual bool Process_Packet(void *client);	// +0x08 0x867D10 (clientstub.cpp l.53)
	virtual int Get_Data_Size();	// +0x0C 0x894F50
	virtual int Print(char *buffer, int len);	// +0x10 0x894F70

	int ID;	// +0x0C (network order)  // NAME 3: the client's Print text 'ID='
	InventoryEnum FromInv;	// +0x10  // NAME 3: the client's Print text 'FromInv='
	int FromSlot;	// +0x14  // NAME 3: the client's Print text 'FromSlot='
	InventoryEnum ToInv;	// +0x18  // NAME 3: the client's Print text 'ToInv='
	int ToSlot;	// +0x1C  // NAME 3: the client's Print text 'ToSlot='
	int Num;	// +0x20  // NAME 3: the client's Print text 'Num='
};

#endif
