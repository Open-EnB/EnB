/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\sclient.h (name assumed from SClient::Process_Packets)
**
**	SClient, the game's client object (0x14F8 bytes, constructor 0x7405E0, destructor 0x740BB0): declared
**	only as far as ClientTaskClass (tsk_game.cpp) uses it. Its own files (client.cpp, the class's
**	0x7405E0-0x7427C0 file) aren't rebuilt (docs/kernel-map.md 6, step 9).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SCLIENT_H  // NAME 3 [sclient.h]: header name assumed from the class
#define SCLIENT_H

#include "tasks\tsk_game.h"

class KernelClass;
class PacketClass;

class SClientUnknown1368Class	// NAME 1: neutral (D10): what SClient +0x1368 points to
{
public:
	char Unknown00[0x68];
	bool Unknown68;				// +0x68 ClientTaskClass::Render renders when it is set
};

class SClient	// NAME 2: the client's 'SClient::Process_Packets' (0x7427C0)
{
public:
	SClient(ClientTaskClass *task);						// 0x7405E0  // NAME 1 [task]: ClientTaskClass::Init passes this
	~SClient();											// 0x740BB0
	virtual void Unknown_00();							// +0x00: ClientTaskClass::Init calls it once Kernel is set
	virtual void Unknown_04();							// +0x04: ClientTaskClass::Shutdown
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual int Unknown_14(bool focused);				// +0x14: ClientTaskClass::Sector, 0 .. 5  // NAME 1 [focused]: Is_Focused()
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();
	virtual void Unknown_3C();
	virtual void Unknown_40();
	virtual void Unknown_44();
	virtual void Unknown_48();
	virtual void Unknown_4C();
	virtual void Unknown_50();
	virtual void Unknown_54();
	virtual void Unknown_58();
	virtual void Unknown_5C();
	virtual void Unknown_60();
	virtual void Unknown_64();							// +0x64: ClientTaskClass::Render, Wait_For_Redirect

	void Unknown_727F00(unsigned long ip, unsigned short port);	// 0x727F00 (client.cpp): the redirect's sector server
	void Unknown_728150(PacketClass *packet);			// 0x728150 (client.cpp): sends the login packet

	char Unknown04[0x10];								// +0x04
	KernelClass *Kernel;								// +0x14 NAME 1: ClientTaskClass::Init sets it to the kernel
	char Unknown18[0x12D8 - 0x18];						// +0x18
	TicketClass Ticket;									// +0x12D8 NAME 1: ClientTaskClass::Init copies the ticket here
	char Unknown1320[0x1368 - 0x1320];					// +0x1320
	SClientUnknown1368Class *Unknown1368;				// +0x1368
	char Unknown136C[0x14F8 - 0x136C];					// +0x136C
};

#endif
