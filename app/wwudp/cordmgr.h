/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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
**	handshake's ACK1 / ACK2, go-away, Pump_Connections, Pump_Releases, Get_Data_Socket and
**	Compute_Ticks are not.
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
#include <time.h>

struct CordDataSocketClass		// NAME 2: a data socket's record (the +0x40 list, 16 bytes)
{
	UDP *Socket;
	int Unknown04;		// +0x04 (Pump_Releases takes one off for each connection on the socket it frees)
	int Unknown08;
	int Unknown0C;
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
	unsigned long EncryptedKey[0x40];	// +0x34 NAME 2: the session key, RSA-encrypted (XMP digits)
	int Unknown134;					// +0x134
	__int64 Key;					// +0x138 NAME 2: the session key (RC4)
};

class CordMgr
{
public:
	CordMgr();
	~CordMgr();

	// Reads and dispatches every datagram waiting on the sockets. 0x857F20 (data path only)
	void Pump();
	CordDataSocketClass *Find_Data_Socket(UDP *socket);											// 0x859F80
	// A data datagram (type 4 or 5): to the connection with its session id and address. 0x85A000
	bool Process_CORD_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown,
		char type);
	bool Process_SYN1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85A3B0
	bool Process_SYN2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85AB30
	bool Process_ACK1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85A430
	bool Process_ACK2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown);	// 0x85ABB0
	// Starts a connection to ip:port on the first data socket (0x858EC0, reduced: no socket binding).
	CordConnectingClass *Connect(unsigned int ip, unsigned short port, bool encrypt);	// NAME 2: what it does
	// Sends SYN1 / SYN2 for the connections in the handshake (0x858A60, reduced: every call, no
	// resend timer or time-out).
	bool Pump_Connections();
	// A go-away (type 6) from the peer: the connection it names goes away. 0x8586D0
	bool Process_Go_Away_Packet(unsigned char *data, int len, sockaddr_in *from);
	// A 0-byte read (the peer's port unreachable): every connection to that address goes away.
	// 0x858980
	void Process_Zero_Read(sockaddr_in *from);	// NAME 2: what it does
	// Closes a connection and frees it 30 s later (Pump_Releases). 0x8596F0
	bool Release(CordCon *con);	// NAME 1: after Pump_Releases
	// Waits up to sec / usec for a datagram on any of the sockets (wnet's UDP::Wait on their set). 0x857DE0
	int Wait(int sec, int usec);	// NAME 2: what it does
	// The CordCon a handshake made, taken out of AllConnections_: a client's by its session id, or
	// (session 0) a server's by its port. 0x859440
	bool Find_Connection(CordCon **con, unsigned int port, int session);	// NAME 2: what it does
	// A listening socket on ip:port (0: any) for handshakes, and its removal (0x857940 / 0x857C00;
	// declared: not rebuilt)
	bool Listen(unsigned int ip, unsigned short port);		// NAME 2: what it does
	bool Unlisten(unsigned int ip, unsigned short port);	// NAME 2: what it does
	// Drops the connection with that session id: its CordCon (deleted), else its handshake record
	// (deleted, left in Pending_ as the client leaves it). 0x8595B0
	bool Cancel_Connect(int session);	// NAME 2: what it does
	// Frees the released connections whose time has come. 0x859940
	bool Pump_Releases();

	unsigned char Protocol_;							// +0x00 NAME 2: the handshake's second byte (0x14 in the captures)
	int Unknown04;										// +0x04
	unsigned short NextSession_;						// +0x08 NAME 2: the next session id (+0x1337 each time)
	int Unknown0C;										// +0x0C
	ArrayList<CordConnectingClass *> Pending_;		// +0x10 NAME 2: connections in the handshake
	ArrayList<CordReleaseClass> Releases_;			// NAME 2: +0x1C connections to free (Release, Pump_Releases)
	ArrayList<CordCon *> AllConnections_;			// +0x28 NAME 1: ACK2 adds the CordCon here too
	ArrayList<CordCon *> Connections_;				// +0x34 NAME 2: the open connections
	ArrayList<CordDataSocketClass> DataSockets_;	// +0x40 NAME 2: the sockets data arrives on
	ArrayList<CordListenSocketClass> ListenSockets_;	// +0x4C NAME 2: the sockets handshakes arrive on

	int Unexpected_;									// +0x58 (the harness's count of datagrams Pump didn't take, in the client's unknown +0x58)
	RSAKeyClass KeyPair;								// +0x5C the server's key pair (0x856D80 loads it: 0x85AFB0); 0x95C bytes in all  // NAME 2: what it holds
};

#endif
