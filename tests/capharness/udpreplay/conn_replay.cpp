// udpreplay --conn: CordCon's delivered messages through the game's connection layer (docs/kernel-map.md
// 4, 5): app\common\udpconn.cpp's UDPConnection::Receive (CordCon::read into the message buffer, then
// the packet factory), then each packet through app\common\connect.cpp's stream connection over a
// memory transport (Send_Packet: the {size, opcode} header and the payload; Receive: the ring, the
// header, the whole message, the factory again), read back in chunks of varying size so that messages
// straddle Fill's reads and the ring's end. Prints, per message:
//
//     M n dir session size opcode payload-hex
//
// What the app code calls that isn't rebuilt yet is supplied here: the rate average
// (unknown_rate.h), wwonline's 64-bit byte order pair and the factory 0x8696C0 (a test packet
// holding the bytes),
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
#include "..\..\..\app\packets\pk.h"
#include "cordcon.h"
#include "cordmgr.h"

bool Conn_Mode = false;
int Conn_Errors = 0;
int Conn_Short = 0;		// messages under 4 bytes (the replay's 1-byte fillers): the factory refuses them

// ------------------------------------------------------------------------- wlib's CritSec

// the imported one (libs\wwonline\wlib, D73), compiled into the program here
#include "..\..\..\libs\wwonline\wlib\critsec.cpp"

// ------------------------------------------------------------------------- the rate (0x820830), functional

Unknown_RateClass::Unknown_RateClass(int type) : Type(type), Bytes(0), Rate(0), Start(0), Period(5) {}
Unknown_RateClass::~Unknown_RateClass() {}
int Unknown_RateClass::Add(int bytes)
{
	if (bytes < 1) {
		return 1;
	}
	Bytes += bytes;
	return 0;
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

// wwonline's 64-bit byte order pair (pk.cpp's Add / Get of an __int64)
static unsigned __int64 Swap64(unsigned __int64 v)
{
	unsigned __int64 r = 0;
	for (int i = 0; i < 8; i++) {
		r = (r << 8) | ((v >> (8 * i)) & 0xFF);
	}
	return r;
}
unsigned __int64 Unknown_8B4510(unsigned __int64 value) { return Swap64(value); }
unsigned __int64 Unknown_8B4560(unsigned __int64 value) { return Swap64(value); }

// The packets the client's factory made (pk.cpp: Unknown_Create_Packet, the generated switch, a
// class of pk_classes.h): their constructors aren't rebuilt, so pk_ctor_stubs.cpp's hand each raw
// message here; the harness keeps the bytes per packet and builds a raw message back from them.
static std::map<PacketClass *, std::vector<unsigned char> > Packets;

void Conn_Record(PacketClass *packet, RawPacketClass *raw)
{
	packet->Connection = 0;
	packet->Serial = 0;
	short size = raw->Header.Size;
	std::vector<unsigned char> &msg = Packets[packet];
	msg.resize(size < 4 ? 4 : size);
	memcpy(&msg[0], &raw->Header, 4);
	if (size > 4) {
		memcpy(&msg[4], raw->Data.Buffer, size - 4);
	}
}

static RawPacketClass *Build_Raw(const std::vector<unsigned char> &bytes)
{
	RawPacketClass *raw = (RawPacketClass *)new char[sizeof(RawPacketClass)];
	memset(raw, 0, sizeof(RawPacketClass));
	memcpy(&raw->Header, &bytes[0], sizeof(PacketHeaderClass));
	if (bytes.size() > 4) {
		new (&raw->Data) LinearBufferClass(&bytes[4], (int)bytes.size() - 4, 0x400, true);
	} else {
		new (&raw->Data) LinearBufferClass(0x10, 0x400);
	}
	raw->Channel = -1;
	raw->Reliable = true;
	return raw;
}

// A packet's message: the bytes Conn_Record kept (a class not rebuilt), else what the packet's own
// Build makes (a rebuilt class: its constructor parsed the message, Build writes it back).
int Conn_Built = 0;
static bool Packet_Bytes(PacketClass *p, std::vector<unsigned char> &bytes)
{
	std::map<PacketClass *, std::vector<unsigned char> >::iterator it = Packets.find(p);
	if (it != Packets.end()) {
		bytes = it->second;
		return true;
	}
	RawPacketClass *raw = p->Build();
	if (raw == 0) {
		return false;
	}
	short size = raw->Header.Size;
	bytes.resize(size < 4 ? 4 : size);
	memcpy(&bytes[0], &raw->Header, 4);
	if (size > 4) {
		memcpy(&bytes[4], raw->Data.Buffer, size - 4);
	}
	delete raw;
	Conn_Built++;
	return true;
}

static void Delete_Packet(PacketClass *p)
{
	Packets.erase(p);
	delete p;
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
			if (p) {
				Packets.erase(p);
			}
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
		std::vector<unsigned char> sent;
		if (!Packet_Bytes(p, sent)) {
			Conn_Errors++;
			delete p;
			continue;
		}
		RawPacketClass *raw = Build_Raw(sent);
		if (!Stream.Send_Message(raw)) {
			Conn_Errors++;
		}
		delete raw;
		Delete_Packet(p);
		for (int guard = 0; guard < 100000; guard++) {
			PacketClass *q = Stream.Receive();
			if (q == 0) {
				if (Stream.Pipe.empty() && Stream.Ring.Get_Used() == 0) {
					break;
				}
				continue;
			}
			if (q->Connection != &Stream) {
				Conn_Errors++;
			}
			std::vector<unsigned char> bytes;
			if (!Packet_Bytes(q, bytes)) {
				Conn_Errors++;
				Delete_Packet(q);
				continue;
			}
			short size = *(short *)&bytes[0];
			unsigned short opcode = *(unsigned short *)&bytes[2];
			fprintf(out, "M %d %s %s %d %d ", n, dir, name, (int)size, (int)opcode);
			for (size_t i = 4; i < bytes.size(); i++) {
				fprintf(out, "%02x", bytes[i]);
			}
			fprintf(out, "\n");
			delivered++;
			Delete_Packet(q);
		}
		if (!Stream.Pipe.empty() || Stream.Ring.Get_Used() != 0) {
			Conn_Errors++;
		}
	}
	if (udp->Con == 0) {
		for (int r = 0; r < mgr->Releases_.length(); r++) {
			CordReleaseClass rel;
			mgr->Releases_.get(rel, r);
			if (rel.Con == con) {
				mgr->Releases_.remove(r);
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
