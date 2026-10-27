/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\sclient.h (name assumed from SClient::Process_Packets)
**
**	SClient, the game's client object (0x14F8 bytes, constructor 0x7405E0, destructor 0x740BB0, vtable
**	0xB03240: the base's 23 slots and three): the client base ClientClass (client.h, client.cpp) at +0x00,
**	a one-slot class at +0x1150 (soptions.cpp, not declared yet). Declared only as far as ClientTaskClass
**	(tsk_game.cpp) uses it; sclient.cpp isn't rebuilt (docs/kernel-map-step9.md 2.2, D90 9d).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SCLIENT_H  // NAME 3 [sclient.h]: header name assumed from the class
#define SCLIENT_H

#include "client.h"
#include "tasks\tsk_game.h"

class SClientUnknown1368Class	// NAME 1: neutral (D10): what SClient +0x1368 points to
{
public:
	char Unknown00[0x68];
	bool Unknown68;				// +0x68 ClientTaskClass::Render renders when it is set
};

class SClient : public ClientClass	// NAME 2: the client's 'SClient::Process_Packets' (0x7427C0)
{
public:
	SClient(ClientTaskClass *task);						// 0x7405E0  // NAME 1 [task]: ClientTaskClass::Init passes this
	~SClient();											// 0x740BB0
	virtual void Init();								// +0x00 0x741180: ClientTaskClass::Init calls it once Kernel is set
	virtual void Shutdown();							// +0x04 0x741BB0: ClientTaskClass::Shutdown
	virtual void Unknown_08();							// +0x08 0x7427A0
	virtual void Process_Packets();						// +0x0C 0x7427C0
	virtual void Unknown_10();							// +0x10 0x7428F0
	virtual int Do_Frame(bool focused);					// +0x14 0x742B60: ClientTaskClass::Sector, 0 .. 5
	virtual void Unknown_5C();							// +0x5C 0x740F60
	virtual void Unknown_60();							// +0x60 0x741B70
	virtual void Unknown_64();							// +0x64 0x744690: ClientTaskClass::Render, Wait_For_Redirect

	char Unknown1150[0x12D8 - 0x1150];					// +0x1150 (+0x1150: the soptions.cpp base's vtable 0xB0323C)
	TicketClass Ticket;									// +0x12D8 NAME 1: ClientTaskClass::Init copies the ticket here
	char Unknown1320[0x1368 - 0x1320];					// +0x1320
	SClientUnknown1368Class *Unknown1368;				// +0x1368
	char Unknown136C[0x14F8 - 0x136C];					// +0x136C
};

#endif
