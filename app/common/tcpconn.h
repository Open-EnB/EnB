/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\tcpconn.cpp's header (the client's asserts name
**	the .cpp, l.119-731).
**
**	PARTIAL (docs/kernel-map.md 4, step 8): the TCP connection (vtable 0xB08838, 0x248 bytes), a
**	stream connection over WWOnline wnet's TCPCon with an RC4 stream each way, keyed by an RSA
**	exchange at connect time. The members are at the client's offsets (constructor 0x828DE0).
**	Not rebuilt yet: the key exchange 0x829180, the listener and its key file (0x829E50-0x82AA10).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TCPCONN_H  // NAME 3 [tcpconn.h]: header name assumed from the client's tcpconn.cpp
#define TCPCONN_H

#include "connect.h"
#include "rc4.h"
#include "..\..\libs\wwonline\wnet\tcpmgr.h"
#include "..\..\libs\wwonline\wnet\tcpcon.h"

class TCPConnection : public StreamConnectionClass	// NAME 1: after the file tcpconn.cpp (no RTTI or string)
{
public:
	TCPConnection(const char *host, unsigned short port, bool encrypt);	// 0x828C80
	TCPConnection(SOCKET socket);										// 0x829C80: over an accepted socket, encrypted
	TCPConnection(unsigned int ip, unsigned short port, bool encrypt);	// 0x828DE0
	virtual ~TCPConnection();										// +0x00 0x828DB0 / 0x828EC0
	virtual unsigned int Get_Address();								// +0x10 0x8299D0: the peer's address
	virtual bool Connect(bool wait);								// +0x14 0x828F90
	virtual bool Is_Connected();									// +0x18 0x829930
	virtual bool Disconnect();										// +0x1C 0x829960
	virtual int Transport_Write(const void *data, int len);			// +0x34 0x829A30: RC4, then TCPCon::write
	virtual int Transport_Read(void *buffer, int len);				// +0x38 0x829BC0: TCPCon::read, then RC4

	bool Try_Connect();												// 0x8290A0: Connect without waiting  // NAME 2: what it does
	bool Key_Exchange(bool wait, int unknown);						// 0x829180 (not rebuilt): the RSA exchange, 'CONNECT RSA EXCH FAILED: %d : %d'  // NAME 2: what it does
	void Close();													// 0x829A10: Disconnect through the vtable  // NAME 1: describes what it does

	TCPCon *Con;					// +0x24 NAME 4: as Renegade's TCPMgr calls a TCPCon (getOutgoingConnection's conn)
	uint32 Handle;					// +0x28 NAME 4: TCPMgr::connect's handle, while the connect is pending
	bool Encrypt;					// +0x2C (the constructors' third argument)  // NAME 2: what it holds
	__int64 Key;					// +0x30 the session key; 0 sends in the clear  // NAME 2: what it holds
	RC4Class SendRC4;				// +0x38  // NAME 2: what it does
	RC4Class ReceiveRC4;			// +0x13A  // NAME 2: what it does
	char *Name;						// +0x23C the host name ("unknown" by address)  // NAME 2: what it holds
	unsigned int IP;				// +0x240 network order  // NAME 2: what it holds
	unsigned short Port;			// +0x244 network order  // NAME 2: what it holds
};

#endif
