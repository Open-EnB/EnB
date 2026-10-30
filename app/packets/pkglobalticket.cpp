// Earth & Beyond client (reconstruction): app\packets\pkglobalticket.cpp. Notes at the end of the file.
#include "pkglobalticket.h"
#include <stdio.h>

GlobalTicketPacketClass::GlobalTicketPacketClass(int status, const TicketClass &ticket) :
	Status(status),
	Ticket(ticket)
{
}

GlobalTicketPacketClass::GlobalTicketPacketClass(RawPacketClass *raw) :
	Ticket(0)
{
	raw->Get(Status);
	raw->Get(&Ticket);
}

RawPacketClass *GlobalTicketPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(111, Get_Data_Size());
	raw->Add(Status);
	raw->Add(Ticket);
	return(raw);
}

int GlobalTicketPacketClass::Get_Data_Size()
{
	return(((Unknown_PacketDataClass &)Ticket).Get_Size() + sizeof(int));
}

int GlobalTicketPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Global Ticket"));
}


/*
**	Reconstruction notes (pkglobalticket.cpp)
**
**	* Client 0x867520-0x867830 (each object ends with <string>'s guarded static 0xC416FC): GLOBAL_TICKET 111,
**	  written by hand (builder P9, 2026-10-27): the status and the login ticket (unknown_login_data.h's
**	  TicketClass) through RawPacketClass's Get / Add of its wire interface; the destructor is implicit (the
**	  client's 0x8675E0 stores no vtable of its own). Process_Packet is tsk_login.cpp's (0x76B730).
**	* The file name is provisional (no assert names it).
*/
