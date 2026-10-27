/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmessageline.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89D730-0x89E0F0 rebuilt so far (opcode 36, 35); the file's
**	MessageLinePacketClass (the base of 32-34) is still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMESSAGELINE_H  // NAME 1 [pkmessageline]: provisional, pk + the object's MessageLinePacketClass
#define PKMESSAGELINE_H

#include "pk.h"

// opcode 36 (0x0024, ?): vtable 0xB0B3E0, constructor 0x89DC60, 0xC bytes
class ClearMessageLinePacketClass : public PacketClass
{
public:
	ClearMessageLinePacketClass();	// 0x89DBB0
	ClearMessageLinePacketClass(RawPacketClass *raw);	// 0x89DC60
	virtual RawPacketClass *Build();	// +0x04 0x89DCC0
	virtual bool Process_Packet(void *client);	// +0x08 0x736D00 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x89DD40
	virtual int Print(char *buffer, int len);	// +0x10 0x89DD60
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89DC10
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89DBF0
};

// opcode 35 (0x0023, ?): vtable 0xB0B410, constructor 0x89DEB0, 0x10 bytes
class SuperLinePacketClass : public PacketClass
{
public:
	SuperLinePacketClass(char *string);	// 0x89DD90 (a copy: nstrdup)
	SuperLinePacketClass(RawPacketClass *raw);	// 0x89DEB0
	virtual ~SuperLinePacketClass();	// +0x00 0x89DE80 / 0x89DF40
	virtual RawPacketClass *Build();	// +0x04 0x89DF80
	virtual bool Process_Packet(void *client);	// +0x08 0x736C60 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x89E050
	virtual int Print(char *buffer, int len);	// +0x10 0x89E080
	virtual int Get_Channel() { return(-2); }	// +0x18 0x89DE40
	virtual int Get_Priority() { return(0); }	// +0x1C 0x89DE20
	virtual int Get_Type() { return(35); }	// +0x28 0x89DE60  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	char *String;	// +0x0C (new[]'d, a 0-terminated string)  // NAME 3: the client's Print text 'String='
};

#endif
