// Earth & Beyond client (reconstruction): the registry of memory listeners by port, client
// 0x827560-0x828A00: Servers' initialiser 0x827560 (a file's first function), the five members
// 0x827700-0x827B10 and the std::map instances. PROVISIONAL file name (D10): it starts after
// memconn.cpp's template instances and memconn.cpp calls it out of line, so it is another file;
// no assert or string names it. Register and Connect test through Is_Listening (inlined).
#include "memconn.h"
#include <map>

// The memory listeners by port (0xBE9160). Its atexit stub 0x8275A0 is guarded (0xBE9170 bit 1), which VC6 gives a class
// static member, not a file static; the class is probably MemoryConnectionFactoryClass (its members are the users),
// held here by a local class (interim, D137) because memconn.h reaches sclient.cpp (pad-sensitive)
struct MemoryServerRegistryClass { static std::map<unsigned short, MemoryServerClass *> Servers; }; std::map<unsigned short, MemoryServerClass *> MemoryServerRegistryClass::Servers;	// NAME 2: what it holds; the holder's name provisional

bool MemoryConnectionFactoryClass::Register(unsigned short port, MemoryServerClass *server)
{
	if (Is_Listening(port)) {
		return(false);
	}
	MemoryServerRegistryClass::Servers[port] = server;
	return(true);
}

bool MemoryConnectionFactoryClass::Is_Listening(unsigned short port)
{
	return(MemoryServerRegistryClass::Servers.find(port) != MemoryServerRegistryClass::Servers.end());
}

bool MemoryConnectionFactoryClass::Unregister(unsigned short port)
{
	MemoryServerRegistryClass::Servers.erase(port);
	return(true);
}

bool MemoryConnectionFactoryClass::Unregister_All()
{
	MemoryServerRegistryClass::Servers.clear();
	return(true);
}

ConnectionClass *MemoryConnectionFactoryClass::Connect(unsigned short port)
{
	if (!Is_Listening(port)) {
		return(0);
	}
	return(MemoryServerRegistryClass::Servers[port]->Connect());
}
