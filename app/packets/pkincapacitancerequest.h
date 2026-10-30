/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkincapacitancerequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x871F70-0x873770 rebuilt so far (opcode 141, 142, 143, 144, 145); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKINCAPACITANCEREQUEST_H  // NAME 1 [pkincapacitancerequest]: provisional, pk + its first named class's name
#define PKINCAPACITANCEREQUEST_H

#include "pk.h"
#include "..\shared\avatar_desc.h"

// opcode 141 (0x008D, INCAPACITANCE_REQUEST): vtable 0xB099F4, constructor 0x871FE0, 0x10 bytes
class IncapacitanceRequestPacketClass : public PacketClass  // NAME 2: Net-7's opcode name INCAPACITANCE_REQUEST (the Net-7 opcode table; no structure check)
{
public:
	IncapacitanceRequestPacketClass(int game_id);	// 0x871F70
	IncapacitanceRequestPacketClass(RawPacketClass *raw);	// 0x871FE0
	virtual RawPacketClass *Build();	// +0x04 0x872070
	virtual bool Process_Packet(void *client);	// +0x08 0x868A30 (clientstub.cpp l.263)
	virtual int Get_Data_Size();	// +0x0C 0x872130
	virtual int Print(char *buffer, int len);	// +0x10 0x872150

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

// opcode 142 (0x008E, ?): vtable 0xB09A24, constructor 0x872200, 0x14 bytes
class IncapacitanceDistressBeaconPacketClass : public PacketClass  // NAME 3: the client's Print text 'Incapacitance Distress Beacon {'
{
public:
	IncapacitanceDistressBeaconPacketClass(int game_id, bool on);	// 0x872180
	IncapacitanceDistressBeaconPacketClass(RawPacketClass *raw);	// 0x872200
	virtual RawPacketClass *Build();	// +0x04 0x8722A0
	virtual bool Process_Packet(void *client);	// +0x08 0x868A60 (clientstub.cpp l.266)
	virtual int Get_Data_Size();	// +0x0C 0x872370
	virtual int Print(char *buffer, int len);	// +0x10 0x872390

	int GameID;	// +0x0C (network order; -1 by default)  // NAME 3: the client's Print text 'GameID='
	bool On;	// +0x10 (true by default)  // NAME 3: the client's Print text 'On='
};

// opcode 143 (0x008F, ?): vtable 0xB09A54, constructor 0x872450, 0x10 bytes
class Packet_008FClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x008F
{
public:
	Packet_008FClass(int game_id);	// 0x8723E0
	Packet_008FClass(RawPacketClass *raw);	// 0x872450
	virtual RawPacketClass *Build();	// +0x04 0x8724E0
	virtual bool Process_Packet(void *client);	// +0x08 0x868A90 (clientstub.cpp l.269)
	virtual int Get_Data_Size();	// +0x0C 0x8725A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8725C0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

// opcode 144 (0x0090, ?): vtable 0xB09A84, constructor 0x872A80, 0x198 bytes
class IncapacitanceDetailsPacket : public PacketClass  // NAME 3: the client's Print text 'Incapacitance Details' (kept from pk_classes.h)
{
public:
	IncapacitanceDetailsPacket(const std::string &unknown0C, const AvatarDescClass &desc, const std::string &unknown128, const std::string &unknown138, const std::string &unknown148, const std::string &unknown158, const std::string &unknown168, const std::string &unknown178, const std::string &unknown188);	// 0x8725F0
	// the destructor is the implicit one: +0x00 0x872840 / 0x872870
	IncapacitanceDetailsPacket(RawPacketClass *raw);	// 0x872A80
	virtual RawPacketClass *Build();	// +0x04 0x872F70
	virtual bool Process_Packet(void *client);	// +0x08 0x739BB0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x873100
	virtual int Print(char *buffer, int len);	// +0x10 0x873230

	std::string Unknown0C;	// +0x0C  // NAME 1: neutral (D10)
	AvatarDescClass Desc;	// +0x1C (its Read / Write called directly on the message's buffer)  // NAME 2: what it holds
	std::string Unknown128;	// +0x128  // NAME 1: neutral (D10)
	std::string Unknown138;	// +0x138  // NAME 1: neutral (D10)
	std::string Unknown148;	// +0x148  // NAME 1: neutral (D10)
	std::string Unknown158;	// +0x158  // NAME 1: neutral (D10)
	std::string Unknown168;	// +0x168  // NAME 1: neutral (D10)
	std::string Unknown178;	// +0x178  // NAME 1: neutral (D10)
	std::string Unknown188;	// +0x188  // NAME 1: neutral (D10)
};

// opcode 145 (0x0091, ?): vtable 0xB09AB4, constructor 0x8734E0, 0x1C bytes
class IncapacitanceRescueConfirmationPacket : public PacketClass
{
public:
	IncapacitanceRescueConfirmationPacket(const std::string &unknown);	// 0x873260  // NAME 1 [unknown]: neutral (D10)
	IncapacitanceRescueConfirmationPacket(RawPacketClass *raw);	// 0x8734E0
	virtual RawPacketClass *Build();	// +0x04 0x873650
	virtual bool Process_Packet(void *client);	// +0x08 0x739CD0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x873720
	virtual int Print(char *buffer, int len);	// +0x10 0x873740

	std::string Unknown0C;	// +0x0C
};

#endif
