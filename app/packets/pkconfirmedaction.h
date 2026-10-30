/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkconfirmedaction.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8703C0-0x870E60 (opcodes 190, 191, 192).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCONFIRMEDACTION_H  // NAME 1 [pkconfirmedaction]: provisional, pk + its classes' common name
#define PKCONFIRMEDACTION_H

#include "pk.h"

// A confirmed action's type (read through an int and stored: a 4-byte enum)
enum ConfirmedActionEnum  // NAME 1: neutral placeholder (D10)
{
	CONFIRMED_ACTION_NONE = 0	// NAME 1: neutral placeholder (D10)
};

// opcode 190 (0x00BE, CONFIRMED_ACTION_OFFER): vtable 0xB0989C, constructor 0x870650, 0x24 bytes
class ConfirmedActionOfferPacketClass : public PacketClass  // NAME 2: Net-7's opcode name CONFIRMED_ACTION_OFFER (the Net-7 opcode table; no structure check)
{
public:
	ConfirmedActionOfferPacketClass(int id, const std::string &message, ConfirmedActionEnum type);	// 0x8703D0
	ConfirmedActionOfferPacketClass(RawPacketClass *raw);	// 0x870650
	virtual RawPacketClass *Build();	// +0x04 0x870800
	virtual bool Process_Packet(void *client);	// +0x08 0x8692A0 (clientstub.cpp l.398)
	virtual int Get_Data_Size();	// +0x0C 0x8708E0
	virtual int Print(char *buffer, int len);	// +0x10 0x870900

	int ID;	// +0x0C (network order)  // NAME 3: the client's Print text 'ID='
	std::string Message;	// +0x10  // NAME 3: the client's Print text 'Message='
	ConfirmedActionEnum Type;	// +0x20 (network order)  // NAME 3: the client's Print text 'Type='
};

// opcode 191 (0x00BF, ?): vtable 0xB098CC, constructor 0x8709E0, 0x14 bytes
class ConfirmedActionTimeoutPacketClass : public PacketClass  // NAME 3: the client's Print text 'Confirmed Action Timeout {'
{
public:
	ConfirmedActionTimeoutPacketClass(ConfirmedActionEnum type, int id);	// 0x870940
	ConfirmedActionTimeoutPacketClass(RawPacketClass *raw);	// 0x8709E0
	virtual RawPacketClass *Build();	// +0x04 0x870AA0
	virtual bool Process_Packet(void *client);	// +0x08 0x73DCB0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x870B70
	virtual int Print(char *buffer, int len);	// +0x10 0x870B90

	int ID;	// +0x0C (network order)  // NAME 3: the client's Print text 'ID='
	ConfirmedActionEnum Type;	// +0x10 (network order)  // NAME 3: the client's Print text 'Type='
};

// opcode 192 (0x00C0, CONFIRMED_ACTION_RESPONSE): vtable 0xB098FC, constructor 0x870C50, 0x18 bytes
class ConfirmedActionResponsePacketClass : public PacketClass  // NAME 2: Net-7's opcode name CONFIRMED_ACTION_RESPONSE (the Net-7 opcode table; no structure check)
{
public:
	ConfirmedActionResponsePacketClass(int avatar_id, int id, bool response);	// 0x870BD0
	ConfirmedActionResponsePacketClass(RawPacketClass *raw);	// 0x870C50
	virtual RawPacketClass *Build();	// +0x04 0x870D20
	virtual bool Process_Packet(void *client);	// +0x08 0x868E20 (clientstub.cpp l.326)
	virtual int Get_Data_Size();	// +0x0C 0x870E00
	virtual int Print(char *buffer, int len);	// +0x10 0x870E20

	int AvatarID;	// +0x0C (network order)  // NAME 3: the client's Print text 'AvatarID ='
	int ID;	// +0x10 (network order)  // NAME 3: the client's Print text 'ID='
	bool Response;	// +0x14 (sent as a network-order int, > 0 when read)  // NAME 3: the client's Print text 'Response='
};

#endif
