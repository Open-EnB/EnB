// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkitemstate.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x895C00-0x895ED0 rebuilt so far (opcode 41); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKITEMSTATE_H  // NAME 1 [pkitemstate]: provisional, pk + its first named class's name
#define PKITEMSTATE_H

#include "pk.h"

// opcode 41 (0x0029, ITEM_STATE): vtable 0xB0AEC4, constructor 0x895CA0, 0x18 bytes
class ItemStatePacketClass : public PacketClass  // NAME 2: Net-7's opcode name ITEM_STATE (the Net-7 opcode table; no structure check)
{
public:
	ItemStatePacketClass(RawPacketClass *raw);	// 0x895CA0
	virtual RawPacketClass *Build();	// +0x04 0x895D70
	virtual bool Process_Packet(void *client);	// +0x08 0x867D70 (clientstub.cpp l.59)
	virtual int Get_Data_Size();	// +0x0C 0x895E60
	virtual int Print(char *buffer, int len);	// +0x10 0x895E80

	int ID;	// +0x0C  // NAME 3: the client's Print text 'ID='
	int State;	// +0x10  // NAME 3: the client's Print text 'State='
	char BoolVal;	// +0x14  // NAME 3: the client's Print text 'BoolVal='
	char Inventory;	// +0x15  // NAME 3: the client's Print text 'Inventory='
	char Slot;	// +0x16  // NAME 3: the client's Print text 'Slot='
	char Unknown17[0x1];	// +0x17
};

#endif
