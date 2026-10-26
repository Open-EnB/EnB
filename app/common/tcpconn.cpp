// Earth & Beyond client (reconstruction): app\common\tcpconn.cpp (PARTIAL). Notes at the end of the file.
#include "tcpconn.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\nstrdup.h"
#include <string.h>
#include <winsock.h>
#include <ctype.h>
// The one TCP manager (0xBE9180, constructed by the static initialiser 0x828A10) and the buffer
// small writes are encrypted in (0xBE91B8).
static TCPMgr TCPManager;		// NAME 2: what it is
static std::set<TCPListenerClass *> Listeners;	// 0xBE91A8 (initialiser 0x828AA0): every listener  // NAME 2: what it holds
static unsigned char WriteBuffer[0x400];	// NAME 2: what it holds
// E&B's message reporter 0x4BC000 (cdecl, printf-style: the sender (the pooled "" 0xBDA908), the
// category, the file and line, a format); not rebuilt
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line,
	const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000











































TCPConnection::TCPConnection(const char *host, unsigned short port, bool encrypt) :
	Con(0),
	Handle(0),
	Encrypt(encrypt),
	Key(0),
	Name(0),
	IP(0),
	Port(htons(port))
{
	if (isdigit(*host)) {
		IP = inet_addr(host);
		Name = nstrdup("unknown");
	} else {
		struct hostent *h = gethostbyname(host);
		G_ASSERT(h);
		IP = *(unsigned int *)h->h_addr_list[0];
		Name = nstrdup(h->h_name);
	}
}

TCPConnection::TCPConnection(unsigned int ip, unsigned short port, bool encrypt) :
	Con(0),
	Handle(0),
	Encrypt(encrypt),
	Key(0),
	Name(0),
	IP(htonl(ip)),
	Port(htons(port))
{
	Name = nstrdup("unknown");
}

TCPConnection::~TCPConnection()
{
	if (Handle != 0) {
		TCPManager.cancelConnect(Handle);
		Handle = 0;
	}
	if (Con != 0) {
		Con->pumpWrites();
		Con->close(true);
		delete Con;
		Con = 0;
	}
	if (Name != 0) {
		delete [] Name;
	}
}








bool TCPConnection::Connect(bool wait)
{
	if (wait) {
		G_ASSERT(Con == 0);
		uint32 handle = 0;
		TCPManager.connect(ntohl(IP), ntohs(Port), &handle);
		if (!TCPManager.getOutgoingConnection(&Con, handle, 15)) {
			return(false);
		}
		if (Con == 0) {
			return(false);
		}
		TCPManager.setBufferedWrites(Con, true);
		Con->setInputDelay(0);
		Con->setOutputDelay(0);
		Key_Exchange(wait, 0);
		return(true);
	}
	return(Try_Connect());
}

// Each success returns from inside its own if-block (the client keeps the three `Handle = 0; return(true);` apart).
bool TCPConnection::Try_Connect()
{
	if (Handle == 0) {
		if (Con == 0) {
			TCPManager.connect(ntohl(IP), ntohs(Port), &Handle);
			return(false);
		}
		return(true);
	}
	if (Con == 0) {
		if (TCPManager.getOutgoingConnection(&Con, Handle, 0)) {
			if (Con == 0) {
				Handle = 0;
				return(true);
			}
			if (Key_Exchange(false, 0)) {
				Handle = 0;
				return(true);
			}
		}
		return(false);
	}
	if (Key_Exchange(false, 0)) {
		Handle = 0;
		return(true);
	}
	return(false);
}


// The client's half: the server's public key ({u32 n bytes, n, u32 e bytes, e}, network order, <= 0x200
// bytes each) in, the 64-bit session key (Encrypt; else 0) keys both RC4 streams and goes back
// RSA-encrypted in the clear ({u32 length, the number}). Waits only with wait (pumping the listeners).
// error: 0, -1 (closed), -2 (a bad key: closed). False: no key yet.
// NOT EXACT: the client inlines the listener loop's iterator increment (_Tree::_Inc), ours calls it (inline budget); two register swaps.
bool TCPConnection::Key_Exchange(bool wait, int *error)
{
	if (error != 0) {
		*error = 0;
	}
	unsigned char *data;
	int nbytes;
	int ebytes;
	for (;;) {
		Con->pumpReads();
		int available = Con->peekRead(&data);
		if (!Con->isConnected()) {
			goto closed;
		}
		ebytes = 0;
		if (available > 4) {
			nbytes = ntohl(*(unsigned long *)data);
			if (nbytes <= 0x200 && available > nbytes + 8) {
				ebytes = ntohl(*(unsigned long *)(data + nbytes + 4));
				if (ebytes <= 0x200 && available >= nbytes + ebytes + 8) {
					goto received;
				}
			}
			if (nbytes > 0x200 || ebytes > 0x200) {
				goto failed;
			}






		}
		if (!wait) {
			return(false);
		}
		Sleep(10);
		for (std::set<TCPListenerClass *>::iterator it = Listeners.begin(); it != Listeners.end(); ++it) {
			(*it)->Accept(false);
		}
	}

closed:
	if (error != 0) {
		*error = -1;
	}
	return(true);

received:
	{
		unsigned char buffer[0x408];
		Con->read(buffer, ebytes + nbytes + 8, 0);
		BigInt n;
		BigInt e;
		int offset = 0;
		int len = ntohl(*(unsigned long *)(buffer + offset)); offset += 4;
		n.Unsigned_Decode(buffer + offset, len); offset += len;
		len = ntohl(*(unsigned long *)(buffer + offset)); offset += 4;
		e.Unsigned_Decode(buffer + offset, len);
		if (Encrypt) {
			SecureRandomClass random;
			Key = random.Randval();
			Key <<= 24;
			Key ^= random.Randval();
		} else {
			Key = 0;
		}
		SendRC4.Prepare_Key((unsigned char *)&Key, 8);
		ReceiveRC4.Prepare_Key((unsigned char *)&Key, 8);
		BigInt zero;
		BigInt c;
		if (n != zero && e != zero) {
			RSAKeyClass key;
			key.Modulus = n;
			key.Exponent = e;
			BigInt m;
			unsigned __int64 mask = 1;
			for (unsigned int bit = 0; bit < 64; bit++) {
				if (Key & mask) {
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
			c = key.Encrypt(m);
		} else if (n == zero || e == zero) {
			c = BigInt((unsigned long)Key);
		}
		unsigned char encoded[0x200];
		int clen = c.Encode(encoded);
		*(unsigned long *)buffer = htonl(clen);
		memcpy(buffer + 4, encoded, clen);
		__int64 key = Key;
		Key = 0;
		Transport_Write(buffer, clen + 4);
		Key = key;
		return(true);
	}
















































































failed:
	Con->close(true);
	Unknown_4BC000("", "TCP", __FILE__, __LINE__, "CONNECT RSA EXCH FAILED: %d : %d", nbytes, ebytes);
	if (error != 0) {
		*error = -2;
	}
	return(true);
}


bool TCPConnection::Is_Connected()
{
	if (Con != 0 && Handle == 0) {
		return(Con->isConnected() != 0);
	}
	return(false);
}

bool TCPConnection::Disconnect()
{
	if (Handle != 0) {
		TCPManager.cancelConnect(Handle);
		Handle = 0;
	}
	if (Con != 0) {
		Con->pumpWrites();
		Con->close(true);
		delete Con;
		Con = 0;
	}
	return(true);
}

unsigned int TCPConnection::Get_Address()
{
	if (Con == 0) {
		return(0);
	}
	uint32 ip;
	uint16 port;
	Con->getRemoteAddr(&ip, &port);
	return(ip);
}

void TCPConnection::Close()
{
	Disconnect();
}


































int TCPConnection::Transport_Write(const void *buf, int len)
{
	G_ASSERT(buf);
	if (Con == 0) {
		return(-1);
	}
	unsigned char *allocated = 0;
	unsigned char *out;
	if (len <= (int)sizeof(WriteBuffer)) {
		out = WriteBuffer;
	} else {
		out = allocated = new unsigned char[len];
	}
	memcpy(out, buf, len);
	RC4Class saved;
	if (Key != 0) {
		saved = SendRC4;
		SendRC4.RC4(out, len);
	}
	int written = Con->write(out, len, 0);
	if (written != len && Key != 0) {
		SendRC4 = saved;
		if (written > 0) {
			memcpy(out, buf, written);
			SendRC4.RC4(out, written);
		}
	}
	if (allocated != 0) {
		delete [] allocated;
	}
	return(written);
}














































int TCPConnection::Transport_Read(void *buf, int len)
{
	G_ASSERT(buf);
	if (Con == 0) {
		return(-1);
	}
	Con->pumpWrites();
	int count = Con->read((uint8 *)buf, len, 0);
	if (Key != 0 && count > 0) {
		ReceiveRC4.RC4((unsigned char *)buf, count);
	}
	if (count == 0) {
		Disconnect();
		return(-1);
	}
	return(count == -1 ? 0 : count);
}






















TCPConnection::TCPConnection(SOCKET socket) :
	Con(0),
	Handle(0),
	Encrypt(true),
	Key(0),
	Name(0),
	IP(0),
	Port(0)
{
	G_ASSERT(socket != INVALID_SOCKET);
	Con = new TCPCon(socket);
	Con->isConnected();
	TCPManager.setBufferedWrites(Con, true);
	Con->setInputDelay(0);
	Con->setOutputDelay(0);
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	getpeername(socket, (sockaddr *)&addr, &addrlen);
	IP = addr.sin_addr.s_addr;
	Port = addr.sin_port;
	Name = nstrdup("unknown");
}


// TCPListenerClass (vtable 0xB08880): the server's TCP listener; its Pump_Pending 0x82AA10 (the key
// exchange's second step) isn't rebuilt. The key pair comes from 'server_rsa_keyset.dat'; every listener is in Listeners.
TCPListenerClass::TCPListenerClass() :
	Socket(INVALID_SOCKET),
	PortNumber(0)
{
	KeyPair.Load(file_auto_ptr(_TheFileFactory, "server_rsa_keyset.dat"));
	
	Listeners.insert(this);
}

bool TCPListenerClass::Is_Listening() { return(PortNumber != 0); }
unsigned short TCPListenerClass::Get_Port() { return(PortNumber); }


















































// Stop (inlined) if still listening, out of the set of listeners; Pending and the key pair go with it
TCPListenerClass::~TCPListenerClass()
{
	FAIL_IF(PortNumber != 0) {
		Stop();
	}
	Listeners.erase(this);
}











// NOT EXACT: ours merges the two `ret == SOCKET_ERROR` failure tails (lines 706 / 716); the client keeps both.
bool TCPListenerClass::Listen(unsigned short port)
{
	FAIL_IF(port == 0) return(false);


	FAIL_IF(Socket != INVALID_SOCKET) return(false);

	Socket = socket(AF_INET, SOCK_STREAM, 0);



	FAIL_IF(Socket == INVALID_SOCKET) return(false);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(Socket, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
		closesocket(Socket);
		Socket = INVALID_SOCKET;
		return(false);
	}

	int ret = listen(Socket, 256);




















	FAIL_IF(ret == SOCKET_ERROR) {
		closesocket(Socket);
		Socket = INVALID_SOCKET;
		return(false);
	}

	unsigned long on = 1;
	ret = ioctlsocket(Socket, FIONBIO, &on);


	FAIL_IF(ret == SOCKET_ERROR) {
		closesocket(Socket);
		Socket = INVALID_SOCKET;
		return(false);
	}
	PortNumber = port;
	return(true);
}

bool TCPListenerClass::Stop()
{




	FAIL_IF(Socket == INVALID_SOCKET) return(false);
	closesocket(Socket);
	Socket = INVALID_SOCKET;
	PortNumber = 0;
	return(true);
}

ConnectionClass *TCPListenerClass::Accept()
{
	return(Accept(true));
}

// A finished key exchange (pump), else a new socket: its connection starts the exchange (the
// server's key, Send_Key) and waits in Pending
ConnectionClass *TCPListenerClass::Accept(bool pump)
{
	if (Socket == INVALID_SOCKET) {
		return(0);
	}
	if (pump) {
		ConnectionClass *connection = Pump_Pending();
		if (connection != 0) {
			return(connection);
		}
	}
	SOCKET s = accept(Socket, 0, 0);
	if (s == INVALID_SOCKET) {
		return(0);
	}
	unsigned long on = 1;
	if (ioctlsocket(s, FIONBIO, &on) == SOCKET_ERROR) {
		closesocket(s);
		return(0);
	}
	TCPConnection *new_connection = new TCPConnection(s);














	FAIL_IF(!new_connection) return(0);
	Pending.push_back(new_connection);
	Send_Key(new_connection);
	return(0);
}

// The server's half, first step: its public key to a new connection, in the clear ({u32 n bytes, n,
// u32 e bytes, e}, network order); Pump_Pending takes the session key that comes back
void TCPListenerClass::Send_Key(TCPConnection *connection)
{
	BigInt n;
	BigInt e;
	const RSAKeyClass &key = KeyPair;
	n = key.Modulus;
	e = key.Exponent;
	unsigned char encoded[0x200];
	unsigned char buffer[0x408];
	int offset = 0;
	int len = n.Encode(encoded);
	*(unsigned long *)(buffer + offset) = htonl(len); offset += 4;
	memcpy(buffer + offset, encoded, len); offset += len;
	len = e.Encode(encoded);
	*(unsigned long *)(buffer + offset) = htonl(len); offset += 4;
	memcpy(buffer + offset, encoded, len); offset += len;
	connection->Transport_Write(buffer, offset);
}

// Every listener's Accept(false) (the pending connections' exchanges and new sockets); Key_Exchange
// inlines it while it waits
void TCPListenerClass::Pump_All()
{
	for (std::set<TCPListenerClass *>::iterator it = Listeners.begin(); it != Listeners.end(); ++it) {
		(*it)->Accept(false);
	}
}


/*
**	Reconstruction notes (tcpconn.cpp, PARTIAL; docs/kernel-map.md 4, step 8)
**
**	* The client's functions, in order: the static initialiser 0x828A10 (TCPManager, 0xBE9180) and
**	  two std::map helpers 0x828B10 / 0x828C40, the constructor by host name 0x828C80 (assert line 74),
**	  the scalar destructor 0x828DB0, the constructor by address 0x828DE0, the destructor
**	  0x828EC0, Connect 0x828F90 (assert 'Con == 0', line 119), Try_Connect 0x8290A0, the RSA key
**	  exchange 0x829180 (assert 'TCP', line 358; 'CONNECT RSA EXCH FAILED: %d : %d': not rebuilt),
**	  Is_Connected 0x829930, Disconnect 0x829960, Get_Address 0x8299D0, Close 0x829A10,
**	  Transport_Write 0x829A30 (line 440), Transport_Read 0x829BC0 (line 518), the constructor over an
**	  accepted socket 0x829C80 (line 564); the listener 0x829E00-0x82AA10 (lines 645-780, the
**	  server's): the constructor 0x829E50 (the key file), destructor, Listen 0x82A470 (functional), Stop, Accept and
**	  Send_Key 0x82A830, Pump_All 0x82A990 rebuilt; Pump_Pending 0x82AA10 (the server's exchange) not. The exchange 0x829180 carries a marker.
**	* The TCP layer is WWOnline wnet's TCPMgr / TCPCon (libs\wwonline\wnet, D73): E&B's TCPCon is an
**	  OutputDevice with a lock and tcpqueue.cpp's queues (wnet's tcpcon.cpp and tcpqueue.cpp),
**	  TCPMgr::cancelConnect 0x8BD940 is E&B's.
**	* Key (+0x30) is the session key from the exchange; while it is 0 the bytes go in the clear.
**	  Transport_Write encrypts a copy (in WriteBuffer up to 0x400 bytes) with SendRC4 and, when
**	  TCPCon::write takes fewer bytes, rewinds the RC4 state and encrypts only what went.
*/
