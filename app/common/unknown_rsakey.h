/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (D10, D86): the RSA key class of tcpconn.cpp and cordmgr.cpp. Both use it
**	inline (cordmgr.obj keeps Load out of line at 0x85AFB0, tcpconn.obj Read_Number 0x82C260 and
**	Compute 0x82C330), so it lives in a header both include; no string names it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_RSAKEY_H  // NAME 1 [unknown_rsakey.h]: provisional, no evidence of the original header
#define UNKNOWN_RSAKEY_H

#include "int.h"
#include "wwfile.h"
#include <string.h>
#include <winsock.h>

// An RSA key (0x900 bytes, nine 2048-bit numbers): the TCP listener's key pair (+0x18, read from
// 'server_rsa_keyset.dat', an SSH1 private key file) and CordMgr's (+0x5C, loaded by 0x856D80); the
// client builds one from the server's public key in Key_Exchange 0x829180. Its destructor is empty (the client's unwind states count it).
#define SSH1_KEY_HEADER "SSH PRIVATE KEY FILE FORMAT 1.1\n"	// NAME 2: what it is

class RSAKeyClass	// NAME 2: what it holds
{
public:
	~RSAKeyClass() {}
	BigInt Encrypt(BigInt m) const { return(m.exp_b_mod_c(Exponent, Modulus)); }	// inlined in 0x829180  // NAME 2: what it does
	// m = c^d mod n by the Chinese remainder theorem (p, q, d mod (p-1), d mod (q-1) and the two
	// factors from Compute); inlined in Pump_Pending 0x82AA10, its callees out of line there (0x82C0B0-0x82C220)
	void Decrypt(BigInt &m, const BigInt &c) const	// NAME 2: what it does
	{
		BigInt quotient;
		BigInt cp;
		BigInt cq;
		BigInt::Unsigned_Divide(cp, quotient, c, Unknown300);
		BigInt::Unsigned_Divide(cq, quotient, c, Unknown400);
		BigInt mp;
		mp = cp.exp_b_mod_c(Unknown500, Unknown300);
		BigInt mq;
		mq = cq.exp_b_mod_c(Unknown600, Unknown400);
		BigInt tp;
		BigInt tq;
		XMP_Prepare_Modulus(Modulus, MAX_UNIT_PRECISION);
		XMP_Mod_Mult(tp, mp, Unknown700, MAX_UNIT_PRECISION);
		XMP_Mod_Mult(tq, mq, Unknown800, MAX_UNIT_PRECISION);
		XMP_Mod_Mult_Clear(MAX_UNIT_PRECISION);
		m = tp + tq;
		if (m >= Modulus) {
			m -= Modulus;
		}
	}

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

#endif
