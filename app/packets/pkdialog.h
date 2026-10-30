// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkdialog.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8812D0-0x8816A0 (opcode 98).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKDIALOG_H  // NAME 1 [pkdialog]: provisional, pk + its class's name
#define PKDIALOG_H

#include "pk.h"

// opcode 98 (0x0062, ?): vtable 0xB0A260, constructor 0x881410, 0x18 bytes
class DialogPacketClass : public PacketClass
{
public:
	DialogPacketClass(int type, char *text);	// 0x881300
	DialogPacketClass(RawPacketClass *raw);	// 0x881410
	virtual ~DialogPacketClass();	// +0x00 0x8813E0 / 0x8814E0
	virtual RawPacketClass *Build();	// +0x04 0x881520
	virtual bool Process_Packet(void *client);	// +0x08 0x736DA0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x881600
	virtual int Print(char *buffer, int len);	// +0x10 0x881620
	virtual int Get_Channel() { return(-2); }	// +0x18 0x8813C0
	virtual int Get_Priority() { return(0); }	// +0x1C 0x8813A0

	char *String;	// +0x0C (Size bytes, the text and its 0)  // NAME 3: the client's Print text 'String='
	int Type;	// +0x10  // NAME 3: the client's Print text 'Type='
	short Size;	// +0x14  // NAME 3: the client's Print text 'Size='
};

#endif
