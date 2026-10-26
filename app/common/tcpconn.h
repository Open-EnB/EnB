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
**	Not rebuilt yet: the listener's half of the exchange (Send_Key 0x82A830, Pump_Pending 0x82AA10).
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

// An RSA key (0x900 bytes, nine 2048-bit numbers): the listener's key pair (+0x18, read from
// 'server_rsa_keyset.dat', an SSH1 private key file); the client builds one from the server's
// public key in Key_Exchange 0x829180. Its destructor is empty (the client's unwind states count it).
#define SSH1_KEY_HEADER "SSH PRIVATE KEY FILE FORMAT 1.1\n"	// NAME 2: what it is

class RSAKeyClass	// NAME 2: what it holds
{
public:
	~RSAKeyClass() {}
	BigInt Encrypt(BigInt m) const { return(m.exp_b_mod_c(Exponent, Modulus)); }	// inlined in 0x829180  // NAME 2: what it does

	// The key file: an SSH1 private key file ('SSH PRIVATE KEY FILE FORMAT 1.1\n', no cipher): n, e,
	// the comment, the check bytes, d, u (not kept), p, q. Inlined in the listener's constructor.
	bool Load(FileClass *file)	// NAME 2: what it does
	{
		if (file == 0 || !file->Open(1)) {
			return(false);
		}
		unsigned char buffer[0x400];
		file->Read(buffer, strlen(SSH1_KEY_HEADER) + 1);
		buffer[strlen(SSH1_KEY_HEADER)] = 0;
		if (strcmp((char *)buffer, SSH1_KEY_HEADER) != 0) {
			return(false);
		}
		char cipher;
		file->Read(&cipher, 1);
		if (cipher != 0) {
			return(false);
		}
		file->Read(buffer, 4);
		file->Read(buffer, 4);
		if (!Read_Number(file, Modulus)) {
			return(false);
		}
		if (!Read_Number(file, Exponent)) {
			return(false);
		}
		unsigned long length;
		file->Read(&length, 4);
		length = ntohl(length);
		file->Read(buffer, length);
		file->Read(buffer, 4);
		if (buffer[0] != buffer[2] || buffer[1] != buffer[3]) {
			return(false);
		}
		BigInt u;
		if (!Read_Number(file, Unknown200)) {
			return(false);
		}
		if (!Read_Number(file, u)) {
			return(false);
		}
		if (!Read_Number(file, Unknown300)) {
			return(false);
		}
		if (!Read_Number(file, Unknown400)) {
			return(false);
		}
		Compute();
		return(true);
	}

	// One SSH1 number: u16 bits, then the bytes, big-endian (0x82C260; ignores this)
	bool Read_Number(FileClass *file, BigInt &number)	// NAME 2: what it does
	{
		unsigned short bits;
		unsigned char buffer[0x400];
		if (file->Read(&bits, 2) != 2) {
			return(false);
		}
		bits = ntohs(bits);
		unsigned short bytes = (bits + 7) / 8;
		if (file->Read(buffer, bytes) != bytes) {
			return(false);
		}
		number.Unsigned_Decode(buffer, bytes);
		return(true);
	}

	// The private part's derived numbers (0x82C330): p > q, d mod (p - 1), d mod (q - 1), and
	// q^(p-1), p^(q-1) mod n
	void Compute()	// NAME 2: what it does
	{
		BigInt t;
		if (Unknown300 < Unknown400) {
			t = Unknown300;
			Unknown300 = Unknown400;
			Unknown400 = t;
		}
		BigInt p1 = Unknown300;
		--p1;
		BigInt q1 = Unknown400;
		--q1;
		BigInt::Unsigned_Divide(Unknown500, t, Unknown200, p1);
		BigInt::Unsigned_Divide(Unknown600, t, Unknown200, q1);
		Unknown700 = Unknown400.exp_b_mod_c(p1, Modulus);
		Unknown800 = Unknown300.exp_b_mod_c(q1, Modulus);
	}

	BigInt Modulus;			// +0x000 n  // NAME 2: what it holds
	BigInt Exponent;		// +0x100 e  // NAME 2: what it holds
	BigInt Unknown200;		// +0x200 the private part, the listener's (read from the key file)
	BigInt Unknown300;		// +0x300
	BigInt Unknown400;		// +0x400
	BigInt Unknown500;		// +0x500 (computed: 0x82C330)
	BigInt Unknown600;		// +0x600
	BigInt Unknown700;		// +0x700
	BigInt Unknown800;		// +0x800
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
	ConnectionClass *Pump_Pending();								// 0x82AA10 (not rebuilt): a connection whose exchange finished  // NAME 2: what it does
	void Send_Key(TCPConnection *connection);						// 0x82A830 (not rebuilt): the server's public key  // NAME 2: what it does
	static void Pump_All();										// 0x82A990 (no caller; inlined in 0x829180): Accept(false) on every listener  // NAME 2: what it does

	SOCKET Socket;							// +0x04 (S: the asserts)
	unsigned short PortNumber;				// +0x08 (S: the asserts)
	std::list<TCPConnection *> Pending;		// +0x0C  // NAME 2: what it holds
	RSAKeyClass KeyPair;					// +0x18 the server's key pair (constructor 0x829E50)  // NAME 2: what it holds
};

#endif
