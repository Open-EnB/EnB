// M1.4's harness (milestones.md 2.1 gate (a)): the EA-UDP datagrams of an Alex dump go through the
// rebuilt app\wwudp (CordMgr::Pump -> CordMgr::Process_CORD_Packet -> CordCon::handlePacket ->
// CordCon::read) over a replay socket (this file's UDP class), in both directions: a client-side
// CordCon takes the server's datagrams, a server-side one the client's. Each message one side
// delivers is written on the other side (CordCon::openc when the channel is new, then
// CordCon::write) and that side pumps (CordCon::pumpInternal): its datagrams are compared, ack
// list and blocks, with the next datagram the capture has in that direction.
//
//     udpreplay IN.txt OUT.txt [MS [WINDOW]]   (IN: tools/harness/udpreplay.py's datagram file)
//
// MS: the clock (wtime_replay.cpp) advances MS milliseconds before each datagram (default 0: the
// dumps have no times, and a frozen clock fires no resend timer). WINDOW: the send window limit
// each CordCon gets (ACK2's, which the dumps lack; default 65535).
//
// IN lines:  K session keyhex                 a session's RC4 key (8 bytes)
//            W n session dir channel mode hex a message datagram n sends first: written before it
//            Z n session dir seq              channel 0's counter on the sender before datagram n (the
//                                             keep-alive timer that advances it isn't replayed)
//            Q n session dir                  the sender closes before datagram n (CordCon::close)
//            D n session dir counter hex      datagram n as the dump has it (decrypted, from the
//                                             checksum on); dir C or S; counter: the dump's seq
//                                             (the wire header's datagram counter)
// The harness rebuilds the wire datagram: type (5 from the client, 4 from the server), session,
// counter, then the dump's bytes encrypted with the session key (CordCon::Encrypt_Data's key).
// OUT lines: R n dir session channel hex      a message the receiving CordCon delivered (n: the
//                                             datagram that completed it)
//            P n dir session status           status: ok, rejected (examinePacket), nocon
//            G n dir session acks blocks      what our sending side's pump produced before
//                                             datagram n: acks "k,ch,seq;...", blocks "ch,seq;..."
//            E dir session text               a write CordCon refused
//            O dir session wanted got         openc gave another channel than the capture's
// The end: counters per session.
//
//     udpreplay --loop N IN.txt OUT.txt [MS [WINDOW]]
//
// The loop test: the same messages (W) and closes (Q), but each side's own datagrams go to the
// other side over a link that drops every Nth datagram in each direction (N = 0: none); each D
// line is one tick of MS ms for its session (both sides pump, then read), and after the input the
// sessions run on until nothing is queued, unacked or due (at most 20,000 ticks). It tests what a
// dump can't: resend requests, the resend timer, the send window, the close. OUT: R lines as
// above, then per side "L session dir sent N dropped N resends N resend-requests N dup-acks N
// close-state N queued N unacked N". Then a session still open gets a go-away (type 6) from the
// server's address on the client side and runs 12 s on: "A session sent-after N close-state N".
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include "cordcon.h"
#include "cordmgr.h"
#include "udp.h"
#include "wtime.h"

extern unsigned __int64 Replay_Time_Ms;		// wtime_replay.cpp
static int Window_Limit = 65535;
static int Loop_Drop = -1;		// --loop N: drop every Nth datagram (-1: the capture replay)

// ------------------------------------------------------------------------- E&B's log (cordmgr.cpp)

class UnknownLogClass
{
public:
	void Unknown_4BC130(const char *category, const char *file, int line, const char *format, ...);
};
UnknownLogClass UnknownLog;
static int LogLines = 0;
void UnknownLogClass::Unknown_4BC130(const char *, const char *, int, const char *, ...)
{
	LogLines++;
}

// ------------------------------------------------------------------------- the replay socket

struct ReplayState
{
	std::deque<std::vector<unsigned char> > In;		// datagrams to Read
	std::vector<std::vector<unsigned char> > Out;	// datagrams written
	unsigned int FromIp;
	unsigned short FromPort;
};

static std::map<const UDP *, ReplayState> Replay;

UDP::UDP() : fd(-1), myIP(0), myPort(0)
{
	memset(&addr, 0, sizeof(addr));
	Replay[this];
}

UDP::~UDP()
{
	Replay.erase(this);
}

int UDP::Bind(unsigned int ip, unsigned short port)
{
	myIP = ip;
	myPort = port;
	return 0;
}

int UDP::Write(unsigned char *msg, unsigned int len, unsigned int ip, unsigned short port)
{
	Replay[this].Out.push_back(std::vector<unsigned char>(msg, msg + len));
	return (int)len;
}

int UDP::Read(unsigned char *msg, unsigned int len, sockaddr_in *from)
{
	ReplayState &s = Replay[this];
	if (s.In.empty()) {
		return -1;
	}
	std::vector<unsigned char> d = s.In.front();
	s.In.pop_front();
	unsigned int n = d.size() < len ? (unsigned int)d.size() : len;
	memcpy(msg, &d[0], n);
	memset(from, 0, sizeof(*from));
	from->sin_family = AF_INET;
	from->sin_addr.s_addr = htonl(s.FromIp);
	from->sin_port = htons(s.FromPort);
	return (int)n;
}

bool UDP::getLocalAddr(unsigned int &ip, unsigned short &port)
{
	ip = myIP;
	port = myPort;
	return true;
}

// ------------------------------------------------------------------------- one session, two sides

static const unsigned int SERVER_IP = 0x9F99E892;	// 159.153.232.146 (any fixed address)
static const unsigned int CLIENT_IP = 0x0A000001;

struct Side
{
	UDP *Sock;			// what the other side's datagrams arrive on
	UDP *OutSock;		// what this side's CordCon writes on
	CordMgr Mgr;
	CordCon *Con;
};

struct Session
{
	int Id;
	unsigned char Key[8];
	Side Client, Server;	// Client takes S datagrams, Server takes C datagrams
	int Datagrams[2], Accepted[2], Rejected[2], NoCon[2], Delivered[2], OpenMismatch[2], Generated[2], PeerOk[2];
	int LoopSent[2], LoopDropped[2];
};

static std::map<std::string, Session *> Sessions;

static int Hex(int c)
{
	return c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10;
}

static std::vector<unsigned char> From_Hex(const char *s)
{
	std::vector<unsigned char> v;
	while (s[0] && s[1] && s[0] != '\n' && s[0] != '\r') {
		v.push_back((unsigned char)(Hex(s[0]) * 16 + Hex(s[1])));
		s += 2;
	}
	return v;
}

static void Put_Hex(FILE *f, const unsigned char *p, int n)
{
	for (int i = 0; i < n; i++) {
		fprintf(f, "%02x", p[i]);
	}
}

static void Make_Side(Session *s, Side &side, bool server)
{
	side.Sock = new UDP();
	side.OutSock = new UDP();
	Replay[side.Sock].FromIp = server ? CLIENT_IP : SERVER_IP;
	Replay[side.Sock].FromPort = server ? 1024 : 3500;
	CordDataSocketClass ds = { side.Sock, 0, 0, 0 };
	side.Mgr.DataSockets_.Add(ds);
	unsigned int key[2];
	memcpy(key, s->Key, 8);
	side.Con = new CordCon(server ? CLIENT_IP : SERVER_IP, server ? 1024 : 3500, s->Id, 0, server, side.OutSock,
		Window_Limit, key[0], key[1]);
	side.Mgr.Connections_.Add(side.Con);
}

static Session *Get_Session(const std::string &name)
{
	std::map<std::string, Session *>::iterator it = Sessions.find(name);
	if (it != Sessions.end()) {
		return it->second;
	}
	Session *s = new Session();
	memset(s->Key, 0, 8);
	s->Id = (int)(strtoul(name.c_str(), 0, 16) & 0xFFFF);
	memset(s->Datagrams, 0, sizeof(s->Datagrams));
	memset(s->Accepted, 0, sizeof(s->Accepted));
	memset(s->Rejected, 0, sizeof(s->Rejected));
	memset(s->NoCon, 0, sizeof(s->NoCon));
	memset(s->Delivered, 0, sizeof(s->Delivered));
	memset(s->OpenMismatch, 0, sizeof(s->OpenMismatch));
	memset(s->Generated, 0, sizeof(s->Generated));
	memset(s->PeerOk, 0, sizeof(s->PeerOk));
	memset(s->LoopSent, 0, sizeof(s->LoopSent));
	memset(s->LoopDropped, 0, sizeof(s->LoopDropped));
	s->Client.Con = 0;
	s->Server.Con = 0;
	Sessions[name] = s;
	return s;
}

static void Ensure_Sides(Session *s)
{
	if (s->Client.Con == 0) {
		Make_Side(s, s->Client, false);
		Make_Side(s, s->Server, true);
	}
}

// Parse a datagram our side generated (plain: decrypt a copy) into the G line's two lists.
static void Describe(FILE *out, CordCon *by, std::vector<unsigned char> d)
{
	int len = (int)d.size();
	unsigned short counter = (unsigned short)((d[3] << 8) | d[4]);
	by->Encrypt_Data(&d[5], len - 5, counter);
	unsigned char *p = &d[0];
	int pos = 10, n = 0;
	if (p[9] & 1) {
		n = p[10];
		pos = 11;
	}
	pos++;
	std::string acks, blocks;
	char tmp[64];
	for (int i = 0; i < n; i++) {
		int kind = p[pos++];
		unsigned int ch, seq;
		if (p[pos] < 0x80) { ch = p[pos]; pos += 1; } else { ch = ((p[pos] << 8) | p[pos + 1]) & 0x7FFF; pos += 2; }
		if (p[pos] < 0x80) { seq = (p[pos] << 8) | p[pos + 1]; pos += 2; }
		else { seq = ((p[pos] << 24) | (p[pos + 1] << 16) | (p[pos + 2] << 8) | p[pos + 3]) & 0x7FFFFFFF; pos += 4; }
		sprintf(tmp, "%s%d,%u,%u", acks.empty() ? "" : ";", kind, ch, seq);
		acks += tmp;
	}
	while (pos < len) {
		unsigned int size, ch, seq;
		if (p[pos] < 0x80) { size = p[pos]; pos += 1; } else { size = ((p[pos] << 8) | p[pos + 1]) & 0x7FFF; pos += 2; }
		if (p[pos] < 0x80) { ch = p[pos]; pos += 1; } else { ch = ((p[pos] << 8) | p[pos + 1]) & 0x7FFF; pos += 2; }
		if (p[pos] < 0x80) { seq = (p[pos] << 8) | p[pos + 1]; pos += 2; }
		else { seq = ((p[pos] << 24) | (p[pos + 1] << 16) | (p[pos + 2] << 8) | p[pos + 3]) & 0x7FFFFFFF; pos += 4; }
		int type = p[pos++];
		unsigned int frag = 0, part = size;
		if (type & 0x10) {
			frag = p[pos++];
			part = size > (frag + 1) * 200 ? 200 : size - frag * 200;
		}
		sprintf(tmp, "%s%u,%u", blocks.empty() ? "" : ";", ch, seq);
		blocks += tmp;
		pos += part;
	}
	fprintf(out, " %s %s\n", acks.empty() ? "-" : acks.c_str(), blocks.empty() ? "-" : blocks.c_str());
}

// The capture replay's receive: CordMgr::Pump's data path (types 4 / 5 to Process_CORD_Packet)
// without Pump's tail, which pumps every connection: here each side pumps when the capture has it
// send (Pump_Side), so that its acks follow the capture's datagrams.
static void Deliver(Side &rx)
{
	unsigned char buf[0x400];
	sockaddr_in from;
	int len;
	while ((len = rx.Sock->Read(buf, sizeof(buf), &from)) >= 0) {
		if (len <= 3) {
			continue;
		}
		char type = (char)buf[0];
		if (!((type == 4 || type == 5) && rx.Mgr.Process_CORD_Packet(buf, len, &from, rx.Sock, 0, type))) {
			rx.Mgr.Unexpected_++;
		}
	}
}

// Our sending side's datagrams: described (G lines), and checked by the peer's examinePacket on a
// decrypted copy (checksum, layout, acks for what the peer sent).
static void Pump_Side(FILE *out, Session *s, Side &tx, Side &rx, int n, const char *dir, const char *name, int d)
{
	size_t before = Replay[tx.OutSock].Out.size();
	Wtime now;
	tx.Con->pump(now);
	for (size_t g = before; g < Replay[tx.OutSock].Out.size(); g++) {
		fprintf(out, "G %d %s %s", n, dir, name);
		Describe(out, tx.Con, Replay[tx.OutSock].Out[g]);
		s->Generated[d]++;
		std::vector<unsigned char> copy = Replay[tx.OutSock].Out[g];
		unsigned short counter = (unsigned short)((copy[3] << 8) | copy[4]);
		tx.Con->Encrypt_Data(&copy[5], (int)copy.size() - 5, counter);
		if (rx.Con->examinePacket(&copy[0], (int)copy.size())) {
			s->PeerOk[d]++;
		}
	}
}

// A message the capture's sender sends for the first time in datagram n: written on our sending
// side (openc when the channel is new to it).
static void Write_Side(FILE *out, Session *s, Side &tx, const char *dir, const char *name, int d, int channel,
	int mode, const std::vector<unsigned char> &data)
{
	CordChannelClass *tc = tx.Con->Get_Channel(channel);
	if (tc->Mode == 0) {
		unsigned int got = 0;
		tx.Con->openc(&got, mode, 0);
		if ((int)got != channel) {
			fprintf(out, "O %s %s %d %u\n", dir, name, channel, got);
			s->OpenMismatch[d]++;
			CordChannelClass *wrong = tx.Con->Channels_[got];
			wrong->Mode = 0;
			wrong->IsClosed = true;
			tx.Con->Active_.pop_back();
			tc->Mode = mode;
			tc->IsClosed = false;
			tx.Con->Active_.push_back(tc);
		}
	}
	if (!tx.Con->write(channel, data.empty() ? 0 : &data[0], (unsigned int)data.size(), 0)) {
		fprintf(out, "E %s %s write refused on channel %d\n", dir, name, channel);
	}
}

// The handshake (H lines): a client-side CordMgr with one data socket connects to ip:port, its
// connection given the capture's session id and key; ACK1 and ACK2 from the capture go through
// CordMgr::Pump (Process_ACK1_Packet, Process_ACK2_Packet). Prints what Pump_Connections sent
// (HS1 = SYN1, HS2 = SYN2) and the CordCon ACK2 made (HC n port key stats).
static void Handshake(FILE *out, const char *line)
{
	int n, at = 0;
	unsigned int ip, port, session;
	char key[32], ack1[1024], ack2[64];
	if (sscanf(line, "H %d %u %u %u %31s %1023s %63s", &n, &ip, &port, &session, key, ack1, ack2) < 7) {
		return;
	}
	(void)at;
	CordMgr mgr;
	UDP *sock = new UDP();
	Replay[sock].FromIp = ip;
	Replay[sock].FromPort = (unsigned short)port;
	CordDataSocketClass ds = { sock, 0, 0, 0 };
	mgr.DataSockets_.Add(ds);
	CordConnectingClass *c = mgr.Connect(ip, (unsigned short)port, true);
	c->Session = (int)session;
	std::vector<unsigned char> k = From_Hex(key);
	memcpy(&c->Key, &k[0], 8);
	size_t before = Replay[sock].Out.size();
	mgr.Pump_Connections();
	for (size_t g = before; g < Replay[sock].Out.size(); g++) {
		fprintf(out, "HS1 %d ", n);
		Put_Hex(out, &Replay[sock].Out[g][0], (int)Replay[sock].Out[g].size());
		fprintf(out, "\n");
	}
	Replay[sock].In.push_back(From_Hex(ack1));
	before = Replay[sock].Out.size();
	mgr.Pump();				// its tail: Pump_Connections sends SYN2 (ACK1 cleared the resend timer)
	mgr.Pump_Connections();	// not again within the second
	for (size_t g2 = before; g2 < Replay[sock].Out.size(); g2++) {
		fprintf(out, "HS2 %d ", n);
		Put_Hex(out, &Replay[sock].Out[g2][0], (int)Replay[sock].Out[g2].size());
		fprintf(out, "\n");
	}
	Replay[sock].In.push_back(From_Hex(ack2));
	mgr.Pump();
	if (mgr.Connections_.Length() == 1) {
		CordCon *con = mgr.Connections_[0];
		unsigned char kb[8];
		memcpy(kb, &con->Key_, 8);
		fprintf(out, "HC %d %u ", n, con->Port_);
		Put_Hex(out, kb, 8);
		fprintf(out, " %d\n", con->Session_);
	} else {
		fprintf(out, "HC %d none\n", n);
	}
}

// --loop: one side pumps, its datagrams cross the lossy link, the other side takes and reads them.
static void Loop_Send(FILE *out, Session *s, Side &tx, Side &rx, int d, const char *name, int n)
{
	static unsigned char msg[0x10000];
	Wtime now;
	tx.Con->pump(now);
	std::vector<std::vector<unsigned char> > &sent = Replay[tx.OutSock].Out;
	for (size_t g = 0; g < sent.size(); g++) {
		s->LoopSent[d]++;
		if (Loop_Drop > 0 && s->LoopSent[d] % Loop_Drop == 0) {
			s->LoopDropped[d]++;
			continue;
		}
		Replay[rx.Sock].In.push_back(sent[g]);
	}
	sent.clear();
	rx.Mgr.Pump();
	int channel, len;
	while (rx.Con->read(&channel, &len, msg, false)) {
		fprintf(out, "R %d %s %s %d ", n, d == 0 ? "C" : "S", name, channel);
		Put_Hex(out, msg, len);
		fprintf(out, "\n");
		s->Delivered[d]++;
	}
}

static void Loop_Tick(FILE *out, Session *s, const char *name, int n, int step_ms)
{
	Replay_Time_Ms += step_ms;
	Loop_Send(out, s, s->Client, s->Server, 0, name, n);
	Loop_Send(out, s, s->Server, s->Client, 1, name, n);
}

static bool Loop_Busy(CordCon *c)
{
	if (c->Queued_ != 0 || c->AcksPending_ != 0 || c->ResendsPending_ != 0) {
		return true;
	}
	for (size_t i = 0; i < c->Active_.size(); i++) {
		if (c->Active_[i]->Sent.size() != 0) {
			return true;
		}
	}
	return false;
}

static int Loop_Unacked(CordCon *c)
{
	int n = 0;
	for (size_t i = 0; i < c->Active_.size(); i++) {
		n += (int)c->Active_[i]->Sent.size();
	}
	return n;
}

int main(int argc, char **argv)
{
	if (argc > 2 && strcmp(argv[1], "--loop") == 0) {
		Loop_Drop = atoi(argv[2]);
		argv += 2;
		argc -= 2;
	}
	if (argc < 3) {
		fprintf(stderr, "usage: udpreplay IN.txt OUT.txt [MS [WINDOW]]\n");
		return 2;
	}
	int step_ms = argc > 3 ? atoi(argv[3]) : 0;
	if (argc > 4) {
		Window_Limit = atoi(argv[4]);
	}
	FILE *in = fopen(argv[1], "r");
	FILE *out = fopen(argv[2], "w");
	if (!in || !out) {
		fprintf(stderr, "cannot open the files\n");
		return 2;
	}
	static char line[1 << 17];
	static unsigned char msg[0x10000];
	int total = 0;
	while (fgets(line, sizeof(line), in)) {
		if (line[0] == 'K') {
			char name[32], key[32];
			if (sscanf(line, "K %31s %31s", name, key) == 2) {
				Session *s = Get_Session(name);
				std::vector<unsigned char> k = From_Hex(key);
				memcpy(s->Key, &k[0], 8);
			}
			continue;
		}
		if (line[0] == 'H' && Loop_Drop >= 0) {
			continue;
		}
		if (line[0] == 'Z' && Loop_Drop >= 0) {
			continue;
		}
		if (line[0] == 'D' && Loop_Drop >= 0) {
			int n;
			char name[32];
			if (sscanf(line, "D %d %31s", &n, name) == 2) {
				Session *s = Get_Session(name);
				Ensure_Sides(s);
				Loop_Tick(out, s, name, n, step_ms);
				total++;
			}
			continue;
		}
		if (line[0] == 'H') {
			Handshake(out, line);
			continue;
		}
		if (line[0] == 'W') {
			int n, channel, mode, at = 0;
			char name[32], dir[4];
			if (sscanf(line, "W %d %31s %3s %d %d %n", &n, name, dir, &channel, &mode, &at) < 5) {
				continue;
			}
			Session *s = Get_Session(name);
			Ensure_Sides(s);
			int d = dir[0] == 'C' ? 0 : 1;
			Write_Side(out, s, d == 0 ? s->Client : s->Server, dir, name, d, channel, mode, From_Hex(line + at));
			continue;
		}
		if (line[0] == 'Z' || line[0] == 'Q') {
			int n, seq = 0;
			char name[32], dir[4];
			if (sscanf(line + 1, " %d %31s %3s %d", &n, name, dir, &seq) < 3) {
				continue;
			}
			Session *s = Get_Session(name);
			Ensure_Sides(s);
			CordCon *tx = (dir[0] == 'C' ? s->Client : s->Server).Con;
			if (line[0] == 'Z') {
				tx->Channels_[0]->NextSeq = (unsigned int)seq;
			} else {
				tx->close();
			}
			continue;
		}
		if (line[0] != 'D') {
			continue;
		}
		int n, counter, at = 0;
		char name[32], dir[4];
		if (sscanf(line, "D %d %31s %3s %d %n", &n, name, dir, &counter, &at) < 4) {
			continue;
		}
		std::vector<unsigned char> dump = From_Hex(line + at);
		Session *s = Get_Session(name);
		Ensure_Sides(s);
		int d = dir[0] == 'C' ? 0 : 1;
		Side &rx = d == 0 ? s->Server : s->Client;		// receives this datagram
		Side &tx = d == 0 ? s->Client : s->Server;		// sent it
		s->Datagrams[d]++;
		total++;
		Replay_Time_Ms += step_ms;

		// What our sending side sends now: the messages written for this datagram, the acks due.
		Pump_Side(out, s, tx, rx, n, dir, name, d);

		// The captured datagram on the wire.
		std::vector<unsigned char> wire;
		wire.push_back((unsigned char)(d == 0 ? 5 : 4));
		wire.push_back((unsigned char)(s->Id >> 8));
		wire.push_back((unsigned char)s->Id);
		wire.push_back((unsigned char)(counter >> 8));
		wire.push_back((unsigned char)counter);
		wire.insert(wire.end(), dump.begin(), dump.end());
		tx.Con->Encrypt_Data(&wire[5], (int)wire.size() - 5, (unsigned short)counter);
		Replay[rx.Sock].In.push_back(wire);

		int rejected = rx.Con->Rejected_;
		int unexpected = rx.Mgr.Unexpected_;
		Deliver(rx);
		const char *status = "ok";
		if (rx.Mgr.Unexpected_ != unexpected) {
			status = "nocon";
			s->NoCon[d]++;
		} else if (rx.Con->Rejected_ != rejected) {
			status = "rejected";
			s->Rejected[d]++;
		} else {
			s->Accepted[d]++;
		}
		fprintf(out, "P %d %s %s %s\n", n, dir, name, status);

		int channel, len;
		while (rx.Con->read(&channel, &len, msg, false)) {
			fprintf(out, "R %d %s %s %d ", n, dir, name, channel);
			Put_Hex(out, msg, len);
			fprintf(out, "\n");
			s->Delivered[d]++;
		}
	}
	if (Loop_Drop >= 0) {
		for (std::map<std::string, Session *>::iterator it = Sessions.begin(); it != Sessions.end(); ++it) {
			Session *s = it->second;
			if (!s->Client.Con) {
				continue;
			}
			for (int t = 0; t < 20000 && (Loop_Busy(s->Client.Con) || Loop_Busy(s->Server.Con)); t++) {
				Loop_Tick(out, s, it->first.c_str(), -1, step_ms);
			}
			for (int d = 0; d < 2; d++) {
				CordCon *c = d == 0 ? s->Client.Con : s->Server.Con;
				fprintf(out, "L %s %s sent %d dropped %d resends %d resend-requests %d dup-acks %d close-state %d "
					"queued %d unacked %d\n", it->first.c_str(), d == 0 ? "C" : "S", s->LoopSent[d],
					s->LoopDropped[d], c->Resends_, c->ResendRequests_, c->DupAcks_, c->CloseState_, c->Queued_,
					Loop_Unacked(c));
			}
		}
		for (std::map<std::string, Session *>::iterator jt = Sessions.begin(); jt != Sessions.end(); ++jt) {
			Session *s = jt->second;
			if (!s->Client.Con || s->Client.Con->CloseState_ != 0) {
				continue;
			}
			std::vector<unsigned char> g(10, 0);
			g[0] = 6;
			g[7] = 4;
			g[8] = (unsigned char)(s->Id >> 8);
			g[9] = (unsigned char)s->Id;
			Replay[s->Client.Sock].In.push_back(g);
			s->Client.Mgr.Pump();
			int before = s->LoopSent[0];
			for (int t = 0; t < 600; t++) {		// the server is gone: only the client pumps
				Replay_Time_Ms += step_ms;
				Loop_Send(out, s, s->Client, s->Server, 0, jt->first.c_str(), -2);
			}
			fprintf(out, "A %s sent-after %d close-state %d\n", jt->first.c_str(), s->LoopSent[0] - before,
				s->Client.Con->CloseState_);
		}
		fprintf(out, "# ticks %d\n", total);
		fclose(out);
		fclose(in);
		return 0;
	}
	fprintf(out, "# datagrams %d\n", total);
	for (std::map<std::string, Session *>::iterator it = Sessions.begin(); it != Sessions.end(); ++it) {
		Session *s = it->second;
		for (int d = 0; d < 2; d++) {
			CordCon *rx = d == 0 ? s->Server.Con : s->Client.Con;
			if (!rx) {
				continue;
			}
			fprintf(out, "S %s %s datagrams %d accepted %d rejected %d nocon %d delivered %d generated %d "
				"peer-accepts %d openc-mismatch %d acks-matched %d dup-acks %d resend-requests %d\n",
				it->first.c_str(), d == 0 ? "C" : "S", s->Datagrams[d], s->Accepted[d], s->Rejected[d],
				s->NoCon[d], s->Delivered[d], s->Generated[d], s->PeerOk[d], s->OpenMismatch[d], rx->AcksMatched_,
				rx->DupAcks_, rx->ResendRequests_);
		}
	}
	fclose(out);
	fclose(in);
	return 0;
}
