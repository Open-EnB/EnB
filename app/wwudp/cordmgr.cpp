/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordmgr.cpp (client 0x857590-0x85AED0)
**
**	Functional (NOT EXACT, D42): the data path only (cordmgr.h). The client's asserts 'UDP'
**	(lines 592, 618, 1272) are not on their lines.
*/

#include "cordmgr.h"
#include "cordcon.h"
#include "udp.h"
#include <string.h>

CordMgr::CordMgr() :
	Unexpected_(0)
{
}

CordMgr::~CordMgr()
{
}

// 0x857F20: each socket until Read says there is nothing more (< 0); 0x400-byte buffer.
void CordMgr::Pump()
{
	unsigned char buf[0x400];
	for (size_t s = 0; s < Sockets_.size(); s++) {
		UDP *udp = Sockets_[s];
		sockaddr_in from;
		int len;
		while ((len = udp->Read(buf, sizeof(buf), &from)) >= 0) {
			if (len <= 3) {
				continue;		// (len 0: the client's 0x858980, a connection reset)
			}
			unsigned char type = buf[0];
			bool taken = false;
			if (type == 4 || type == 5) {
				taken = Process_CORD_Packet(buf, len, ntohl(from.sin_addr.s_addr), ntohs(from.sin_port), type);
			}
			// 0 / 1 / 2 / 3 (the handshake) and 6 (go away) are not rebuilt
			if (!taken) {
				Unexpected_++;		// the client logs 'Unexpected packet: ...' and answers with type 6
			}
		}
	}
}

// 0x85A000
bool CordMgr::Process_CORD_Packet(unsigned char *data, int len, unsigned int ip, unsigned short port, int type)
{
	if (len < 0xC) {
		return false;
	}
	unsigned short session = (unsigned short)((data[1] << 8) | data[2]);
	for (size_t i = 0; i < Connections_.size(); i++) {
		CordCon *con = Connections_[i];
		if (con->Ip_ != ip || (unsigned short)con->Session_ != session) {
			continue;
		}
		// the client's side takes type 4 (from the server), the server's type 5
		if ((type == 4 && !con->IsServer_) || (type == 5 && con->IsServer_)) {
			if (con->handlePacket(data, len)) {
				con->Port_ = port;
				return true;
			}
			// the client logs 'Packet failed validation (%p : %d (key=%d))'
			return true;
		}
	}
	return false;
}
