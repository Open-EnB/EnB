// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\message\msgserv.h (name assumed from msgserv.cpp)
**
**	The message server task: WinMain makes it with -ENABLE_MESSAGES only (main_client.cpp). It holds
**	the message server (unknown_message.h's Unknown4BCA80Class), listens on the first free TCP port
**	of 9999-10009, installs it as the reporters' server (0xBE55D0) and, as a kernel task, pumps it
**	every 200 ms (accepts listeners, reads their filters, sends them the queued messages).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MSGSERV_H  // NAME 1 [msgserv.h]: msgserv.cpp's header
#define __MSGSERV_H

#include "..\..\kernel\kernel.h"
#include "..\unknown_message.h"

// 0x48 bytes; vtable 0xB0C418
class MsgServer : public TaskClass	// NAME 5: the client's profiler label 'MsgServer::Run'
{
public:
	MsgServer();									// 0x8B20D0 (WSAStartup 1.1)
	virtual ~MsgServer();							// 0x8B21B0 (WSACleanup); 0x8B2180 deleting

	virtual void Describe(void *table);				// +0x00 0x8B2350 'TaskType' = 'MessageServer'
	virtual void Run();								// +0x30 0x8B22C0 'MsgServer::Run'
	virtual int Get_Interval();						// +0x34 0x8B22A0 200 ms
	virtual bool Shutdown();						// +0x40 0x8B2280 clears the reporters' server
	virtual bool Delete_On_Remove() { return true; }	// +0x44 0x8B2160 (emitted after the constructor: inline)

	void Unknown_8B2230();							// 0x8B2230 listens, installs the server (assert 'Get_Internal_Server_Protocol() == NULL')  // NAME 1: neutral (D10)

	Unknown4BCA80Class Server;						// +0x0C  // NAME 2: what it holds
};

#endif //__MSGSERV_H
