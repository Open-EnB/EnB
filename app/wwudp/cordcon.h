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
**	Layout (from the constructor's users; the constructor 0x84F640 is not rebuilt):
**	  +0x00 Ip_ (host order), +0x04 Port_, +0x08 Session_, +0x10 Udp_, +0x20 Key_[8],
**	  +0x30 Channels_ (std::vector), +0x40 Active_ (channels in use, in first-use order),
**	  +0x50 Ready_ (channels with received messages, in arrival order), +0x5C Queued_,
**	  +0x60 AcksPending_, +0x64 ResendsPending_, +0x74 BytesRead_, +0x78 BytesAcked_,
**	  +0x80 DupAcks_, +0x88 PacketsIn_, +0x8C PacketsOut_, +0x90 HeaderOut_, +0x94 HeaderIn_,
**	  +0xA4 LastReceive_, +0xB0 CloseState_, +0xB4 CloseSeq_, +0xB8 IsServer_.
**	This reconstruction keeps the members and their roles, not the byte layout (NOT EXACT, D42):
**	the client's lists are WWOnline's wlib ArrayList / LinkedList (the bounds-checked Get with a
**	cached cursor), here std::vector.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CORDCON_H
#define CORDCON_H

#include <vector>

class UDP;

#define PACKET_ENCRYPTION_OFFSET	5		// the client's assert 'datalen < PACKET_ENCRYPTION_OFFSET'
#define CORD_FRAGMENT_SIZE			200	// NAME 2 [CORD_FRAGMENT_SIZE]: the fragment size in write 0x850C70 / examinePacket
#define CORD_MAX_PACKET				500	// NAME 2 [CORD_MAX_PACKET]: pumpInternal's datagram limit (examinePacket: < 0x1F5)

// One message, sent or received (0x30 bytes in the client: the records of a channel's send queue
// and sent list, and the nodes of its receive list; write 0x850C70, handlePacket 0x851AE0).
struct CordMessageClass		// NAME 2: describes what it holds
{
	unsigned int SentTime[2];		// +0x00 (a Wtime: 0x8BA850 / 0x8BA870)
	int SendCount;					// +0x08 0 = never sent
	unsigned int Length;			// +0x0C this record's bytes (a fragment's)
	unsigned int TotalLength;		// +0x10 the whole message's
	int Priority;					// +0x14
	unsigned int Seq;				// +0x18
	unsigned char *Data;			// +0x1C
	unsigned char *Fragments;		// +0x20 received: one flag per fragment
	unsigned int Time;				// +0x24 received: when complete (0 = not yet)
	int FragIndex;					// +0x28 sent
	int Flags;						// +0x2C bit 0 fragmented, bit 1 first on the channel (block flag 0x20)
};

// One channel of a connection (0x78 bytes; constructor 0x84DFC0, made 8 or 2 at a time when a
// datagram names a channel the connection doesn't have yet).
class CordChannelClass		// NAME 2: describes what it is (the client's name not found)
{
public:
	CordChannelClass(int index);
	~CordChannelClass();

	int Mode;									// +0x00 0 unused, 3 reliable ordered, 4 sequenced, 1, 5 (datagram counter)
	unsigned int LastWrite;						// +0x04
	unsigned int NextSeq;						// +0x08 the next sequence number to send
	int Priority;								// +0x0C
	std::vector<CordMessageClass> Queue;		// +0x10 written, not sent
	std::vector<CordMessageClass> Sent;			// +0x20 sent, waiting for an ack
	int Written;								// +0x30
	unsigned int Delivered;						// +0x34 mode 3: the next sequence number read() delivers
	unsigned int Expected;						// +0x38 mode 3: the first sequence number not received in order
	unsigned int NextSequenced;					// +0x3C mode 4: older ones are dropped
	unsigned int LastSeq;						// +0x40
	std::vector<CordMessageClass> Incoming;		// +0x44 received, not read
	std::vector<unsigned int> ToAck;			// +0x58
	std::vector<unsigned int> Missing;			// +0x64 resend requests to send
	int Index;									// +0x70
	bool IsClosed;								// +0x74 NAME 2: write() refuses while set; the constructor sets it, openc clears it
};

class CordCon
{
public:
	CordCon(unsigned int ip, unsigned short port, int session, const unsigned char key[8], UDP *udp,
		bool is_server);
	virtual ~CordCon();

	// Queues a message on an open channel, in 200-byte records. 0x850C70
	bool write(int channel, const void *data, unsigned int len, int priority);
	// Opens a free channel (the client odd ones, the server even ones). 0x84FF70
	bool openc(unsigned int *channel, int mode, int priority);
	// The next message received, in the order the client reads them. 0x850FD0
	bool read(int *channel, int *len, void *buf, bool peek);
	// A received datagram (the whole wire datagram, decrypted in place). 0x851AE0
	bool handlePacket(unsigned char *data, int datalen);
	// Asks for the close ('CordCon:close' 0x850870, reduced). pumpClose 0x850260 sends it.
	void close();
	bool pumpClose();
	// Sends what is queued, and the acks due on an empty channel-0 message. 0x853560
	bool pump();
	// Sends what is queued. 0x853A30 (NOT EXACT: no resend timer, no bandwidth limit)
	bool pumpInternal();

	void Encrypt_Data(unsigned char *data, int len, unsigned short counter);			// 0x854D00
	bool examinePacket(unsigned char *data, int datalen);								// 0x8514D0
	bool buildHeader(unsigned char *out, int *len, unsigned short *counter);			// 0x8551F0
	bool buildMsgHeader(unsigned char *out, int *len, unsigned int size, int channel,
		unsigned int seq, int frag, int flags);											// 0x854FC0

	CordChannelClass *Get_Channel(int channel);	// NAME 2: grows Channels_ as the client does

	unsigned int Ip_;
	unsigned short Port_;
	int Session_;
	UDP *Udp_;
	unsigned char Key_[8];
	std::vector<CordChannelClass *> Channels_;
	std::vector<CordChannelClass *> Active_;
	std::vector<CordChannelClass *> Ready_;
	int Queued_;
	int AcksPending_;
	int ResendsPending_;
	int BytesRead_;
	int BytesAcked_;
	int DupAcks_;
	int PacketsIn_;
	unsigned short PacketsOut_;
	unsigned char HeaderOut_;
	unsigned char HeaderIn_;
	unsigned int LastReceive_;
	int CloseState_;
	unsigned int CloseSeq_;
	bool IsServer_;

	// The harness's counters (not the client's).
	int AcksMatched_;
	int ResendRequests_;
	int Rejected_;

private:
	void Add_Channels(int count);
};

unsigned int Cord_CRC(const unsigned char *data, int len, unsigned int crc);	// 0x8B3EC0 (wwonline)

#endif
