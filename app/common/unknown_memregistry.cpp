// Earth & Beyond client (reconstruction): the registry of memory listeners by port, client
// 0x827560-0x828A00: Servers' initialiser 0x827560 (a file's first function), the four members
// 0x827700-0x827B10 and the std::map instances. PROVISIONAL file name (D10): it starts after
// memconn.cpp's template instances and memconn.cpp calls it out of line, so it is another file;
// no assert or string names it. Register and Connect test through Is_Listening (inlined).
#include "memconn.h"
#include <map>

// The memory listeners by port (0xBE9160)
static std::map<unsigned short, MemoryServerClass *> Servers;	// NAME 2: what it holds

bool MemoryConnectionFactoryClass::Register(unsigned short port, MemoryServerClass *server)
{
	if (Is_Listening(port)) {
		return(false);
	}
	Servers[port] = server;
	return(true);
}

bool MemoryConnectionFactoryClass::Is_Listening(unsigned short port)
{
	return(Servers.find(port) != Servers.end());
}

bool MemoryConnectionFactoryClass::Unregister(unsigned short port)
{
	Servers.erase(port);
	return(true);
}

ConnectionClass *MemoryConnectionFactoryClass::Connect(unsigned short port)
{
	if (!Is_Listening(port)) {
		return(0);
	}
	return(Servers[port]->Connect());
}
