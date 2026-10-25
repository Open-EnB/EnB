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
**	wlib ArrayList, CordArrayClass in cordcon.h): +0x10 Pending_ (connections in the handshake;
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

struct CordDataSocketClass		// NAME 2: a data socket's record (the +0x40 list, 16 bytes)
{
	UDP *Socket;
	int Unknown04;
	int Unknown08;
	int Unknown0C;
};

struct CordListenSocketClass	// NAME 2: a listening socket's record (the +0x4C list, 12 bytes)
{
	UDP *Socket;
	int Unknown04;
	int Unknown08;
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

	int Unknown00[4];									// +0x00
	CordArrayClass<CordCon *> Pending_;					// +0x10 NAME 2: connections in the handshake
	int Unknown1C[6];									// +0x1C
	CordArrayClass<CordCon *> Connections_;				// +0x34 NAME 2: the open connections
	CordArrayClass<CordDataSocketClass> DataSockets_;	// +0x40 NAME 2: the sockets data arrives on
	CordArrayClass<CordListenSocketClass> ListenSockets_;	// +0x4C NAME 2: the sockets handshakes arrive on

	int Unexpected_;									// (the harness's count of datagrams Pump didn't take)
};

#endif
