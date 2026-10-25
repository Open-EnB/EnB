// Earth & Beyond client (reconstruction): app\common\udpconn.cpp (PARTIAL). Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "udpconn.h"
#include "critsec.h"
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
static unsigned int Channel_Hash(const UDPChannelKeyClass &key)
{
	return(key.Channel + key.Reliable);
}


// functional (NOT EXACT): the dictionary's inlined constructor is the stand-in's.
UDPConnection::UDPConnection(unsigned int ip, unsigned short port, bool encrypt) :
	Channels(Channel_Hash), InRate(2), OutRate(2)
{
	LastOut = 0;
	LastIn = 0;
	Encrypt = encrypt;
	Mgr = 0;
	Con = 0;
	Pending = 0;
	Default = (unsigned int)-1;
	Name = 0;
	Address = htonl(ip);
	Port = htons(port);
	Name = nstrdup("unknown");
	CordMgrTaskClass::Init();
	Mgr = CordMgrTaskClass::Mgr;
	for (int i = 0; i < 16; i++) {
		Fixed[i] = (unsigned int)-1;
	}
}


// functional (NOT EXACT): the dictionary's inlined constructor is the stand-in's.
UDPConnection::UDPConnection(CordCon *con, CordMgr *mgr) :
	Channels(Channel_Hash), InRate(2), OutRate(2)
{
	LastOut = 0;
	LastIn = 0;
	Mgr = mgr;
	Con = con;
	Pending = 0;
	Default = (unsigned int)-1;
	Name = 0;
	Address = htonl(con->Ip_);
	Port = htons(con->Port_);
	for (int i = 0; i < 16; i++) {
		Fixed[i] = (unsigned int)-1;
	}
	Con->openc(&Default, 3, 0);
	Fixed[0] = Default;
	Con->Stats_->Set_Rate_Limit(0x10000);
}


// functional (NOT EXACT): the dictionary's inlined clear and shrink are the stand-in's.
UDPConnection::~UDPConnection()
{
	if (Con) {
		Lock.lock();
		bool closed = Con->Is_Closed();
		Lock.unlock();
		if (closed == true) {
			Disconnect();
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
	*in_rate = (float)InRate.Get();
	*out_rate = (float)OutRate.Get();
	if (Con) {
		int messages_sent, packets_in, bytes_sent, bytes_read, resends, dup_acks;
		unsigned int window_bytes, in_flight_bytes, round_trip_ms, quality_percent;
		int idle_seconds;
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
		if (Con) {
			return(true);
		}
		CordConnectingClass *c = Mgr->Connect(ntohl(Address), ntohs(Port), Encrypt);
		Pending = c ? c->Session : 0;
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
	}
	return(false);
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












































































































































// functional (NOT EXACT): the dictionary's inlined code is the stand-in's.
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
	unsigned int cord_channel = Default;
	if (!Get_Unknown04()) {
		channel = -4;
	}
	Lock.lock();
	UDPChannelKeyClass key;
	key.Channel = channel;
	key.Reliable = reliable;
	if (channel < 0) {
		if (channel == -1) {
			cord_channel = Default;
		} else if (channel > -16) {
			Open_Fixed_Channel(channel);
			cord_channel = Fixed[-channel - 1];
		}
	} else {
		UDPChannelClass *found = 0;
		if (!Channels.getPointer(key, &found)) {
			bool open_ok = Con->openc(&cord_channel, reliable ? 3 : 4, 0);
			G_ASSERT(open_ok);
			UDPChannelClass value;
			value.Channel = cord_channel;
			Channels.add(key, value);
		} else if (found) {
			cord_channel = found->Channel;
		}
	}
	// (the client's drop test: a reliable message, or rand() % 101 at least 0, is always sent)
	if (reliable || rand() % 101 >= 0) {
		Con->write(cord_channel, Buffer, len, priority);
	}
	if (close && channel >= 0) {
		for (int mode = 0; mode < 2; mode++) {
			key.Reliable = mode != 0;
			UDPChannelClass *found = 0;
			if (Channels.getPointer(key, &found)) {
				if (found) {
					cord_channel = found->Channel;
				}
				if (!Is_Fixed_Channel(cord_channel)) {
					Con->closec(cord_channel);
				}
				Channels.remove(key);
			}
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


void UDPConnection::Open_Fixed_Channel(int channel)
{
	unsigned int *slot = &Fixed[-1 - channel];
	if (*slot == (unsigned int)-1) {
		int priority = 0;
		if (channel == -4) {
			Con->openc(slot, 3, 0x80);
			return;
		}
		if (channel == -5) {
			priority = -127;
		}
		Con->openc(slot, 3, priority);
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


/*
**	Reconstruction notes (udpconn.cpp, PARTIAL)
**
**	* The client's functions, in order: static initialisers 0x82C530 (a std::map helper) and
**	  0x82C5E0 (here Channel_Hash; the lock's initialiser sits beside it), 0x82C660 (a static: the
**	  pump task and the CordMgr, kept in 0xBF16BC; not rebuilt), the constructor by name 0x82C680
**	  (gethostbyname or the dotted address; assert line 91; not rebuilt), by address 0x82C980, over
**	  an accepted CordCon 0x82CBF0, the destructor 0x82CDC0, the slots 0x82D060-0x82DB30, the pump
**	  task 0x82DCB0-0x82E020, UDPConnectionManager 0x82E040-0x82E1D0 and the listener
**	  0x82E200-0x82E5B0 (the server's: not rebuilt), wlib Dictionary's out-of-line members
**	  0x82E6A0-0x82EBB0.
**	* Exact: Get_Address, Connect, Is_Connected, Is_Dead, Disconnect, the key's operator ==,
**	  Is_Fixed_Channel, Send_Message, Send_Packet, Receive, the task's destructor, Delete_On_Remove,
**	  Get_Mgr, Shutdown_Task, Run (with its profile sample) and Get_Interval. NOT EXACT (functional,
**	  D42): what inlines wlib's Dictionary (a stand-in, dictionary.h: the constructors, the
**	  destructor, Send_Data), Get_Stats, Try_Connect (the tree's CordMgr::Connect returns its record,
**	  the client's the session id), Open_Fixed_Channel, the task's constructor and Init (the tree's
**	  CordMgr isn't the client's 0x95C bytes).
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
