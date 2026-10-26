/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\dbgconn.h (assumed: dbgconn.cpp's asserts name
**	the .cpp, l.298)
**
**	DECLARATIONS ONLY: dbgconn.cpp's debugging wrappers (0x823F70-0x8251xx), which
**	ConnectionManagerClass::Add_Connection 0x822CC0 and the factory 0x821450 put round a connection
**	(docs/kernel-map.md 4). Their code isn't rebuilt yet: only the constructors and sizes here.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DBGCONN_H
#define DBGCONN_H	// NAME 3: a .h paired with the client's dbgconn.cpp (D24)

#include "connect.h"

// The packet log ('netlog', 'Sent' / 'Received' lines into LogPath's file; vtable 0xB086C4,
// constructor 0x823F70, 0x14 bytes)
class LogConnectionClass : public ForwardingConnectionClass	// NAME 2: what it does
{
public:
	LogConnectionClass(ConnectionClass *connection, const char *name);	// 0x823F70

	void *Unknown0C;	// +0x0C
	void *Unknown10;	// +0x10
};

// The packet log with a timestamp per line (vtable 0xB0870C, constructor 0x824540, 0x18 bytes):
// the manager's wrapper when its +0x1C flag is set
class TimedLogConnectionClass : public LogConnectionClass	// NAME 2: what it does
{
public:
	TimedLogConnectionClass(ConnectionClass *connection, const char *name);	// 0x824540

	unsigned long Unknown14;	// +0x14 (the clock at construction)
};

// The statistics wrapper (vtable 0xB08794, constructor 0x8250B0, 0x0C bytes): with -PACKET_STATS
class StatsConnectionClass : public ForwardingConnectionClass	// NAME 2: what it does (-PACKET_STATS)
{
public:
	StatsConnectionClass(ConnectionClass *connection);					// 0x8250B0
};

#endif
