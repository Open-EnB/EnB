/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\dbgconn.h (assumed: dbgconn.cpp's asserts name
**	the .cpp, l.298)
**
**	PARTIAL: dbgconn.cpp's debugging wrappers (0x823F70-0x826470), which
**	ConnectionManagerClass::Add_Connection 0x822CC0 and the factory 0x821450 put round a connection
**	(docs/kernel-map.md 4): the packet logs (rebuilt) and the statistics wrapper (declared).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DBGCONN_H
#define DBGCONN_H	// NAME 3: a .h paired with the client's dbgconn.cpp (D24)

#include "connect.h"
#include <stdio.h>

// The log directory: network.ini's [Network] LogPath with the first free 3-digit suffix (0x824450)
const char *Get_Log_Directory();	// NAME 2: what it does

// The packet log ('netlog': a 'Sent' / 'Received' line per message into <LogPath>nnn\<name>-nnn.log;
// vtable 0xB086C4, constructor 0x823F70, 0x14 bytes)
class LogConnectionClass : public ForwardingConnectionClass	// NAME 2: what it does
{
public:
	LogConnectionClass(ConnectionClass *connection, const char *name);	// 0x823F70
	virtual ~LogConnectionClass();									// +0x00 0x824180 / 0x8241B0
	virtual bool Send_Message(RawPacketClass *packet);				// +0x04 0x824270
	virtual bool Send_Packet(PacketClass *packet);					// +0x08 0x8242E0
	virtual PacketClass *Receive();									// +0x0C 0x824350
	virtual int Describe(PacketClass *packet, const char *label);	// +0x38 0x8243C0: the line into Buffer (overloads: slots in reverse order)  // NAME 2: what it does
	virtual int Describe(RawPacketClass *packet, const char *label);	// +0x34 0x824410  // NAME 2: what it does

	FILE *File;		// +0x0C  // NAME 2: what it holds
	char *Buffer;	// +0x10 0x800 bytes: the line  // NAME 2: what it holds
};

// The packet log with the time since it opened (vtable 0xB0870C, constructor 0x824540, 0x18 bytes):
// the manager's wrapper when its +0x1C flag is set
class TimedLogConnectionClass : public LogConnectionClass	// NAME 2: what it does
{
public:
	TimedLogConnectionClass(ConnectionClass *connection, const char *name);	// 0x824540
	virtual int Describe(PacketClass *packet, const char *label);	// +0x38 0x824860
	virtual int Describe(RawPacketClass *packet, const char *label);	// +0x34 0x824950

	int Format_Time(char *buffer, unsigned int ms);				// 0x824A20: mm:ss.mmm  // NAME 2: what it does

	unsigned int Start;	// +0x14 (Unknown_Get_Ticks at construction)  // NAME 2: what it holds
};

// The statistics wrapper (vtable 0xB08794, constructor 0x8250B0, 0x0C bytes): with -PACKET_STATS
class StatsConnectionClass : public ForwardingConnectionClass	// NAME 2: what it does (-PACKET_STATS)
{
public:
	StatsConnectionClass(ConnectionClass *connection);					// 0x8250B0
};

#endif
