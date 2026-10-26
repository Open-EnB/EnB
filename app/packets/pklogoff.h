/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pklogoff.h (the file name assumed: see pklogoff.cpp)
**
**	The log-off packets (opcodes 3, 185, 186, 187): client 0x89AB30-0x89B340.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKLOGOFF_H  // NAME 2 [pklogoff.h]: the file's name assumed from its classes and its place in the link order
#define PKLOGOFF_H

#include "pk.h"

// opcode 3 (0x0003, LOGOFF): vtable 0xB0B044, constructor 0x89ABE0, 0x10 bytes
class LogoffPacketClass : public PacketClass  // NAME 3: the client's Print text 'Logoff' and the PacketClass convention
{
public:
	LogoffPacketClass(int player_id);					// 0x89AB30
	LogoffPacketClass(RawPacketClass *raw);				// 0x89ABE0
	virtual ~LogoffPacketClass() {}						// +0x00 0x89AB90 (0x89ABC0)
	virtual RawPacketClass *Build();					// +0x04 0x89AC70
	virtual bool Process_Packet(void *client);			// +0x08 0x867B00 (clientstub.cpp l.20)
	virtual int Get_Data_Size();						// +0x0C 0x89AD30
	virtual int Print(char *buffer, int len);			// +0x10 0x89AD50
	virtual bool Close_Channel() { return(true); }		// +0x24 0x89AB70

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID=%d'
};

// opcode 185 (0x00B9, LOGOFF_REQUEST): vtable 0xB0B074, constructor 0x89ADF0, 0x10 bytes
class LogoffRequestPacketClass : public PacketClass  // NAME 3: the client's Print text 'Logoff Request {' and the PacketClass convention
{
public:
	LogoffRequestPacketClass(int player_id);			// 0x89AD80
	LogoffRequestPacketClass(RawPacketClass *raw);		// 0x89ADF0
	virtual RawPacketClass *Build();					// +0x04 0x89AE80
	virtual bool Process_Packet(void *client);			// +0x08 0x867B30 (clientstub.cpp l.23)
	virtual int Get_Data_Size();						// +0x0C 0x89AF40
	virtual int Print(char *buffer, int len);			// +0x10 0x89AF60

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID=%d'
};

// opcode 186 (0x00BA, LOGOFF_CONFIRMATION): vtable 0xB0B0A4, constructor 0x89B020, 0xC bytes
class LogoffConfirmationPacketClass : public PacketClass
{
public:
	LogoffConfirmationPacketClass();					// 0x89AF90
	LogoffConfirmationPacketClass(RawPacketClass *raw);	// 0x89B020
	virtual ~LogoffConfirmationPacketClass() {}			// +0x00 0x89AFD0 (0x89B000)
	virtual RawPacketClass *Build();					// +0x04 0x89B060
	virtual bool Process_Packet(void *client);			// +0x08 0x7333B0 (cpacket.cpp)
	virtual int Get_Data_Size();						// +0x0C 0x89B0F0
	virtual int Print(char *buffer, int len);			// +0x10 0x89B110
};

// opcode 187 (0x00BB, ?): vtable 0xB0B0D4, constructor 0x89B1D0, 0xC bytes; its Print text is 186's
class Packet_00BBClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00BB
{
public:
	Packet_00BBClass();									// 0x89B140
	Packet_00BBClass(RawPacketClass *raw);				// 0x89B1D0
	virtual ~Packet_00BBClass() {}						// +0x00 0x89B180 (0x89B1B0)
	virtual RawPacketClass *Build();					// +0x04 0x89B210
	virtual bool Process_Packet(void *client);			// +0x08 0x867B60 (clientstub.cpp l.26)
	virtual int Get_Data_Size();						// +0x0C 0x89B2A0
	virtual int Print(char *buffer, int len);			// +0x10 0x89B2C0
	virtual bool Close_Channel();						// +0x24 0x89B2F0
};

#endif
