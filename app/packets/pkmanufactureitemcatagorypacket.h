/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmanufactureitemcatagorypacket.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89B340-0x89C610 rebuilt so far (opcode 121, 122, 123, 124, 125, 126, 127, 128): all of them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMANUFACTUREITEMCATAGORYPACKET_H  // NAME 1 [pkmanufactureitemcatagorypacket]: provisional, pk + its first named class's name
#define PKMANUFACTUREITEMCATAGORYPACKET_H

#include "pk.h"
#include "..\shared\strconv_lib.h"

char *Manufacturing_Mode_Name(char *buffer, int len, int mode);	// 0x845910 (manufacturing_defs.cpp, not rebuilt: 'None', 'Manufacture', 'Analyze', 'Dismantle')  // NAME 2: what it does

// opcode 121 (0x0079, MANUFACTURE_ITEM_CATAGORY): vtable 0xB0B104, constructor 0x89B3C0, 0x14 bytes
class ManufactureItemCatagoryPacketClass : public PacketClass  // NAME 2: Net-7's opcode name MANUFACTURE_ITEM_CATAGORY (the Net-7 opcode table; no structure check)
{
public:
	ManufactureItemCatagoryPacketClass(int player, int mode);	// 0x89B340
	ManufactureItemCatagoryPacketClass(RawPacketClass *raw);	// 0x89B3C0
	virtual RawPacketClass *Build();	// +0x04 0x89B460
	virtual bool Process_Packet(void *client);	// +0x08 0x868670 (clientstub.cpp l.203)
	virtual int Get_Data_Size();	// +0x0C 0x89B530
	virtual int Print(char *buffer, int len);	// +0x10 0x89B550

	int player;	// +0x0C (network order)  // NAME 3: the client's Print text 'player='
	int Mode;	// +0x10 (network order; Manufacturing_Mode_Name)  // NAME 3: the client's Print text 'Mode = '
};

// opcode 122 (0x007A, MANUFACTURE_ITEM_CATAGORY): vtable 0xB0B134, constructor 0x89B620, 0x14 bytes
class ManufactureItemCatagoryPacket_122Class : public PacketClass  // NAME 2: Net-7's opcode name MANUFACTURE_ITEM_CATAGORY (the Net-7 opcode table; no structure check); opcode added (another opcode's class has the name)
{
public:
	ManufactureItemCatagoryPacket_122Class(int player, int category);	// 0x89B5A0
	ManufactureItemCatagoryPacket_122Class(RawPacketClass *raw);	// 0x89B620
	virtual RawPacketClass *Build();	// +0x04 0x89B6C0
	virtual bool Process_Packet(void *client);	// +0x08 0x8686A0 (clientstub.cpp l.206)
	virtual int Get_Data_Size();	// +0x0C 0x89B790
	virtual int Print(char *buffer, int len);	// +0x10 0x89B7B0

	int player;	// +0x0C  // NAME 3: the client's Print text 'player='
	int Category;	// +0x10  // NAME 3: the client's Print text 'Category='
};

// opcode 123 (0x007B, MANUFACTURE_ITEM_ID): vtable 0xB0B164, constructor 0x89B860, 0x14 bytes
class ManufactureItemIdPacketClass : public PacketClass  // NAME 2: Net-7's opcode name MANUFACTURE_ITEM_ID (the Net-7 opcode table; no structure check)
{
public:
	ManufactureItemIdPacketClass(int player, int item_id);	// 0x89B7E0
	ManufactureItemIdPacketClass(RawPacketClass *raw);	// 0x89B860
	virtual RawPacketClass *Build();	// +0x04 0x89B900
	virtual bool Process_Packet(void *client);	// +0x08 0x8686D0 (clientstub.cpp l.209)
	virtual int Get_Data_Size();	// +0x0C 0x89B9D0
	virtual int Print(char *buffer, int len);	// +0x10 0x89B9F0

	int player;	// +0x0C  // NAME 3: the client's Print text 'player='
	int ItemID;	// +0x10  // NAME 3: the client's Print text 'ItemID='
};

// opcode 124 (0x007C, REFINERY_ITEM_ID): vtable 0xB0B194, constructor 0x89BAA0, 0x14 bytes
class RefineryItemIdPacketClass : public PacketClass  // NAME 2: Net-7's opcode name REFINERY_ITEM_ID (the Net-7 opcode table; no structure check)
{
public:
	RefineryItemIdPacketClass(int player, int item_id);	// 0x89BA20
	RefineryItemIdPacketClass(RawPacketClass *raw);	// 0x89BAA0
	virtual RawPacketClass *Build();	// +0x04 0x89BB40
	virtual bool Process_Packet(void *client);	// +0x08 0x868700 (clientstub.cpp l.212)
	virtual int Get_Data_Size();	// +0x0C 0x89BC10
	virtual int Print(char *buffer, int len);	// +0x10 0x89BC30

	int player;	// +0x0C  // NAME 3: the client's Print text 'player='
	int ItemID;	// +0x10  // NAME 3: the client's Print text 'ItemID='
};

// opcode 125 (0x007D, ?): vtable 0xB0B1C4, constructor 0x89BD00, 0x18 bytes
class Packet_007DClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x007D
{
public:
	Packet_007DClass(int player, int cargo_slot_id, int vault_slot_id);	// 0x89BC60
	Packet_007DClass(RawPacketClass *raw);	// 0x89BD00
	virtual RawPacketClass *Build();	// +0x04 0x89BDC0
	virtual bool Process_Packet(void *client);	// +0x08 0x868730 (clientstub.cpp l.215)
	virtual int Get_Data_Size();	// +0x0C 0x89BE90
	virtual int Print(char *buffer, int len);	// +0x10 0x89BEB0

	int player;	// +0x0C  // NAME 3: the client's Print text 'player='
	int CargoSlotID;	// +0x10  // NAME 3: the client's Print text 'CargoSlotID='
	int VaultSlotID;	// +0x14  // NAME 3: the client's Print text 'VaultSlotID='
};

// opcode 126 (0x007E, MANUFACTURE_ACTION): vtable 0xB0B1F4, constructor 0x89BF70, 0x14 bytes
class ManufactureActionPacketClass : public PacketClass  // NAME 2: Net-7's opcode name MANUFACTURE_ACTION (the Net-7 opcode table; no structure check)
{
public:
	ManufactureActionPacketClass(int player, int action_id);	// 0x89BEF0
	ManufactureActionPacketClass(RawPacketClass *raw);	// 0x89BF70
	virtual RawPacketClass *Build();	// +0x04 0x89C010
	virtual bool Process_Packet(void *client);	// +0x08 0x868760 (clientstub.cpp l.218)
	virtual int Get_Data_Size();	// +0x0C 0x89C0E0
	virtual int Print(char *buffer, int len);	// +0x10 0x89C100

	int player;	// +0x0C  // NAME 3: the client's Print text 'player='
	int ActionID;	// +0x10  // NAME 3: the client's Print text 'ActionID='
};

// opcode 127 (0x007F, MANUFACTURE_SET_MANUFACTURE_ID): vtable 0xB0B224, constructor 0x89C1C0, 0x10 bytes
class ManufactureSetPacketClass : public PacketClass
{
public:
	ManufactureSetPacketClass(int manufacture_id);	// 0x89C130
	ManufactureSetPacketClass(RawPacketClass *raw);	// 0x89C1C0
	virtual RawPacketClass *Build();	// +0x04 0x89C250
	virtual bool Process_Packet(void *client);	// +0x08 0x739330 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89C310
	virtual int Print(char *buffer, int len);	// +0x10 0x89C330

	int ManufactureID;	// +0x0C  // NAME 3: the client's Print text 'ManufactureID='
};

// opcode 128 (0x0080, MANUFACTURE_TECH_LEVEL_FILTER): vtable 0xB0B254, constructor 0x89C3E0, 0x18 bytes
class ManufactureTechLevelFilterPacketClass : public PacketClass  // NAME 2: Net-7's opcode name MANUFACTURE_TECH_LEVEL_FILTER (the Net-7 opcode table; no structure check)
{
public:
	ManufactureTechLevelFilterPacketClass(int player, bool mode, unsigned int bit_field);	// 0x89C360
	ManufactureTechLevelFilterPacketClass(RawPacketClass *raw);	// 0x89C3E0
	virtual RawPacketClass *Build();	// +0x04 0x89C4A0
	virtual bool Process_Packet(void *client);	// +0x08 0x868790 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89C580
	virtual int Print(char *buffer, int len);	// +0x10 0x89C5A0 (its format has three conversions for two values, as the client's)

	int Player;	// +0x0C (network order)  // NAME 3: the client's Print text 'player='
	bool Mode;	// +0x10 (true by default)  // NAME 3: the client's Print text 'Mode = '
	unsigned int BitField;	// +0x14 (network order)  // NAME 3: the client's Print text 'BitField = '
};

#endif
