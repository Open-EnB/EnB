// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkstarbasetow.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A2D70-0x8A35D0 rebuilt so far (opcode 78, 79, 80); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSTARBASETOW_H  // NAME 1 [pkstarbasetow]: provisional, pk + its first named class's name
#define PKSTARBASETOW_H

#include "pk.h"

char *Sector_Type_Name(int sector_type);	// 0x84A3B0 (sector_types.cpp, not rebuilt: 'SectorSpace', ...)  // NAME 2: what it returns

// opcode 78 (0x004E, STARBASE_REQUEST): vtable 0xB0B868, constructor 0x8A2DF0, 0x18 bytes
class StarbaseRequestPacketClass : public PacketClass  // NAME 2: Net-7's opcode name STARBASE_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	StarbaseRequestPacketClass(RawPacketClass *raw);	// 0x8A2DF0
	virtual RawPacketClass *Build();	// +0x04 0x8A2EB0
	virtual bool Process_Packet(void *client);	// +0x08 0x867F80 (clientstub.cpp l.92)
	virtual int Get_Data_Size();	// +0x0C 0x8A2F90
	virtual int Print(char *buffer, int len);	// +0x10 0x8A2FB0

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
	int StarbaseID;	// +0x10  // NAME 3: the client's Print text 'StarbaseID='
	bool Action;	// +0x14  // NAME 3: the client's Print text 'Action='
	char Unknown15[0x3];	// +0x15
};

// opcode 79 (0x004F, STARBASE_SET): vtable 0xB0B898, constructor 0x8A3090, 0x14 bytes
class StarbaseSetPacketClass : public PacketClass
{
public:
	StarbaseSetPacketClass(unsigned char action, unsigned char exit_mode, int starbase_id);	// 0x8A2FF0
	StarbaseSetPacketClass(RawPacketClass *raw);	// 0x8A3090
	virtual RawPacketClass *Build();	// +0x04 0x8A3150
	virtual bool Process_Packet(void *client);	// +0x08 0x7381C0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A3230
	virtual int Print(char *buffer, int len);	// +0x10 0x8A3250

	int StarbaseID;	// +0x0C  // NAME 3: the client's Print text 'StarbaseID='
	unsigned char Action;	// +0x10  // NAME 3: the client's Print text 'Action='
	unsigned char ExitMode;	// +0x11 (a sector type: Sector_Type_Name)  // NAME 3: the client's Print text 'ExitMode='
};

// opcode 80 (0x0050, ?): vtable 0xB0B8C8, constructor 0x8A3370, 0x24 bytes
class StarbaseTowPacketClass : public PacketClass
{
public:
	StarbaseTowPacketClass(int starbase_id1, int tow_cost1, int starbase_id2, int tow_cost2, int starbase_id3, int tow_cost3);	// 0x8A32A0
	StarbaseTowPacketClass(RawPacketClass *raw);	// 0x8A3370
	virtual RawPacketClass *Build();	// +0x04 0x8A3450
	virtual bool Process_Packet(void *client);	// +0x08 0x738330 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A3560
	virtual int Print(char *buffer, int len);	// +0x10 0x8A3580

	int StarbaseID1;	// +0x0C  // NAME 3: the client's Print text 'StarbaseID1='
	int StarbaseID2;	// +0x10  // NAME 3: the client's Print text 'StarbaseID2='
	int StarbaseID3;	// +0x14  // NAME 3: the client's Print text 'StarbaseID3='
	int TowCost1;	// +0x18  // NAME 3: the client's Print text 'TowCost1='
	int TowCost2;	// +0x1C  // NAME 3: the client's Print text 'TowCost2='
	int TowCost3;	// +0x20  // NAME 3: the client's Print text 'TowCost3='
};

#endif
