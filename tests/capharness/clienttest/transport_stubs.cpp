// clienttest: stand-ins for the transports connect.cpp's factories name and the tests never open (as
// udpreplay's connect_stubs.cpp: the classes are declared here with only the constructor, so no vtable
// is emitted and the names mangle as the real ones). Each stops the program if it is reached.
#include <stdio.h>
#include <stdlib.h>
#include "clienttest.h"


static void Not_In_Test(const char *what)
{
	Trace("stand-in reached: %s", what);
	printf("clienttest: %s is a stand-in (transport_stubs.cpp)\n", what);
	exit(3);
}
class TCPConnection { public: TCPConnection(const char *host, unsigned short port, bool encrypt); };
TCPConnection::TCPConnection(const char *, unsigned short, bool) { Not_In_Test("TCPConnection"); }
class UDPConnection { public: UDPConnection(const char *host, unsigned short port, bool encrypt); };
UDPConnection::UDPConnection(const char *, unsigned short, bool) { Not_In_Test("UDPConnection"); }
class CompositeListenerClass { public: CompositeListenerClass(); };
CompositeListenerClass::CompositeListenerClass() { Not_In_Test("CompositeListenerClass"); }
