/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\udpconn.h (name assumed from udpconn.cpp)
**
**	The game's connection over EA-UDP (docs/kernel-map.md 4): UDPConnection (vtable 0xB088F4,
**	0xD0 bytes) sends packets on a CordCon's channels and receives whole messages from it, each
**	{s16 size, s16 opcode, payload} as CordCon::read delivers it; the one CordMgr of the process
**	is pumped by a kernel task every 100 ms ('CordMgr::Run'). PARTIAL: UDPConnectionManager
**	(0xB08934) and the UDP listener (0xB08958), the server's side, are not rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UDPCONN_H  // NAME 3 [udpconn.h]: header name assumed from the client's udpconn.cpp
#define UDPCONN_H

#include "connect.h"
#include "..\..\libs\wwonline\wlib\dictionary.h"
#include "unknown_rate.h"
#include "..\kernel\kernel.h"

class CordMgr;
class CordCon;
class CordConnectingClass;

#define UDP_DATABUFFER_SIZE	0x8000	// NAME 5: the assert 'packet->Header.Size > UDP_DATABUFFER_SIZE'

// A hashed channel's key (8 bytes): the packet's channel and its mode. Hash 0x82C5E0 (channel +
// reliable), compare 0x82D880.
struct UDPChannelKeyClass	// NAME 1: describes what it is
{
	int Channel;			// NAME 2: what it holds
	bool Reliable;			// NAME 2: 3 reliable, 4 sequenced

	bool operator == (const UDPChannelKeyClass &other) const;	// 0x82D880
};

// A hashed channel (the dictionary's value, 8 bytes with the node's next at +0x0C).
struct UDPChannelClass		// NAME 1: describes what it is
{
	unsigned int Channel;	// NAME 2: the CordCon channel openc gave
};

// The connection (0xD0 bytes; constructors 0x82C680 by name, 0x82C980 by address, 0x82CBF0 over
// an accepted CordCon).
class UDPConnection : public ConnectionClass	// NAME 1: after the file udpconn.cpp (no RTTI or string)
{
public:
	UDPConnection(const char *host, unsigned short port, bool encrypt);	// 0x82C680 (declared: not rebuilt)
	UDPConnection(unsigned int ip, unsigned short port, bool encrypt);	// 0x82C980 (host order)
	UDPConnection(CordCon *con, CordMgr *mgr);							// 0x82CBF0
	virtual ~UDPConnection();											// +0x00 0x82C950 / 0x82CDC0
	virtual bool Send_Message(RawPacketClass *packet);					// +0x04 0x82D980
	virtual bool Send_Packet(PacketClass *packet);						// +0x08 0x82DA20
	virtual PacketClass *Receive();										// +0x0C 0x82DB30
	virtual unsigned int Get_Address();									// +0x10 0x82D190: the CordCon's address
	virtual bool Connect(bool wait);									// +0x14 0x82D1B0
	virtual bool Is_Connected();										// +0x18 0x82D320
	virtual bool Disconnect();											// +0x1C 0x82D3B0
	virtual bool Is_Dead();												// +0x20 0x82D390
	virtual bool Get_Stats(float *out_rate, float *in_rate, float *round_trip, float *window, float *in_flight, float *quality, int *idle);	// +0x2C 0x82D060

	bool Try_Connect();													// 0x82D240: one step of the handshake  // NAME 2: what it does
	// The send core: Buffer's message (len bytes) on the channel channel names (0x82D420).
	bool Send_Data(int len, int channel, int priority, bool reliable, bool close);	// NAME 2: what it does
	void Open_Fixed_Channel(int channel);								// 0x82D8C0 (channel -2 .. -15)  // NAME 2: what it does
	bool Is_Fixed_Channel(unsigned int channel);						// 0x82D940: the default or one of the 16  // NAME 2: what it does

	Dictionary<UDPChannelKeyClass, UDPChannelClass> Channels;	// +0x08 NAME 2: the hashed channels
	Unknown_RateClass InRate;		// +0x40 NAME 2: the bytes in (Receive's samples)
	Unknown_RateClass OutRate;		// +0x54 NAME 2: the bytes out

	int LastOut;					// +0x68 NAME 2: the CordCon's BytesSent_ at the last sample
	int LastIn;						// +0x6C NAME 2: the CordCon's BytesRead_ at the last sample
	bool Encrypt;					// +0x70 NAME 2: CordMgr::Connect's third argument
	CordMgr *Mgr;					// +0x74 NAME 2: what it holds
	CordCon *Con;					// +0x78 NAME 2: what it holds
	int Pending;					// +0x7C NAME 2: the session id of the handshake in progress (0: none)
	unsigned int Fixed[16];			// +0x80 NAME 2: channels -1 .. -16 (-1: Default)
	unsigned int Default;			// +0xC0 NAME 2: the default channel (openc mode 3)
	char *Name;						// +0xC4 NAME 2: the host name ('unknown')
	unsigned int Address;			// +0xC8 NAME 2: network order
	unsigned short Port;			// +0xCC NAME 2: network order
};

// The manager of a UDP server's connections (vtable 0xB08934, constructor 0x82E040): the CordMgr's
// task made on first use
class UDPConnectionManagerClass : public ConnectionManagerClass	// NAME 1: after the file udpconn.cpp
{
public:
	UDPConnectionManagerClass();									// 0x82E040
	virtual ~UDPConnectionManagerClass();							// +0x00 0x82E180 / 0x82E1B0
};

// The UDP listener (vtable 0xB08958, constructor 0x82E200, 0x0C bytes): handshakes on a port
class UDPListenerClass : public ConnectionListenerClass	// NAME 1: after the file udpconn.cpp
{
public:
	UDPListenerClass();												// 0x82E200
	virtual ~UDPListenerClass();									// +0x00 0x82E360 / 0x82E390
	virtual bool Is_Listening();									// +0x04 0x82E320
	virtual unsigned short Get_Port();								// +0x08 0x82E340
	virtual bool Listen(unsigned short port);						// +0x0C 0x82E470
	virtual bool Stop();											// +0x10 0x82E530
	virtual ConnectionClass *Accept();								// +0x14 0x82E5B0

	CordMgr *Mgr;					// +0x04  // NAME 2: what it holds
	unsigned short PortNumber;		// +0x08 (S: the asserts)
};

// The task that pumps the CordMgr (12 bytes, vtable 0xB0889C; constructor 0x82DCE0).
class CordMgrTaskClass : public TaskClass	// NAME 1: after its profile label 'CordMgr::Run'
{
public:
	CordMgrTaskClass();					// 0x82DCE0: makes the CordMgr
	virtual ~CordMgrTaskClass();		// +0x04 0x82DD70 / 0x82DDA0: deletes it
	virtual void Run();					// +0x30 0x82DF90 ('CordMgr::Run')
	virtual int Get_Interval();			// +0x34 0x82E020 (100 ms)
	virtual bool Delete_On_Remove();	// +0x44 0x82DE30 (false)

	static CordMgrTaskClass *Init();	// 0x82DE50: the task (added to the kernel) and the CordMgr, once  // NAME 2: what it does
	static CordMgr *Get_Mgr();			// 0x82DF40  // NAME 2: what it returns
	static void Shutdown_Task();		// 0x82DF60: deletes the task  // NAME 2: what it does

	static CordMgrTaskClass *Task;		// 0xBF16C4  // NAME 2: what it holds
	static CordMgr *Mgr;				// 0xBF16C0  // NAME 2: what it holds
};

#endif
