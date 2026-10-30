// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgadget.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x887900-0x887EF0 (opcode 103).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGADGET_H  // NAME 1 [pkgadget]: provisional, pk + its class's name
#define PKGADGET_H

#include "pk.h"

// opcode 103 (0x0067, ?): vtable 0xB0A65C, constructor 0x887A20, 0x18 bytes
class GadgetPacketClass : public PacketClass  // NAME 3: the client's Print text 'Gadget Control'
{
public:
	GadgetPacketClass(int function, char *name);	// 0x887900 (Name not copied, not owned)
	GadgetPacketClass(RawPacketClass *raw);	// 0x887A20
	virtual ~GadgetPacketClass();	// +0x00 0x8879F0 / 0x887AF0
	virtual RawPacketClass *Build();	// +0x04 0x887B30
	virtual bool Process_Packet(void *client);	// +0x08 0x739180 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x887C10
	virtual int Print(char *buffer, int len);	// +0x10 0x887C30
	virtual int Get_Channel() { return(-2); }	// +0x18 0x887990
	virtual int Get_Priority() { return(0); }	// +0x1C 0x887970
	virtual int Get_Type() { return(103); }	// +0x28 0x8879B0  // NAME 2: what it returns (the opcode Build sends), as SuperLinePacketClass's
	virtual const char *Get_Name() { return("Gadget Control"); }	// +0x2C 0x8879D0  // NAME 2: what it returns (Print's name), as MessageLinePacketClass's

	int Function;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Function='
	char *Name;	// +0x10 (Size bytes)  // NAME 3: the client's Print text 'Name='
	short Size;	// +0x14 (2 bytes as they are: the name's length with its 0)  // NAME 3: the client's Print text 'Size='
	bool Owned;	// +0x16 (the raw constructor's new[]'d Name; the destructor deletes it then)  // NAME 2: what it means
};

// opcode 104 (0x0068, ?): vtable 0xB0A698, constructor 0x887DB0, 0x1C bytes
class Packet_0068Class : public GadgetPacketClass  // NAME 1: neutral (D10): opcode 0x0068 (Get_Name 'Request Gadget Control')
{
public:
	Packet_0068Class(int unknown18, int function, char *name);	// 0x887CC0
	Packet_0068Class(RawPacketClass *raw);	// 0x887DB0
	virtual RawPacketClass *Build();	// +0x04 0x887EC0
	virtual bool Process_Packet(void *client);	// +0x08 0x868640 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x887EA0
	virtual int Get_Type() { return(104); }	// +0x28 0x887D40  // NAME 2: as GadgetPacketClass's
	virtual const char *Get_Name() { return("Request Gadget Control"); }	// +0x2C 0x887D60  // NAME 2: as GadgetPacketClass's

	int Unknown18;	// +0x18 (4 bytes as they are)
};

#endif
