/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\wwudp\cordcon.h (name assumed from cordcon.cpp)
**
**	CordCon: one EA-UDP connection (the client's "CORD" data connection after the SYN1/ACK1/
**	SYN2/ACK2 handshake that CordMgr runs). The class and method names are the client's
**	(profile labels 'CordCon::handlePacket' ..., the assert 'Udp_ == NULL'); member names other
**	than Udp_ are descriptive.
**
**	The datagram on the wire (CordCon::buildHeader 0x8551F0, examinePacket 0x8514D0):
**	  0     type: 4 from the server, 5 from the client (CordMgr::Pump dispatches 0..6 on it)
**	  1-2   session id (big-endian)
**	  3-4   the sender's datagram counter (big-endian; CordCon +0x8C): part of the RC4 key
**	  5-8   CRC-32 (0x8B3EC0: reflected table 0xEDB88320, init 0xFFFFFFFF, no final xor) of the
**	        whole datagram with this field zero; big-endian
**	  9     flags: bit 0 an ack list follows, bit 1 closing
**	  10    (bit 0) the number of ack entries
**	  then  one header byte (< 0x65; CordCon +0x90 out, +0x94 in), the ack entries
**	        {kind (0 ack, 1 resend request, 2 everything up to), channel, seq}, then blocks to
**	        the end: {size, channel, seq, type (channel mode | flags << 4), [fragment index]}.
**	        size and channel take 1 byte below 0x80, else 2 (top bit set); seq 2 bytes below
**	        0x8000, else 4 (top bit set). Fragments are 200 bytes.
**	Bytes 5 to the end are RC4-encrypted (PACKET_ENCRYPTION_OFFSET 5) with a fresh key per datagram:
**	the session key (8 bytes), the encrypted length (4), the datagram counter (2), 0 (2)
**	(CordCon::Encrypt_Data 0x854D00, WWLib's RC4Class).
**
**	Layout: the client's, member for member (CordCon 0xBC bytes, constructor 0x84F640; the channel
**	0x78, constructor 0x84DFC0; the message records 0x30), from the constructors and the code
**	that uses them. The client's lists are WWOnline's wlib LinkedList / ArrayList, inlined
**	(the bounds-checked get with a cached cursor); CordListClass / CordArrayClass below are
**	stand-ins with their layout.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CORDCON_H  // NAME 3 [cordcon.h]: header name assumed from the client's cordcon.cpp
#define CORDCON_H

#include <vector>
#include <string.h>
#include "mempool.h"
#include "wtime.h"

class UDP;

#define PACKET_ENCRYPTION_OFFSET	5		// the client's assert 'datalen < PACKET_ENCRYPTION_OFFSET'
#define CORD_FRAGMENT_SIZE			200	// NAME 2 [CORD_FRAGMENT_SIZE]: the fragment size in write 0x850C70 / examinePacket
#define CORD_MAX_PACKET				500	// NAME 2 [CORD_MAX_PACKET]: pumpInternal's datagram limit (examinePacket: < 0x1F5)

// Stand-ins with the layout of WWOnline's wlib LinkedList<T> (0x14 bytes: Entries, Head, Tail,
// Current, CurIndex; nodes {T, Next, Prev}) and ArrayList<T> (0xC: Entries_, Slots_, Vector_),
// whose inlined code the client has (EA's copy: Generals Tools\matchbot\wlib). Not EA's code:
// only what CordCon needs, with the same field order and the cursor lookup of getPointer.
template <class T> class CordListClass		// NAME 1 [CordListClass]: stand-in for wlib's LinkedList<T>
{
public:
	struct NodeType { T Node; NodeType *Next; NodeType *Prev; };
	CordListClass() : Entries(0), Head(0), Tail(0), Current(0), CurIndex(-1) {}
	~CordListClass() { Clear(); }
	void Clear()
	{
		NodeType *temp = Head;
		while (temp) {
			NodeType *del = temp;
			temp = temp->Next;
			delete del;
		}
		Entries = 0;
		CurIndex = -1;
		Head = Tail = Current = 0;
	}
	int Length() const { return Entries; }
	T *Get(int pos)
	{
		if (Entries == 0 || pos < 0 || pos >= Entries) {
			return 0;
		}
		if (pos == 0) {
			return &Head->Node;
		}
		if (pos == Entries - 1) {
			return &Tail->Node;
		}
		if (pos == CurIndex) {
			return &Current->Node;
		}
		if (pos == CurIndex + 1) {
			CurIndex++;
			Current = Current->Next;
			return &Current->Node;
		}
		if (pos == CurIndex - 1) {
			CurIndex--;
			Current = Current->Prev;
			return &Current->Node;
		}
		NodeType *n = Head->Next;
		for (int i = 1; i < pos; i++) {
			n = n->Next;
		}
		Current = n;
		CurIndex = pos;
		return &n->Node;
	}
	void Add_Tail(const T &v)
	{
		NodeType *n = new NodeType;
		n->Node = v;
		n->Next = 0;
		n->Prev = Tail;
		if (Tail) {
			Tail->Next = n;
		} else {
			Head = n;
		}
		Tail = n;
		Entries++;
	}
	void Remove(int pos)
	{
		if (pos < 0 || pos >= Entries) {
			return;
		}
		NodeType *n = Head;
		for (int i = 0; i < pos; i++) {
			n = n->Next;
		}
		if (n->Prev) { n->Prev->Next = n->Next; } else { Head = n->Next; }
		if (n->Next) { n->Next->Prev = n->Prev; } else { Tail = n->Prev; }
		delete n;
		Entries--;
		Current = 0;
		CurIndex = -1;
	}
private:
	int Entries;			// +0x00 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
	NodeType *Head;			// +0x04 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
	NodeType *Tail;			// +0x08 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
	NodeType *Current;		// +0x0C NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
	int CurIndex;			// +0x10 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
};

template <class T> class CordArrayClass		// NAME 1 [CordArrayClass]: stand-in for wlib's ArrayList<T>
{
public:
	CordArrayClass() : Entries_(0), Slots_(0), Vector_(0) {}
	~CordArrayClass() { Clear(); delete [] (unsigned char *)Vector_; }	// wlib's: clear(), then the bytes
	void Clear() { Entries_ = 0; }
	int Length() const { return Entries_; }
	T &operator[](int i) { return Vector_[i]; }
	T *Get_Pointer(int pos) { if (pos < 0 || pos >= Entries_) { return 0; } return &Vector_[pos]; }
	bool Get(T &v, int pos) { if (pos < 0 || pos >= Entries_) { return false; } v = Vector_[pos]; return true; }
	void Add(const T &v)
	{
		if (Entries_ == Slots_) {
			int slots = Slots_ ? Slots_ * 2 : 8;
			T *p = new T[slots];
			for (int i = 0; i < Entries_; i++) {
				p[i] = Vector_[i];
			}
			delete [] Vector_;
			Vector_ = p;
			Slots_ = slots;
		}
		Vector_[Entries_++] = v;
	}
	void Remove(int pos)
	{
		if (pos < 0 || pos >= Entries_) {
			return;
		}
		for (int i = pos + 1; i < Entries_; i++) {
			Vector_[i - 1] = Vector_[i];
		}
		Entries_--;
	}
private:
	int Entries_;			// +0x00 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
	int Slots_;				// +0x04 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
	T *Vector_;				// +0x08 NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
};

// One message, sent or received (0x30 bytes in the client: the records of a channel's send queue
// and sent list, and the nodes of its receive list; write 0x850C70, handlePacket 0x851AE0).
class CordMessageClass		// NAME 2: describes what it holds
{
public:
	CordMessageClass();				// 0x85BE00 (another object, 0x85AEC0-0x85C600: unknown_cordmsg.cpp)
	~CordMessageClass();			// 0x85BEA0
	void Free();					// 0x85BEC0: deletes Data and Fragments

	Wtime SentTime;					// NAME 2: +0x00 the last send (day 0 = never, or a resend asked for)
	int SendCount;					// NAME 2: +0x08 0 = never sent
	unsigned int Length;			// NAME 2: +0x0C this record's bytes (a fragment's)
	unsigned int TotalLength;		// NAME 2: +0x10 the whole message's
	int Priority;					// NAME 2: +0x14
	unsigned int Seq;				// NAME 2: +0x18
	unsigned char *Data;			// NAME 2: +0x1C
	unsigned char *Fragments;		// NAME 2: +0x20 received: one flag per fragment
	unsigned int Time;				// NAME 2: +0x24 received: when complete (0 = not yet)
	int FragIndex;					// NAME 2: +0x28 sent
	int Flags;						// NAME 2: +0x2C bit 0 fragmented, bit 1 first on the channel (block flag 0x20)
};

// One channel of a connection (0x78 bytes; constructor 0x84DFC0, made 8 or 2 at a time when a
// datagram names a channel the connection doesn't have yet).
class CordChannelClass : public AutoPoolClass<CordChannelClass, 1024>	// NAME 2: describes what it is (the client's name not found)
{
public:
	CordChannelClass();
	~CordChannelClass();
	void Reset();								// 0x84E770 (inlined in the constructor): unused, everything dropped

	int Mode;									// NAME 2: +0x00 0 unused, 3 reliable ordered, 4 sequenced, 1, 5 (channel 0)
	unsigned int LastWrite;						// NAME 2: +0x04
	unsigned int NextSeq;						// NAME 2: +0x08 the next sequence number to send
	int Priority;								// NAME 2: +0x0C
	std::vector<CordMessageClass> Queue;		// NAME 2: +0x10 written, not sent
	std::vector<CordMessageClass> Sent;			// NAME 2: +0x20 sent, waiting for an ack
	int Written;								// NAME 2: +0x30
	unsigned int Delivered;						// NAME 2: +0x34 mode 3: the next sequence number read() delivers
	unsigned int Expected;						// NAME 2: +0x38 mode 3: the first sequence number not received in order
	unsigned int NextSequenced;					// NAME 2: +0x3C mode 4: older ones are dropped
	unsigned int LastSeq;						// NAME 2: +0x40
	CordListClass<CordMessageClass> Incoming;	// NAME 2: +0x44 received, not read
	CordArrayClass<unsigned int> ToAck;			// NAME 2: +0x58
	CordArrayClass<unsigned int> Missing;		// NAME 2: +0x64 resend requests to send
	int Index;									// NAME 2: +0x70
	bool IsClosed;								// +0x74 NAME 2: write() refuses while set; the constructor sets it, openc clears it
};

// A connection's send window and statistics (0x4C bytes; constructor 0x85BF30 with the window
// limit ACK2 gives, CordCon +0x68): a TCP-like congestion window in bytes (slow start by 200 per
// ack, then 40000 / window per ack, at most 200 per round for all connections; three quarters at
// a loss, half of that at a timeout), the round-trip estimate (ms) behind the resend timeout, and a
// per-second byte count against a rate limit. The members in the client's order.
class CordStatsClass	// NAME 2: describes what it holds (the client's name not found)
{
public:
	CordStatsClass(int max_window);					// 0x85BF30
	~CordStatsClass();								// 0x85C010

	bool Can_Send() const;							// 0x85C090: 200 bytes free in both windows
	bool Update(const Wtime &now);					// 0x85C0D0: a new round after a round trip
	bool Set_Rate_Limit(int bytes);					// 0x85C1C0
	void Get_Resend_Timeout(Wtime &out) const;		// 0x85C1E0: 2 round trips + 50 ms
	bool Add_In_Flight(int bytes);					// 0x85C220
	bool On_Ack(int bytes);							// 0x85C250
	bool On_Round_Trip(int msec);					// 0x85C310: one sample per round
	bool On_Loss(int bytes, bool timeout);			// 0x85C3A0: once per round
	bool Allow(int channel, unsigned int len, int send_count, const Wtime &now);	// 0x85C430

	int Window;							// NAME 2: +0x00 200 at first
	int PeerWindow;						// NAME 2: +0x04 0xFFFFFFF
	int Threshold;						// NAME 2: +0x08 0x7FFFFFFF: slow start below it
	int MaxWindow;						// NAME 2: +0x0C the constructor's argument
	int InFlight;						// NAME 2: +0x10 bytes sent, not acked
	int RateLimit;						// NAME 2: +0x14 bytes per second (0xFFFFFFF)
	Wtime RoundStart;					// NAME 2: +0x18
	int Round;							// NAME 2: +0x20
	int RoundTrip;						// NAME 2: +0x24 ms, 300 at first
	CordArrayClass<int> Unknown28;		// +0x28 (a wlib ArrayList: cleared and freed by the destructor only)
	int LastPeak;						// NAME 2: +0x34 the last round's InFlight peak
	int Peak;							// NAME 2: +0x38
	Wtime SecondStart;					// NAME 2: +0x3C
	int LastSecondBytes;				// NAME 2: +0x44
	int SecondBytes;					// NAME 2: +0x48 bytes sent this second
};

// CordCon, 0xBC bytes in the client (constructor 0x84F640, 9 arguments): the members in its order.
class CordCon
{
public:
	CordCon(unsigned int ip, unsigned short port, int session, unsigned short unknown0c, bool is_server,
		UDP *udp, int stats_arg, unsigned int key_lo, unsigned int key_hi);
	~CordCon();

	// Queues a message on an open channel, in 200-byte records. 0x850C70
	bool write(int channel, const void *data, unsigned int len, int priority);
	// Opens a free channel (the client odd ones, the server even ones). 0x84FF70
	bool openc(unsigned int *channel, int mode, int priority);
	// The next message received, in the order the client reads them. 0x850FD0
	bool read(int *channel, int *len, void *buf, bool peek);
	// A received datagram (the whole wire datagram, decrypted in place). 0x851AE0
	bool handlePacket(unsigned char *data, int datalen);
	// Closes ('CordCon:close' 0x850870): now drops everything queued, unacked and due first; then
	// state 3 (the peer closed) becomes 4, state 0 becomes 1 and pumpClose 0x850260 sends "CLOSE".
	bool close(bool now = false);
	// The peer said go away (a type-6 datagram, or a 0-byte read): nothing more is sent, and the
	// idle close follows within 10 s. 0x850220
	void Go_Away();	// NAME 2: what it does (called from CordMgr::Process_Go_Away_Packet)
	bool pumpClose();
	// Sends what is queued, and the acks due on an empty channel-0 message; the keep-alive, the
	// idle close, dropping closed channels. 0x853560
	bool pump(Wtime &now);
	// Sends by priority within the send window: resends first when their timer is up, drops
	// expired unreliable records. 0x853A30
	bool pumpInternal(Wtime &now);
	// The record to send next: its channel, its place in the channel's list, the record. 0x854890
	bool getHighestPriorityMessage(int *channel, int *index, Wtime &threshold, CordMessageClass **msg, Wtime &now);

	void Encrypt_Data(unsigned char *data, int len, unsigned short counter);			// 0x854D00
	bool examinePacket(unsigned char *data, int datalen);								// 0x8514D0
	bool buildHeader(unsigned char *out, int *len, unsigned short *counter);			// 0x8551F0
	bool buildMsgHeader(unsigned char *out, int *len, int size, int channel, unsigned int seq,
		unsigned char frag, char flags);												// 0x854FC0

	CordChannelClass *Get_Channel(int channel);	// NAME 2: grows Channels_ as the client does

	unsigned int Ip_;							// NAME 2: +0x00 (host order)
	unsigned short Port_;						// NAME 2: +0x04
	int Session_;								// NAME 2: +0x08
	unsigned short Unknown0C;					// +0x0C (the constructor's 4th argument)
	UDP *Udp_;									// +0x10
	int Unknown14;								// +0x14
	int Unknown18;								// +0x18
	int Unknown1C;								// +0x1C
	__int64 Key_;								// NAME 2: +0x20 the RC4 session key (Encrypt_Data tests it as one 64-bit value)
	int OpenChannels_;							// NAME 2: +0x28
	std::vector<CordChannelClass *> Channels_;	// NAME 2: +0x2C
	std::vector<CordChannelClass *> Active_;	// NAME 2: +0x3C channels in use, in first-use order
	std::vector<CordChannelClass *> Ready_;		// NAME 2: +0x4C channels with received messages, in arrival order
	int Queued_;								// NAME 2: +0x5C
	int AcksPending_;							// NAME 2: +0x60
	int ResendsPending_;						// NAME 2: +0x64
	CordStatsClass *Stats_;						// NAME 2: +0x68 (constructor 0x85BF30)
	int BytesSent_;								// NAME 2: +0x6C
	int BytesResent_;							// NAME 2: +0x70
	int BytesRead_;								// NAME 2: +0x74
	int BytesAcked_;							// NAME 2: +0x78
	int Resends_;								// NAME 2: +0x7C
	int DupAcks_;								// NAME 2: +0x80
	int MessagesSent_;							// NAME 2: +0x84
	int PacketsIn_;								// NAME 2: +0x88
	int PacketsOut_;							// NAME 2: +0x8C the datagram counter (the wire's bytes 3-4)
	int Quality_;								// NAME 2: +0x90 sent as the header byte: the running percentage of reliable sends that were resends (pumpInternal)
	int HeaderIn_;								// NAME 2: +0x94 the peer's header byte
	int Unknown98;								// +0x98
	int ExpireCountdown_;						// NAME 2: +0x9C pumpInternal drops expired unreliable records every 8 calls
	unsigned int LastSend_;						// NAME 2: +0xA0 time_t of the last datagram (pump: the keep-alive after 5 s)
	unsigned int LastReceive_;					// NAME 2: +0xA4 time_t (pump: the idle close after 90 s)
	unsigned int LastPump_;						// NAME 2: +0xA8 time_t (pump: a gap over 45 s resets LastReceive_)
	bool GoneAway_;								// NAME 2: +0xAC set by Go_Away: pump sends nothing
	int CloseState_;							// NAME 2: +0xB0 0 open, 1 close asked, 2 CLOSE sent, 3 peer closing, 4 closed
	unsigned int CloseSeq_;						// NAME 2: +0xB4
	bool IsServer_;								// NAME 2: +0xB8

	// The harness's counters (not the client's).
	int AcksMatched_;	// NAME 2: describes its role
	int ResendRequests_;	// NAME 2: describes its role
	int Rejected_;	// NAME 2: describes its role

private:
	void Add_Channels(int count);
};

unsigned int Cord_CRC(const unsigned char *data, int len, unsigned int crc);	// 0x8B3EC0 (wwonline)

#endif
