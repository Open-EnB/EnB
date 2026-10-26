// Earth & Beyond client (reconstruction): app\common\connect.cpp (PARTIAL). Notes at the end of the file.
#include "connect.h"
#include "..\packets\pk.h"
#include "..\shared\unknown_debug.h"
#include <string.h>
#include "..\..\wwlib\nstrdup.h"

ConnectionClass::ConnectionClass()
{
	Unknown04 = true;
}


ConnectionClass::~ConnectionClass()
{
}

// The header Receive reads (0xBE8EA0).
static PacketHeaderClass Header;

// (spacing kept throughout: the asserts are on the client's lines, D25)

#include "dbgconn.h"
#include "..\..\wwlib\argv.h"

#include "udpconn.h"
#include "tcpconn.h"
#include "memconn.h"
#include "..\..\wwlib\ini.h"
#include <winsock.h>
#include <stdio.h>

void Unknown_Debug_Begin_Step(const char *text);	// NAME 1 [Unknown_Debug_Begin_Step]: 0x509EF0 (dbgmgr.cpp), WinMain's step log
void Unknown_Debug_End_Step(const char *text);		// NAME 1 [Unknown_Debug_End_Step]: 0x50A1E0 (dbgmgr.cpp)

static INIClass Network_INI;			// 0xBE8EA8: network.ini (S: the assert l.395)
static char Host_Name[256];				// 0xBE8EBC: gethostname's  // NAME 2: what it holds
static char Local_Address[16];			// 0xBE8FBC: the local address as text  // NAME 2: what it holds
static unsigned long Local_IP = 0xFFFFFFFF;	// 0xBA82A8 (host order; 0 when the host name doesn't resolve)  // NAME 2: what it holds

INIClass &Get_Network_INI();												// 0x821B40  // NAME 2: what it does
bool Is_Local(const char *name);										// 0x821930  // NAME 2: what it does

static unsigned short Next_Port = 3500;	// 0xBA82AC: the next port the manager factory probes  // NAME 2: what it holds
void Unknown_Debug_Printf(const char *format, ...);	// 0x509DC0 (dbgmgr.cpp, D38)
void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line





























// The connection to a server named in network.ini (MasterServer, ChatServer, SectorServer ...):
// its section's Name and Port, or -SERVER_ADDR / -GLOBAL_ADDR's address
ConnectionClass *Create_Connection(const char *server_type, bool connect)	// NAME 2: what it does
{
	G_ASSERT(server_type);
	INIClass &ini = Get_Network_INI();
	char address[32];
	const char *server_addr = ArgvClass().Find_Value("-SERVER_ADDR");
	if (stricmp(server_type, "MasterServer") == 0 && server_addr != 0) {
		strcpy(address, server_addr);
	} else if (stricmp(server_type, "RegisterServer") == 0 && server_addr != 0) {
		strcpy(address, server_addr);
	} else if (stricmp(server_type, "ChatServer") == 0 && server_addr != 0) {
		strcpy(address, server_addr);
	} else if (stricmp(server_type, "ChatServer_Basic") == 0 && server_addr != 0) {
		strcpy(address, server_addr);
	} else if (stricmp(server_type, "GroupServer") == 0 && server_addr != 0) {
		strcpy(address, server_addr);
	} else if (stricmp(server_type, "GuildServer") == 0 && server_addr != 0) {
		strcpy(address, server_addr);
	} else {
		ini.Get_String(server_type, "Name", "localhost", address, sizeof(address));
	}
	const char *global_addr = ArgvClass().Find_Value("-GLOBAL_ADDR");
	if (global_addr == 0) {
		global_addr = ArgvClass().Find_Value("-SERVER_ADDR");
	}
	if ((stricmp(server_type, "GlobalServer_Directory") == 0 && global_addr != 0) ||
		(stricmp(server_type, "GlobalServer_Client") == 0 && global_addr != 0) ||
		(stricmp(server_type, "GlobalServer_Register") == 0 && global_addr != 0) ||
		(stricmp(server_type, "GlobalServer_Parent") == 0 && global_addr != 0)) {
		strcpy(address, global_addr);
	}
	int port = ini.Get_Int(server_type, "Port", 6666);
	return(Create_Connection(address, port, true, false, server_type, connect));
}

// The same by address (0x821400), unlogged
ConnectionClass *Create_Connection(unsigned long ip, unsigned short port, bool encrypt, bool connect)	// NAME 2: what it does
{
	in_addr address;
	address.s_addr = htonl(ip);
	const char *name = inet_ntoa(address);
	if (name == 0) name = "127.0.0.1";
	return(Create_Connection(name, port, encrypt, false, 0, connect));
}







































// The connection itself: -PROTOCOL picks the transport (MEMORY when the server runs in this process,
// UDP, else TCP; ChatServer and GlobalServer_Client always TCP), then the optional wrappers
ConnectionClass *Create_Connection(const char *name, unsigned short port, bool encrypt, bool log, const char *server_type, bool connect)	// NAME 2: what it does
{
	G_ASSERT(name);
	char buffer[256];
	sprintf(buffer, "connecting to %s on port %d", name, port);
	Unknown_Debug_Begin_Step(buffer);

	const char *protocol = ArgvClass().Find_Value("-PROTOCOL");
	bool memory = false;
	if (protocol == 0 || stricmp(protocol, "MEMORY") == 0) {
		memory = true;
	}

	if (server_type != 0) {
		if (stricmp(server_type, "ChatServer") == 0) {
			memory = false;
			protocol = "TCP";
		}
		if (stricmp(server_type, "GlobalServer_Client") == 0) {
			memory = false;
			protocol = "TCP";
		}
	}

	ConnectionClass *new_connection;
	MemoryConnectionFactoryClass memory_factory;





	if (Is_Local(name) && memory_factory.Is_Listening(port) && memory) {
		new_connection = memory_factory.Connect(port);
		G_ASSERT(new_connection);
	} else {
		if (protocol != 0 && stricmp(protocol, "UDP") == 0) {
			new_connection = new UDPConnection(name, port, encrypt);
		} else {
			new_connection = new TCPConnection(name, port, encrypt);
		}






		G_ASSERT(new_connection);
		if (connect) {
			new_connection->Connect(connect);
		}
	}

	if (log) {
		const char *type = server_type;
		if (type == 0) type = "client";

		sprintf(buffer, "%s-%s.%d", type, name, port);
		new_connection = new TimedLogConnectionClass(new_connection, buffer);


		G_ASSERT(new_connection);
	}


	bool stats = ArgvClass().Find("-PACKET_STATS") != 0;
	if (stats) {
		new_connection = new StatsConnectionClass(new_connection);
		G_ASSERT(new_connection);
	}
	Unknown_Debug_End_Step("done");
	return(new_connection);
}





// This host's name (0x821900) and address (0x821890), asked once
const char *Get_Host_Name()	// NAME 2: what it does
{
	if (Host_Name[0] == 0) {
		gethostname(Host_Name, sizeof(Host_Name));
	}
	return(Host_Name);
}

unsigned long Get_Local_IP()	// NAME 2: what it does
{
	if (Local_IP == 0xFFFFFFFF) {
		struct hostent *host = gethostbyname(Get_Host_Name());
		if (host == 0) {
			Local_IP = 0;
			return(0);
		}
		Local_IP = ntohl(*(unsigned long *)host->h_addr_list[0]);
	}
	return(Local_IP);
}





























































































// Whether name is this host: its address, its name, localhost or 127.0.0.1
bool Is_Local(const char *name)
{
	G_ASSERT(name);
	if (Local_Address[0] == 0) {
		in_addr address;
		address.s_addr = htonl(Get_Local_IP());
		const char *text = inet_ntoa(address);
		if (text != 0) {
			strcpy(Local_Address, text);
		} else {
			strcpy(Local_Address, "127.0.0.1");
		}
	}
	if (strcmp(name, Local_Address) == 0) return(true);
	if (stricmp(name, Get_Host_Name()) == 0) return(true);
	if (stricmp(name, "localhost") == 0) return(true);
	if (strcmp(name, "127.0.0.1") == 0) return(true);
	return(false);
}
















// network.ini, loaded on first use
INIClass &Get_Network_INI()
{
	if (!Network_INI.Is_Loaded()) {
		Network_INI.Load("network.ini");
		G_ASSERT(Network_INI.Is_Loaded());
	}
	return(Network_INI);
}












// ForwardingConnectionClass (vtable 0xB08590): every slot passes to the wrapped connection
PacketClass *ForwardingConnectionClass::Receive()
{
	if (Connection == 0) return(0);
	PacketClass *packet = Connection->Receive();
	if (packet != 0) packet->Connection = this;
	return(packet);
}

bool ForwardingConnectionClass::Get_Stats(float *a, float *b, float *c, float *d, float *e, float *f, int *g)
{
	if (Connection == 0) return(false);
	return(Connection->Get_Stats(a, b, c, d, e, f, g));
}

// PeekConnectionClass (vtable 0xB0854C)
PeekConnectionClass::PeekConnectionClass(ConnectionClass *connection) :
	ForwardingConnectionClass(connection),
	Last_Packet(0)
{
}

bool ForwardingConnectionClass::Send_Message(RawPacketClass *packet) { return(Connection->Send_Message(packet)); }
bool ForwardingConnectionClass::Send_Packet(PacketClass *packet) { return(Connection->Send_Packet(packet)); }
unsigned int ForwardingConnectionClass::Get_Address() { if (Connection != 0) return(Connection->Get_Address()); return(0); }
void ForwardingConnectionClass::Set_Unknown04(bool on) { if (Connection != 0) Connection->Set_Unknown04(on); }
bool ForwardingConnectionClass::Get_Unknown04() { if (Connection != 0) return(Connection->Get_Unknown04()); return(false); }
bool ForwardingConnectionClass::Connect(bool wait) { if (Connection != 0) return(Connection->Connect(wait)); return(false); }
bool ForwardingConnectionClass::Is_Connected() { if (Connection != 0) return(Connection->Is_Connected()); return(false); }
bool ForwardingConnectionClass::Disconnect() { if (Connection != 0) return(Connection->Disconnect()); return(true); }
bool ForwardingConnectionClass::Is_Dead() { if (Connection != 0) return(Connection->Is_Dead()); return(true); }
bool ForwardingConnectionClass::Unknown_30(int a, int b) { if (Connection != 0) return(Connection->Unknown_30(a, b)); return(false); }

ForwardingConnectionClass::~ForwardingConnectionClass()
{
	delete Connection;
	Connection = 0;
}

PeekConnectionClass::~PeekConnectionClass()
{
	if (Last_Packet != 0) {
		delete Last_Packet;
		Last_Packet = 0;
	}
}

PacketClass *PeekConnectionClass::Peek()
{
	if (Last_Packet != 0) return(Last_Packet);
	Last_Packet = ForwardingConnectionClass::Receive();
	return(Last_Packet);

}

PacketClass *PeekConnectionClass::Receive()
{
	PacketClass *packet = Peek();
	if (packet == 0) return(0);
	G_ASSERT(Last_Packet);
	Last_Packet = 0;
	return(packet);
}

bool PeekConnectionClass::Is_Dead()
{
	return(Last_Packet == 0 && ForwardingConnectionClass::Is_Dead());
}







// ConnectionManagerClass (vtable 0xB085D0, 0x28 bytes): connections polled round robin
ConnectionManagerClass::ConnectionManagerClass() :
	RoundRobin(0),
	Current(0),
	Unknown1C(false),
	Name(0),
	Listener(0)
{
}

ConnectionManagerClass::~ConnectionManagerClass()
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		it->Connection->Disconnect();
		delete it->Connection;
	}
	Connections.clear();
	if (Listener != 0) {
		if (Listener->Is_Listening()) {
			Listener->Stop();
		}
		delete Listener;
		Listener = 0;
	}
	Listener = 0;
	delete [] Name;
	Name = 0;
}

unsigned short ConnectionManagerClass::Get_Port()
{
	if (Listener != 0) {
		return(Listener->Get_Port());
	}
	return(0);
}

void ConnectionManagerClass::Set_Name(const char *name)
{
	FAIL_IF(!name) return;
	if (Name != 0) {
		delete [] Name;
		Name = 0;
	}
	Name = nstrdup(name);
}

void ConnectionManagerClass::Set_Listener(ConnectionListenerClass *listener)
{
	if (Listener != 0) {
		if (Listener->Is_Listening()) {
			Listener->Stop();
		}
		delete Listener;
		Listener = 0;
	}
	Listener = listener;
}


void ConnectionManagerClass::Accept_New(std::list<ConnectionClass *> *accepted)
{
	while (Listener != 0) {
		ConnectionClass *connection = Listener->Accept();
		if (connection == 0) return;
		if (!Add_Connection(connection)) {
			connection->Disconnect();
			delete connection;
			return;
		}
		if (accepted != 0) {
			accepted->push_back(connection);
		}
	}
}
















































PacketClass *ConnectionManagerClass::Peek(bool next)
{
	int count = Connections.size();
	for (int i = 0; i < count; i++) {
		int index;
		if (next) index = (RoundRobin + 1 + i) % count;
		else index = (RoundRobin + i) % count;
		ConnectionEntryClass &entry = Connections[index];
		if (!entry.Marked && !entry.Connection->Is_Dead()) {
			PacketClass *packet = entry.Connection->Peek();
			if (packet != 0) {
				RoundRobin = index;
				Current = entry.Connection;
				return(packet);
			}
		}
	}
	return(0);
}



PacketClass *ConnectionManagerClass::Get_Packet(bool next)
{
	PacketClass *retval = Peek(next);
	if (retval == 0) {
		return(0);
	}
	PacketClass *testpacket = Connections[RoundRobin].Connection->Receive();
	G_ASSERT(retval == testpacket);
	return(retval);
}

PacketClass *ConnectionManagerClass::Receive()
{
	if (RoundRobin < (int)Connections.size()) {
		ConnectionEntryClass &entry = Connections[RoundRobin];
		if (entry.Connection == Current) return(entry.Connection->Receive());
	}
	return(0);
}

int ConnectionManagerClass::Broadcast(PacketClass *packet)
{
	int count = 0;
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection != 0 && it->Connection->Is_Connected()) {
			it->Connection->Send_Packet(packet);
			count++;
		}
	}
	return(count);
}

bool ConnectionManagerClass::Mark(PacketClass *packet)
{
	int count = Connections.size();
	for (int i = 0; i < count; i++) {
		ConnectionEntryClass &entry = Connections[i];
		if (entry.Connection->Peek() == packet) {
			entry.Marked = true;
			return(true);
		}
	}
	return(false);
}

void ConnectionManagerClass::Clear_Marks()
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		it->Marked = false;
	}
}




























bool ConnectionManagerClass::Set_Unknown05(ConnectionClass *connection, bool on)
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			bool old = it->Unknown05;
			it->Unknown05 = on;
			return(old);
		}
	}
	G_ASSERT(false);
	return(false);
}


bool ConnectionManagerClass::Remove_Connection(ConnectionClass *connection)
{
	FAIL_IF(!connection) return(false);
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			if (it - Connections.begin() < RoundRobin) {
				RoundRobin--;
			}
			Connections.erase(it);
			return(true);
		}
	}
	return(false);
}






bool ConnectionManagerClass::Add_Unwrapped(PeekConnectionClass *connection)
{
	FAIL_IF(!connection) return(false);
	Connections.push_back(ConnectionEntryClass(connection));
	return(true);
}









bool ConnectionManagerClass::Destroy_Connection(ConnectionClass *connection)
{
	FAIL_IF(!connection) return(false);
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			if (it - Connections.begin() < RoundRobin) {
				RoundRobin--;
			}
			Connections.erase(it);
			connection->Disconnect();
			delete connection;
			return(true);
		}
	}
	return(false);
}

bool ConnectionManagerClass::Destroy_All()
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		it->Connection->Disconnect();
		delete it->Connection;
	}
	Connections.clear();
	return(true);
}





bool ConnectionManagerClass::Is_Managed(ConnectionClass *connection)
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			return(true);
		}
	}
	return(false);
}

// NOT EXACT: the client calls the list's element construct 0x823DB0 out of line (on a copy of the pointer); ours inlines it.
bool ConnectionManagerClass::Remove_Dead(std::list<ConnectionClass *> *removed)
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end();) {
		if (!it->Unknown05 && it->Connection->Is_Dead()) {
			if (removed != 0) {
				removed->push_back(it->Connection);
			}
			if (it - Connections.begin() < RoundRobin) {
				RoundRobin--;
			}
			delete it->Connection;
			it = Connections.erase(it);
		} else {
			it++;
		}
	}
	return(true);
}


















bool ConnectionManagerClass::Add_Connection(ConnectionClass *connection)
{
	FAIL_IF(!connection) return(false);
	bool stats = ArgvClass().Find("-PACKET_STATS") != 0;
	PeekConnectionClass *peek;
	if (Unknown1C) {
		peek = new PeekConnectionClass(new TimedLogConnectionClass(connection, Name));
	} else if (stats) {
		peek = new PeekConnectionClass(new StatsConnectionClass(connection));
	} else {
		peek = new PeekConnectionClass(connection);
	}
	Connections.push_back(ConnectionEntryClass(peek));
	return(true);
}












// A manager listening for server_type: on its network.ini port, else on the first free one from
// Next_Port (3500-3754)
ConnectionManagerClass *Create_Connection_Manager(const char *server_type)
{
	INIClass &ini = Get_Network_INI();
	ConnectionListenerClass *listener = new CompositeListenerClass;
	unsigned short port;
	if (server_type != 0 && (port = ini.Get_Int(server_type, "Port", 0)) != 0) {
		bool success = listener->Listen(port);
		FAIL_IF(!success) {
			delete listener;
			return(0);
		}
	} else {
		bool success = false;
		int tries = 255;


		while (tries--) {
			if (success) break;
			Unknown_Debug_Printf("ConnectionManager: probing port %hd for server %s ", Next_Port, server_type);
			success = listener->Listen(Next_Port);
			if (success) {
				Unknown_Debug_Printf("ConnectionManager: port %hd is available!", Next_Port);
			} else {
				Unknown_Debug_Printf("ConnectionManager: port %hd is not available.", Next_Port);
			}
			Next_Port++;
			if (Next_Port > 3754) {
				Next_Port = 3500;
			}
		}
		FAIL_IF(!success) {
			delete listener;
			return(0);
		}
	}

	char buffer[100];
	sprintf(buffer, "%s is using composite manager\n", server_type);
	Unknown_Debug_Print(buffer);
	ConnectionManagerClass *manager = new ConnectionManagerClass;

	G_ASSERT(manager);
	manager->Set_Listener(listener);
	if (server_type != 0) {
		manager->Set_Name(server_type);
	} else {
		manager->Set_Name("Unknown");
	}
	return(manager);
}




































































































StreamConnectionClass::StreamConnectionClass() : Ring(100000)
{
	ReadBuffer = new char[0x1000];
}

// 0x8233E0: exact since pk.h holds the full packet base and <string> (before, its unwind states were one lower; cause not isolated)
StreamConnectionClass::~StreamConnectionClass()
{
	delete ReadBuffer;
}


bool StreamConnectionClass::Write_Message(RawPacketClass *packet)
{
	if (Transport_Write(&packet->Header, 4) != 4) {
		return(false);
	}
	int bytes = packet->Header.Size - 4;
	if (bytes) {
		char *data = packet->Data.Buffer;
		if (Transport_Write(data, bytes) != bytes) {
			return(false);
		}
	}
	return(true);
}


bool StreamConnectionClass::Send_Packet(PacketClass *packet)
{
	RawPacketClass *raw = packet->Build();
	bool ok = Write_Message(raw);
	delete raw;
	return(ok);
}


bool StreamConnectionClass::Send_Message(RawPacketClass *packet)
{
	return(Write_Message(packet));
}


bool StreamConnectionClass::Message_Ready()
{
	Fill();
	PacketHeaderClass header;
	unsigned int got = Ring.Read(&header, sizeof(header));
	if (got < sizeof(header)) {
		Ring.Unread(got);
		return(false);
	}
	int bytes = header.Size - 4;
	if (bytes < 0) {
		Ring.Unread(got);
		return(false);
	}
	if (bytes > Ring.Get_Used()) {
		Ring.Unread(got);
		return(false);
	}
	Ring.Unread(got);
	return(true);
}


PacketClass *StreamConnectionClass::Receive()
{
	if (Ring.Get_Free() > 0x80) {
		Fill();
	}
	int got = Ring.Read(&Header, sizeof(Header));
	if (got != sizeof(Header)) {
		Ring.Unread(got);
		return(0);
	}
	int readbytes = Header.Size - 4;
	if (readbytes < 0) {
		Disconnect();
		return(0);
	}
	if (readbytes > Ring.Get_Used()) {
		Ring.Unread(sizeof(Header));
		return(0);
	}
	char *buffer = new char[Header.Size];
	*(PacketHeaderClass *)buffer = Header;
	int bytes = Ring.Read(buffer + sizeof(Header), readbytes);
	G_ASSERT(readbytes == bytes);
	PacketClass *packet = Unknown_Create_Packet(buffer);
	delete buffer;
	if (packet) {
		packet->Connection = this;
	}
	return(packet);
}

// (spacing kept: the next assert is on the client's line 1138)















void StreamConnectionClass::Fill()
{
	int len = Ring.Get_Free();
	if (len >= 0x1000) {
		len = 0x1000;
	}
	int bytes_from_transport = Transport_Read(ReadBuffer, len);
	if (bytes_from_transport > 0) {
		int bytes_written = Ring.Write(ReadBuffer, bytes_from_transport);
		G_ASSERT(bytes_written == bytes_from_transport);
	}
}


bool StreamConnectionClass::Is_Dead()
{
	return(!Is_Connected() && !Message_Ready());
}


/*
**	Reconstruction notes (connect.cpp, PARTIAL)
**
**	* Rebuilt: the connection base's constructor 0x820F90 and destructor 0x820FE0 (the scalar
**	  deleting destructor 0x820FB0 is the compiler's), and the stream connection 0x823320-0x8238C0:
**	  constructor, destructor (see above), Write_Message 0x823460 (also inlined in Send_Packet
**	  0x8234D0 and Send_Message 0x823570, out of line because it is a member), Message_Ready
**	  0x8235E0, Receive 0x8236D0, Fill 0x823840 (defined after Receive, inlined there: its assert is
**	  on line 1138, after Receive's 1104) and Is_Dead 0x8238C0 (Message_Ready inlined).
**	* Receive's assert names the size it wants `readbytes` and what the ring gave `bytes`
**	  (`cmp ebp, eax`: the wanted size first).
**	* The header Receive reads is a static (0xBE8EA0); Message_Ready reads into a local.
**	* The forwarding wrapper's slots and the peek connection 0x821BB0-0x822060 (the forwarding
**	  constructor inline in connect.h, its assert on line 74; Send_Packet 0x821CF0 compiles to the
**	  client's bytes but has a twin, so it counts as ambiguous), ConnectionManager 0x822070-0x822EA0
**	  but the vector helper 0x822B50. An entry is pushed as a temporary built by a constructor
**	  (`push_back(ConnectionEntryClass(peek))`): with three stores the compiler keeps 0 in ebx (the
**	  client's `cmp eax, ebx` / `mov [esp+x], bl`) and, in Add_Connection, calls the vector's
**	  insert 0x823A30 instead of inlining it as Add_Unwrapped 0x822790 does.
**	* Peek 0x821FB0 returns the member early (`if (Last_Packet != 0) return(Last_Packet);`): an
**	  if-block around the read reloads it.
**	* The factories 0x821000-0x821B40 and the manager factory 0x822F10: Network_INI is a file
**	  static (0xBE8EA8) with its getter on line 395, inlined in both factories; the host name,
**	  local address and port helpers are global functions (out of line in the client with no
**	  caller), Is_Local returns through `if (...) return(true); return(false);`, Get_Local_IP
**	  returns early when the name doesn't resolve. The factory by name takes the log name's type
**	  into a local; the manager factory probes with `while (tries--)` and names the manager in
**	  two Set_Name calls (both inlined).
**	* Remove_Dead 0x822B50 is functional (marked there). Too short to count: 0x8232C0, 0x8232E0,
**	  0x823300 (not identified). memconn.cpp, dbgconn.cpp and the composite listener are
**	  declared only (memconn.h, dbgconn.h, connect.h): docs/kernel-map.md 4.
*/
