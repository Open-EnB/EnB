// Earth & Beyond client (reconstruction): the composite listener, client 0x820B30-0x820EE5.
// PROVISIONAL file name (D10): no assert or __FILE__ names the file. Its functions sit between the
// rate average 0x820830-0x820B20 (unknown_rate.h, likely the same object) and connect.cpp's static
// initialiser 0x820F30, and the object ends with <string>'s guarded static 0x820F00.
#include "connect.h"
#include "tcpconn.h"
#include "udpconn.h"
#include "memconn.h"

// One port on every transport: TCP, UDP and in-process
CompositeListenerClass::CompositeListenerClass() :
	TCP(0),
	UDP(0),
	Memory(0)
{
	TCP = new TCPListenerClass;
	UDP = new UDPListenerClass;
	Memory = new MemoryListenerClass;
}

CompositeListenerClass::~CompositeListenerClass()
{
	delete TCP;
	TCP = 0;
	delete UDP;
	UDP = 0;
	delete Memory;
	Memory = 0;
}

bool CompositeListenerClass::Is_Listening()
{
	return(TCP->Is_Listening() && UDP->Is_Listening() && Memory->Is_Listening());
}

unsigned short CompositeListenerClass::Get_Port()
{
	return(TCP->Get_Port());
}

// All three or none
bool CompositeListenerClass::Listen(unsigned short port)
{
	if (!UDP->Listen(port)) {
		return(false);
	}
	if (!TCP->Listen(port)) {
		UDP->Stop();
		return(false);
	}
	if (!Memory->Listen(port)) {
		TCP->Stop();
		UDP->Stop();
		return(false);
	}
	return(true);
}

bool CompositeListenerClass::Stop()
{
	bool tcp = TCP->Stop();
	bool udp = UDP->Stop();
	bool memory = Memory->Stop();
	return(tcp && udp && memory);
}

ConnectionClass *CompositeListenerClass::Accept()
{
	ConnectionClass *connection = TCP->Accept();
	if (connection == 0) {
		connection = UDP->Accept();
		if (connection == 0) {
			connection = Memory->Accept();
		}
	}
	return(connection);
}
