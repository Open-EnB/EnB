// Earth & Beyond client (reconstruction): app\common\udpconn.cpp (PARTIAL). Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "udpconn.h"
#include "..\..\libs\wwonline\wlib\critsec.h"
#include "..\packets\pk.h"
#include "..\wwudp\cordcon.h"
#include "..\wwudp\cordmgr.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"
#include "..\..\wwlib\nstrdup.h"
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

// The lock around every CordMgr / CordCon call (0xBE96A0) and the message buffer Receive reads
// into and the sends copy into (0xBE96B8: the header, then the payload).
static CritSec Lock;
static char Buffer[UDP_DATABUFFER_SIZE];

CordMgrTaskClass *CordMgrTaskClass::Task = 0;
CordMgr *CordMgrTaskClass::Mgr = 0;

// 0x82C5E0
static uint32 Channel_Hash(const UDPChannelKeyClass &key)
{
	return(key.Channel + key.Reliable);
}


// The process's CordMgr as the connections and the manager keep it (0xBF16BC)
static CordMgr *TheMgr;	// NAME 2: what it holds

// The pump task and the CordMgr, once (0x82C660; inlined in the constructors)
void Init_Mgr()	// NAME 2: what it does
{
	CordMgrTaskClass::Init();
	TheMgr = CordMgrTaskClass::Mgr;
}


































UDPConnection::UDPConnection(const char *host, unsigned short port, bool encrypt) :
	Channels(Channel_Hash), InRate(2), OutRate(2),
	LastOut(0),
	LastIn(0),
	Encrypt(encrypt),
	Mgr(0),
	Con(0),
	Pending(0),
	Default((unsigned int)-1),
	Name(0),
	Address(0),
	Port(htons(port))
{
	if (isdigit(*host)) {
		Address = inet_addr(host);
		Name = nstrdup("unknown");
	} else {
		struct hostent *h = gethostbyname(host);
		G_ASSERT(h);
		Address = *(unsigned int *)h->h_addr_list[0];
		Name = nstrdup(h->h_name);
	}
	for (int i = 0; i < 16; i++) {
		Fixed[i] = (unsigned int)-1;
	}
	ntohs(Port);	// a compiled-out message's argument: the client keeps the call
	Init_Mgr();
	Mgr = TheMgr;
}

UDPConnection::UDPConnection(unsigned int ip, unsigned short port, bool encrypt) :
	Channels(Channel_Hash), InRate(2), OutRate(2),
	LastOut(0),
	LastIn(0),
	Encrypt(encrypt),
	Mgr(0),
	Con(0),
	Pending(0),
	Default((unsigned int)-1),
	Name(0),
	Address(htonl(ip)),
	Port(htons(port))
{
	Name = nstrdup("unknown");
	ntohs(Port);	// a compiled-out message's argument: the client keeps the call
	Init_Mgr();
	Mgr = TheMgr;
	for (int i = 0; i < 16; i++) {
		Fixed[i] = (unsigned int)-1;
	}
}


// NOT EXACT: two register choices (con's Ip_ / Port_ reads, the openc tail); Encrypt stays unset
UDPConnection::UDPConnection(CordCon *con, CordMgr *mgr) :
	Channels(Channel_Hash), InRate(2), OutRate(2),
	LastOut(0), LastIn(0),
	Mgr(mgr),
	Con(con),
	Pending(0),
	Default((unsigned int)-1),
	Name(0),
	Address(htonl(con->Ip_)),
	Port(htons(con->Port_))
{
	ntohs(Port);	// a compiled-out message's argument: the client keeps the call
	for (int i = 0; i < 16; i++) {
		Fixed[i] = (unsigned int)-1;
	}
	Con->openc(&Default, 3, 0);
	Fixed[0] = Default;
	Con->Stats_->Set_Rate_Limit(0x10000);
}


// Disconnect through the base class (the client's virtual call); Pending and Name go with it
UDPConnection::~UDPConnection()
{
	if (Con) {
		Lock.lock();
		bool closed = Con->Is_Closed();
		Lock.unlock();
		if (closed == true) {
			((ConnectionClass &)*this).Disconnect();
		} else {
			if (Pending) {
				Mgr->Cancel_Connect(Pending);
				Pending = 0;
			}
			if (Con) {
				Lock.lock();
				Mgr->Release(Con);
				Con = 0;
				Lock.unlock();
			}
		}
	}
	if (Pending) {
		Mgr->Cancel_Connect(Pending);
		Pending = 0;
	}
	if (Name) {
		delete Name;
	}
}


bool UDPConnection::Get_Stats(float *out_rate, float *in_rate, float *round_trip, float *window, float *in_flight,
	float *quality, int *idle)
{
	int messages_sent, packets_in, bytes_sent, bytes_read, resends, dup_acks;
	unsigned int window_bytes, in_flight_bytes, round_trip_ms, quality_percent;
	int idle_seconds;
	*in_rate = (float)InRate.Get();
	*out_rate = (float)OutRate.Get();
	if (Con) {
		Con->getStats(&messages_sent, &packets_in, &bytes_sent, &bytes_read, &resends, &dup_acks, &window_bytes,
			&in_flight_bytes, &round_trip_ms, &quality_percent, &idle_seconds);
		*round_trip = (float)round_trip_ms;
		*window = (float)window_bytes;
		*in_flight = (float)in_flight_bytes;
		*quality = (float)quality_percent;
		*idle = idle_seconds;
		return(true);
	}
	return(false);
}


unsigned int UDPConnection::Get_Address()
{
	if (Con) {
		return(Con->Ip_);
	}
	return(0);
}


bool UDPConnection::Connect(bool wait)
{
	if (!wait) {
		return(Try_Connect());
	}
	while (!Try_Connect()) {
		Mgr->Wait(0, 1000);
	}
	if (Con->Is_Closed() == true) {
		Mgr->Release(Con);
		Con = 0;
		return(false);
	}
	return(true);
}


bool UDPConnection::Try_Connect()
{
	if (Pending == 0) {
		if (Con == 0) {
			Pending = Mgr->Connect(ntohl(Address), ntohs(Port), Encrypt);
			return(false);
		}
	} else {
		Mgr->Pump();
		if (Mgr->Find_Connection(&Con, 0, Pending)) {
			Pending = 0;
			Lock.lock();
			Con->openc(&Default, 3, 0);
			Con->Stats_->Set_Rate_Limit(0x10000);
			Lock.unlock();
			return(true);
		}
		return(false);
	}
	return(true);
}


bool UDPConnection::Is_Connected()
{
	if (Con == 0) {
		return(false);
	}
	Lock.lock();
	bool closed = Con->Is_Closed();
	Lock.unlock();
	if (closed == true) {
		Disconnect();
		return(false);
	}
	return(true);
}


bool UDPConnection::Is_Dead()
{
	return(!Is_Connected());
}


bool UDPConnection::Disconnect()
{
	if (Pending) {
		Mgr->Cancel_Connect(Pending);
		Pending = 0;
	}
	if (Con) {
		Lock.lock();
		Mgr->Release(Con);
		Con = 0;
		Lock.unlock();
	}
	return(true);
}

// (spacing kept: the next assert is on the client's line 391)





































































// NOT EXACT (inline budget): the last lookup keeps keyHash out of line (the client inlines it there and
// keeps operator == out of line; getPointer direct there inlines both); CordCon::write takes (channel, len, data, priority) in the client
bool UDPConnection::Send_Data(int len, int channel, int priority, bool reliable, bool close)
{
	if (Con == 0) {
		return(false);
	}
	Lock.lock();
	bool closed = Con->Is_Closed();
	Lock.unlock();
	if (closed) {
		Disconnect();
		return(false);
	}
	if (channel == 0) {
		channel = -1;
	}
	UDPChannelClass chan;
	chan.Channel = Default;
	if (!Get_Unknown04()) {
		channel = -4;
	}
	Lock.lock();
	UDPChannelKeyClass key;
	key.Channel = channel;
	key.Reliable = reliable;
	if (channel >= 0) {
		if (!Channels.getValue(key, chan)) {
			int mode = 3;
			if (!key.Reliable) {
				mode = 4;
			}
			bool open_ok = Con->openc(&chan.Channel, mode, 0);
			G_ASSERT(open_ok);
			Channels.add(key, chan);
		}
	} else if (channel == -1) {
		chan.Channel = Default;
	} else if (channel > -16) {
		Open_Fixed_Channel(channel);
		chan.Channel = Fixed[-channel - 1];
	}
	if (key.Reliable || rand() % 101 >= 0) {
		Con->write(chan.Channel, Buffer, len, priority);
	}
	if (close && channel >= 0) {
		key.Reliable = false;
		if (Channels.getValue(key, chan)) {
			if (!Is_Fixed_Channel(chan.Channel)) {
				Con->closec(chan.Channel);
			}
			Channels.remove(key);
		}
		key.Reliable = true;
		if (Channels.getValue(key, chan)) {
			if (!Is_Fixed_Channel(chan.Channel)) {
				Con->closec(chan.Channel);
			}
			Channels.remove(key);
		}
	}
	Lock.unlock();
	return(true);
}

bool UDPChannelKeyClass::operator == (const UDPChannelKeyClass &other) const
{
	if (Channel == other.Channel && Reliable == other.Reliable) {
		return(true);
	}
	return(false);
}

// NOT EXACT: the client keeps -1 - channel and Fixed's +0x80 apart, and the -4 priority in esi
void UDPConnection::Open_Fixed_Channel(int channel)
{
	// an index subscripted twice keeps the client's unfolded [this + index*4 + 0x80]
	// (a slot pointer folds the -1 into the displacement)
	int index = -1 - channel;
	if (Fixed[index] == (unsigned int)-1) {
		int priority = 0;
		if (channel == -4) {
			priority = 0x80;
		} else if (channel == -5) {
			priority = -127;
		}
		Con->openc(&Fixed[index], 3, priority);
	}
}


bool UDPConnection::Is_Fixed_Channel(unsigned int channel)
{
	if (channel == Default) {
		return(true);
	}
	for (int i = 0; i < 16; i++) {
		if (Fixed[i] == channel) {
			return(true);
		}
	}
	return(false);
}

// (spacing kept: the next assert is on the client's line 489)
























bool UDPConnection::Send_Message(RawPacketClass *packet)
{
	FAIL_IF(packet->Header.Size > UDP_DATABUFFER_SIZE) return(false);
	*(PacketHeaderClass *)Buffer = packet->Header;
	memcpy(Buffer + sizeof(PacketHeaderClass), packet->Data.Buffer, packet->Header.Size - sizeof(PacketHeaderClass));
	return(Send_Data(packet->Header.Size, packet->Channel, packet->Priority, packet->Reliable, packet->Close_Channel));
}

// (spacing kept: the next assert is on the client's line 514)















bool UDPConnection::Send_Packet(PacketClass *source)
{
	RawPacketClass *packet = source->Build();
	FAIL_IF(packet->Header.Size > UDP_DATABUFFER_SIZE) {
		delete packet;
		return(false);
	}
	*(PacketHeaderClass *)Buffer = packet->Header;
	memcpy(Buffer + sizeof(PacketHeaderClass), packet->Data.Buffer, packet->Header.Size - sizeof(PacketHeaderClass));
	bool ok = Send_Data(packet->Header.Size, source->Get_Channel(), source->Get_Priority(), source->Is_Reliable(),
		source->Close_Channel());
	delete packet;
	return(ok);
}


PacketClass *UDPConnection::Receive()
{
	if (Con == 0) {
		return(0);
	}
	Lock.lock();
	bool closed = Con->Is_Closed();
	Lock.unlock();
	if (closed) {
		Disconnect();
		return(0);
	}
	int channel = 0;
	int len = UDP_DATABUFFER_SIZE;
	Lock.lock();
	int messages_sent, packets_in, bytes_sent, bytes_read, resends, dup_acks;
	unsigned int window_bytes, in_flight_bytes, round_trip_ms, quality_percent;
	int idle_seconds;
	Con->getStats(&messages_sent, &packets_in, &bytes_sent, &bytes_read, &resends, &dup_acks, &window_bytes,
		&in_flight_bytes, &round_trip_ms, &quality_percent, &idle_seconds);
	InRate.Add(bytes_read - LastIn);
	OutRate.Add(bytes_sent - LastOut);
	LastOut = bytes_sent;
	LastIn = bytes_read;
	bool got = Con->read(&channel, &len, Buffer, false);
	Lock.unlock();
	if (!got) {
		return(0);
	}
	PacketClass *packet = Unknown_Create_Packet(Buffer);
	if (packet == 0) {
		return(0);
	}
	packet->Connection = this;
	return(packet);
}

void UDPConnection::Unknown_82DCB0() { Name = nstrdup("unknown"); }	// NAME 1 [Unknown_82DCB0]: no caller, no string
CordMgrTaskClass::CordMgrTaskClass()
{
	Mgr = new CordMgr;
}


CordMgrTaskClass::~CordMgrTaskClass()
{
	delete Mgr;
	Mgr = 0;
}


bool CordMgrTaskClass::Delete_On_Remove()
{
	return(false);
}


CordMgrTaskClass *CordMgrTaskClass::Init()
{
	if (Task == 0) {
		Task = new CordMgrTaskClass;
		KernelClass::Get_Instance()->Add_Task(Task);
	}
	return(Task);
}


CordMgr *CordMgrTaskClass::Get_Mgr()
{
	Init();
	return(Mgr);
}


void CordMgrTaskClass::Shutdown_Task()
{
	if (Task) {
		delete Task;
	}
}


void CordMgrTaskClass::Run()
{
	WWPROFILE("CordMgr::Run");
	Mgr->Pump();
}


int CordMgrTaskClass::Get_Interval()
{
	return(100);
}


// UDPConnectionManagerClass (vtable 0xB08934): a ConnectionManagerClass over the process's CordMgr
// Init_Mgr inlined: the task, the CordMgr (0x95C bytes with its key pair) and TheMgr
UDPConnectionManagerClass::UDPConnectionManagerClass()
{
	Init_Mgr();
}

UDPConnectionManagerClass::~UDPConnectionManagerClass()
{
}

// UDPListenerClass (vtable 0xB08958): handshakes arriving on a port of the CordMgr
// Init_Mgr inlined, as in UDPConnectionManagerClass's constructor
UDPListenerClass::UDPListenerClass() :
	Mgr(0),
	PortNumber(0)
{
	Init_Mgr();
	Mgr = TheMgr;
}

bool UDPListenerClass::Is_Listening() { return(PortNumber != 0); }
unsigned short UDPListenerClass::Get_Port() { return(PortNumber); }

UDPListenerClass::~UDPListenerClass()
{



























































	FAIL_IF(PortNumber != 0) Stop();
}

bool UDPListenerClass::Listen(unsigned short port)
{




	FAIL_IF(port == 0) return(false);


	FAIL_IF(PortNumber != 0) return(false);
	Lock.lock();
	bool ok = Mgr->Listen(0, port);
	Lock.unlock();
	if (!ok) {
		return(false);
	}
	PortNumber = port;
	return(true);
}

bool UDPListenerClass::Stop()
{










	FAIL_IF(PortNumber == 0) return(false);
	Lock.lock();
	bool ok = Mgr->Unlisten(0, PortNumber);
	Lock.unlock();
	PortNumber = 0;
	return(ok == true);
}

// The connection a finished handshake on the port made, or NULL
ConnectionClass *UDPListenerClass::Accept()
{
	CordCon *con;
	Lock.lock();
	bool ok = Mgr->Find_Connection(&con, PortNumber, 0);
	Lock.unlock();
	if (ok) {
		ConnectionClass *new_connection = new UDPConnection(con, Mgr);



















		FAIL_IF(!new_connection) return(0);
		return(new_connection);
	}
	return(0);
}

// 0x82E1D0 (between the manager and the listener in the client; here last, so the listener's
// assert lines stay): the CordMgr's pending messages, 0 without one (ClientTask::Run)
int Unknown_82E1D0()
{
	int pending = 0;
	if (TheMgr) {
		pending = TheMgr->Pending_Messages();
	}
	return(pending);
}

/*
**	Reconstruction notes (udpconn.cpp, PARTIAL)
**
**	* The client's functions, in order: static initialisers 0x82C530 (a std::map helper) and
**	  0x82C5E0 (here Channel_Hash; the lock's initialiser sits beside it), 0x82C660 (Init_Mgr: the
**	  pump task and the CordMgr, kept in 0xBF16BC as TheMgr), the constructor by name 0x82C680
**	  (gethostbyname or the dotted address; assert line 91), by address 0x82C980, over
**	  an accepted CordCon 0x82CBF0, the destructor 0x82CDC0, the slots 0x82D060-0x82DB30, a member
**	  with no caller 0x82DCB0 (Name = nstrdup("unknown"); one line, so the asserts below keep their
**	  lines), the pump task 0x82DCE0-0x82E020, UDPConnectionManager 0x82E040-0x82E1B0 (0x82E1D0, a
**	  static over 0xBF16BC) and the listener 0x82E200-0x82E5B0 (the server's: its destructor's
**	  FAIL_IF on line 707, Listen 716 / 719, Stop 742, Accept 778), wlib Dictionary's out-of-line members
**	  0x82E6A0-0x82EBB0 (shrink, remove, expand: exact, from libs\wwonline\wlib\dictionary.h, D73).
**	* Exact: Get_Address, Connect, Is_Connected, Is_Dead, Disconnect, the key's operator ==,
**	  Is_Fixed_Channel, Send_Message, Send_Packet, Receive, the task's destructor, Delete_On_Remove,
**	  Get_Mgr, Shutdown_Task, Run (with its profile sample) and Get_Interval; since CordMgr has its
**	  key pair (0x95C bytes, step 8's tail): the constructors by name and by address (their members
**	  in the initialiser list, a dead ntohs(Port) each), the task's constructor and Init,
**	  UDPConnectionManager's and the listener's constructors; the destructor (Disconnect through
**	  the base class), Get_Stats (its locals declared at the top: declared in the block they take
**	  the dead parameters' slots) and Try_Connect (CordMgr::Connect returns the session id).
**	  NOT EXACT: the constructor over a CordCon (registers), Send_Data (inline budget: the last
**	  lookup's keyHash; and CordCon::write's argument order, see below).
**	* Send_Data (NW1, 2026-11-01): one UDPChannelClass for the channel (getValue fills it, add copies
**	  it), `channel >= 0` tested first, key.Reliable read for the open mode (an if, not ?:) and the
**	  drop test, the two close lookups written out (no loop): getPointer goes out of line (0x82E800,
**	  exact) at the first two. The client calls CordCon::write 0x850C70 as (channel, len, data,
**	  priority): its body compares the second argument with 0xC800; cordcon.h declares (channel,
**	  data, len, priority). Forms in docs/data/attempts.csv (0x82D420).
**	* The checks of the CordCon's state compare with true (`cmp bl, 1`): `closed == true`, except in
**	  Send_Data and Receive.
**	* Pending (+0x7C) holds the session id CordMgr::Connect started (the client's 0x859440 and
**	  0x8595B0 search by it), not a pointer.
**	* Send_Data's channel argument: 0 (or the flag at +0x04 off: -4) the default channel; -2 .. -15
**	  a fixed channel opened on first use (mode 3; -4 at priority 0x80, -5 at -127); >= 0 a hashed
**	  channel per (channel, reliable), opened on first use (mode 3 reliable, 4 sequenced) and closed
**	  after the send when close is set. The `rand() % 101 >= 0` test always sends (a drop
**	  percentage of 0).
*/
