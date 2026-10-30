// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkglobalerror.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x864A10-0x864FF0 (opcode 117).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALERROR_H  // NAME 1 [pkglobalerror]: provisional, pk + its class's name
#define PKGLOBALERROR_H

#include "pk.h"
#include <string>

// opcode 117 (0x0075, GLOBAL_ERROR): vtable 0xB09490, constructor 0x864C90, 0x24 bytes
class GlobalErrorPacketClass : public PacketClass  // NAME 3: the client's log 'GlobalErrorPacketClass recv: %d : %d : %s' (tsk_login.cpp)
{
public:
	GlobalErrorPacketClass(int unknown0c, int error, const char *message);	// 0x864A10 (no message: the error's text, GlobalErrorStrings)
	GlobalErrorPacketClass(RawPacketClass *raw);	// 0x864C90
	virtual RawPacketClass *Build();	// +0x04 0x864EB0
	virtual bool Process_Packet(void *client);	// +0x08 0x76B4A0 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x864FA0
	virtual int Print(char *buffer, int len);	// +0x10 0x864FC0

	int Unknown0C;	// +0x0C (network order)
	int Error;	// +0x10 (network order; an index of GlobalErrorStrings)  // NAME 1: the log 'GlobalErrorPacketClass recv: %d : %d : %s' (tsk_login.cpp)
	std::string Message;	// +0x14 (sent as a string)  // NAME 1: the same log
};

#endif
