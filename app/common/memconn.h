// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\memconn.h (assumed: a .h paired with memconn.cpp)
**
**	memconn.cpp's in-process transport (0x826480-0x827B10, docs/kernel-map.md 4): a pair of memory
**	connections share two pipes of messages, each delivered after its pipe's delay; a memory listener
**	registers itself by port in a process-wide map (0xBE9160), and the factory 0x821450 connects to
**	it through a stack object of MemoryConnectionFactoryClass. No class here has RTTI or a string
**	naming it: the names are descriptive.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef MEMCONN_H
#define MEMCONN_H	// NAME 3: a .h paired with the client's memconn.cpp (D24)

#include "connect.h"
#include <list>

// One message in a pipe (8 bytes): the raw message and the time it may be received
struct MemoryMessageClass	// NAME 1: describes what it is
{


	RawPacketClass *Packet;	// +0x00  // NAME 2: what it holds
	unsigned int Time;		// +0x04 (Unknown_Get_Ticks')  // NAME 2: what it holds
};

// One direction of a pair (0x14 bytes): its messages, its delay, and the connections using it
class MemoryPipeClass	// NAME 1: describes what it is
{
public:
	MemoryPipeClass() : Delay(0), RefCount(0) {}

	std::list<MemoryMessageClass> Messages;	// +0x00  // NAME 2: what it holds
	int Delay;								// +0x0C (ms)  // NAME 2: what it holds
	unsigned int RefCount;					// +0x10 (2 while both ends are open)  // NAME 2: what it holds
};

// The memory connection (vtable 0xB087D4, 0x10 bytes)
class MemoryConnectionClass : public ConnectionClass	// NAME 1: after the file memconn.cpp
{
public:
	MemoryConnectionClass();										// 0x826480
	virtual ~MemoryConnectionClass();								// +0x00 0x8264B0 / 0x8264E0
	virtual bool Send_Message(RawPacketClass *packet);				// +0x04 0x826550 (asserts: not supported)
	virtual bool Send_Packet(PacketClass *vp);						// +0x08 0x826580
	virtual PacketClass *Receive();									// +0x0C 0x8266A0
	virtual bool Is_Connected();									// +0x18 0x826760: the other end still open
	virtual bool Disconnect();										// +0x1C 0x8267C0
	virtual bool Is_Dead();											// +0x20 0x826790
	virtual bool Unknown_30(int option, int *data);					// +0x30 0x826D00: 0 / 1 get, 2 / 3 set the send / receive delay

	void Release();													// 0x826870: leaves both pipes  // NAME 2: what it does
	void Set_Send_Delay(int delay);									// 0x826C60  // NAME 2: what it does
	void Set_Recv_Delay(int delay);									// 0x826CB0  // NAME 2: what it does
	void Clear_Pipe(MemoryPipeClass *pipe);					// 0x8267E0: frees its messages (no caller)  // NAME 2: what it does
	static void Create_Pair(MemoryConnectionClass *&c1, MemoryConnectionClass *&c2);	// 0x826A50  // NAME 2: what it does

	MemoryPipeClass *SendPipe;	// +0x08 (S: the asserts)
	MemoryPipeClass *RecvPipe;	// +0x0C (S: the asserts)
	int Unknown10;				// +0x10 (not set: Create_Pair's new is 0x14 bytes)
};

// What a memory listener offers the factory (vtable 0xB08834: one slot)
class MemoryServerClass	// NAME 1: describes what it is
{
public:
	virtual ConnectionClass *Connect() = 0;							// +0x00: the client's end of a new pair  // NAME 2: what it does
};

// The memory listener (vtables 0xB08818 / 0xB08814, constructor 0x826EB0, 0x18 bytes)
class MemoryListenerClass : public ConnectionListenerClass, public MemoryServerClass	// NAME 1: describes what it is
{
public:
	MemoryListenerClass();											// 0x826EB0
	virtual ~MemoryListenerClass();									// +0x00 0x826F90 / 0x826FC0
	virtual bool Is_Listening();									// +0x04 0x826F70
	virtual unsigned short Get_Port();								// +0x08 0x826F50
	virtual bool Listen(unsigned short port);						// +0x0C 0x8272A0
	virtual bool Stop();											// +0x10 0x827340
	virtual ConnectionClass *Accept();								// +0x14 0x827250
	virtual ConnectionClass *Connect();								// MemoryServerClass +0x00 0x8271B0

	void Clear_Pending();											// 0x827130  // NAME 2: what it does

	std::list<ConnectionClass *> Pending;	// +0x08 the server's ends not accepted yet  // NAME 2: what it holds
	unsigned short PortNumber;				// +0x14 (S: the asserts)
};

// The registry of memory listeners by port (0xBE9160), through a stack object (no data of its own)
class MemoryConnectionFactoryClass	// NAME 1: describes what it is
{
public:
	bool Register(unsigned short port, MemoryServerClass *server);	// 0x827700  // NAME 2: what it does
	bool Is_Listening(unsigned short port);							// 0x8277C0  // NAME 2: what it does
	bool Unregister(unsigned short port);							// 0x827840  // NAME 2: what it does
	ConnectionClass *Connect(unsigned short port);					// 0x827A80  // NAME 2: what it does
};

#endif
