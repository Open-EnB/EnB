// Earth & Beyond client (reconstruction)
// udpreplay: stand-ins for what connect.cpp's factories call outside the replay's sources (tcpconn.cpp,
// memconn.cpp, the composite listener, dbgmgr.cpp). The replay
// never calls the factories, so each stops the program if it is reached. The classes are declared
// here with only the member defined, so no vtable is emitted (the names mangle as the real ones).
#include <stdio.h>
#include <stdlib.h>

static void Not_In_Replay(const char *what)
{
	printf("udpreplay: %s is a stand-in (connect_stubs.cpp)\n", what);
	exit(3);
}

class TCPConnection { public: TCPConnection(const char *host, unsigned short port, bool encrypt); };
TCPConnection::TCPConnection(const char *, unsigned short, bool) { Not_In_Replay("TCPConnection"); }

class CompositeListenerClass { public: CompositeListenerClass(); };
CompositeListenerClass::CompositeListenerClass() { Not_In_Replay("CompositeListenerClass"); }


class ConnectionClass;
class MemoryConnectionFactoryClass
{
public:
	bool Is_Listening(unsigned short port);
	ConnectionClass *Connect(unsigned short port);
};
bool MemoryConnectionFactoryClass::Is_Listening(unsigned short) { return(false); }
ConnectionClass *MemoryConnectionFactoryClass::Connect(unsigned short) { Not_In_Replay("memconn"); return(0); }

void Unknown_Debug_Begin_Step(const char *text) { printf("step: %s\n", text); }
void Unknown_Debug_End_Step(const char *text) { printf("step end: %s\n", text); }
void Unknown_Debug_Print(const char *text) { printf("log: %s\n", text); }
