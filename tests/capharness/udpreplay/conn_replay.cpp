// udpreplay --conn: CordCon's delivered messages through the game's connection layer (docs/kernel-map.md
// 4, 5): app\common\udpconn.cpp's UDPConnection::Receive (CordCon::read into the message buffer, then
// the packet factory), then each packet through app\common\connect.cpp's stream connection over a
// memory transport (Send_Packet: the {size, opcode} header and the payload; Receive: the ring, the
// header, the whole message, the factory again), read back in chunks of varying size so that messages
// straddle Fill's reads and the ring's end. Prints, per message:
//
//     M n dir session size opcode payload-hex
//
// What the app code calls that isn't rebuilt yet is supplied here: wlib's CritSec, the rate average
// (unknown_rate.h), the packet base class and the factory 0x8696C0 (a test packet holding the bytes),
// and kernel.cpp's externals (as tests/capharness/kerneltest).
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <new.h>
#include <map>
#include <deque>
#include <vector>
#include "..\..\..\app\common\connect.h"
#include "..\..\..\app\common\udpconn.h"
#include "..\..\..\app\common\critsec.h"
#include "..\..\..\app\packets\pk.h"
#include "cordcon.h"
#include "cordmgr.h"

bool Conn_Mode = false;
int Conn_Errors = 0;
int Conn_Short = 0;		// messages under 4 bytes (the replay's 1-byte fillers): the factory refuses them

// ------------------------------------------------------------------------- wlib's CritSec

CritSec::CritSec() { InitializeCriticalSection(&CritSec_); }
CritSec::~CritSec() { DeleteCriticalSection(&CritSec_); }
int CritSec::lock() const { EnterCriticalSection(&CritSec_); return 0; }
int CritSec::unlock() const { LeaveCriticalSection(&CritSec_); return 0; }

// ------------------------------------------------------------------------- the rate (0x820830), functional

Unknown_RateClass::Unknown_RateClass(int type) : Type(type), Bytes(0), Rate(0), Start(0), Period(5) {}
Unknown_RateClass::~Unknown_RateClass() {}
bool Unknown_RateClass::Add(int bytes)
{
	if (bytes < 1) {
		return true;
	}
	Bytes += bytes;
	return false;
}
int Unknown_RateClass::Get()
{
	int now = (int)time(0);
	if (Start != 0 && now > Start) {
		Rate = (Bytes / (now - Start) + Rate) / 2;
	}
	Bytes = 0;
	Start = now;
	return Rate;
}

// ------------------------------------------------------------------------- kernel.cpp's externals

unsigned int Unknown_Get_Ticks() { return 0; }
int Unknown_9179D0() { return 0; }
bool Unknown_42EA80(KernelEventClass *) { return false; }
void Unknown_42E8A0(KernelEventClass *) {}
class UnknownBDADC0Class
{
public:
	char Unknown0000[0x4074];
	bool Unknown4074;
};
static UnknownBDADC0Class BDADC0;
UnknownBDADC0Class *Unknown_BDADC0 = &BDADC0;
class UnknownBFAEE0Class
{
public:
	virtual void Unknown_00() {}
	virtual void Unknown_04() {}
	virtual void Unknown_08() {}
	virtual void Unknown_0C() {}
	virtual void Unknown_10() {}
};
static UnknownBFAEE0Class BFAEE0;
UnknownBFAEE0Class *Unknown_BFAEE0 = &BFAEE0;

// ------------------------------------------------------------------------- the packets

PacketClass::~PacketClass() {}
RawPacketClass *PacketClass::Build() { return 0; }
bool PacketClass::Process_Packet(void *) { return true; }
int PacketClass::Get_Data_Size() { return 0; }
void PacketClass::Print() {}
bool PacketClass::Unknown_14() { return false; }
int PacketClass::Get_Channel() { return -1; }
int PacketClass::Get_Priority() { return 0; }
bool PacketClass::Is_Reliable() { return true; }
bool PacketClass::Close_Channel() { return false; }

// A packet that keeps its message: Build gives it back as a raw message (as a packet class's Build
// writes its fields into one).
class TestPacketClass : public PacketClass
{
public:
	TestPacketClass(const unsigned char *msg, int size) : Bytes(msg, msg + size) { Connection = 0; Serial = 0; }
	virtual RawPacketClass *Build()
	{
		RawPacketClass *raw = (RawPacketClass *)new char[sizeof(RawPacketClass)];
		memset(raw, 0, sizeof(RawPacketClass));
		memcpy(&raw->Header, &Bytes[0], sizeof(PacketHeaderClass));
		if (Bytes.size() > 4) {
			new (&raw->Data) LinearBufferClass(&Bytes[4], (int)Bytes.size() - 4, 0x400, true);
		} else {
			new (&raw->Data) LinearBufferClass(0x10, 0x400);
		}
		raw->Channel = -1;
		raw->Reliable = true;
		return raw;
	}
	std::vector<unsigned char> Bytes;
};

PacketClass *Unknown_Create_Packet(void *buffer)
{
	const unsigned char *msg = (const unsigned char *)buffer;
	short size = *(const short *)msg;
	if (size < 4) {
		return 0;		// no header (Conn_Deliver counts a message under 4 bytes)
	}
	return new TestPacketClass(msg, size);
}

// ------------------------------------------------------------------------- the memory transport

static const int Chunks[] = { 1, 3, 4, 5, 64, 999, 4096, 7, 200, 4095 };

class MemoryStreamClass : public StreamConnectionClass
{
public:
	MemoryStreamClass() : Turn(0) {}
	virtual int Transport_Write(const void *data, int len)
	{
		const unsigned char *p = (const unsigned char *)data;
		for (int i = 0; i < len; i++) {
			Pipe.push_back(p[i]);
		}
		return len;
	}
	virtual int Transport_Read(void *buffer, int len)
	{
		int n = Chunks[Turn++ % (sizeof(Chunks) / sizeof(Chunks[0]))];
		if (n > len) {
			n = len;
		}
		if (n > (int)Pipe.size()) {
			n = (int)Pipe.size();
		}
		for (int i = 0; i < n; i++) {
			((unsigned char *)buffer)[i] = Pipe.front();
			Pipe.pop_front();
		}
		return n;
	}
	std::deque<unsigned char> Pipe;
	int Turn;
};

static MemoryStreamClass Stream;
static std::map<CordCon *, UDPConnection *> Connections;

// The UDPConnection over a side's CordCon (the constructor for an accepted CordCon, 0x82CBF0). Its
// constructor opens a default channel (openc mode 3); the replay's sides open the capture's own
// channels, so that one is taken back (as Write_Side does for a channel openc gave wrongly).
static UDPConnection *Get_Connection(CordCon *con, CordMgr *mgr)
{
	std::map<CordCon *, UDPConnection *>::iterator it = Connections.find(con);
	if (it != Connections.end()) {
		return it->second;
	}
	int active = (int)con->Active_.size();
	UDPConnection *c = new UDPConnection(con, mgr);
	if ((int)con->Active_.size() == active + 1) {
		CordChannelClass *ch = con->Channels_[c->Default];
		ch->Mode = 0;
		ch->IsClosed = true;
		con->Active_.pop_back();
	}
	Connections[con] = c;
	return c;
}

// Every message the CordCon has for its reader: UDPConnection::Receive, then the stream framing.
// A message under 4 bytes (the replay's 1-byte fillers) has no header: the client's factory would
// read the previous message's from the shared buffer; the harness tells them by the CordCon's
// BytesRead_ (read adds each message's length) and counts them instead (Conn_Short). Once the CordCon is closed (state
// 4), Receive disconnects (CordMgr::Release) and reads nothing more, as the client's does: what the
// CordCon still holds is counted as dropped (a Z line), and the harness keeps the CordCon (its
// release entry is taken out, so Pump_Releases never frees it under the replay).
int Conn_Deliver(FILE *out, int n, const char *dir, const char *name, CordCon *con, CordMgr *mgr)
{
	static unsigned char peek[0x10000];
	UDPConnection *udp = Get_Connection(con, mgr);
	int delivered = 0;
	for (;;) {
		int read_before = con->BytesRead_;
		PacketClass *p = udp->Receive();
		if (con->BytesRead_ != read_before && con->BytesRead_ - read_before < 4) {
			Conn_Short++;
			fprintf(out, "Y %d %s %s %d\n", n, dir, name, con->BytesRead_ - read_before);
			delete p;
			continue;
		}
		if (p == 0) {
			break;
		}
		if (p->Connection != udp) {
			Conn_Errors++;
		}
		if (!Stream.Send_Packet(p)) {
			Conn_Errors++;
		}
		delete p;
		for (int guard = 0; guard < 100000; guard++) {
			PacketClass *q = Stream.Receive();
			if (q == 0) {
				if (Stream.Pipe.empty() && Stream.Ring.Get_Used() == 0) {
					break;
				}
				continue;
			}
			TestPacketClass *t = (TestPacketClass *)q;
			if (q->Connection != &Stream) {
				Conn_Errors++;
			}
			short size = *(short *)&t->Bytes[0];
			unsigned short opcode = *(unsigned short *)&t->Bytes[2];
			fprintf(out, "M %d %s %s %d %d ", n, dir, name, (int)size, (int)opcode);
			for (size_t i = 4; i < t->Bytes.size(); i++) {
				fprintf(out, "%02x", t->Bytes[i]);
			}
			fprintf(out, "\n");
			delivered++;
			delete q;
		}
		if (!Stream.Pipe.empty() || Stream.Ring.Get_Used() != 0) {
			Conn_Errors++;
		}
	}
	if (udp->Con == 0) {
		for (int r = 0; r < mgr->Releases_.Length(); r++) {
			CordReleaseClass rel;
			mgr->Releases_.Get(rel, r);
			if (rel.Con == con) {
				mgr->Releases_.Remove(r);
				r--;
			}
		}
		int dropped = 0;
		int channel, len;
		while (con->read(&channel, &len, peek, false)) {
			dropped++;
		}
		if (dropped) {
			fprintf(out, "Z %d %s %s %d\n", n, dir, name, dropped);
		}
	}
	return delivered;
}
