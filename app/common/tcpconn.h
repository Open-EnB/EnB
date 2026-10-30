// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\tcpconn.cpp's header (the client's asserts name
**	the .cpp, l.119-731).
**
**	PARTIAL (docs/kernel-map.md 4, step 8): the TCP connection (vtable 0xB08838, 0x248 bytes), a
**	stream connection over WWOnline wnet's TCPCon with an RC4 stream each way, keyed by an RSA
**	exchange at connect time. The members are at the client's offsets (constructor 0x828DE0).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TCPCONN_H  // NAME 3 [tcpconn.h]: header name assumed from the client's tcpconn.cpp
#define TCPCONN_H

#include "connect.h"
#include "rc4.h"
#include "int.h"
#include "srandom.h"
#include "unknown_rsakey.h"
#include "wwfile.h"
#include "ffactory.h"
#include <string.h>
#include <set>
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
	bool Key_Exchange(bool wait, int *error);						// 0x829180: the RSA exchange, 'CONNECT RSA EXCH FAILED: %d : %d'  // NAME 2: what it does
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

// The server's TCP listener (vtable 0xB08880, constructor 0x829E50, 0x918 bytes): a listening socket,
// the accepted connections still in the RSA key exchange, the server's key pair
class TCPListenerClass : public ConnectionListenerClass	// NAME 1: after the file tcpconn.cpp
{
public:
	TCPListenerClass();												// 0x829E50: loads 'server_rsa_keyset.dat'
	virtual ~TCPListenerClass();									// +0x00 0x82A2C0 / 0x82A2F0 (assert line 645)
	virtual bool Is_Listening();									// +0x04 0x82A280
	virtual unsigned short Get_Port();								// +0x08 0x82A2A0
	virtual bool Listen(unsigned short port);						// +0x0C 0x82A470
	virtual bool Stop();											// +0x10 0x82A650
	virtual ConnectionClass *Accept();								// +0x14 0x82A6A0: Accept(true)
	ConnectionClass *Accept(bool pump);								// 0x82A6C0  // NAME 2: what it does
	ConnectionClass *Pump_Pending();								// 0x82AA10: a connection whose exchange finished  // NAME 2: what it does
	void Send_Key(TCPConnection *connection);						// 0x82A830: the server's public key  // NAME 2: what it does
	static void Pump_All();										// 0x82A990 (no caller; inlined in 0x829180): Accept(false) on every listener  // NAME 2: what it does

	SOCKET Socket;							// +0x04 (S: the asserts)
	unsigned short PortNumber;				// +0x08 (S: the asserts)
	std::list<TCPConnection *> Pending;		// +0x0C  // NAME 2: what it holds
	RSAKeyClass KeyPair;					// +0x18 the server's key pair (constructor 0x829E50)  // NAME 2: what it holds
};

#endif
