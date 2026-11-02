// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordmgr.h (name assumed from cordmgr.cpp)
**
**	CordMgr: the EA-UDP connection manager (client 0x857590-0x85AED0). It owns the UDP sockets,
**	reads every datagram and dispatches on its first byte (Pump 0x857F20): 0 SYN1 0x85A3B0,
**	1 ACK1 0x85A430 (the server's RSA key: XMP), 2 SYN2 0x85AB30, 3 ACK2 0x85ABB0 (makes the
**	CordCon), 4 / 5 CORD data 0x85A000 (to CordCon::handlePacket), 6 go-away 0x8586D0; anything
**	else is logged ('Unexpected packet: ...') and answered with a 10-byte type-6 datagram.
**	Rebuilt: Pump's data path (functional), Process_CORD_Packet, Process_SYN1_Packet,
**	Process_SYN2_Packet (the client's take nothing: the server's side), Find_Data_Socket. The
**	handshake's ACK1 / ACK2, go-away, Pump_Connections, Pump_Releases and Get_Data_Socket are not.
**
**	Layout (the members the rebuilt code uses, at the client's offsets; the lists are WWOnline's
**	wlib ArrayList): +0x10 Pending_ (connections in the handshake;
**	Pump: 'We must have dropped an ack2 packet to the client!'), +0x34 Connections_, +0x40
**	DataSockets_ (16-byte records), +0x4C ListenSockets_ (12-byte records).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CORDMGR_H  // NAME 3 [cordmgr.h]: header name assumed from the client's cordmgr.cpp
#define CORDMGR_H

#include "cordcon.h"
#include "udp.h"
#include "..\common\unknown_rsakey.h"
#include "ffactory.h"		// file_auto_ptr (Init's key file)
#include <time.h>

// 0x8B4A90 (WWOnline's library, cdecl; not in the tree): SHA-1 of len bytes into digest[5]. A program that
// links CordMgr defines it (tests/udpreplay does).
void Cord_SHA(const void *data, int len, unsigned int *digest);	// NAME 1: placeholder

struct CordDataSocketClass		// NAME 2: a data socket's record (the +0x40 list, 16 bytes)
{
	UDP *Socket;
	int Unknown04;		// +0x04 (Pump_Releases takes one off for each connection on the socket it frees)
	int Unknown08;
	bool Unknown0C;		// +0x0C a byte (Connect takes no socket that has it set; Connect's new record copies the whole dword)
};

struct CordListenSocketClass	// NAME 2: a listening socket's record (the +0x4C list, 12 bytes)
{
	UDP *Socket;
	int Unknown04;
	int Unknown08;
};

// A released connection and when to free it (Release 0x8596F0: now + 30 s).
struct CordReleaseClass		// NAME 2: describes what it holds (the +0x1C list, 8 bytes)
{
	CordCon *Con;
	unsigned int Time;		// time_t
};

// A connection in the handshake (0x140 bytes; made by Connect 0x858EC0, kept in CordMgr's +0x10
// list until ACK2 makes its CordCon).
class CordConnectingClass	// NAME 2: describes what it is
{
public:
	unsigned int Ip;				// +0x00 NAME 2: the server's address (host order)
	unsigned short Port;			// +0x04 NAME 2: the server's handshake port
	int Session;					// +0x08 NAME 2: the session id SYN1 carries
	unsigned int Ack1Word1;			// +0x0C NAME 2: ACK1's bytes 4-7, sent back in SYN2
	unsigned int Ack1Word2;			// +0x10 NAME 2: ACK1's bytes 8-11, sent back in SYN2
	Xtime Created;					// +0x14 NAME 2: Connect's time; 60 s later Pump_Connections gives up
	Xtime LastSend;					// +0x1C NAME 2: SYN1 / SYN2 go again 1 s after it
	int Unknown24;					// +0x24
	UDP *Socket;					// +0x28 NAME 2: the data socket it was made on
	CordDataSocketClass *Record;	// +0x2C NAME 2: that socket's record
	int State;						// +0x30 NAME 2: 1 SYN1 sent, 3 SYN2 sent, 5 done
	BigInt EncryptedKey;			// +0x34 NAME 2: the session key, RSA-encrypted (WWLib's Int: its inline constructor is Connect's XMP_Init(+0x34, 0, 0x40))
	int Unknown134;					// +0x134
	__int64 Key;					// +0x138 NAME 2: the session key (RC4)
};

class CordMgr
{
public:
	// Zeroes the members, then Init (inlined). 0x856970
	CordMgr();
	// The same, then Listen(ip, port) (both inlined). 0x856D80 (no caller)
	CordMgr(unsigned int ip, unsigned short port);
	~CordMgr();
	// A new random word (+0x04) and session counter (random ^ time ^ 0x2121), and the server's key
	// pair loaded again from 'server_rsa_keyset.dat'. 0x8570C0 (no caller; inlined in the constructors)
	void Init();	// NAME 2: what it does

	// Reads and dispatches every datagram waiting on the sockets. 0x857F20 (data path only)
	bool Pump();
	CordDataSocketClass *Find_Data_Socket(UDP *socket);											// 0x859F80
	// A data datagram (type 4 or 5): to the connection with its session id and address. 0x85A000
	bool Process_CORD_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown,
		char type);
	bool Process_SYN1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85A3B0
	bool Process_SYN2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85AB30
	bool Process_ACK1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85A430
	bool Process_ACK2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85ABB0
	// Starts a connection to ip:port on a data socket with room (else a new one bound to 3755-3759, else any port)
	// and returns its session id. 0x858EC0
	int Connect(unsigned int ip, unsigned short port, bool encrypt);	// NAME 2: what it does
	// Sends SYN1 / SYN2 for the connections in the handshake (0x858A60, reduced: every call, no
	// resend timer or time-out).
	bool Pump_Connections();
	// A go-away (type 6) from the peer: the connection it names goes away. 0x8586D0
	// The five arguments of the other Process_* (ret 0x14); socket and unknown unused.
	bool Process_Go_Away_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);
	// Sends a go-away (type 6) to ip:port on udp, answering a datagram of that type and session. 0x858640
	// (no caller; Pump has it inlined)
	void Send_Go_Away(UDP *udp, unsigned int ip, unsigned short port, unsigned char type, unsigned short session);	// NAME 2: what it sends
	// A 0-byte read (the peer's port unreachable): every connection to that address goes away.
	// 0x858980
	void Process_Zero_Read(sockaddr_in *from);	// NAME 2: what it does
	// Closes a connection and frees it 30 s later (Pump_Releases). 0x8596F0
	bool Release(CordCon *con);	// NAME 1: after Pump_Releases
	// CordCon::Pending_Messages summed over Connections_ (ClientTask::Run waits for 0). 0x8598F0
	int Pending_Messages();	// NAME 2: what it counts
	// Waits up to sec / usec for a datagram on any of the sockets (wnet's UDP::Wait on their set). 0x857DE0
	int Wait(int sec, int usec);	// NAME 2: what it does
	// The CordCon a handshake made, taken out of AllConnections_: a client's by its session id, or
	// (session 0) a server's by its port. 0x859440
	bool Find_Connection(CordCon **con, unsigned int port, int session);	// NAME 2: what it does
	// A listening socket on ip:port (0: any) for handshakes (32 KB buffers), and its removal
	// (0x857940 / 0x857C00)
	bool Listen(unsigned int ip, unsigned short port);		// NAME 2: what it does
	bool Unlisten(unsigned int ip, unsigned short port);	// NAME 2: what it does
	// udp's receive / send buffers for MaxPerSocket_ connections (8 when 0) of 16 KB, the limit lowered
	// until both can be had (Connect's loop; no caller). 0x8578C0
	void Set_Buffers(UDP *udp);	// NAME 2: what it does
	// Drops the connection with that session id: its CordCon (deleted), else its handshake record
	// (deleted, left in Pending_ as the client leaves it). 0x8595B0
	bool Cancel_Connect(int session);	// NAME 2: what it does
	// Frees the released connections whose time has come. 0x859940
	bool Pump_Releases();
	// A timed-out handshake's CordCon into both lists, its record freed and taken out (inlined in Pump_Connections, as
	// one block: ACK2 0x85ABB0 has the same code inline but matches better without it)
	void Add_Connection(CordCon *con, CordConnectingClass *c, int i) { AllConnections_.addTail(con); Connections_.addTail(con); delete c; Pending_.remove(i); }	// NAME 2: what it does
	// A hash of the six values and Unknown04 (WWOnline's SHA-1, the digest's words xored). 0x85A220 (no caller)
	unsigned int Compute_Ticks(unsigned short port1, unsigned int ip1, unsigned int ip2, unsigned short port2,
		unsigned int ip3, unsigned short port3);	// NAME 2: the parameters (the name is the client's profile string)

	unsigned char Protocol_;							// +0x00 NAME 2: the handshake's second byte (0x14 in the captures)
	int Unknown04;										// +0x04 (Init: a random word)
	unsigned short NextSession_;						// +0x08 NAME 2: the next session id (+0x1337 each time)
	int Unknown0C;										// +0x0C
	ArrayList<CordConnectingClass *> Pending_;		// +0x10 NAME 2: connections in the handshake
	ArrayList<CordReleaseClass> Releases_;			// NAME 2: +0x1C connections to free (Release, Pump_Releases)
	ArrayList<CordCon *> AllConnections_;			// +0x28 NAME 1: ACK2 adds the CordCon here too
	ArrayList<CordCon *> Connections_;				// +0x34 NAME 2: the open connections
	ArrayList<CordDataSocketClass> DataSockets_;	// +0x40 NAME 2: the sockets data arrives on
	ArrayList<CordListenSocketClass> ListenSockets_;	// +0x4C NAME 2: the sockets handshakes arrive on

	int MaxPerSocket_;									// +0x58 NAME 2: the connections one data socket takes (Connect: 8 at first, one less while the socket buffers of MaxPerSocket_ << 14 bytes can't be had)
	RSAKeyClass KeyPair;								// +0x5C the server's key pair (0x856D80 loads it: 0x85AFB0); 0x95C bytes in all  // NAME 2: what it holds
};

#endif
