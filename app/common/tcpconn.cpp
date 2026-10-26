// Earth & Beyond client (reconstruction): app\common\tcpconn.cpp (PARTIAL). Notes at the end of the file.
#include "tcpconn.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\nstrdup.h"
#include <string.h>
#include <winsock.h>

// The one TCP manager (0xBE9180, constructed by the static initialiser 0x828A10) and the buffer
// small writes are encrypted in (0xBE91B8).
static TCPMgr TCPManager;		// NAME 2: what it is
static unsigned char WriteBuffer[0x400];	// NAME 2: what it holds

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

// NOT EXACT: the client keeps two copies of `Handle = 0; return(true);` (0x829108, 0x829123); ours shares one.
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
		if (!TCPManager.getOutgoingConnection(&Con, Handle, 0)) {
			return(false);
		}
		if (Con != 0) {
			if (!Key_Exchange(false, 0)) {
				return(false);
			}
			Handle = 0;
			return(true);
		}
		Handle = 0;
		return(true);
	}
	if (!Key_Exchange(false, 0)) {
		return(false);
	}
	Handle = 0;
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


/*
**	Reconstruction notes (tcpconn.cpp, PARTIAL; docs/kernel-map.md 4, step 8)
**
**	* The client's functions, in order: the static initialiser 0x828A10 (TCPManager, 0xBE9180) and
**	  two std::map helpers 0x828B10 / 0x828C40, the constructor by host name 0x828C80 (assert line 74:
**	  not rebuilt), the scalar destructor 0x828DB0, the constructor by address 0x828DE0, the destructor
**	  0x828EC0, Connect 0x828F90 (assert 'Con == 0', line 119), Try_Connect 0x8290A0, the RSA key
**	  exchange 0x829180 (assert 'TCP', line 358; 'CONNECT RSA EXCH FAILED: %d : %d': not rebuilt),
**	  Is_Connected 0x829930, Disconnect 0x829960, Get_Address 0x8299D0, Close 0x829A10,
**	  Transport_Write 0x829A30 (line 440), Transport_Read 0x829BC0 (line 518), the constructor over an
**	  accepted socket 0x829C80 (line 564) and the listener with its key file 'server_rsa_keyset.dat'
**	  0x829E00-0x82AA10 (lines 645-780, the server's): not rebuilt.
**	* The TCP layer is WWOnline wnet's TCPMgr / TCPCon (libs\wwonline\wnet, D73): E&B's TCPCon is an
**	  OutputDevice with a lock and tcpqueue.cpp's queues (wnet's tcpcon.cpp and tcpqueue.cpp),
**	  TCPMgr::cancelConnect 0x8BD940 is E&B's.
**	* Key (+0x30) is the session key from the exchange; while it is 0 the bytes go in the clear.
**	  Transport_Write encrypts a copy (in WriteBuffer up to 0x400 bytes) with SendRC4 and, when
**	  TCPCon::write takes fewer bytes, rewinds the RC4 state and encrypts only what went.
*/
