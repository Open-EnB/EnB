// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordmgr.cpp (client 0x856970-0x85AED0)
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

// E&B's message reporter 0x4BC130 (cdecl, printf-style; unknown_debug.cpp's 0x4BC260 is its sibling): the
// sender, the category, the file and line, a format (D81: the client pushes its pooled "" 0xBDA908 as the
// sender, as dbgmgr.cpp does). Not rebuilt: a program that links CordMgr defines it
// (tests/udpreplay does). The asserts and these calls sit on the client's lines (D25), so
// this block keeps the eight lines the stand-in class took.
//
void Unknown_4BC130(const char *sender, const char *category, const char *file, int line,
	const char *format, ...);	// NAME 1 [Unknown_4BC130]: 0x4BC130

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

// 0x856970: the members zeroed (+0x04 left to Init), then Init inlined.
CordMgr::CordMgr() :
	Protocol_(0x14),
	NextSession_(0),
	Unknown0C(0),
	MaxPerSocket_(0)
{
	Init();
}

// 0x857590: the listening sockets closed, then the connections, the handshakes and the data sockets
// deleted from the front of their lists. The client calls the destructor of Connections_ (+0x34)
// out of line; exact since wlib's getPointer NULLs its pointer on a miss itself (NW6: the inliner's
// budget), the call-site NULL below kept (same code).
CordMgr::~CordMgr()
{
	Unlisten(0, 0);
	while (Connections_.length() != 0) {
		CordCon *con = NULL;
		Connections_.get(con, 0);
		delete con;
		Connections_.remove(0);
	}
	while (Pending_.length() != 0) {
		CordConnectingClass *d = NULL;
		Pending_.get(d, 0);
		delete d;
		Pending_.remove(0);
	}
	Releases_.clear();
	while (DataSockets_.length() != 0) {
		CordDataSocketClass *ds;
		if (!DataSockets_.getPointer(&ds, 0)) {
			ds = NULL;
		}
		delete ds->Socket;
		DataSockets_.remove(0);
	}
}

// 0x857C00: closes the listening sockets on ip / port (0: any) and drops their records; true when it
// closed one. The record pointer is NULL past the end, as the client's (it then reads through it).
bool CordMgr::Unlisten(unsigned int ip, unsigned short port)
{
	int index = 0;
	int removed = 0;
	for (int n = 0; n < ListenSockets_.length(); n++) {
		CordListenSocketClass *ls;
		if (!ListenSockets_.getPointer(&ls, index)) {
			ls = NULL;
		}
		if ((ls->Unknown04 == ip || ip == 0) && (ls->Unknown08 == port || port == 0)) {
			delete ls->Socket;
			ListenSockets_.remove(index);
			removed++;
		} else {
			index++;
		}
	}
	if (removed != 0) {
		return(true);
	}
	return(false);
}

// 0x858EC0: a data socket with fewer than MaxPerSocket_ connections, else a new one (ports 3755-3759, else
// any) with the largest buffers it can get; the session id is the manager's counter (+0x1337 each time), the
// key Randval() << 24 ^ Randval() (SecureRandomClass) when the connection is encrypted.
int CordMgr::Connect(unsigned int ip, unsigned short port, bool encrypt)
{
	int index = -1;
	CordDataSocketClass *record;
	for (int i = 0; i < DataSockets_.length(); i++) {
		if (!DataSockets_.getPointer(&record, i)) {
			record = NULL;
		}
		if (record->Unknown04 < MaxPerSocket_ && !record->Unknown0C) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		UDP *udp = new UDP;
		int result;
		for (int j = 0; j < 5; j++) {
			result = udp->Bind((unsigned int)0, (unsigned short)(3755 + j));
			if (result == 0) {
				break;
			}
		}
		if (result != 0) {
			udp->Bind((unsigned int)0, 0);
		}
		if (MaxPerSocket_ == 0) {
			MaxPerSocket_ = 8;
		}
		while (MaxPerSocket_ > 1) {
			int bytes = MaxPerSocket_ << 14;
			int ok = 1 & udp->SetInputBuffer(bytes);
			ok &= udp->SetOutputBuffer(bytes);
			if (ok == 1) {
				break;
			}
			MaxPerSocket_--;
		}
		CordDataSocketClass rec;
		rec.Socket = udp;
		rec.Unknown04 = 0;
		rec.Unknown08 = 0;
		rec.Unknown0C = false;
		DataSockets_.addTail(rec);
		if (!DataSockets_.getPointer(&record, DataSockets_.length() - 1)) {
			record = NULL;
		}
	}
	record->Unknown04++;
	CordConnectingClass *c = new CordConnectingClass;
	c->Ip = ip;
	c->Port = port;
	c->Session = NextSession_;
	NextSession_ += 0x1337;
	c->Socket = record->Socket;
	c->Record = record;
	c->State = 1;
	c->Ack1Word2 = 0;
	c->Ack1Word1 = 0;
	c->Unknown24 = 0;
	c->LastSend.setDay(0);
	c->Created.update();
	if (encrypt) {
		SecureRandomClass random;
		c->Key = random.Randval();
		c->Key <<= 24;
		c->Key ^= random.Randval();
	} else {
		c->Key = 0;
	}
	Pending_.addTail(c);
	return(c->Session);
}

// 0x858A60, functional (NOT EXACT): SYN1 (0, protocol, session) while waiting for ACK1, SYN2 (2,
// protocol, session, ACK1's two words, 0x4000, the key's length, the encrypted key) while waiting
// for ACK2, each again a second after the last; a connection 60 s old gives up: it becomes a
// CordCon without a socket (so the caller finds it, closed) and leaves the list. (The client
// then reads the freed record once more; not rebuilt.)
bool CordMgr::Pump_Connections()
{
	WWPROFILE("CordMgr::Pump_Connections");
	unsigned char buf[0x400];
	Xtime now;
	Xtime resend;
	Xtime timeout;
	resend.addSeconds(-1);
	timeout.addSeconds(-60);
	for (int i = 0; i < Pending_.length(); i++) {
		CordConnectingClass *c; Pending_.get(c, i);
		if (c->Created < timeout) {
			CordCon *con = new CordCon(c->Ip, c->Port, c->Session, 0, false, NULL, 0,
				c->Key);
			AllConnections_.addTail(con);
			Connections_.addTail(con);
			delete c;
			Pending_.remove(i);
			i--;
			continue;
		}
		if (c->Socket == NULL) {
			continue;
		}
		if (c->State == 1 && resend > c->LastSend) {
			buf[0] = 0;
			buf[1] = Protocol_;
			*(unsigned short *)(buf + 2) = htons((unsigned short)c->Session);
			c->Socket->Write(buf, 4, c->Ip, c->Port);
			c->LastSend = now;
		} else if (c->State == 3 && resend > c->LastSend) {
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
			c->LastSend = now;
		}
	}
	return true;
}

#include <stdio.h>
#include <ctype.h>
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter

// NOT EXACT: 0x857F20 (frame 0x6E4 vs 0x6EC, registers: the client keeps this in ebp). Every data socket, then every
// listening one, until Read says nothing more (< 0); SYN1 / SYN2 only on a listening socket, the data only on a data
// socket. A datagram nobody takes is logged (line 592) and answered with a go-away (Send_Go_Away, inlined) unless it
// is an ACK1 / ACK2 or the ACK2 of a connection still in the handshake (line 618). Then every connection's pump.
bool CordMgr::Pump()
{
	WWPROFILE("CordMgr::Pump");
	CordCon *con = NULL;
	unsigned char buf[0x400];
	sockaddr_in from;
	int data_count = DataSockets_.length();
	int total = ListenSockets_.length() + data_count;
	for (int i = 0; i < total; i++) {
		bool listening;
		UDP *socket;
		int unknown;
		int logged;
		if (i < DataSockets_.length()) {
			CordDataSocketClass d;
			if (!DataSockets_.get(d, i)) {
				continue;
			}
			listening = false;
			unknown = 0;
			socket = d.Socket;
			logged = d.Unknown08;
		} else {
			CordListenSocketClass l;
			if (!ListenSockets_.get(l, i - data_count)) {
				continue;
			}
			listening = true;
			logged = 0;
			socket = l.Socket;
			unknown = l.Unknown08;
		}
		int len;
		while ((len = socket->Read(buf, 0x400, &from)) >= 0) {
			if (len == 0) {
				Process_Zero_Read(&from);
				continue;
			}
			if (len < 4) {
				continue;
			}
			char type = buf[0];
			bool taken = false;
			if (type == 0) {
				if (listening == true) {
					taken = Process_SYN1_Packet(buf, len, &from, socket, unknown);
				}
			} else if (type == 1) {
				taken = Process_ACK1_Packet(buf, len, &from, socket, unknown);
			} else if (type == 2) {
				if (listening == true) {
					taken = Process_SYN2_Packet(buf, len, &from, socket, unknown);
				}
			} else if (type == 3) {
				taken = Process_ACK2_Packet(buf, len, &from, socket, unknown);
			} else if ((type == 4 || type == 5) && !listening) {
				taken = Process_CORD_Packet(buf, len, &from, socket, unknown, type);
			} else if (type == 6) {
				Process_Go_Away_Packet(buf, len, &from, socket, unknown);
				continue;
			}
			if (taken) {
				continue;
			}
			uint32 lip = 0;
			unsigned short lport = 0;
			socket->getLocalAddr(lip, lport);
			char from_text[0x80];
			char local_text[0x80];
			char bytes[0x80];
			sprintf(from_text, "%s", inet_ntoa(from.sin_addr));
			sprintf(local_text, "%s", inet_ntoa(*(in_addr *)&lip));
			memset(bytes, 0, sizeof(bytes));
			for (int k = 0; k < (len > 16 ? 16 : len); k++) {
				if (isgraph(buf[k])) {
					sprintf(bytes + strlen(bytes), "'%c'", buf[k]);
				} else {
					sprintf(bytes + strlen(bytes), "%d ", buf[k]);
				}
			}
































































































































































































































































				Unknown_4BC130("", "UDP", __FILE__, __LINE__, "Unexpected packet: (%d) (%s : %d) -> (%d [%d]) (%s)",
					len, from_text, from.sin_port, lport, logged, bytes);
			unsigned short session = 0;
			if ((type == 4 || type == 5) && len >= 3) {
				session = ntohs(*(unsigned short *)(buf + 1));
			}
			bool reply = true;
			if (type == 1 || type == 3) {
				reply = false;
			}
			if (type == 4) {
				CordConnectingClass *c = NULL;
				for (int p = 0; p < Pending_.length(); p++) {
					Pending_.get(c, p);
					if (c->Ip == ntohl(from.sin_addr.s_addr) && c->Session == session) {











						Unknown_4BC000("", "UDP", __FILE__, __LINE__, "We must have dropped an ack2 packet to the client!: %d", session);
						goto next;
					}
				}
			}
			if (reply) {
				Send_Go_Away(socket, ntohl(from.sin_addr.s_addr), ntohs(from.sin_port), type, session);
			}
next:
			;
		}
	}
	Xtime now;
	for (int c = 0; c < Connections_.length(); c++) {
		Connections_.get(con, c);
		con->pump(now);
	}
	Pump_Connections();
	Pump_Releases();
	return true;
}

// 0x859F80: the record of a data socket.
CordDataSocketClass *CordMgr::Find_Data_Socket(UDP *socket)
{
	WWPROFILE("CordMgr::Find_Data_Socket");
	CordDataSocketClass *s = NULL;
	int count = DataSockets_.length();
	for (int i = 0; i < count; i++) {
		DataSockets_.getPointer(&s, i);
		if (s->Socket == socket) {
			break;
		}
	}
	return s;
}

// 0x8586D0, functional (NOT EXACT): 06, the address (4 bytes), the port (2), the type it answers,
// the session (2). Types 4 / 5: the connection on a socket bound to that port, to the sender's
// address and port, with that session, goes away. Types 0 / 2 (a refused handshake): every
// connecting record on such a socket to that address dates from 1970, so Pump_Connections gives up.
bool CordMgr::Process_Go_Away_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_Go_Away_Packet");
	if (len != 10) {
		return false;
	}
	unsigned int ip = ntohl(*(unsigned int *)(data + 1));
	unsigned short port = ntohs(*(unsigned short *)(data + 5));
	unsigned char type = data[7];
	unsigned short session = ntohs(*(unsigned short *)(data + 8));
	CordCon *con = NULL;
	CordConnectingClass *c = NULL;
	switch (type) {
		case 4:
		case 5:
		{
			for (int i = 0; i < Connections_.length(); i++) {
				Connections_.get(con, i);
				uint32 lip = 0;
				unsigned short lport = 0;
				con->Udp_->getLocalAddr(lip, lport);
				if (lport == port) {
					unsigned int cip = con->Ip_;
					if (ntohl(from->sin_addr.s_addr) == cip) {
						unsigned short cport = con->Port_;
						if (ntohs(from->sin_port) == cport && session == con->Session_) {
							con->Go_Away();
							return true;
						}
					}
				}
			}
			break;
		}
		case 0:
		case 2:
		{
			for (int p = 0; p < Pending_.length(); p++) {
				Pending_.get(c, p);
				uint32 lip = 0;
				unsigned short lport = 0;
				c->Socket->getLocalAddr(lip, lport);
				if (lport == port && ntohl(from->sin_addr.s_addr) == c->Ip && ntohs(from->sin_port) == c->Port) {
					c->Created = 1;
				}
			}
			break;
		}
	}
	return false;
}

// 0x858980: the connections to that address and port go away; the connecting records to it age 5 s.
void CordMgr::Process_Zero_Read(sockaddr_in *from)
{
	CordCon *con = NULL;
	for (int i = 0; i < Connections_.length(); i++) {
		Connections_.get(con, i);
		unsigned int ip = con->Ip_;
		if (ip == ntohl(from->sin_addr.s_addr)) {
			unsigned short port = con->Port_;
			if (port == ntohs(from->sin_port)) {
				con->Go_Away();
			}
		}
	}
	CordConnectingClass *c;
	for (int p = 0; p < Pending_.length(); p++) {
		Pending_.get(c, p);
		if (c->Ip == ntohl(from->sin_addr.s_addr) && c->Port == ntohs(from->sin_port)) {
			c->Created.addSeconds(-5);
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
	Releases_.addTail(r);
	return true;
}

// 0x859940, functional (NOT EXACT): as many entries as the list holds while it shrinks; a due one
// leaves the list, and its connection, when still open, leaves Connections_, its socket's count
// and memory (the client logs '%d UDP connections managed').
bool CordMgr::Pump_Releases()
{
	WWPROFILE("CordMgr::Pump_Releases");
	int at = 0;
	for (int k = 0; k < Releases_.length(); k++) {
		CordReleaseClass r;
		Releases_.get(r, at);
		if ((int)time(0) < (int)r.Time) {
			at++;
			continue;
		}
		for (int i = 0; i < Connections_.length(); i++) {
			CordCon *con; Connections_.get(con, i); if (con != r.Con) {
				continue;
			}
			Connections_.remove(i);
			for (int d = 0; d < DataSockets_.length(); d++) {
				CordDataSocketClass *ds; DataSockets_.getPointer(&ds, d); if (ds->Socket == r.Con->Udp_) {
					ds->Unknown04--;
				}
			}
			delete r.Con;
			break;
		}
		Releases_.remove(at);
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
	for (int i = 0; i < Connections_.length(); i++) {
		Connections_.get(con, i);
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
























































































































































































































































































































































































































































































				Unknown_4BC130("", "UDP", __FILE__, __LINE__, "Packet failed validation (%p : %d (key=%d))",
					ntohl(from->sin_addr.s_addr), ntohs(from->sin_port), con->Key_);
			}
		}
	}
	return handled;
}

// 0x85A3B0 / 0x85AB30: the client takes no SYN (the server's side of the handshake): a record known to be null,
// its block compiled out and the delete left (the client's EH frame, state -1 only). The dead block (not evidence:
// any body the optimiser drops) keeps Pump from inlining them, as the client's Pump calls both.
bool CordMgr::Process_SYN1_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_SYN1_Packet");
	CordConnectingClass *c = NULL;
	if (c != NULL) {
		c->Ip = ntohl(from->sin_addr.s_addr);
		c->Port = ntohs(from->sin_port);
		c->Session = ntohs(*(unsigned short *)(data + 2));
		c->Socket = socket;
		c->Unknown24 = unknown;
		c->State = 2;
		Pending_.addTail(c);
		return true;
	}
	delete c;
	return false;
}

bool CordMgr::Process_SYN2_Packet(unsigned char *data, int len, sockaddr_in *from, UDP *socket, int unknown)
{
	WWPROFILE("CordMgr::Process_SYN2_Packet");
	CordConnectingClass *c = NULL;
	if (c != NULL) {
		c->Ip = ntohl(from->sin_addr.s_addr);
		c->Port = ntohs(from->sin_port);
		c->Session = ntohs(*(unsigned short *)(data + 2));
		c->Socket = socket;
		c->Unknown24 = unknown;
		c->State = 2;
		Pending_.addTail(c);
		return true;
	}
	delete c;
	return false;
}

// NOT EXACT: 0x85A430 (0.75, registers: the client reuses eax where ours takes ecx / edx). ACK1 (1, protocol, session,
// two words, the server's modulus and exponent, each a 4-byte length then big-endian bytes; each check its own return)
// for a connection in state 1: the session key in the low 64 bits of a random number below the modulus, encrypted with
// an RSAKeyClass of the two (Key_Exchange 0x829180's form); a zero modulus or exponent: the key's low word. State 3.
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
	BigInt n;
	BigInt e;
	unsigned long mlen = ntohl(*(unsigned long *)(data + 12));
	unsigned long offset = mlen + 0x10;
	if (offset > (unsigned long)len) {
		return false;
	}
	if (mlen == 0) {
		return false;
	}
	n.Unsigned_Decode(data + 0x10, mlen);
	unsigned long elen = ntohl(*(unsigned long *)(data + offset));
	offset += 4;
	if (elen + offset > (unsigned long)len) {
		return false;
	}
	if (elen == 0) {
		return false;
	}
	e.Unsigned_Decode(data + offset, elen);
	if (protocol != Protocol_ || type != 1) {
		return false;
	}
	CordConnectingClass *c = NULL;
	for (int i = 0; i < Pending_.length(); i++) {
		Pending_.get(c, i);
		if (c->Ip == ntohl(from->sin_addr.s_addr) && c->Port == ntohs(from->sin_port)
			&& c->Session == session && c->State == 1) {
			if (word1 == 0 && word2 == 0) {
				c->State = 0;		// (the client's: an unencrypted server)
				break;
			}
			BigInt zero;
			if (n != zero && e != zero && c->Key != 0) {
				RSAKeyClass key;
				key.Modulus = n;
				key.Exponent = e;
				BigInt m;
				unsigned __int64 mask = 1;
				for (unsigned int bit = 0; bit < 64; bit++) {
					if (c->Key & mask) {
						m.Set_Bit(bit);
					}
					mask <<= 1;
				}
				SecureRandomClass random;
				for (bit = 64; bit < (unsigned int)(n.BitCount() - 1); bit++) {
					if (random.Randval() & 1) {
						m.Set_Bit(bit);
					}
				}
				BigInt cipher;
				cipher = key.Encrypt(m);
				c->EncryptedKey = cipher;
			} else if (n == zero || e == zero) {
				c->EncryptedKey = BigInt((unsigned long)c->Key);
			}
			c->State = 3;
			c->LastSend.set(0, 0);
			c->Ack1Word1 = word1;
			c->Ack1Word2 = word2;
			return true;
		}
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
	CordConnectingClass *c = NULL;
	for (int i = 0; i < Pending_.length(); i++) {
		Pending_.get(c, i);
		if (c->Ip != ntohl(from->sin_addr.s_addr) || c->Port != ntohs(from->sin_port)
			|| c->Session != session) {
			continue;
		}
		c->State = 5;
		CordCon *con = new CordCon(c->Ip, port, session, 0, false, c->Socket, (int)word,
			c->Key);
		AllConnections_.addTail(con);
		Connections_.addTail(con);
		delete c;
		Pending_.remove(i);
		return true;
	}
	return false;
}

// 0x857DE0: the set of every data and listening socket, then wnet's UDP::Wait on the last one (FD_SET before
// the udp store keeps this in ecx and the socket in eax, as the client's).
int CordMgr::Wait(int sec, int usec)
{
	fd_set given;
	int count = DataSockets_.length();
	UDP *udp = 0;
	FD_ZERO(&given);
	int i;
	for (i = 0; i < count; i++) {
		CordDataSocketClass record;
		DataSockets_.get(record, i);
		FD_SET((unsigned int)record.Socket->getFD(), &given);
		udp = record.Socket;
	}
	for (i = 0; i < ListenSockets_.length(); i++) {
		CordListenSocketClass record;
		ListenSockets_.get(record, i);
		FD_SET((unsigned int)record.Socket->getFD(), &given);
		udp = record.Socket;
	}
	if (udp) {
		fd_set ready;
		return udp->Wait(sec, usec, given, ready);
	}
	return 0;
}

// 0x859440: wlib ArrayList's remove inlined once, as the client's.
bool CordMgr::Find_Connection(CordCon **con, unsigned int port, int session)
{
	int i;
	bool found = false;
	for (i = 0; i < AllConnections_.length(); i++) {
		AllConnections_.get(*con, i);
		CordCon *c = *con;
		if (session == 0 ? (c->IsServer_ && port == c->Unknown0C) : (!c->IsServer_ && session == c->Session_)) {
			found = true;
			break;
		}
	}
	if (!found) {
		return false;
	}
	AllConnections_.remove(i);
	return true;


}

// 0x8595B0: as 0x859440 (the shrink out of line, 0x85B8B0). A handshake record is deleted but
// left in Pending_, as the client leaves it.
bool CordMgr::Cancel_Connect(int session)
{
	int i;
	CordCon *con;
	for (i = 0; i < AllConnections_.length(); i++) {
		AllConnections_.get(con, i);
		if (con->Session_ == session && !con->IsServer_) {
			AllConnections_.remove(i);
			delete con;
			return true;
		}
	}
	CordConnectingClass *c;
	for (i = 0; i < Pending_.length(); i++) {
		Pending_.get(c, i);
		if (c->Session == session) {
			delete c;
			return true;
		}
	}
	return false;
}

// 0x858640 (between Pump and Process_Go_Away_Packet in the client; inlined in Pump, no caller): a go-away
// (type 6) to ip:port on udp: the address and port again, the type it answers, the session.
void CordMgr::Send_Go_Away(UDP *udp, unsigned int ip, unsigned short port, unsigned char type, unsigned short session)
{
	unsigned char buf[0x100];
	buf[0] = 6;
	*(unsigned int *)(buf + 1) = htonl(ip);
	*(unsigned short *)(buf + 5) = htons(port);
	buf[7] = type;
	*(unsigned short *)(buf + 8) = htons(session);
	udp->Write(buf, 10, ip, port);
}

// 0x8598F0 (via the game's 0x82E1D0; after Release 0x8596F0 in the client, here last so the log
// calls keep their lines)
int CordMgr::Pending_Messages()
{
	CordCon *con = 0;
	int total = 0;
	for (int i = 0; i < Connections_.length(); i++) {
		Connections_.get(con, i);
		total += con->Pending_Messages();
	}
	return total;
}

// 0x856D80 (no caller): the constructor's, then a listening socket on ip:port. In the client after the
// first constructor; here last, so that the log calls keep their lines (D25).
CordMgr::CordMgr(unsigned int ip, unsigned short port) :
	Protocol_(0x14),
	NextSession_(0),
	Unknown0C(0),
	MaxPerSocket_(0)
{
	Init();
	Listen(ip, port);
}

// 0x8570C0 (no caller; in the client after the constructors)
void CordMgr::Init()
{
	SecureRandomClass random;
	Unknown04 = random.Randval();
	NextSession_ = (unsigned short)random.Randval() ^ (unsigned short)time(NULL) ^ 0x2121;
	KeyPair.Load(file_auto_ptr(_TheFileFactory, "server_rsa_keyset.dat"));
}

// 0x8578C0 (no caller; in the client after the destructor): Connect's buffer loop on its own: MaxPerSocket_
// (8 when 0) x 16 KB, one connection less while SetInputBuffer / SetOutputBuffer can't have it.
void CordMgr::Set_Buffers(UDP *udp)
{
	if (MaxPerSocket_ == 0) {
		MaxPerSocket_ = 8;
	}
	while (MaxPerSocket_ > 1) {
		int bytes = MaxPerSocket_ << 14;
		int ok = 1 & udp->SetInputBuffer(bytes);
		ok &= udp->SetOutputBuffer(bytes);
		if (ok == 1) {
			break;
		}
		MaxPerSocket_--;
	}
}

// Compute_Ticks' field writes (inlined): through a pointer, so VC6 keeps every store in source order (written
// straight into the array, the digest's zero stores move above the last field's).
static inline void Cord_Put_Short(unsigned char *p, unsigned short value) { *(unsigned short *)p = htons(value); }	// NAME 2: what it does
static inline void Cord_Put_Long(unsigned char *p, unsigned long value) { *(unsigned long *)p = htonl(value); }	// NAME 2: what it does

// 0x85A220 (no caller; in the client after Process_CORD_Packet): the six values in network order, then
// Unknown04 (Init's random word), 22 bytes in a zeroed 512-byte block, hashed by WWOnline's SHA-1 0x8B4A90;
// the digest's five words xored.
unsigned int CordMgr::Compute_Ticks(unsigned short port1, unsigned int ip1, unsigned int ip2, unsigned short port2,
	unsigned int ip3, unsigned short port3)
{
	WWPROFILE("CordMgr::Compute_Ticks");
	unsigned char buf[0x200];
	memset(buf, 0, sizeof(buf));
	Cord_Put_Short(buf + 0, port1);
	Cord_Put_Long(buf + 2, ip1);
	Cord_Put_Long(buf + 6, ip2);
	Cord_Put_Short(buf + 10, port2);
	Cord_Put_Long(buf + 12, ip3);
	Cord_Put_Short(buf + 16, port3);
	Cord_Put_Long(buf + 18, Unknown04);
	unsigned int digest[5];
	memset(digest, 0, sizeof(digest));
	Cord_SHA(buf, 0x16, digest);
	unsigned int ticks = 0;
	for (int i = 0; i < 5; i++) {
		ticks ^= digest[i];
	}
	return ticks;
}

// 0x857940 (in the client after the destructor): a socket with 32 KB buffers bound to ip:port, kept
// in ListenSockets_; deleted when Bind fails. The record is made before Bind (the client widens the
// port then).
bool CordMgr::Listen(unsigned int ip, unsigned short port)
{
	UDP *udp = new UDP;
	udp->SetInputBuffer(0x8000);
	udp->SetOutputBuffer(0x8000);
	CordListenSocketClass record = { udp, ip, port };
	if (udp->Bind(ip, port) != 0) {
		delete udp;
		return(false);
	}
	ListenSockets_.addTail(record);
	return(true);
}


/*
**	Reconstruction notes
**
**	* The client's functions in this file, in order: the constructors 0x856970 / 0x856D80, Init
**	  0x8570C0, the destructor 0x857590, 0x8578C0, Listen 0x857940, Unlisten, Wait, Pump 0x857F20 (logs on lines 592 and 618), 0x858640,
**	  Process_Go_Away_Packet 0x8586D0, 0x858980, Pump_Connections 0x858A60 (XMP_Encode: the SYN2
**	  key), 0x858EC0 (XMP_Init, SecureRandomClass: the session key), 0x859440-0x8598F0,
**	  Pump_Releases 0x859940, Get_Data_Socket 0x859D50, Find_Data_Socket 0x859F80,
**	  Process_CORD_Packet 0x85A000 (log on line 1272), Compute_Ticks 0x85A220 (here near the end), Process_SYN1_Packet
**	  0x85A3B0, Process_ACK1_Packet 0x85A430 (XMP_Unsigned_Decode, XMP_Exponent_Mod: the server's
**	  RSA key), Process_SYN2_Packet 0x85AB30, Process_ACK2_Packet 0x85ABB0 (the CordCon).
**	* The handshake on the wire (earlier protocol research on the EA captures): SYN1 00 14 <session>, ACK1
**	  01 14 <session> <8 bytes> <modulus> <exponent>, SYN2 02 14 <session> ... <the session key,
**	  RSA-encrypted>, ACK2 03 14 <session> <2 bytes> <the data port> <4 bytes>.
*/
