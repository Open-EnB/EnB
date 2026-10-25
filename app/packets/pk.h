/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pk.h (the client's asserts name pk.h)
**
**	PARTIAL: only what the connection classes (app\common connect.cpp, udpconn.cpp) use of the
**	packet base class and the message format (docs/kernel-map.md 4, 5). The packet classes, the
**	factory's switch (0x869790) and the base's own functions are not rebuilt yet: a program that
**	links the connections supplies PacketClass's members and Unknown_Create_Packet.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PK_H
#define PK_H

#include "..\shared\auxdata\linearbu.h"

class ConnectionClass;

// A message's header on the wire: the size with these 4 bytes, the opcode (both 16-bit, little
// endian). The asserts name it (`packet->Header.Size`, udpconn.cpp).
struct PacketHeaderClass	// NAME 1: describes what it is
{
	short Size;				// NAME 5: 'packet->Header.Size' (udpconn.cpp l.489)
	short Opcode;			// NAME 4: the reference decoder's (the Net-7 opcode table)
};

// A message built from a packet (PacketClass::Build) or handed to the factory: 0x22C bytes (the
// factory 0x8696C0 makes one on its stack). The payload is a LinearBufferClass over the bytes after
// the header; +0x20..+0x29 are the UDP connection's send parameters (Send_Message 0x82D980).
class RawPacketClass	// NAME 1: describes what it is (the asserts call it `packet`)
{
public:
	PacketHeaderClass Header;		// +0x00 NAME 5: 'packet->Header.Size'
	LinearBufferClass Data;			// +0x04 the payload  // NAME 2: what it holds
	int Channel;					// +0x20 NAME 2: UDP: 0 the default, < 0 a fixed one, > 0 a hashed one
	int Priority;					// +0x24 NAME 2: CordCon::write's priority
	bool Reliable;					// +0x28 NAME 2: a hashed channel's mode (3 reliable, 4 sequenced)
	bool Close_Channel;				// +0x29 NAME 2: the hashed channel is closed after the send
	char Unknown2A[0x22C - 0x2A];	// +0x2A
};

// The packet base class (pk.h: asserts l.483-502), vtable 0xAEDE88, 10 slots.
class PacketClass		// NAME 3: by convention (D24) from pk.h / pk.cpp
{
public:
	virtual ~PacketClass();								// +0x00 0x52B350
	virtual RawPacketClass *Build();					// +0x04 0x52B210: the message to send  // NAME 2: what it does
	virtual bool Process_Packet(void *client);			// +0x08 0x52B240 (SClient *)
	virtual int Get_Data_Size();						// +0x0C 0x52B270  // NAME 2: what it returns
	virtual void Print();								// +0x10 0x52B2A0  // NAME 2: what it does
	virtual bool Unknown_14();							// +0x14 0x86F010 (assert pk.cpp l.1344)
	virtual int Get_Channel();							// +0x18 0x52B2D0 (-1)  // NAME 2: Send_Packet's use (udpconn.cpp)
	virtual int Get_Priority();							// +0x1C 0x52B2F0 (0)  // NAME 2: Send_Packet's use
	virtual bool Is_Reliable();							// +0x20 0x52B310 (true)  // NAME 2: Send_Packet's use
	virtual bool Close_Channel();						// +0x24 0x52B330 (false)  // NAME 2: Send_Packet's use

	ConnectionClass *Connection;	// +0x04 the connection it came from (Receive sets it)  // NAME 2: what it holds
	int Serial;						// +0x08 (the global 0xBF4128)  // NAME 2: what it holds
};

// The factory (0x8696C0): a message (header and payload, contiguous) to a new packet object, or
// NULL for an unknown opcode; it wraps the bytes in a RawPacketClass and calls the switch 0x869790.
PacketClass *Unknown_Create_Packet(void *buffer);	// NAME 1: what it does; app\packets (pk.cpp?)

#endif
