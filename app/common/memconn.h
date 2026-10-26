/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\memconn.h (assumed: a .h paired with memconn.cpp)
**
**	DECLARATIONS ONLY: memconn.cpp's in-process transport (0x8277C0-0x8281xx, docs/kernel-map.md 4).
**	The factory 0x821450 asks a stack object of this class (no data of its own: it reads the
**	memory listeners' list at 0xBE9160) whether a server listens on a port in this process, and
**	connects to it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef MEMCONN_H
#define MEMCONN_H	// NAME 3: a .h paired with the client's memconn.cpp (D24)

class ConnectionClass;

class MemoryConnectionFactoryClass	// NAME 1: describes what it is
{
public:
	bool Is_Listening(unsigned short port);					// 0x8277C0  // NAME 2: what it does
	ConnectionClass *Connect(unsigned short port);			// 0x827A80  // NAME 2: what it does
};

#endif
