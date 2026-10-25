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
	Unexpected_(0)
{
	memset(Unknown00, 0, sizeof(Unknown00));
	memset(Unknown1C, 0, sizeof(Unknown1C));
}

CordMgr::~CordMgr()
{
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
			if (len <= 3) {
				continue;
			}
			char type = (char)buf[0];
			bool taken = false;
			if ((type == 4 || type == 5) && !listen) {
				taken = Process_CORD_Packet(buf, len, &from, udp, 0, type);
			} else if (type == 0 && listen) {
				taken = Process_SYN1_Packet(buf, len, &from, udp, 0);
			} else if (type == 2 && listen) {
				taken = Process_SYN2_Packet(buf, len, &from, udp, 0);
			}
			if (!taken) {
				Unexpected_++;



















































































































































































































































































































































































































































































































				UnknownLog.Unknown_4BC130("UDP", __FILE__, __LINE__, "Unexpected packet: (%d) (%s : %d) -> (%d [%d]) (%s)",
					len, inet_ntoa(from.sin_addr), ntohs(from.sin_port), 0, 0, "");
			}
		}
	}
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
