// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordcon.cpp (client 0x84DFC0-0x8557D0)
**
**	Functional (NOT EXACT, D42): the receive path (handlePacket, examinePacket, read) follows the
**	client's code statement by statement; the send path (write, openc, buildHeader,
**	buildMsgHeader, pump, pumpInternal with its resend timer and send window) too; getHighestPriorityMessage
**	is byte-exact. Time is wlib's Xtime (libs\wwonline\wlib, D73), as in the client. The client's
**	asserts sit on lines 521, 632 and 977 (D25); the functions are in the client's order.
**	Notes at the end.
*/

// E&B built its own code with WWLib's profiler on (every CordCon function opens a sample:
// WWProfileManager::Start_Profile 0x9EA930 under the flag 0xC40488).
#define ENABLE_WWPROFILE
#include "cordcon.h"
#include "udp.h"
#include "rc4.h"
#include "wwprofile.h"
#include "..\shared\unknown_debug.h"
#include <string.h>
#include <time.h>
#include <windows.h>

// ------------------------------------------------------------------------ CordChannelClass

// The channels come from a pool of 1024 (0xBF40D8; ObjectPoolClass::Allocate_Object_Memory 0x8563E0
// with blocks of 4 + 1024 x 0x78 bytes).
DEFINE_AUTO_POOL(CordChannelClass, 1024)

// Reset's body, inlined into the constructor and the destructor as the client does (Reset itself is kept
// out of line below, where the CordCon constructor and openc call it).
static inline void Reset_Channel(CordChannelClass *c)	// NAME 2: Reset's body
{
	c->Mode = 0;
	c->IsClosed = true;
	c->LastWrite = (unsigned int)time(0);
	c->Priority = 0;
	c->Written = 0;
	c->Delivered = 0;
	c->Expected = 0;
	c->NextSequenced = 0;
	c->LastSeq = 0;
	c->Queue.clear();
	c->Incoming.Clear();
	c->Sent.clear();
	c->ToAck.clear();
	c->Missing.clear();
}

CordChannelClass::CordChannelClass()	// 0x84DFC0
{
	NextSeq = 0;
	Index = -1;
	Reset_Channel(this);
}

// 0x84E170: the members' constructors, then the copy.
CordChannelClass::CordChannelClass(const CordChannelClass &other)
{
	Copy(other);
}

// 0x84E230
CordChannelClass &CordChannelClass::operator=(const CordChannelClass &other)
{
	Copy(other);
	return *this;
}

// 0x84E260, NOT EXACT (functional, 0.14: inline budget): member by member in the client's order
// (the lists through wlib's operator=, the vectors emptied and refilled one record at a time). The
// client inlines ArrayList::clear, CordListClass's clear and getPointer and calls get / add, the
// vectors' erase / insert and the second size() out of line; ours inlines erase / insert and calls
// clear, Clear and Get.
void CordChannelClass::Copy(const CordChannelClass &other)
{
	if (this == &other) {
		return;
	}
	CordChannelClass &from = (CordChannelClass &)other;
	Mode = from.Mode;
	LastWrite = from.LastWrite;
	NextSeq = from.NextSeq;
	Priority = from.Priority;
	Written = from.Written;
	Delivered = from.Delivered;
	Expected = from.Expected;
	NextSequenced = from.NextSequenced;
	LastSeq = from.LastSeq;
	ToAck = from.ToAck;
	Missing = from.Missing;
	IsClosed = from.IsClosed;
	Incoming = from.Incoming;
	unsigned int i;
	Queue.erase(Queue.begin(), Queue.end());
	for (i = 0; i < from.Queue.size(); i++) {
		Queue.insert(Queue.end(), from.Queue[i]);
	}
	Sent.erase(Sent.begin(), Sent.end());
	for (i = 0; i < from.Sent.size(); i++) {
		Sent.insert(Sent.end(), from.Sent[i]);
	}
	Index = from.Index;
}

// 0x84E570: Reset only, as the client (the records' buffers are not freed: CordMessageClass's
// destructor leaves them; Free 0x85BEC0 is called where a record is dropped). Out of line, as
// ~CordCon 0x84F8C0 calls it.
#pragma auto_inline(off)
CordChannelClass::~CordChannelClass()
{
	Reset_Channel(this);
}
#pragma auto_inline(on)

// 0x84E770: out of line in CordCon's constructor and openc, as the client calls it (CordChannelClass's
// constructor and destructor inline Reset_Channel).
#pragma auto_inline(off)
void CordChannelClass::Reset()
{
	Mode = 0;
	IsClosed = true;
	LastWrite = (unsigned int)time(0);
	Priority = 0;
	Written = 0;
	Delivered = 0;
	Expected = 0;
	NextSequenced = 0;
	LastSeq = 0;
	Queue.clear();
	Incoming.Clear();
	Sent.clear();
	ToAck.clear();
	Missing.clear();
}
#pragma auto_inline(on)

// ------------------------------------------------------------------------ CordCon

// 0x84F640: 64 channels; channel 0 is the connection's own (mode 5: the empty message that carries
// acks when nothing else is queued, the keep-alive, the close). The statistics object (0x4C bytes,
// constructor 0x85BF30) gets stats_arg, the window limit ACK2 carries. Unknown1C and Unknown98 stay
// unset, as in the client. The six size() statements make no code: they spend the inliner's budget so
// that Active_'s push_back keeps insert(P, n, x) 0x855E40 out of line, as the client (with five or
// fewer it is inlined).
CordCon::CordCon(unsigned int ip, unsigned short port, int session, unsigned short unknown0c, bool is_server,
	UDP *udp, int stats_arg, __int64 key)
{
	Channels_.size();
	Channels_.size();
	Channels_.size();
	Channels_.size();
	Channels_.size();
	Channels_.size();
	Add_Channels(0x40);
	Key_ = key;
	Unknown14 = 0;
	Unknown18 = 0;
	Stats_ = new CordStatsClass(stats_arg);
	Ip_ = ip;
	Port_ = port;
	Session_ = session;
	Unknown0C = unknown0c;
	Udp_ = udp;
	IsServer_ = is_server;
	OpenChannels_ = 0;
	AcksPending_ = 0;
	ResendsPending_ = 0;
	Queued_ = 0;
	LastSend_ = 0;
	LastReceive_ = (unsigned int)time(0);
	LastPump_ = (unsigned int)time(0);
	GoneAway_ = false;
	Resends_ = 0;
	DupAcks_ = 0;
	MessagesSent_ = 0;
	PacketsIn_ = 0;
	PacketsOut_ = 0;
	Quality_ = 0;
	HeaderIn_ = 0;
	BytesSent_ = 0;
	BytesRead_ = 0;
	BytesResent_ = 0;
	BytesAcked_ = 0;
	ExpireCountdown_ = 0;
	CloseState_ = 0;
	CloseSeq_ = 0;
	for (unsigned int i = 0; i < Channels_.size(); i++) {
		Channels_[i]->Reset();
	}
	Channels_[0]->Mode = 5;
	Active_.push_back(Channels_[0]);
}

// 0x84F8C0: the channels back to their pool; Active_ and Ready_ emptied with resize(0) (the
// client's size() / erase(F, L) calls and the default value's slot), the socket dropped, then the
// statistics.
CordCon::~CordCon()
{
	for (unsigned int i = 0; i < Channels_.size(); i++) {
		delete Channels_[i];
	}
	Active_.resize(0);
	Ready_.resize(0);
	Udp_ = NULL;
	delete Stats_;
}

// 0x84FA80
int CordCon::Unknown_84FA80()
{
	return CORD_MAX_PACKET;
}

// 0x84FAA0 (out of line with no caller: inlined where it is used). c starts NULL outside the loop
// (the client's dead store to its slot).
void CordCon::Add_Channels(int count)
{
	CordChannelClass *c = NULL;
	for (int i = 0; i < count; i++) {
		c = new CordChannelClass;
		c->Index = (int)Channels_.size();
		Channels_.push_back(c);
	}
}

CordChannelClass *CordCon::Get_Channel(int channel)
{
	while ((int)Channels_.size() <= channel) {
		Add_Channels(2);
	}
	return Channels_[channel];
}

#define CORD_MIN(a, b)	((a) < (b) ? (a) : (b))

// 0x84FDE0: the counts UDPConnection reads (Receive, Get_Stats).
void CordCon::getStats(int *messages_sent, int *packets_in, int *bytes_sent, int *bytes_read, int *resends,
	int *dup_acks, unsigned int *window, unsigned int *in_flight, unsigned int *round_trip, unsigned int *quality, int *idle)
{
	*messages_sent = MessagesSent_;
	*packets_in = PacketsIn_;
	*bytes_sent = BytesSent_;
	*bytes_read = BytesRead_;
	*resends = Resends_;
	*dup_acks = DupAcks_;
	*window = CORD_MIN(Stats_->MaxWindow, CORD_MIN(Stats_->Window, Stats_->PeerWindow));
	*in_flight = Stats_->InFlight;
	*round_trip = Stats_->RoundTrip;
	*quality = (unsigned int)(HeaderIn_ + Quality_) >> 1;
	*idle = time(0) - LastReceive_;
}

// 0x84FED0: the messages the active channels hold, written and not sent or sent and not acked
// (CordMgr::Pending_Messages 0x8598F0 sums it over the connections)
int CordCon::Pending_Messages()
{
	int total = 0;
	int count = Active_.size();
	for (int i = 0; i < count; i++) {
		CordChannelClass *channel = Active_[i];
		total += channel->Queue.size();
		total += channel->Sent.size();
	}
	return total;
}

// 0x84FF70, NOT EXACT (0.48): the client opens odd channels, the server even ones: the unused one
// written least recently, the search going on from where it stopped after two more are made. Not
// reproduced: the client's dead test on the first index (`sete` / `test cl, 1`).
bool CordCon::openc(unsigned int *channel, int mode, int priority)
{
	WWPROFILE("CordCon::openc");
	int start = IsServer_ ? 2 : 1;
	int found = -1;
	for (;;) {
		int i;
		int size = Channels_.size();
		for (i = start; i < size; i += 2) {
			if (Channels_[i]->Mode == 0) {
				if (found <= -1 || (int)Channels_[i]->LastWrite < (int)Channels_[found]->LastWrite) {
					found = i;
				}
			}
		}
		if (found > -1) {
			break;
		}
		Add_Channels(2);
		start = i;
	}
	Channels_[found]->Reset();
	Channels_[found]->Mode = mode;
	Channels_[found]->Priority = priority;
	Channels_[found]->IsClosed = false;
	*channel = found;
	OpenChannels_++;
	Active_.push_back(Channels_[found]);
	return true;
}

// 0x8501A0: marks a channel closed (UDPConnection::Send_Data after a close-channel send).
bool CordCon::closec(int channel)
{
	Channels_[channel]->IsClosed = true;
	return false;
}

// 0x8501D0 (`? true : false`: sete al without the xor)
bool CordCon::Is_Closed()
{
	if (Udp_ == NULL) {
		return true;
	}
	return (CloseState_ == 4) ? true : false;
}

// 0x850200: returns false; no caller and no string in the client (between Is_Closed and Go_Away).
bool CordCon::Unknown_850200()
{
	return false;
}

// 0x850220: the idle close (pump: 90 s without a datagram) 10 s from now at the latest.
void CordCon::Go_Away()
{
	unsigned int t = (unsigned int)time(0) - 80;
	if ((int)LastReceive_ > (int)t) {
		LastReceive_ = t;
	}
	GoneAway_ = true;
}

// 0x850260: state 1 (close asked), nothing queued or due: "CLOSE" on channel 0, state 2 until
// its ack (handlePacket: state 4). State 2 with channel 0 empty: the client logs 'This isn't
// good... waiting for an ACK on a non-existant msg....' and goes back to 1; still waiting 90 s
// after the last datagram, it gives up (state 4, false).
bool CordCon::pumpClose()
{
	WWPROFILE("CordCon::pumpClose");
	if (CloseState_ == 2) {
		if (Channels_[0]->Queue.size() + Channels_[0]->Sent.size() == 0) {










			// the client logs 'This isn't good... waiting for an ACK on a non-existant msg....' (INFMSG, line 355)
			CloseState_ = 1;
		}
		if (CloseState_ == 2 && (int)(time(0) - LastReceive_) > 90) {
			CloseState_ = 4;
			return false;
		}
	}
	if (CloseState_ != 1) {
		return false;
	}
	if (Queued_ == 0 && ResendsPending_ == 0 && AcksPending_ == 0) {
		int count = Active_.size();
		for (int i = 0; i < count; i++) {
			if (Active_[i]->Queue.size() != 0) {
				return true;
			}
			if (Active_[i]->Sent.size() != 0) {
				return true;
			}
		}














		// the client logs 'All data sent, queueing close packet.' (INFMSG, line 390)
		CordMessageClass m;
		m.Data = new unsigned char[strlen("CLOSE")];
		memcpy(m.Data, "CLOSE", strlen("CLOSE"));
		m.Length = strlen("CLOSE");
		m.Priority = 0;
		m.SendCount = 0;
		m.TotalLength = m.Length;
		m.SentTime.setDay(0);
		m.SentTime.setMsec(0);
		m.Seq = Channels_[0]->NextSeq++;
		CloseSeq_ = m.Seq;
		Channels_[0]->Queue.push_back(m);
		Channels_[0]->LastWrite = time(0);
		Channels_[0]->Written++;
		CloseState_ = 2;
		Queued_++;
	}
	return true;
}

// 0x850870 ('CordCon:close'), functional (NOT EXACT: the vectors' and wlib's inlined code): with
// now, every channel's queued and sent records are freed and its acks and resend requests dropped;
// then the state: 3 (the peer's close read) -> 4, 0 -> 1 and pumpClose (the "CLOSE" once nothing
// is queued or due). Nothing changes once closed (4).
bool CordCon::close(bool now)
{
	WWPROFILE("CordCon:close");
	if (CloseState_ == 4) {
		return true;
	}
	if (now) {
		int count = Active_.size();
		for (int i = 0; i < count; i++) {
			CordChannelClass *c = Active_[i];
			std::vector<CordMessageClass>::iterator it = c->Queue.begin();
			while (c->Queue.size() != 0) {
				(*it).Free();
				if (it->SendCount == 0) {
					Queued_--;
				}
				c->Queue.erase(it);
			}
			it = c->Sent.begin();
			while (c->Sent.size() != 0) {
				(*it).Free();
				if (it->SendCount == 0) {
					Queued_--;
				}
				c->Sent.erase(it);
			}
			while (c->ToAck.length() != 0) {
				c->ToAck.remove(0);
				AcksPending_--;
			}
			while (c->Missing.length() != 0) {
				c->Missing.remove(0);
				ResendsPending_--;
			}
		}
		Queued_ = 0;
		AcksPending_ = 0;
		ResendsPending_ = 0;
	}
	if (CloseState_ == 3) {
		CloseState_ = 4;
	} else if (CloseState_ == 0) {
		CloseState_ = 1;
		pumpClose();
	}
	return true;
}























































// 0x850C70: the message in 200-byte records, one sequence number each.
bool CordCon::write(int channel, unsigned int len, const void *data, int priority)
{
	WWPROFILE("CordCon::write");
	FAIL_IF(Udp_ == NULL) return false;
	if (CloseState_ != 0) {
		return false;
	}
	if (len > 0xC800) {
		return false;
	}
	CordChannelClass *c = Channels_[channel];
	if (c->IsClosed) {
		return false;
	}
	if (c->Mode == 4) {
		// a sequenced channel drops what it hasn't sent yet
		std::vector<CordMessageClass>::iterator it = c->Queue.begin();
		while (it != c->Queue.end()) {
			if (it->SendCount == 0) {
				(*it).Free();
				it = c->Queue.erase(it);
				Queued_--;
			} else {
				++it;
			}
		}
	}
	CordMessageClass m;
	int n = (len + CORD_FRAGMENT_SIZE - 1) / CORD_FRAGMENT_SIZE;
	const char *p = (const char *)data;
	for (int i = 0; i < n; i++) {
		m.Seq = c->NextSeq++;
		unsigned int part = len;
		if (n != 1 && (i != n - 1 || (part = len % CORD_FRAGMENT_SIZE) == 0)) {
			part = CORD_FRAGMENT_SIZE;
		}
		m.Data = new unsigned char[part];
		m.Length = part;
		m.TotalLength = len;
		m.Priority = priority;
		m.FragIndex = i;
		m.Flags = 0;
		if (n != 1) {
			m.Flags = 1;
		}
		if (c->Written == 0) {
			m.Flags |= 2;
		}
		m.SentTime.setDay(0);
		m.SentTime.setMsec(0);
		m.SendCount = 0;
		memcpy(m.Data, p, part);
		c->Queue.push_back(m);
		c->Written++;
		c->LastWrite = (unsigned int)time(0);
		p += CORD_FRAGMENT_SIZE;
		Queued_++;
	}
	return true;
}


















































// 0x850FD0
bool CordCon::read(int *channel, int *len, void *buf, bool peek)
{
	WWPROFILE("CordCon::read");
	FAIL_IF(Udp_ == NULL) return false;
	int count = (int)Ready_.size();
	for (int r = 0; r < count; r++) {
		CordChannelClass *c = Ready_[r];
		bool removed = false;
		int i = 0;
		int n = (int)c->Incoming.Length();
		for (int k = 0; k < n; k++) {
			CordMessageClass &m = *c->Incoming.Get(i);
			if (c->Mode == 3) {
				if (m.Seq < c->Delivered) {
					m.Free();
					c->Incoming.Remove(i);
					removed = true;
					continue;
				}
				if (m.Seq != c->Delivered) {
					i++;
					continue;
				}
			}
			bool incomplete = (m.Flags & 1) && m.Fragments != 0 && m.Length != 0
				&& memchr(m.Fragments, 0, (m.TotalLength + CORD_FRAGMENT_SIZE - 1) / CORD_FRAGMENT_SIZE) != 0;
			if (incomplete) {
				i++;
				continue;
			}
			if (c->Mode == 3 && !peek) {
				c->Delivered++;
			}
			if (m.Length != 0) {
				if (buf != 0) {
					memcpy(buf, m.Data, m.Length);
				}
				*len = (int)m.Length;
				*channel = c->Index;
				if (!peek) {
					m.Free();
					c->Incoming.Remove(i);
				}
				BytesRead_ += *len;
				if (c->Incoming.Length() == 0) {
					Ready_.erase(Ready_.begin() + r);
				}
				return true;
			}
			c->Incoming.Remove(i);
			removed = true;
			if (c->Mode == 3 && peek == true) {
				c->Delivered++;
			}
		}
		if (removed && c->Incoming.Length() == 0) {
			Ready_.erase(Ready_.begin() + r);
			r--;
			count--;
		}
	}
	if (CloseState_ == 3) {
		CloseState_ = 4;
	}
	return false;
}

int __stdcall Cord_Get_Small(const unsigned char *in, unsigned short *value);
int __stdcall Cord_Get_Seq(const unsigned char *in, unsigned int *value);

// 0x8514D0: the checks a decrypted datagram must pass before handlePacket reads it, the fields
// read through the Cord_Get helpers (inlined, as the client's `mov eax, n` / `add edi, eax`).
// NOT EXACT (0.73, explain.py: shape, 20 masked rows): the client's ntohs / ntohl arguments go through
// eax (the counter at +3, the CRC at +5, the first loop's seq), ours ecx / edx, and it keeps the channel in
// ebp over the channel-adding loop where we reload it; attempts.py show 0x8514D0.
bool CordCon::examinePacket(unsigned char *data, int datalen)
{
	WWPROFILE("CordCon::examinePacket");
	if (datalen > 500 || datalen < 14) {
		return false;
	}
	if (data[0] != 4 && data[0] != 5) {
		return false;
	}
	if (ntohs(*(unsigned short *)(data + 1)) != (unsigned int)Session_) {
		return false;
	}
	unsigned short counter = ntohs(*(unsigned short *)(data + 3));
	unsigned long crc = ntohl(*(unsigned long *)(data + 5));
	*(unsigned long *)(data + 5) = htonl(0);
	unsigned long calc = Cord_CRC(data, datalen, 0xFFFFFFFF);
	*(unsigned long *)(data + 5) = htonl(crc);
	if (crc != calc) {
		return false;
	}
	int pos = 10;
	if (data[9] > 3) {
		return false;
	}
	unsigned char acks = 0;
	if (data[9] & 1) {
		acks = data[10];
		pos = 11;
		if (acks > 40) {
			return false;
		}
	}
	unsigned char resends = data[pos++];
	if (resends > 100) {
		return false;
	}
	for (int i = 0; i < acks; i++) {
		if (pos > datalen) {
			return false;
		}
		unsigned char kind = data[pos++];
		unsigned short channel;
		pos += Cord_Get_Small(data + pos, &channel);
		unsigned int seq;
		pos += Cord_Get_Seq(data + pos, &seq);
		if (kind > 2) {
			return false;
		}
		if (channel >= Channels_.size()) {
			return false;
		}
		if (seq > Channels_[channel]->NextSeq) {
			return false;
		}
	}
	int total = 0;
	unsigned char frag;
	while (pos < datalen) {
		unsigned short size;
		pos += Cord_Get_Small(data + pos, &size);
		unsigned short channel;
		pos += Cord_Get_Small(data + pos, &channel);
		unsigned int seq;
		pos += Cord_Get_Seq(data + pos, &seq);
		unsigned char flags = data[pos++];
		unsigned char type = flags & 0x0F;
		flags >>= 4;
		if (type > 5) {
			return false;
		}
		while (channel >= Channels_.size()) {
			CordChannelClass *c = NULL;
			for (int k = 0; k < 8; k++) {
				c = new CordChannelClass;
				c->Index = Channels_.size();
				Channels_.push_back(c);
			}
		}
		flags &= 1;
		if (flags) {
			frag = data[pos++];
		} else {
			frag = 0;
		}
		int len = size;
		if (len > CORD_FRAGMENT_SIZE) {
			if (size <= (frag + 1) * CORD_FRAGMENT_SIZE) {
				len -= frag * CORD_FRAGMENT_SIZE;
			} else {
				len = CORD_FRAGMENT_SIZE;
			}
		}
		total += len;
		if (len != size && !flags) {
			return false;
		}
		if ((frag - 1) * CORD_FRAGMENT_SIZE > size) {
			return false;
		}
		int d = seq - Channels_[channel]->LastSeq;
		if (d < -1000 || d > 1000) {
			return false;
		}
		pos += len;
	}
	if (pos != datalen) {
		return false;
	}
	return true;
}

// The harness's counters (D176): udpreplay.py build defines CORD_HARNESS_COUNT to count into udpreplay's
// side table (tests/udpreplay/cord_counters.h); the client has none, and here it is nothing.
#ifndef CORD_HARNESS_COUNT
#define CORD_HARNESS_COUNT(con, counter)
#endif

static CordMessageClass Empty_Message()
{
	CordMessageClass m;
	memset(&m, 0, sizeof(m));
	return m;
}
















































































































































// 0x851AE0
bool CordCon::handlePacket(unsigned char *data, int datalen)
{
	WWPROFILE("CordCon::handlePacket");
	Xtime current;
	int acks = 0;
	FAIL_IF(datalen < PACKET_ENCRYPTION_OFFSET) return false;
	unsigned short counter = ntohs(*(unsigned short *)(data + 3));
	Encrypt_Data(data + PACKET_ENCRYPTION_OFFSET, datalen - PACKET_ENCRYPTION_OFFSET, counter);
	if (!examinePacket(data, datalen)) {
		Encrypt_Data(data + PACKET_ENCRYPTION_OFFSET, datalen - PACKET_ENCRYPTION_OFFSET, counter);
		CORD_HARNESS_COUNT(this, Rejected);
		return false;
	}
	PacketsIn_++;
	LastReceive_ = current.getTime_t();
	GoneAway_ = false;
	int pos = 10;
	if (data[9] & 2) {
		if (CloseState_ == 1 || CloseState_ == 2) {
			CloseState_ = 4;
		} else {
			CloseState_ = 3;
		}
	}
	if (data[9] & 1) {
		acks = data[10];
		pos = 11;
	}
	HeaderIn_ = data[pos++];

	// The ack list: 0 acks one message, 2 every message up to seq, 1 asks for a resend.
	for (int a = 0; a < acks; a++) {
		unsigned int kind = data[pos];
		unsigned short channel;
		unsigned int seq;
		pos++;
		pos += Cord_Get_Small(data + pos, &channel);
		pos += Cord_Get_Seq(data + pos, &seq);
		if (CloseState_ == 2) {
			if ((kind == 0 && CloseSeq_ == seq) || (kind == 2 && CloseSeq_ <= seq)) {
				CloseState_ = 4;
			}






















































			// else the client logs 'This is probably a bug! ACK for non-close message recvd' (INFMSG, line 1068)
		}
		CordChannelClass *c = Channels_[channel];
		if (kind == 0 || kind == 2) {
			bool found = false;
			std::vector<CordMessageClass>::iterator it = c->Sent.begin();
			while (it != c->Sent.end()) {
				if (kind == 0 ? it->Seq == seq : it->Seq <= seq) {
					found = true;
					if (it->SendCount == 1 && kind == 0) {
						Xtime rtt(0);
						rtt = current - it->SentTime;
						if (rtt.getDay() == 0) {
							Stats_->On_Round_Trip(rtt.getMsec());
						}
					}
					Stats_->On_Ack(it->Length);
					BytesAcked_ += it->Length;
					CORD_HARNESS_COUNT(this, AcksMatched);
					(*it).Free();
					it = c->Sent.erase(it);
					if (kind == 0) {
						break;
					}
				} else {
					++it;
				}
			}
			if (!found && kind == 0) {
				Stats_->On_Loss(-1, false);
				Stats_->On_Round_Trip(Stats_->RoundTrip * 2);










































				// the client logs 'Dup ack' (INFMSG, line 1141)
				DupAcks_++;
			}
		} else if (kind == 1) {
			for (std::vector<CordMessageClass>::iterator it = c->Sent.begin(); it != c->Sent.end(); ++it) {
				if (it->Seq == seq) {
					CordMessageClass m = *it;
					m.SentTime.setDay(0);
					m.SentTime.setMsec(0);
					c->Sent.erase(it);
					c->Sent.insert(c->Sent.begin(), m);
					CORD_HARNESS_COUNT(this, ResendRequests);
					break;
				}
			}
		}
	}

	// The blocks.
	for (;;) {
		if (pos == datalen) {
			return true;
		}
		if (pos > datalen) {
			return false;
		}
		unsigned short size, channel;
		unsigned int seq;
		pos += Cord_Get_Small(data + pos, &size);
		pos += Cord_Get_Small(data + pos, &channel);
		pos += Cord_Get_Seq(data + pos, &seq);
		unsigned char type = data[pos];
		pos++;
		if (channel >= Channels_.size()) {
			Add_Channels((int)((channel - Channels_.size()) + 3) & ~1);
		}
		CordChannelClass *c = Channels_[channel];
		c->LastSeq = seq;
		int mode = type & 0x0F;
		int bflags = type >> 4;
		bool isfrag = (bflags & 1) != 0;
		unsigned int len = size;
		unsigned int frag = 0;
		unsigned int first = seq;
		if (isfrag) {
			frag = data[pos];
			pos++;
			first = seq - frag;
			len = (int)((frag * 5 + 5) * 0x28) < (int)size ? CORD_FRAGMENT_SIZE : (unsigned short)(size - frag * CORD_FRAGMENT_SIZE);
		}
		if (mode != c->Mode && c->Mode != 0 && mode == 3) {
			if ((bflags & 2) == 0) {
				pos += len;
				continue;
			}
			c->Delivered = seq;
			c->Expected = seq;
		}
		if (mode != c->Mode && c->Mode == 0) {
			Active_.push_back(c);
		}
		c->Mode = mode;
		if ((unsigned short)len != 0) {
			c->ToAck.addTail(seq);
			AcksPending_++;
		}
		size_t i;
		// A gap before seq: ask for the missing ones back to the first already held.
		if (c->Mode == 3 && seq != 0 && (int)(c->Expected - 1) < (int)(seq - 1)) {
			for (unsigned int s = seq - 1; (int)(c->Expected - 1) < (int)s; s--) {
				bool held = false;
				for (i = 0; i < (size_t)c->Incoming.Length(); i++) {
					if (c->Incoming.Get(i)->Seq == s) {
						held = true;
						break;
					}
				}
				if (held) {
					break;
				}
				c->Missing.addTail(s);
				ResendsPending_++;
			}
		}
		bool found = false;
		for (i = 0; i < (size_t)c->Incoming.Length(); i++) {
			if (c->Incoming.Get(i)->Seq == seq) {
				found = true;
			}
		}
		while (c->Mode == 3) {
			if (seq == c->Expected) {
				c->Expected++;
			}
			bool next = false;
			for (i = 0; i < (size_t)c->Incoming.Length(); i++) {
				if (c->Incoming.Get(i)->Seq == c->Expected && c->Incoming.Get(i)->Time != 0) {
					next = true;
					break;
				}
			}
			if (!next) {
				break;
			}
			c->Expected++;
		}
		if (!isfrag) {
			if (found) {
				pos += len;
				continue;
			}
		} else if (found) {
			if (first == seq) {
				for (i = 0; i < (size_t)c->Incoming.Length(); i++) {
					if (c->Incoming.Get(i)->Seq == first) {
						c->Incoming.Get(i)->Fragments[frag] = 1;
						memcpy(c->Incoming.Get(i)->Data + frag * CORD_FRAGMENT_SIZE, data + pos, len);
					}
				}
			}
			pos += len;
			continue;
		}
		if ((unsigned short)channel == 0) {
			pos += len;		// channel 0 carries no messages
			continue;
		}
		if (c->Mode == 4) {
			if (seq < c->NextSequenced) {
				pos += len;
				continue;
			}
			c->NextSequenced = seq + 1;
		}
		if ((unsigned short)len == 0) {
			continue;
		}
		if (!isfrag) {
			CordMessageClass m = Empty_Message();
			m.Data = new unsigned char[len];
			memcpy(m.Data, data + pos, len);
			m.Length = m.TotalLength = len;
			m.Seq = seq;
			m.Time = current.getTime_t();
			c->Incoming.Add_Tail(m);
			if (c->Incoming.Length() == 1) {
				Ready_.push_back(c);
			}
			pos += len;
			continue;
		}
		int target = -1;
		for (i = 0; i < (size_t)c->Incoming.Length(); i++) {
			if (c->Incoming.Get(i)->Seq == first) {
				target = (int)i;
				break;
			}
		}
		if (target < 0) {
			if (first < c->Delivered) {
				pos += len;
				continue;
			}
			CordMessageClass m = Empty_Message();
			m.Data = new unsigned char[size];
			unsigned int n = (size + CORD_FRAGMENT_SIZE - 1) / CORD_FRAGMENT_SIZE;
			m.Fragments = new unsigned char[n];
			memset(m.Fragments, 0, n);
			m.Length = m.TotalLength = size;
			m.Seq = first;
			m.Flags = 1;
			c->Incoming.Add_Tail(m);
			if (c->Incoming.Length() == 1) {
				Ready_.push_back(c);
			}
			target = (int)c->Incoming.Length() - 1;
		}
		CordMessageClass &t = *c->Incoming.Get(target);
		t.Fragments[frag] = 1;
		memcpy(t.Data + frag * CORD_FRAGMENT_SIZE, data + pos, len);
		if (seq == first) {
			if (t.Time == 0) {
				t.Time = current.getTime_t();
			}
		} else {
			// a place holder for the fragment's own sequence number (read() skips it)
			CordMessageClass m = Empty_Message();
			m.Seq = seq;
			m.Time = current.getTime_t();
			c->Incoming.Add_Tail(m);
			if (c->Incoming.Length() == 1) {
				Ready_.push_back(c);
			}
		}
		pos += len;
	}
}

// 0x853560: pumpInternal,
// and while acks or resend requests are due with nothing queued, an empty message on channel 0
// to carry them; the keep-alive (an empty message after 5 s without a send, every second until
// something arrives), the idle close (90 s without a datagram), and the channels this side
// opened that are closed and empty leave the active list. The client's Ready_ search steps from r to
// i, not r + 1 (kept: it loops for good when Ready_[0] and Ready_[i] are other channels).
bool CordCon::pump(Xtime &now)
{
	WWPROFILE("CordCon::pump");
	if (Udp_ == NULL) {
		return false;
	}
	unsigned int t = now.getTime_t();
	if ((int)(t - LastPump_) > 45) {
		LastReceive_ = t;
	}
	LastPump_ = t;
	while (true) {
		if (GoneAway_) {
			pumpClose();
			break;
		}
		pumpInternal(now);
		if (Queued_ != 0 || (AcksPending_ == 0 && ResendsPending_ == 0)) {
			break;
		}
		Queue_Empty_Message();
	}
	if (((int)(t - LastSend_) > 5 || (t != LastSend_ && PacketsIn_ == 0)) && CloseState_ != 3 && CloseState_ != 4) {
		Queue_Empty_Message();
		LastSend_ = t;
	}
	if ((int)(t - LastReceive_) > 90 && (CloseState_ == 0 || CloseState_ == 1)) {
		close(true);
	}
	for (size_t i = 1; i < Active_.size(); i++) {
		CordChannelClass *c = Active_[i];
		if (!c->IsClosed || c->Queue.size() != 0 || c->Sent.size() != 0) {
			continue;
		}
		if (IsServer_ && (c->Index & 1)) {
			continue;		// the peer's channel
		}
		if (!IsServer_ && !(c->Index & 1)) {
			continue;
		}
		c->Mode = 0;
		unsigned int seq;
		while (c->ToAck.removeHead(seq)) {
			AcksPending_--;
		}
		if (c->Incoming.Length() != 0) {
			for (size_t r = 0; r < Ready_.size(); r = i) {		// sic (the client's)
				if (Ready_[r] == c) {
					Ready_.erase(Ready_.begin() + r);
					break;
				}
			}
		}
		Active_.erase(Active_.begin() + i);
		i--;
		OpenChannels_--;
	}
	return true;
}

static void Finish_Datagram(CordCon *con, unsigned char *buf, int len, unsigned short counter)
{
	unsigned int crc = Cord_CRC(buf, len, 0xFFFFFFFF);		// 'CordCon::Set_Header_CRC'
	buf[5] = (unsigned char)(crc >> 24);
	buf[6] = (unsigned char)(crc >> 16);
	buf[7] = (unsigned char)(crc >> 8);
	buf[8] = (unsigned char)crc;
	con->Encrypt_Data(buf + PACKET_ENCRYPTION_OFFSET, len - PACKET_ENCRYPTION_OFFSET, counter);
	con->Udp_->Write(buf, len, con->Ip_, con->Port_);		// 'CordCon::pumpInternal Udp_->Write'
	con->PacketsOut_++;
}

// 0x853A30, functional (NOT EXACT: Xtime, the vectors' inlined code and the log calls differ):
// pumpClose; every 8 calls the unreliable (mode 1 / 4) sent records older than 6 round trips are
// dropped (the client logs 'Removing unreliable'); then the records by priority
// (getHighestPriorityMessage) while the send window allows (CordStatsClass::Allow), packed into
// datagrams of under 500 bytes. A first send counts in flight; a resend counts as a loss (a
// timeout unless the peer asked for it). Sent records wait at the end of the channel's sent list
// for their ack; 0-byte ones are dropped. The header byte (Quality_) follows the share of
// reliable sends that were resends.
bool CordCon::pumpInternal(Xtime &now)
{
	WWPROFILE("CordCon::pumpInternal");
	unsigned char head[0x100];
	unsigned char buf[0x400];
	int hlen = 0;
	int pos = 0;
	unsigned short counter = 0;
	int channel = 0;
	int index = 0;
	CordMessageClass *m = 0;
	int reliable = 0;
	int resent = 0;
	Xtime expiry(now);
	Xtime threshold(now);
	Xtime timeout(now);
	pumpClose();
	expiry.setDay(0);
	expiry.setMsec(Stats_->RoundTrip * 6);
	Stats_->Update(now);
	Stats_->Get_Resend_Timeout(timeout);
	threshold -= timeout;
	if (--ExpireCountdown_ < 1) {
		WWPROFILE("CordCon::pumpInternal RemoveExpired");
		ExpireCountdown_ = 8;
		for (size_t i = 0; i < Active_.size(); i++) {
			CordChannelClass *c = Active_[i];
			if (c->Mode != 1 && c->Mode != 4) {
				continue;
			}
			std::vector<CordMessageClass>::iterator it = c->Sent.begin();
			while (it != c->Sent.end()) {
				if (it->SendCount != 0 && (now - it->SentTime) > expiry) {



































































































































































































































































































					// the client logs 'Removing unreliable' (INFMSG, line 1749)
					Stats_->InFlight -= it->Length;
					Stats_->On_Loss(it->Length, true);
					(*it).Free();
					it = c->Sent.erase(it);
					continue;
				}
				++it;
			}
		}
	}
	while (getHighestPriorityMessage(&channel, &index, threshold, &m, now)) {
		if (!Stats_->Allow(channel, m->Length, m->SendCount, now)) {
			break;
		}
		CordChannelClass *c = Channels_[channel];
		MessagesSent_++;
		BytesSent_ += m->Length;
		Stats_->SecondBytes += m->Length;
		if (m->SendCount == 0) {
			if (m->Length != 0 && c->Mode == 3) {
				reliable++;
			}
			Stats_->Add_In_Flight(m->Length);
		} else {
			Resends_++;
			if (m->Length != 0 && c->Mode == 3) {
				reliable++;
				resent++;
			}
			BytesResent_ += m->Length;
			Stats_->On_Loss(m->Length, m->SentTime.getDay() != 0);
		}
		if (hlen == 0) {
			buildHeader(head, &hlen, &counter);
			memcpy(buf, head, hlen);
			pos = hlen;
		}
		int mlen = 0;
		if ((int)(m->Length + 10 + pos) < CORD_MAX_PACKET) {
			buildMsgHeader(buf + pos, &mlen, m->TotalLength, channel, m->Seq, (unsigned char)m->FragIndex, (char)m->Flags);
			pos += mlen;
		} else {
			Finish_Datagram(this, buf, pos, counter);
			buildHeader(head, &hlen, &counter);
			memcpy(buf, head, hlen);
			buildMsgHeader(buf + hlen, &mlen, m->TotalLength, channel, m->Seq, (unsigned char)m->FragIndex, (char)m->Flags);
			pos = hlen + mlen;
		}
		memcpy(buf + pos, m->Data, m->Length);
		pos += m->Length;
		if (m->SendCount == 0) {
			Queued_--;
		}
		if (m->Length == 0) {
			c->Queue.erase(c->Queue.begin() + index);
		} else {
			m->SentTime = now;
			m->SendCount++;
			if (m->SendCount == 1) {
				c->Sent.insert(c->Sent.end(), 1, *m);
				c->Queue.erase(c->Queue.begin() + index);
			} else if (m != &c->Sent.back()) {
				CordMessageClass copy = *m;
				c->Sent.erase(c->Sent.begin() + index);
				c->Sent.insert(c->Sent.end(), 1, copy);
			}
		}
	}
	if (pos != 0) {
		Finish_Datagram(this, buf, pos, counter);
		LastSend_ = now.getTime_t();
	}
	for (int i = reliable; i > 0; i--) {
		Quality_ = (int)((unsigned int)(Quality_ * 19 + (100 - ((reliable - resent) * 100) / reliable)) / 20);
	}
	return true;
}

// 0x854890: two passes over the active channels, the sent records first
// (those whose last send is older than threshold: the resend timer, or a resend asked for), then
// the queued ones (only while Can_Send). Per channel: a reliable one offers its first eligible
// record at priority 0 + the channel's, a sequenced one its last, the others each record at the
// record's + the channel's priority (unreliable mode-1 records are never resent). The highest
// wins; when the window is full the first resend found does. Every channel looked at with
// records gets LastWrite = now.
bool CordCon::getHighestPriorityMessage(int *channel, int *index, Xtime &threshold, CordMessageClass **msg, Xtime &now)
{
	WWPROFILE("CordCon::getHighestPriorityMessage");
	int best = -999999;
	int best_channel = 0;
	int best_index = 0;
	bool can_send = false;
	if (Stats_->Can_Send()) {
		can_send = true;
	}
	for (int pass = 0; pass < 2; pass++) {
		if (!can_send && pass == 1) {
			continue;
		}
		int n = Active_.size();
		for (int i = 0; i < n; i++) {
			CordChannelClass *c = Active_[i];
			int count;
			if (pass == 0) {
				count = c->Sent.size();
			} else {
				count = c->Queue.size();
			}
			if (count != 0) {
				c->LastWrite = now.getTime_t();
			}
			for (int k = 0; k < count; k++) {
				if (c->Mode == 4 && k != count - 1) {
					continue;
				}
				CordMessageClass *m;
				if (pass == 0) {
					m = &c->Sent[k];
				} else {
					m = &c->Queue[k];
				}
				if (c->Mode == 1 && pass == 0) {
					continue;
				}
				if (m->SentTime < threshold) {
					if (c->Mode == 3) {
						m->Priority = 0;
					}
					int p = m->Priority + c->Priority;
					if (p > best) {
						best = p;
						best_channel = c->Index;
						best_index = k;
						if (msg != 0) {
							*msg = m;
						}
					}
					if (c->Mode == 3) {
						break;
					}
				}
				if (pass == 0) {
					break;
				}
			}
			if (!can_send && best != -999999) {
				break;
			}
		}
		if (best != -999999) {
			*channel = best_channel;
			*index = best_index;
			return true;
		}
	}
	return false;
}

// 0x854B90-0x854C90: the message header's variable-length fields in network order, out of line
// and called by no one (buildMsgHeader 0x854FC0 and examinePacket 0x8514D0 have the same code
// inline). A size or channel takes 1 byte below 0x80, else 2 with the top bit set; a sequence
// number 2 bytes below 0x8000, else 4 with the top bit set. Each returns the bytes it wrote or
// read. __stdcall: the client's `ret 8`.
int __stdcall Cord_Put_Small(unsigned char *out, unsigned short value)	// NAME 2: what it does (the client has no name)
{
	if (value < 0x80) {
		*out = (unsigned char)value;
		return 1;
	}
	*(unsigned short *)out = htons((unsigned short)(value | 0x8000));
	return 2;
}

int __stdcall Cord_Put_Seq(unsigned char *out, unsigned int value)	// NAME 2: what it does (the client has no name)
{
	if (value < 0x8000) {
		*(unsigned short *)out = htons((unsigned short)value);
		return 2;
	}
	*(unsigned long *)out = htonl(value | 0x80000000);
	return 4;
}

int __stdcall Cord_Get_Small(const unsigned char *in, unsigned short *value)	// NAME 2: what it does (the client has no name)
{
	if (*in < 0x80) {
		*value = *in;
		return 1;
	}
	*value = *(unsigned short *)in;
	*value = ntohs(*value) & 0x7FFF;
	return 2;
}

// NOT EXACT (0x854C90, 0.84, explain.py: order-only): the client loads `value` between the call and the 0xFFFF mask
// of the short branch, ours after the mask (12 forms tried: attempts.py show 0x854C90).
int __stdcall Cord_Get_Seq(const unsigned char *in, unsigned int *value)	// NAME 2: what it does (the client has no name)
{
	if (*in < 0x80) {
		*value = ntohs(*(unsigned short *)in);
		return 2;
	}
	*value = *(unsigned int *)in;
	*value = ntohl(*value) & 0x7FFFFFFF;
	return 4;
}

// 0x854D00: RC4 with a key per datagram: the session key, the length, the datagram counter, 0.
void CordCon::Encrypt_Data(unsigned char *data, int len, unsigned short counter)
{
	WWPROFILE("CordCon::Encrypt_Data");
	if (Key_ == 0) {
		return;
	}
	struct {
		__int64 Key;
		int Length;
		unsigned short Counter;
		unsigned short Zero;
	} key;
	memcpy(&key.Key, &Key_, sizeof(Key_));
	key.Length = len;
	key.Counter = counter;
	key.Zero = 0;
	RC4Class rc4;
	{
		WWPROFILE("rc4.Prepare_Key");
		rc4.Prepare_Key((unsigned char *)&key, 16);
	}
	{
		WWPROFILE("rc4.RC4");
		rc4.RC4(data, len);
	}
}

// 0x854ED0: RC4 both ways.
void CordCon::Decrypt_Data(unsigned char *data, int len, unsigned short counter)
{
	Encrypt_Data(data, len, counter);
}

// 0x854F00: the datagram's CRC at +5 (over the whole datagram, its own field 0).
void CordCon::Set_Header_CRC(unsigned char *data, int len)
{
	WWPROFILE("CordCon::Set_Header_CRC");
	*(unsigned long *)(data + 5) = htonl(Cord_CRC(data, len, 0xFFFFFFFF));
}

// 0x854FC0
bool CordCon::buildMsgHeader(unsigned char *out, int *len, int size, int channel, unsigned int seq,
	unsigned char frag, char flags)
{
	WWPROFILE("CordCon::buildMsgHeader");
	int pos, n;
	if ((unsigned short)size < 0x80) {
		pos = 1;
		*out = (unsigned char)size;
	} else {
		*(unsigned short *)out = htons((unsigned short)size | 0x8000);
		pos = 2;
	}
	if ((unsigned short)channel < 0x80) {
		out[pos] = (unsigned char)channel;
		n = 1;
	} else {
		*(unsigned short *)(out + pos) = htons((unsigned short)channel | 0x8000);
		n = 2;
	}
	pos += n;
	if (seq < 0x8000) {
		*(unsigned short *)(out + pos) = htons((unsigned short)seq);
		n = 2;
	} else {
		*(unsigned long *)(out + pos) = htonl(seq | 0x80000000);
		n = 4;
	}
	pos += n;
	unsigned char type = (unsigned char)Channels_[channel]->Mode | (flags << 4);
	if (size > CORD_FRAGMENT_SIZE) {
		type |= 0x10;
	}
	out[pos++] = type;
	if (size > CORD_FRAGMENT_SIZE) {
		out[pos++] = frag;
	}
	*len = pos;
	return true;
}

// 0x855100: an empty message on channel 0 (pump: to carry acks, the keep-alive).
void CordCon::Queue_Empty_Message()
{
	CordMessageClass m;
	m.Length = 0;
	m.TotalLength = 0;
	m.Data = NULL;
	m.Priority = 0;
	m.SendCount = 0;
	m.SentTime.setDay(0);
	m.SentTime.setMsec(0);
	m.Seq = Channels_[0]->NextSeq++;
	Channels_[0]->Queue.push_back(m);
	Channels_[0]->Written++;
	Queued_++;
}

static int Write_Small(unsigned char *p, unsigned int v)
{
	if ((unsigned short)v < 0x80) {
		p[0] = (unsigned char)v;
		return 1;
	}
	v |= 0x8000;
	p[0] = (unsigned char)(v >> 8);
	p[1] = (unsigned char)v;
	return 2;
}

static int Write_Seq(unsigned char *p, unsigned int v)
{
	if (v < 0x8000) {
		p[0] = (unsigned char)(v >> 8);
		p[1] = (unsigned char)v;
		return 2;
	}
	v |= 0x80000000;
	p[0] = (unsigned char)(v >> 24);
	p[1] = (unsigned char)(v >> 16);
	p[2] = (unsigned char)(v >> 8);
	p[3] = (unsigned char)v;
	return 4;
}

// 0x8551F0: the datagram header and the ack list (at most 8 acks and 32 resend requests).
bool CordCon::buildHeader(unsigned char *out, int *len, unsigned short *counter)
{
	unsigned char list[0x320];
	int pos = 0;
	int acks = 0, resends = 0, entries = 0, last = 0;
	size_t i;
	out[0] = IsServer_ ? 4 : 5;
	out[1] = (unsigned char)(Session_ >> 8);
	out[2] = (unsigned char)Session_;
	*counter = (unsigned short)PacketsOut_;
	out[3] = (unsigned char)(PacketsOut_ >> 8);
	out[4] = (unsigned char)PacketsOut_;
	memset(out + 5, 0, 4);
	// One "everything up to" ack per reliable channel for what arrived in order.
	for (i = 0; i < Active_.size() && acks < 8; i++) {
		CordChannelClass *c = Active_[i];
		if (c->Mode != 3 || c->ToAck.length() == 0 || c->Expected <= 1) {
			continue;
		}
		bool any = false;
		for (size_t k = 0; k < (size_t)c->ToAck.length();) {
			unsigned int acked; c->ToAck.get(acked, (int)k); if (acked <= c->Expected - 2) {
				c->ToAck.remove((int)k);
				AcksPending_--;
				any = true;
			} else {
				k++;
			}
		}
		if (any) {
			list[pos++] = 2;
			pos += Write_Small(list + pos, c->Index);
			pos += Write_Seq(list + pos, c->Expected - 2);
			acks++;
			entries++;
		}
	}
	// Then single acks and resend requests, one per channel per round.
	int k2 = -1;
	do {
		k2++;
		if (k2 == (int)Active_.size()) {
			if (entries == last) {
				break;
			}
			last = entries;
			k2 = 0;
		}
		if (Active_.size() == 0) {
			break;
		}
		CordChannelClass *c = Active_[k2];
		if (acks < 8 && c->ToAck.length() != 0) {
			unsigned int seq; c->ToAck.get(seq, 0);
			c->ToAck.remove(0);
			AcksPending_--;
			list[pos++] = 0;
			pos += Write_Small(list + pos, c->Index);
			pos += Write_Seq(list + pos, seq);
			acks++;
			entries++;
		}
		if (resends < 0x20 && c->Missing.length() != 0) {
			unsigned int seq; c->Missing.get(seq, 0);
			c->Missing.remove(0);
			ResendsPending_--;
			list[pos++] = 1;
			pos += Write_Small(list + pos, c->Index);
			pos += Write_Seq(list + pos, seq);
			resends++;
			entries++;
		}
	} while (acks != 8 || resends != 0x20);
	unsigned char flags = (CloseState_ == 2 || CloseState_ == 4) ? 2 : 0;
	if (entries != 0) {
		flags |= 1;
	}
	out[9] = flags;
	int at = 10;
	if (entries != 0) {
		out[10] = (unsigned char)entries;
		at = 11;
	}
	out[at++] = (unsigned char)Quality_;
	if (entries != 0) {
		memcpy(out + at, list, pos);
		at += pos;
	}
	*len = at;
	return true;
}

/*
**	Reconstruction notes
**
**	* The client's handlePacket holds a lock object for the whole call (0x8BA300 / 0x8BA4F0) and
**	  logs through WWOnline's debug macros ('INF ', the time, file and line: 'Dup ack', 'This is
**	  probably a bug! ACK for non-close message recvd'); neither is rebuilt.
**	* Acks: kind 0 removes the one sent record with that sequence number (none: 'Dup ack',
**	  +0x80), kind 2 every record up to it, kind 1 moves the record to the front of the sent list
**	  with its timer cleared (a resend request). An ack takes the record's bytes out of the send
**	  window (CordStatsClass::On_Ack); an ack for a record sent once, under a day old, is a
**	  round-trip sample; a dup ack counts as a loss and doubles the round-trip sample.
**	* Channel 0 carries the 0-byte mode-5 block that ends most datagrams (its sequence number counts
**	  the sender's datagrams); handlePacket acks nothing and stores nothing for it.
**	* buildHeader acks a reliable channel's in-order run with one kind-2 entry up to Expected - 2,
**	  then the rest one by one; the first datagram's ack therefore has the form [2 ch n][0 ch n+1].
*/
