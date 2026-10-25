/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	The UDP socket CordMgr and CordCon use (CordCon::Udp_, +0x10). The client's class is Westwood's
**	wnet UDP (libs\wwonline\wnet, the library at 0x8BF990-0x8C0540 beside tcpqueue.cpp's
**	__FILE__): ctor 0x8BF990, dtor 0x8BFA10, Bind 0x8BFB70, getLocalAddr 0x8BFCD0, Write 0x8BFD50
**	(sendto), Read 0x8BFE30 (recvfrom), Wait 0x8C0220 (select), SetInputBuffer / SetOutputBuffer
**	0x8C04C0 / 0x8C0500 (setsockopt). Its interface is EA's (Generals Tools\matchbot\wnet\udp.h);
**	this header declares only the part the wwudp code calls, so that a program can link either
**	the real socket (wnet's udp.cpp, not imported yet) or the capture harness's replay socket
**	(tests/capharness/udpreplay).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UDP_HEADER  // NAME 4 [udp.h]: wnet's udp.h (EA's Generals Tools matchbot wnet); its place here assumed
#define UDP_HEADER

#include <winsock.h>

class UDP
{
public:
	UDP();
	~UDP();

	int Bind(unsigned int ip, unsigned short port);
	// Sends len bytes to ip:port (host order). Returns the bytes sent, or < 0.
	int Write(unsigned char *msg, unsigned int len, unsigned int ip, unsigned short port);
	// Reads one datagram (at most len bytes) and its sender. Returns its length, 0, or < 0 when
	// there is nothing to read (CordMgr::Pump reads until < 0).
	int Read(unsigned char *msg, unsigned int len, sockaddr_in *from);
	bool getLocalAddr(unsigned int &ip, unsigned short &port);

private:
	int fd;
	unsigned int myIP;
	unsigned short myPort;
	sockaddr_in addr;
};

#endif
