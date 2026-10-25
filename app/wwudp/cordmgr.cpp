/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordmgr.cpp (client 0x857590-0x85AED0)
**
**	Process_CORD_Packet, Process_SYN1_Packet, Process_SYN2_Packet and Find_Data_Socket follow the
**	client's code; Pump is functional (NOT EXACT, D42: the data path). The client's log calls
**	(E&B's log 0x4BC130 with __FILE__ / __LINE__, category "UDP") sit on their lines, 592 and 1272
**	(D25): the blank stretches below hold the place of the functions not rebuilt. Notes at the end.
*/

// E&B built its own code with WWLib's profiler on (WWProfileManager::Start_Profile 0x9EA930).
#define ENABLE_WWPROFILE
#include "cordmgr.h"
#include "wwprofile.h"
#include "mpmath.h"
#include "srandom.h"
#include <string.h>

// E&B's log (the object 0xBDA908; 0x4BC130 takes a category, the file and line, a format). Not
// rebuilt: a program that links CordMgr defines it (tests/capharness/udpreplay does).
class UnknownLogClass		// NAME 1 [UnknownLogClass]: placeholder for E&B's log object
{
public:
	void Unknown_4BC130(const char *category, const char *file, int line, const char *format, ...);	// NAME 1
};
extern UnknownLogClass UnknownLog;	// NAME 1 [UnknownLog]: 0xBDA908

// 0x8B3EC0 (WWOnline's library, not app\wwudp; here until that library is in the tree, so that
// CordCon calls it as the client does instead of inlining it): CRC-32 with the reflected table of 0xEDB88320,
// built on first use, no final inversion.
unsigned int Cord_CRC(const unsigned char *data, int len, unsigned int crc)	// NAME 1: placeholder
{
	static unsigned int table[256];
	static bool made = false;
	if (!made) {
		for (unsigned int i = 0; i < 256; i++) {
			unsigned int c = i;
			for (int k = 8; k != 0; k--) {
				c = (c & 1) ? (c >> 1) ^ 0xEDB88320 : c >> 1;
			}
			table[i] = c;
		}
		made = true;
	}
	for (int i = 0; i < len; i++) {
		crc = (crc >> 8) ^ table[(data[i] ^ crc) & 0xFF];
	}
	return crc;
}

CordMgr::CordMgr() :
	Protocol_(0x14),
	Unknown04(0),
	NextSession_(0),
	Unknown0C(0),
	Unexpected_(0)
{
	// Releases_ (+0x1C) starts empty
}

CordMgr::~CordMgr()
{
	for (int i = 0; i < Pending_.Length(); i++) {
		delete Pending_[i];
	}
}

// 0x858EC0, reduced: the client binds a data socket (ports 3755-3759) if none has room; here the
// first data socket. The session id is the manager's counter (+0x1337 each time), the key two
// random words (SecureRandomClass) when the connection is encrypted.
CordConnectingClass *CordMgr::Connect(unsigned int ip, unsigned short port, bool encrypt)
{
	CordDataSocketClass *record = DataSockets_.Get_Pointer(0);
	CordConnectingClass *c = new CordConnectingClass;
	memset(c, 0, sizeof(*c));
	c->Ip = ip;
	c->Port = port;
	c->Session = NextSession_;
	NextSession_ += 0x1337;
	c->Record = record;
	c->Socket = record ? record->Socket : NULL;
	c->State = 1;
	if (encrypt) {
		SecureRandomClass random;
		c->Key = (__int64)random.Randval() << 32;
		c->Key ^= random.Randval();
	}
	Pending_.Add(c);
	return c;
}

// 0x858A60, reduced: SYN1 (0, protocol, session) while waiting for ACK1; SYN2 (2, protocol,
// session, ACK1's two words, 0x4000, the key's length, the encrypted key) while waiting for ACK2.
bool CordMgr::Pump_Connections()
{
	WWPROFILE("CordMgr::Pump_Connections");
	unsigned char buf[0x400];
	for (int i = 0; i < Pending_.Length(); i++) {
		CordConnectingClass *c = Pending_[i];
		if (c->Socket == NULL) {
			continue;
		}
		if (c->State == 1) {
			buf[0] = 0;
			buf[1] = Protocol_;
			*(unsigned short *)(buf + 2) = htons((unsigned short)c->Session);
			c->Socket->Write(buf, 4, c->Ip, c->Port);
		} else if (c->State == 3) {
			unsigned char key[0x200];
			buf[0] = 2;
			buf[1] = Protocol_;
			*(unsigned short *)(buf + 2) = htons((unsigned short)c->Session);
			*(unsigned long *)(buf + 4) = htonl(c->Ack1Word1);
			*(unsigned long *)(buf + 8) = htonl(c->Ack1Word2);
			*(unsigned long *)(buf + 12) = htonl(0x4000);
			unsigned int n = XMP_Encode(key, c->EncryptedKey, 0x40);
			*(unsigned long *)(buf + 16) = htonl(n);
			memcpy(buf + 20, key, n);
			c->Socket->Write(buf, n + 20, c->Ip, c->Port);
		}
	}
	return true;
}

// 0x857F20, functional (NOT EXACT): the data sockets, then the listening ones, each until Read
// says there is nothing more (< 0); 0x400-byte buffer. The data sockets take ACK1 / ACK2 / the
// data / go-away, the listening ones SYN1 / SYN2 / ACK1 / ACK2 / go-away. Not rebuilt: ACK1, ACK2,
// go-away, a 0-byte read (0x858980), the type-6 answer to an unexpected datagram.
void CordMgr::Pump()
{
	WWPROFILE("CordMgr::Pump");
	unsigned char buf[0x400];
	int total = DataSockets_.Length() + ListenSockets_.Length();
	for (int i = 0; i < total; i++) {
		bool listen = i >= DataSockets_.Length();
		UDP *udp = listen ? ListenSockets_.Get_Pointer(i - DataSockets_.Length())->Socket
			: DataSockets_.Get_Pointer(i)->Socket;
		sockaddr_in from;
		int len;
		while ((len = udp->Read(buf, sizeof(buf), &from)) >= 0) {
			if (len == 0) {
				Process_Zero_Read(&from);
				continue;
			}
			if (len <= 3) {
				continue;
			}
			char type = (char)buf[0];
			bool taken = false;
			if ((type == 4 || type == 5) && !listen) {
				taken = Process_CORD_Packet(buf, len, &from, udp, 0, type);
			} else if (type == 1) {
				taken = Process_ACK1_Packet(buf, len, &from, udp, 0);
			} else if (type == 3) {
				taken = Process_ACK2_Packet(buf, len, &from, udp, 0);
			} else if (type == 0 && listen) {
				taken = Process_SYN1_Packet(buf, len, &from, udp, 0);
			} else if (type == 2 && listen) {
				taken = Process_SYN2_Packet(buf, len, &from, udp, 0);
			} else if (type == 6) {
				taken = Process_Go_Away_Packet(buf, len, &from);
			}
			if (!taken) {
				Unexpected_++;








































































































































































































































































































































































































































				UnknownLog.Unknown_4BC130("UDP", __FILE__, __LINE__, "Unexpected packet: (%d) (%s : %d) -> (%d [%d]) (%s)",
					len, inet_ntoa(from.sin_addr), ntohs(from.sin_port), 0, 0, "");
			}
		}
	}
	// The client then pumps every connection (CordCon::pump), Pump_Connections and Pump_Releases;
	// a program pumps its connections itself here (Pump_Connections has no timer yet).
	Pump_Releases();
}

// 0x859F80: the record of a data socket.
CordDataSocketClass *CordMgr::Find_Data_Socket(UDP *socket)
{
	WWPROFILE("CordMgr::Find_Data_Socket");
	CordDataSocketClass *s = NULL;
	int count = DataSockets_.Length();
	for (int i = 0; i < count; i++) {
		s = DataSockets_.Get_Pointer(i);
		if (s->Socket == socket) {
			break;
		}
	}
	return s;
}

// 0x8586D0, functional (NOT EXACT): 06, the address (4 bytes), the port (2), the type it answers,
// the session (2). Types 4 / 5: the connection on a socket bound to that port, to the sender's
// address and port, with that session, goes away. Types 0 / 2 (a refused handshake): the client
// expires the connecting record's timer (not rebuilt: Pump_Connections has no timer).
bool CordMgr::Process_Go_Away_Packet(unsigned char *data, int len, sockaddr_in *from)
{
	WWPROFILE("CordMgr::Process_Go_Away_Packet");
	if (len != 10) {
		return false;
	}
	unsigned short port = ntohs(*(unsigned short *)(data + 5));
	unsigned short session = ntohs(*(unsigned short *)(data + 8));
	if (data[7] != 4 && data[7] != 5) {
		return false;
	}
	for (int i = 0; i < Connections_.Length(); i++) {
		CordCon *con = Connections_[i];
		unsigned int ip = 0;
		unsigned short local = 0;
		con->Udp_->getLocalAddr(ip, local);
		if (local == port && con->Ip_ == ntohl(from->sin_addr.s_addr) && con->Port_ == ntohs(from->sin_port)
			&& (unsigned short)con->Session_ == session) {
			con->Go_Away();
			return true;
		}
	}
	return false;
}

// 0x858980: the connections to that address and port go away (the client also expires the
// matching connecting records' timers: not rebuilt).
void CordMgr::Process_Zero_Read(sockaddr_in *from)
{
	for (int i = 0; i < Connections_.Length(); i++) {
		CordCon *con = Connections_[i];
		if (con->Ip_ == ntohl(from->sin_addr.s_addr) && con->Port_ == ntohs(from->sin_port)) {
			con->Go_Away();
		}
	}
}

// 0x8596F0: called by the game's connection (app\common, 0x82CDC0 / 0x82D1B0 / 0x82D3B0).
bool CordMgr::Release(CordCon *con)
{
	con->close(false);
	CordReleaseClass r;
	r.Con = con;
	r.Time = (unsigned int)time(0) + 30;
	Releases_.Add(r);
	return true;
}

// 0x859940, functional (NOT EXACT): as many entries as the list holds while it shrinks; a due one
// leaves the list, and its connection, when still open, leaves Connections_, its socket's count
// and memory (the client logs '%d UDP connections managed').
bool CordMgr::Pump_Releases()
{
	WWPROFILE("CordMgr::Pump_Releases");
	int at = 0;
	for (int k = 0; k < Releases_.Length(); k++) {
		CordReleaseClass r;
		Releases_.Get(r, at);
		if ((int)time(0) < (int)r.Time) {
			at++;
			continue;
		}
		for (int i = 0; i < Connections_.Length(); i++) {
			if (Connections_[i] != r.Con) {
				continue;
			}
			Connections_.Remove(i);
			for (int d = 0; d < DataSockets_.Length(); d++) {
				if (DataSockets_[d].Socket == r.Con->Udp_) {
					DataSockets_[d].Unknown04--;
				}
			}
			delete r.Con;
			break;
		}
		Releases_.Remove(at);
	}
	return true;
}

// 0x85A000: the client's side takes type 4 (from the server), the server's type 5.
bool CordMgr::Process_CORD_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown,
	char type)
{
	WWPROFILE("CordMgr::Process_CORD_Packet");
	CordCon *con = NULL;
	bool handled = false;
	unsigned short id;
	if (len < 12) {
		return false;
	}
	for (int i = 0; i < Connections_.Length(); i++) {
		Connections_.Get(con, i);
		unsigned long ip = con->Ip_;
		int session = con->Session_;
		id = ntohs(*(unsigned short *)(data + 1));
		if (ip == ntohl(from->sin_addr.s_addr) && (unsigned short)session == id) {
			if ((type == 4 && con->IsServer_ == false) || (type == 5 && con->IsServer_ == true)) {
				handled = true;
				if (con->handlePacket(data, len)) {
					con->Port_ = ntohs(from->sin_port);
					break;
				}




































































































































































































































































































































































































































































































































































				UnknownLog.Unknown_4BC130("UDP", __FILE__, __LINE__, "Packet failed validation (%p : %d (key=%d))",
					ntohl(from->sin_addr.s_addr), ntohs(from->sin_port), con->Key_);
			}
		}
	}
	return handled;
}

// 0x85A3B0 / 0x85AB30: the client takes no SYN (the server's side of the handshake).
bool CordMgr::Process_SYN1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_SYN1_Packet");
	return false;
}

bool CordMgr::Process_SYN2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_SYN2_Packet");
	return false;
}

// 0x85A430, functional: ACK1 (1, protocol, session, two words, the server's modulus and exponent,
// each a 4-byte length then big-endian bytes) for a connection waiting in state 1. The session key
// becomes the low 64 bits of a number filled with random bits up to one below the modulus's
// length, raised to the exponent modulo the modulus (WWLib's XMP); state 3 (SYN2 next).
bool CordMgr::Process_ACK1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_ACK1_Packet");
	if (len < 0x14) {
		return false;
	}
	unsigned char type = data[0];
	unsigned char protocol = data[1];
	unsigned short session = ntohs(*(unsigned short *)(data + 2));
	unsigned long word1 = ntohl(*(unsigned long *)(data + 4));
	unsigned long word2 = ntohl(*(unsigned long *)(data + 8));
	digit modulus[0x40], exponent[0x40], zero[0x40];
	XMP_Init(modulus, 0, 0x40);
	XMP_Init(exponent, 0, 0x40);
	XMP_Init(zero, 0, 0x40);
	unsigned long mlen = ntohl(*(unsigned long *)(data + 12));
	if (mlen + 0x10 > (unsigned long)len || mlen == 0) {
		return false;
	}
	XMP_Unsigned_Decode(modulus, data + 0x10, mlen, 0x40);
	unsigned long elen = ntohl(*(unsigned long *)(data + mlen + 0x10));
	if (elen + mlen + 0x14 > (unsigned long)len) {
		return false;
	}
	if (elen != 0) {
		XMP_Unsigned_Decode(exponent, data + mlen + 0x14, elen, 0x40);
	}
	if (protocol != Protocol_ || type != 1) {
		return false;
	}
	for (int i = 0; i < Pending_.Length(); i++) {
		CordConnectingClass *c = Pending_[i];
		if (c->Ip != ntohl(from->sin_addr.s_addr) || c->Port != ntohs(from->sin_port)
			|| c->Session != session || c->State != 1) {
			continue;
		}
		if (word1 == 0 && word2 == 0) {
			c->State = 0;		// (the client's: an unencrypted server)
			return false;
		}
		if (memcmp(modulus, zero, sizeof(zero)) != 0 && memcmp(exponent, zero, sizeof(zero)) != 0
			&& c->Key != 0) {
			digit plain[0x40], cipher[0x40];
			XMP_Init(plain, 0, 0x40);
			for (unsigned int bit = 0; bit < 64; bit++) {
				if (c->Key & ((__int64)1 << bit)) {
					plain[bit >> 5] |= 1 << (bit & 0x1F);
				}
			}
			SecureRandomClass random;
			for (unsigned int b = 64; b < XMP_Count_Bits(modulus, 0x40) - 1; b++) {
				if (random.Randval() & 1) {
					plain[b >> 5] |= 1 << (b & 0x1F);
				}
			}
			XMP_Init(cipher, 0, 0x40);
			XMP_Exponent_Mod(cipher, plain, exponent, modulus, 0x40);
			memcpy(c->EncryptedKey, cipher, sizeof(cipher));
		} else {
			memset(c->EncryptedKey, 0, sizeof(c->EncryptedKey));
		}
		c->State = 3;
		c->Ack1Word1 = word1;
		c->Ack1Word2 = word2;
		return true;
	}
	return false;
}

// 0x85ABB0, functional: ACK2 (3, protocol, session, 2 bytes, the data port, a word) for a
// connection in the handshake: its CordCon (the word is the constructor's 7th argument), in
// both connection lists; the handshake record goes.
bool CordMgr::Process_ACK2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_ACK2_Packet");
	if (len != 12) {
		return false;
	}
	unsigned char type = data[0];
	unsigned char protocol = data[1];
	unsigned short session = ntohs(*(unsigned short *)(data + 2));
	ntohs(*(unsigned short *)(data + 4));
	unsigned short port = ntohs(*(unsigned short *)(data + 6));
	unsigned long word = ntohl(*(unsigned long *)(data + 8));
	if (protocol != Protocol_ || type != 3) {
		return false;
	}
	for (int i = 0; i < Pending_.Length(); i++) {
		CordConnectingClass *c = Pending_[i];
		if (c->Ip != ntohl(from->sin_addr.s_addr) || c->Port != ntohs(from->sin_port)
			|| c->Session != session) {
			continue;
		}
		c->State = 5;
		CordCon *con = new CordCon(c->Ip, port, session, 0, false, c->Socket, (int)word,
			(unsigned int)c->Key, (unsigned int)(c->Key >> 32));
		AllConnections_.Add(con);
		Connections_.Add(con);
		delete c;
		Pending_.Remove(i);
		return true;
	}
	return false;
}

/*
**	Reconstruction notes
**
**	* The client's functions in this file, in order: 0x857590-0x857DE0 (CordMgr's constructor and
**	  destructor, socket setup: not rebuilt), Pump 0x857F20 (logs on lines 592 and 618), 0x858640,
**	  Process_Go_Away_Packet 0x8586D0, 0x858980, Pump_Connections 0x858A60 (XMP_Encode: the SYN2
**	  key), 0x858EC0 (XMP_Init, SecureRandomClass: the session key), 0x859440-0x8598F0,
**	  Pump_Releases 0x859940, Get_Data_Socket 0x859D50, Find_Data_Socket 0x859F80,
**	  Process_CORD_Packet 0x85A000 (log on line 1272), Compute_Ticks 0x85A220, Process_SYN1_Packet
**	  0x85A3B0, Process_ACK1_Packet 0x85A430 (XMP_Unsigned_Decode, XMP_Exponent_Mod: the server's
**	  RSA key), Process_SYN2_Packet 0x85AB30, Process_ACK2_Packet 0x85ABB0 (the CordCon).
**	* The handshake on the wire (the reference decoder docs/design.md 16.4): SYN1 00 14 <session>, ACK1
**	  01 14 <session> <8 bytes> <modulus> <exponent>, SYN2 02 14 <session> ... <the session key,
**	  RSA-encrypted>, ACK2 03 14 <session> <2 bytes> <the data port> <4 bytes>.
*/
