/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordcon.cpp (client 0x84FED0-0x854E70)
**
**	Functional (NOT EXACT, D42): the receive path (handlePacket, examinePacket, read) follows the
**	client's code statement by statement; the send path (write, openc, buildHeader,
**	buildMsgHeader) too; pumpInternal is reduced to building the datagrams (no resend timer, no
**	bandwidth limit, no priority order). The client's asserts sit on lines 521, 632 and 977
**	(D25): this file does not keep them on their lines. Notes at the end.
*/

#include "cordcon.h"
#include "udp.h"
#include "rc4.h"
#include <string.h>
#include <time.h>
#include <windows.h>

static unsigned int Cord_Time()		// NAME 1: the client's clock 0x8BA510 (a Wtime); never 0 here
{
	unsigned int t = GetTickCount();
	return t != 0 ? t : 1;
}

static unsigned int Get_Short(const unsigned char *p)
{
	return ((unsigned int)p[0] << 8) | p[1];
}

static unsigned int Get_Long(const unsigned char *p)
{
	return ((unsigned int)p[0] << 24) | ((unsigned int)p[1] << 16) | ((unsigned int)p[2] << 8) | p[3];
}

// size / channel: 1 byte below 0x80, else 2 with the top bit; seq: 2 bytes below 0x8000, else 4.
static int Read_Small(const unsigned char *p, unsigned int *v)
{
	if (p[0] < 0x80) {
		*v = p[0];
		return 1;
	}
	*v = Get_Short(p) & 0x7FFF;
	return 2;
}

static int Read_Seq(const unsigned char *p, unsigned int *v)
{
	if (p[0] < 0x80) {
		*v = Get_Short(p);
		return 2;
	}
	*v = Get_Long(p) & 0x7FFFFFFF;
	return 4;
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

// 0x8B3EC0 (WWOnline's library, not app\wwudp): CRC-32 with the reflected table of 0xEDB88320,
// built on first use, no final inversion.
unsigned int Cord_CRC(const unsigned char *data, int len, unsigned int crc)	// NAME 1: placeholder
{
	static unsigned int table[256];
	static bool made = false;
	if (!made) {
		for (unsigned int i = 0; i < 256; i++) {
			unsigned int c = i;
			for (int k = 8; k != 0; k--) {
				c = (c & 1) ? (c >> 1) ^ 0xEDB88320 : c >> 1;
			}
			table[i] = c;
		}
		made = true;
	}
	for (int i = 0; i < len; i++) {
		crc = (crc >> 8) ^ table[(data[i] ^ crc) & 0xFF];
	}
	return crc;
}

static void Free_Message(CordMessageClass &m)
{
	delete [] m.Data;
	delete [] m.Fragments;
	m.Data = 0;
	m.Fragments = 0;
}

static CordMessageClass Empty_Message()
{
	CordMessageClass m;
	memset(&m, 0, sizeof(m));
	return m;
}

// ------------------------------------------------------------------------ CordChannelClass

CordChannelClass::CordChannelClass(int index) :	// 0x84DFC0
	Mode(0),
	LastWrite((unsigned int)time(0)),
	NextSeq(0),
	Priority(0),
	Written(0),
	Delivered(0),
	Expected(0),
	NextSequenced(0),
	LastSeq(0),
	Index(index),
	IsClosed(true)
{
}

CordChannelClass::~CordChannelClass()
{
	size_t i;
	for (i = 0; i < Queue.size(); i++) {
		Free_Message(Queue[i]);
	}
	for (i = 0; i < Sent.size(); i++) {
		Free_Message(Sent[i]);
	}
	for (i = 0; i < Incoming.size(); i++) {
		Free_Message(Incoming[i]);
	}
}

// ------------------------------------------------------------------------ CordCon

CordCon::CordCon(unsigned int ip, unsigned short port, int session, const unsigned char key[8], UDP *udp,
	bool is_server) :
	Ip_(ip),
	Port_(port),
	Session_(session),
	Udp_(udp),
	Queued_(0),
	AcksPending_(0),
	ResendsPending_(0),
	BytesRead_(0),
	BytesAcked_(0),
	DupAcks_(0),
	PacketsIn_(0),
	PacketsOut_(0),
	HeaderOut_(0),
	HeaderIn_(0),
	LastReceive_(0),
	CloseState_(0),
	CloseSeq_(0),
	IsServer_(is_server),
	AcksMatched_(0),
	ResendRequests_(0),
	Rejected_(0)
{
	memcpy(Key_, key, 8);
	// 0x84F640: 64 channels; channel 0 is the connection's own (mode 5: the empty message that
	// carries acks when nothing else is queued, and the keep-alive)
	Add_Channels(0x40);
	Channels_[0]->Mode = 5;
	Active_.push_back(Channels_[0]);
}

CordCon::~CordCon()
{
	for (size_t i = 0; i < Channels_.size(); i++) {
		delete Channels_[i];
	}
}

void CordCon::Add_Channels(int count)
{
	for (int i = 0; i < count; i++) {
		Channels_.push_back(new CordChannelClass((int)Channels_.size()));
	}
}

CordChannelClass *CordCon::Get_Channel(int channel)
{
	while ((int)Channels_.size() <= channel) {
		Add_Channels(2);
	}
	return Channels_[channel];
}

// 0x84FF70: the client opens odd channels, the server even ones; the unused one written least
// recently, two more made when none is free.
bool CordCon::openc(unsigned int *channel, int mode, int priority)
{
	int found = -1;
	for (;;) {
		for (int i = IsServer_ ? 2 : 1; i < (int)Channels_.size(); i += 2) {
			if (Channels_[i]->Mode == 0 && (found < 0 || Channels_[i]->LastWrite < Channels_[found]->LastWrite)) {
				found = i;
			}
		}
		if (found >= 0) {
			break;
		}
		Add_Channels(2);
	}
	CordChannelClass *c = Channels_[found];
	c->Mode = mode;
	c->Priority = priority;
	c->IsClosed = false;
	*channel = found;
	Active_.push_back(c);
	return true;
}

// 0x854D00
void CordCon::Encrypt_Data(unsigned char *data, int len, unsigned short counter)
{
	if (*(unsigned int *)&Key_[0] != 0 || *(unsigned int *)&Key_[4] != 0) {
		unsigned char key[16];
		memcpy(key, Key_, 8);
		*(int *)&key[8] = len;
		*(unsigned short *)&key[12] = counter;
		*(unsigned short *)&key[14] = 0;
		RC4Class rc4;
		rc4.Prepare_Key(key, 16);
		rc4.RC4(data, len);
	}
}

// 0x8514D0: the checks a decrypted datagram must pass before handlePacket reads it.
bool CordCon::examinePacket(unsigned char *data, int datalen)
{
	if (datalen >= 0x1F5 || datalen <= 0xD || (data[0] != 4 && data[0] != 5)
		|| (int)Get_Short(data + 1) != Session_) {
		return false;
	}
	unsigned int crc = Get_Long(data + 5);
	unsigned char saved[4];
	memcpy(saved, data + 5, 4);
	memset(data + 5, 0, 4);
	unsigned int calc = Cord_CRC(data, datalen, 0xFFFFFFFF);
	memcpy(data + 5, saved, 4);
	if (crc != calc) {
		return false;
	}
	int pos = 10;
	if (data[9] >= 4) {
		return false;
	}
	unsigned int acks = 0;
	if (data[9] & 1) {
		pos = 11;
		acks = data[10];
		if (acks > 0x28) {
			return false;
		}
	}
	if (data[pos] >= 0x65) {
		return false;
	}
	pos++;
	unsigned int i, v;
	for (i = 0; i < acks; i++) {
		if (datalen < pos) {
			return false;
		}
		unsigned int kind = data[pos];
		unsigned int channel, seq;
		pos++;
		pos += Read_Small(data + pos, &channel);
		pos += Read_Seq(data + pos, &seq);
		if (kind > 2) {
			return false;
		}
		if (channel >= Channels_.size() || Channels_[channel]->NextSeq < seq) {
			return false;
		}
	}
	while (pos < datalen) {
		unsigned int size, channel, seq;
		pos += Read_Small(data + pos, &size);
		pos += Read_Small(data + pos, &channel);
		pos += Read_Seq(data + pos, &seq);
		unsigned int type = data[pos];
		pos++;
		unsigned int flags = type >> 4;
		if ((type & 0x0F) > 5) {
			return false;
		}
		while (channel >= Channels_.size()) {
			Add_Channels(8);
		}
		unsigned int frag = 0;
		if (flags & 1) {
			frag = data[pos];
			pos++;
		}
		unsigned int len = size;
		if (size > CORD_FRAGMENT_SIZE) {
			len = (frag * 5 + 5) * 0x28 < size ? CORD_FRAGMENT_SIZE : size - frag * CORD_FRAGMENT_SIZE;
		}
		int d = (int)(seq - Channels_[channel]->LastSeq);
		if ((len != size && (flags & 1) == 0) || (int)size < (int)(frag * CORD_FRAGMENT_SIZE - CORD_FRAGMENT_SIZE)
			|| d < -1000 || d > 1000) {
			return false;
		}
		pos += len;
	}
	(void)v;
	return pos == datalen;
}

// 0x851AE0
bool CordCon::handlePacket(unsigned char *data, int datalen)
{
	if (datalen < PACKET_ENCRYPTION_OFFSET) {
		return false;		// the client asserts 'datalen < PACKET_ENCRYPTION_OFFSET' (line 977)
	}
	unsigned short counter = (unsigned short)Get_Short(data + 3);
	Encrypt_Data(data + PACKET_ENCRYPTION_OFFSET, datalen - PACKET_ENCRYPTION_OFFSET, counter);
	if (!examinePacket(data, datalen)) {
		Encrypt_Data(data + PACKET_ENCRYPTION_OFFSET, datalen - PACKET_ENCRYPTION_OFFSET, counter);
		Rejected_++;
		return false;
	}
	unsigned int now = Cord_Time();
	PacketsIn_++;
	LastReceive_ = now;
	int pos = 10;
	unsigned char flags = data[9];
	if (flags & 2) {
		CloseState_ = (CloseState_ == 1 || CloseState_ == 2) ? 4 : 3;
	}
	unsigned int acks = 0;
	if (flags & 1) {
		pos = 11;
		acks = data[10];
	}
	HeaderIn_ = data[pos];
	pos++;

	// The ack list: 0 acks one message, 2 every message up to seq, 1 asks for a resend.
	for (unsigned int a = 0; a < acks; a++) {
		unsigned int kind = data[pos];
		unsigned int channel, seq;
		pos++;
		pos += Read_Small(data + pos, &channel);
		pos += Read_Seq(data + pos, &seq);
		if (CloseState_ == 2) {
			if ((kind == 0 && CloseSeq_ == seq) || (kind == 2 && CloseSeq_ <= seq)) {
				CloseState_ = 4;
			}
			// else the client logs 'This is probably a bug! ACK for non-close message recvd'
		}
		CordChannelClass *c = Channels_[channel];
		if (kind == 0 || kind == 2) {
			bool found = false;
			std::vector<CordMessageClass>::iterator it = c->Sent.begin();
			while (it != c->Sent.end()) {
				if (kind == 0 ? it->Seq == seq : it->Seq <= seq) {
					found = true;
					// (kind 0, first send: the client updates its round-trip estimate)
					BytesAcked_ += it->Length;
					AcksMatched_++;
					Free_Message(*it);
					it = c->Sent.erase(it);
					if (kind == 0) {
						break;
					}
				} else {
					++it;
				}
			}
			if (!found && kind == 0) {
				DupAcks_++;		// the client logs 'Dup ack'
			}
		} else if (kind == 1) {
			for (std::vector<CordMessageClass>::iterator it = c->Sent.begin(); it != c->Sent.end(); ++it) {
				if (it->Seq == seq) {
					CordMessageClass m = *it;
					m.SentTime[0] = m.SentTime[1] = 0;
					c->Sent.erase(it);
					c->Sent.insert(c->Sent.begin(), m);
					ResendRequests_++;
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
		unsigned int size, channel, seq;
		pos += Read_Small(data + pos, &size);
		pos += Read_Small(data + pos, &channel);
		pos += Read_Seq(data + pos, &seq);
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
			c->ToAck.push_back(seq);
			AcksPending_++;
		}
		size_t i;
		// A gap before seq: ask for the missing ones back to the first already held.
		if (c->Mode == 3 && seq != 0 && (int)(c->Expected - 1) < (int)(seq - 1)) {
			for (unsigned int s = seq - 1; (int)(c->Expected - 1) < (int)s; s--) {
				bool held = false;
				for (i = 0; i < c->Incoming.size(); i++) {
					if (c->Incoming[i].Seq == s) {
						held = true;
						break;
					}
				}
				if (held) {
					break;
				}
				c->Missing.push_back(s);
				ResendsPending_++;
			}
		}
		bool found = false;
		for (i = 0; i < c->Incoming.size(); i++) {
			if (c->Incoming[i].Seq == seq) {
				found = true;
			}
		}
		while (c->Mode == 3) {
			if (seq == c->Expected) {
				c->Expected++;
			}
			bool next = false;
			for (i = 0; i < c->Incoming.size(); i++) {
				if (c->Incoming[i].Seq == c->Expected && c->Incoming[i].Time != 0) {
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
				for (i = 0; i < c->Incoming.size(); i++) {
					if (c->Incoming[i].Seq == first) {
						c->Incoming[i].Fragments[frag] = 1;
						memcpy(c->Incoming[i].Data + frag * CORD_FRAGMENT_SIZE, data + pos, len);
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
			m.Time = now;
			c->Incoming.push_back(m);
			if (c->Incoming.size() == 1) {
				Ready_.push_back(c);
			}
			pos += len;
			continue;
		}
		int target = -1;
		for (i = 0; i < c->Incoming.size(); i++) {
			if (c->Incoming[i].Seq == first) {
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
			c->Incoming.push_back(m);
			if (c->Incoming.size() == 1) {
				Ready_.push_back(c);
			}
			target = (int)c->Incoming.size() - 1;
		}
		CordMessageClass &t = c->Incoming[target];
		t.Fragments[frag] = 1;
		memcpy(t.Data + frag * CORD_FRAGMENT_SIZE, data + pos, len);
		if (seq == first) {
			if (t.Time == 0) {
				t.Time = now;
			}
		} else {
			// a place holder for the fragment's own sequence number (read() skips it)
			CordMessageClass m = Empty_Message();
			m.Seq = seq;
			m.Time = now;
			c->Incoming.push_back(m);
			if (c->Incoming.size() == 1) {
				Ready_.push_back(c);
			}
		}
		pos += len;
	}
}

// 0x850FD0
bool CordCon::read(int *channel, int *len, void *buf, bool peek)
{
	if (Udp_ == NULL) {
		return false;		// the client asserts 'Udp_ == NULL' (line 632)
	}
	int count = (int)Ready_.size();
	for (int r = 0; r < count; r++) {
		CordChannelClass *c = Ready_[r];
		bool removed = false;
		int i = 0;
		int n = (int)c->Incoming.size();
		for (int k = 0; k < n; k++) {
			CordMessageClass &m = c->Incoming[i];
			if (c->Mode == 3) {
				if (m.Seq < c->Delivered) {
					Free_Message(m);
					c->Incoming.erase(c->Incoming.begin() + i);
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
					Free_Message(m);
					c->Incoming.erase(c->Incoming.begin() + i);
				}
				BytesRead_ += *len;
				if (c->Incoming.size() == 0) {
					Ready_.erase(Ready_.begin() + r);
				}
				return true;
			}
			Free_Message(m);
			c->Incoming.erase(c->Incoming.begin() + i);
			removed = true;
			if (c->Mode == 3 && peek) {
				c->Delivered++;
			}
		}
		if (removed && c->Incoming.size() == 0) {
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

// 0x850C70: the message in 200-byte records, one sequence number each.
bool CordCon::write(int channel, const void *data, unsigned int len, int priority)
{
	if (Udp_ == NULL) {
		return false;		// the client asserts 'Udp_ == NULL' (line 521)
	}
	if (CloseState_ != 0 || len > 0xC800) {
		return false;
	}
	CordChannelClass *c = Get_Channel(channel);
	if (c->IsClosed) {
		return false;
	}
	if (c->Mode == 4) {
		// a sequenced channel drops what it hasn't sent yet
		std::vector<CordMessageClass>::iterator it = c->Queue.begin();
		while (it != c->Queue.end()) {
			if (it->SendCount == 0) {
				Free_Message(*it);
				it = c->Queue.erase(it);
				Queued_--;
			} else {
				++it;
			}
		}
	}
	unsigned int n = (len + CORD_FRAGMENT_SIZE - 1) / CORD_FRAGMENT_SIZE;
	const unsigned char *p = (const unsigned char *)data;
	for (unsigned int i = 0; i < n; i++) {
		CordMessageClass m = Empty_Message();
		m.Seq = c->NextSeq++;
		unsigned int part = len;
		if (n != 1 && (i != n - 1 || (part = len % CORD_FRAGMENT_SIZE) == 0)) {
			part = CORD_FRAGMENT_SIZE;
		}
		m.Data = new unsigned char[part];
		memcpy(m.Data, p, part);
		m.Length = part;
		m.TotalLength = len;
		m.Priority = priority;
		m.Flags = (n != 1 ? 1 : 0) | (c->Written == 0 ? 2 : 0);
		m.FragIndex = i;
		c->Queue.push_back(m);
		c->Written++;
		c->LastWrite = (unsigned int)time(0);
		p += CORD_FRAGMENT_SIZE;
		Queued_++;
	}
	return true;
}

// 0x854FC0
bool CordCon::buildMsgHeader(unsigned char *out, int *len, unsigned int size, int channel, unsigned int seq,
	int frag, int flags)
{
	int pos = Write_Small(out, size);
	pos += Write_Small(out + pos, channel);
	pos += Write_Seq(out + pos, seq);
	unsigned char type = (unsigned char)(Channels_[channel]->Mode | (flags << 4));
	if ((unsigned short)size > CORD_FRAGMENT_SIZE) {
		type |= 0x10;
	}
	out[pos++] = type;
	if ((unsigned short)size > CORD_FRAGMENT_SIZE) {
		out[pos++] = (unsigned char)frag;
	}
	*len = pos;
	return true;
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
	*counter = PacketsOut_;
	out[3] = (unsigned char)(PacketsOut_ >> 8);
	out[4] = (unsigned char)PacketsOut_;
	memset(out + 5, 0, 4);
	// One "everything up to" ack per reliable channel for what arrived in order.
	for (i = 0; i < Active_.size() && acks < 8; i++) {
		CordChannelClass *c = Active_[i];
		if (c->Mode != 3 || c->ToAck.size() == 0 || c->Expected <= 1) {
			continue;
		}
		bool any = false;
		for (size_t k = 0; k < c->ToAck.size();) {
			if (c->ToAck[k] <= c->Expected - 2) {
				c->ToAck.erase(c->ToAck.begin() + k);
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
		if (acks < 8 && c->ToAck.size() != 0) {
			unsigned int seq = c->ToAck[0];
			c->ToAck.erase(c->ToAck.begin());
			AcksPending_--;
			list[pos++] = 0;
			pos += Write_Small(list + pos, c->Index);
			pos += Write_Seq(list + pos, seq);
			acks++;
			entries++;
		}
		if (resends < 0x20 && c->Missing.size() != 0) {
			unsigned int seq = c->Missing[0];
			c->Missing.erase(c->Missing.begin());
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
	out[at++] = HeaderOut_;
	if (entries != 0) {
		memcpy(out + at, list, pos);
		at += pos;
	}
	*len = at;
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

// 0x850870 ('CordCon:close'), reduced: asks for the close; pumpClose sends it once everything
// is sent and acked. The client's also closes the channels and can close at once.
void CordCon::close()
{
	if (CloseState_ == 0) {
		CloseState_ = 1;
	}
}

// 0x850260: state 1 (close asked), nothing queued or due: "CLOSE" on channel 0, state 2 until
// its ack (handlePacket: state 4). State 2 with channel 0 empty: the client logs 'This isn't
// good... waiting for an ACK on a non-existant msg....' and goes back to 1. Not rebuilt: the
// 90-second give-up (state 4).
bool CordCon::pumpClose()
{
	if (CloseState_ == 2) {
		CordChannelClass *c0 = Channels_[0];
		if (c0->Queue.size() + c0->Sent.size() == 0) {
			CloseState_ = 1;
		}
	}
	if (CloseState_ != 1) {
		return false;
	}
	if (Queued_ != 0 || ResendsPending_ != 0 || AcksPending_ != 0) {
		return true;
	}
	for (size_t i = 0; i < Active_.size(); i++) {
		if (Active_[i]->Queue.size() != 0 || Active_[i]->Sent.size() != 0) {
			return true;
		}
	}
	// the client logs 'All data sent, queueing close packet.'
	static const char text[] = "CLOSE";
	CordChannelClass *c = Channels_[0];
	CordMessageClass m = Empty_Message();
	m.Length = m.TotalLength = sizeof(text) - 1;
	m.Data = new unsigned char[m.Length];
	memcpy(m.Data, text, m.Length);
	m.Seq = c->NextSeq++;
	CloseSeq_ = m.Seq;
	c->Queue.push_back(m);
	c->LastWrite = (unsigned int)time(0);
	c->Written++;
	CloseState_ = 2;
	Queued_++;
	return true;
}

// 0x853560, reduced (NOT EXACT): pumpInternal, and while acks or resend requests are due with
// nothing queued, an empty message on channel 0 to carry them. Not rebuilt: the keep-alive (an
// empty message every 5 time units), the idle close (90), dropping closed channels.
bool CordCon::pump()
{
	if (Udp_ == NULL) {
		return false;
	}
	for (;;) {
		pumpInternal();
		if (Queued_ != 0 || (AcksPending_ == 0 && ResendsPending_ == 0)) {
			break;
		}
		CordChannelClass *c = Channels_[0];
		CordMessageClass m = Empty_Message();
		m.Seq = c->NextSeq++;
		c->Queue.push_back(m);
		c->Written++;
		Queued_++;
	}
	return true;
}

// 0x853A30, reduced (NOT EXACT): every queued record once, in channel order, as many datagrams
// as it takes; reliable ones (mode 3) wait in Sent for their ack. The client picks the records
// by priority (getHighestPriorityMessage 0x854890), resends the unacked ones on a timer, drops
// expired unreliable ones and keeps to the bandwidth estimate (0x85C430).
bool CordCon::pumpInternal()
{
	unsigned char buf[0x400];
	int hlen = 0, pos = 0;
	unsigned short counter = 0;
	bool open = false;
	pumpClose();
	for (size_t i = 0; i < Active_.size(); i++) {
		CordChannelClass *c = Active_[i];
		while (c->Queue.size() != 0) {
			CordMessageClass m = c->Queue[0];
			unsigned char mh[16];
			int mlen = 0;
			buildMsgHeader(mh, &mlen, m.TotalLength, c->Index, m.Seq, m.FragIndex, m.Flags & 2 ? 2 : 0);
			if (open && pos + mlen + (int)m.Length > CORD_MAX_PACKET) {
				Finish_Datagram(this, buf, pos, counter);
				open = false;
			}
			if (!open) {
				buildHeader(buf, &hlen, &counter);
				pos = hlen;
				open = true;
			}
			memcpy(buf + pos, mh, mlen);
			pos += mlen;
			memcpy(buf + pos, m.Data, m.Length);
			pos += m.Length;
			c->Queue.erase(c->Queue.begin());
			Queued_--;
			m.SendCount++;
			if (m.Length != 0) {
				c->Sent.push_back(m);		// every mode waits for its ack (RemoveExpired drops modes 1 and 4)
			} else {
				Free_Message(m);
			}
		}
	}
	if (open) {
		Finish_Datagram(this, buf, pos, counter);
	}
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
**	  with its timer cleared (a resend request). The client also updates its round-trip estimate
**	  and the bandwidth accounting (0x85C250 / 0x85C310 / 0x85C3A0) here: not rebuilt.
**	* Channel 0 carries the 0-byte mode-5 block that ends most datagrams (its sequence number counts
**	  the sender's datagrams); handlePacket acks nothing and stores nothing for it.
**	* buildHeader acks a reliable channel's in-order run with one kind-2 entry up to Expected - 2,
**	  then the rest one by one; the first datagram's ack therefore has the form [2 ch n][0 ch n+1].
*/
