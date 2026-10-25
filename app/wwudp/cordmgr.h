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
**	Only the data path is rebuilt (functional, NOT EXACT, D42): Pump's dispatch of types 4 / 5 and
**	Process_CORD_Packet. The handshake, Pump_Connections, Pump_Releases, Get_Data_Socket and
**	Compute_Ticks are not.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CORDMGR_H
#define CORDMGR_H

#include <vector>

class UDP;
class CordCon;

class CordMgr
{
public:
	CordMgr();
	~CordMgr();

	// Reads and dispatches every datagram waiting on the sockets. 0x857F20 (data path only)
	void Pump();
	// A data datagram (type 4 or 5) from ip:port: to the connection with its session id and
	// address. 0x85A000
	bool Process_CORD_Packet(unsigned char *data, int len, unsigned int ip, unsigned short port, int type);

	std::vector<UDP *> Sockets_;			// NAME 2 [Sockets_]: the client's two socket lists at +0x48 / +0x54
	std::vector<CordCon *> Connections_;	// NAME 2 [Connections_]: +0x34 count, +0x3C items
	int Unexpected_;						// NAME 2: datagrams Pump didn't take
};

#endif
