/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\connect.h (the client's asserts name it, l.74)
**
**	PARTIAL (docs/kernel-map.md 4): the connection base (vtable 0xB0850C) and the stream
**	connection (vtable 0xB085F4: the {size, opcode, payload} framing over a byte transport, the TCP
**	connection's base). Not rebuilt yet: the wrappers (forwarding 0xB08590, peek 0xB0854C, the
**	logging and statistics ones), ConnectionManager and the factories (connect.cpp 0x821000-
**	0x8232E0). No class here has RTTI or a string naming it: the names are descriptive.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CONNECT_H
#define CONNECT_H

#include "circbuff.h"
#include <vector>

class PacketClass;
class RawPacketClass;

// The connection base class (constructor 0x820F90, destructor 0x820FE0, 0x08 bytes). The inline
// slots' bodies are the client's (emitted in client.cpp, 0x726AF0-0x726BF0).
class ConnectionClass	// NAME 1: describes what it is (the client's name not found)
{
public:
	ConnectionClass();												// 0x820F90
	virtual ~ConnectionClass();										// +0x00 0x820FB0 / 0x820FE0
	virtual bool Send_Message(RawPacketClass *packet) = 0;			// +0x04  // NAME 2: what it does
	virtual bool Send_Packet(PacketClass *packet) = 0;				// +0x08 (Build, then +0x04)  // NAME 2: what it does
	virtual PacketClass *Receive() = 0;								// +0x0C: the next whole message as a packet, or NULL  // NAME 2: what it does
	virtual unsigned int Get_Address() { return(0); }				// +0x10 0x726AF0 (UDP: the CordCon's address)  // NAME 2: what UDP's returns
	virtual bool Connect(bool wait) { return(true); }				// +0x14 0x726B10  // NAME 2: what UDP's does
	virtual bool Is_Connected() { return(true); }					// +0x18 0x726B30  // NAME 2: what UDP's does
	virtual bool Disconnect() { return(true); }						// +0x1C 0x726B50  // NAME 2: what UDP's does
	virtual bool Is_Dead() { return(false); }						// +0x20 0x726B70: closed, nothing left to read  // NAME 2: what the stream's does
	virtual void Set_Unknown04(bool on) { Unknown04 = on; }			// +0x24 0x726B90
	virtual bool Get_Unknown04() { return(Unknown04); }				// +0x28 0x726BB0 (UDP: false sends on fixed channel -4)
	virtual bool Get_Stats(float *a, float *b, float *c, float *d, float *e, float *f, int *g) { return(false); }	// +0x2C 0x726BD0  // NAME 2: what UDP's does
	virtual bool Unknown_30(int a, int b) { return(false); }		// +0x30 0x726BF0

	bool Unknown04;		// +0x04 (1 from the constructor)
};

// The stream connection (constructor 0x823320, 0x24 bytes): messages framed as {s16 size with the
// 4-byte header, s16 opcode, payload} on a byte transport (slots +0x34 / +0x38, the derived
// class's: TCP's RC4 over wnet's TCPCon), read through a 100,000-byte ring.
class StreamConnectionClass : public ConnectionClass	// NAME 1: describes what it is
{
public:
	StreamConnectionClass();										// 0x823320
	virtual ~StreamConnectionClass();								// +0x00 0x8233B0 / 0x8233E0
	virtual bool Send_Message(RawPacketClass *packet);				// +0x04 0x823570
	virtual bool Send_Packet(PacketClass *packet);					// +0x08 0x8234D0
	virtual PacketClass *Receive();									// +0x0C 0x8236D0
	virtual bool Is_Dead();											// +0x20 0x8238C0
	virtual int Transport_Write(const void *data, int len) = 0;		// +0x34: the bytes written  // NAME 2: what TCP's does
	virtual int Transport_Read(void *buffer, int len) = 0;			// +0x38: the bytes read  // NAME 2: what TCP's does

	bool Write_Message(RawPacketClass *packet);						// 0x823460 (inlined in +0x04 / +0x08)  // NAME 2: what it does
	bool Message_Ready();											// 0x8235E0 (inlined in Is_Dead): a whole message is in the ring  // NAME 2: what it does
	void Fill();													// 0x823840 (inlined): the transport into the ring  // NAME 2: what it does

	CircBuffClass Ring;		// +0x08 100,000 bytes  // NAME 2: what it is
	char *ReadBuffer;		// +0x20 0x1000 bytes: Transport_Read's  // NAME 2: what it holds
};

// A connection the manager looks ahead on: +0x34 gives the next message without taking it (Peek
// 0x8223D0, Mark 0x8225C0 call it; Get_Packet 0x822490 then takes it with +0x0C)
class PeekConnectionClass : public ConnectionClass	// NAME 1: describes what it is
{
public:
	virtual PacketClass *Peek() = 0;								// +0x34  // NAME 2: what it does
};

// The manager's listener (+0x24; the destructor 0x8220E0 stops it, 0x822200 asks its port)
class ConnectionListenerClass	// NAME 1: describes what it is
{
public:
	virtual ~ConnectionListenerClass() {}							// +0x00
	virtual bool Is_Listening() = 0;								// +0x04  // NAME 2: what it does
	virtual unsigned short Get_Port() = 0;							// +0x08  // NAME 2: what it does
	virtual void Unknown_0C() = 0;									// +0x0C
	virtual void Stop() = 0;										// +0x10  // NAME 2: what it does
};

// One managed connection (8 bytes): Mark sets the flag, Clear_Marks 0x822630 clears them all
struct ConnectionEntryClass	// NAME 1: describes what it is
{
	PeekConnectionClass *Connection;	// +0x00  // NAME 2: what it holds
	bool Marked;						// +0x04  // NAME 2: what it holds
	bool Unknown05;						// +0x05 (Set_Unknown05 0x822660)
};

// The connection manager (S: 'ConnectionManager: probing port %hd...'; vtable 0xB085D0, constructor
// 0x822070, 0x28 bytes): connections polled round robin, a listener (docs/kernel-map.md 4)
class ConnectionManagerClass	// NAME 1 [ConnectionManager]: the client's log string
{
public:
	ConnectionManagerClass();										// 0x822070
	virtual ~ConnectionManagerClass();								// +0x00 0x8220B0 / 0x8220E0
	virtual PacketClass *Peek(bool next);							// +0x04 0x8223D0  // NAME 2: what it does
	virtual PacketClass *Get_Packet(bool next);						// +0x08 0x822490  // NAME 2: what it does
	virtual PacketClass *Receive();									// +0x0C 0x822500  // NAME 2: what it does
	virtual int Broadcast(PacketClass *packet);						// +0x10 0x822550  // NAME 2: what it does
	virtual bool Mark(PacketClass *packet);							// +0x14 0x8225C0  // NAME 2: what it does
	virtual void Clear_Marks();										// +0x18 0x822630  // NAME 2: what it does

	unsigned short Get_Port();										// 0x822200: the listener's  // NAME 2: what it does
	void Set_Name(const char *name);								// 0x822220  // NAME 2: what it does
	void Set_Listener(ConnectionListenerClass *listener);			// 0x822290 (inlined in the destructor with 0)  // NAME 2: what it does
	bool Set_Unknown05(ConnectionClass *connection, bool on);		// 0x822660: returns the old value
	bool Remove_Connection(ConnectionClass *connection);			// 0x8226D0  // NAME 2: what it does

	std::vector<ConnectionEntryClass> Connections;	// +0x04  // NAME 2: what it holds
	int RoundRobin;						// +0x14 the last connection read  // NAME 2: what it holds
	PeekConnectionClass *Current;		// +0x18 the connection Get_Packet read  // NAME 2: what it holds
	bool Unknown1C;						// +0x1C
	char *Name;							// +0x20  // NAME 2: what it holds
	ConnectionListenerClass *Listener;	// +0x24  // NAME 2: what it holds
};

#endif
