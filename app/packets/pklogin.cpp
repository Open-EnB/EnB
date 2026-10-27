// Earth & Beyond client (reconstruction): app\packets\pklogin.cpp. Notes at the end of the file.
#include "pklogin.h"
#include "..\shared\auxdata\unknown_root.h"
#include <stdio.h>
#include <string.h>

LoginPacketClass::LoginPacketClass(const TicketClass &ticket, const char *name, int unknown) :
	Ticket(ticket),
	UnknownA4(unknown)
{
	TimeReceived = 0;
	TimeSent = 0;
	UnknownA8 = 0;
	UnknownAC = 0;
	UnknownB0 = 0;
	Name[0] = 0;
	if (name != NULL) {
		strncpy(Name, name, sizeof(Name) - 1);
	}
	Name[sizeof(Name) - 1] = 0;
}

LoginPacketClass::LoginPacketClass(RawPacketClass *raw) :
	Ticket(0)
{
	raw->Get(&Ticket);
	raw->Get(&TimeSent, sizeof(TimeSent));
	TimeReceived = Unknown_Get_Ticks();
	raw->Get(Name, sizeof(Name));
	raw->Get(&UnknownA4, sizeof(UnknownA4));
	UnknownA8 = 0;
	UnknownAC = 0;
	UnknownB0 = 0;
}

LoginPacketClass::~LoginPacketClass()
{
}

RawPacketClass *LoginPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(2, Get_Data_Size());
	raw->Add(Ticket);
	TimeSent = Unknown_Get_Ticks();
	raw->Add(&TimeSent, sizeof(TimeSent));
	raw->Add(Name, sizeof(Name));
	raw->Add(&UnknownA4, sizeof(UnknownA4));
	return(raw);
}

int LoginPacketClass::Get_Data_Size()
{
	return(((Unknown_PacketDataClass &)Ticket).Get_Size() + sizeof(TimeSent) + sizeof(Name) + sizeof(UnknownA4));
}

int LoginPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {TimeSent=%u TimeReceived=%u}", "Login", TimeSent, TimeReceived));
}


/*
**	Reconstruction notes (pklogin.cpp)
**
**	* Client 0x89A740-0x89AB00 (each object ends with <string>'s guarded static 0xC416FC): LOGIN 2, written
**	  by hand (builder P9, 2026-10-27): the login ticket (unknown_login_data.h's TicketClass) through
**	  RawPacketClass's Get / Add of its wire interface, a time stamp as pkrequesttime.cpp's, a 0x41-byte name
**	  and an int. The destructor is declared (the client's 0x89A940 stores the class's own vtable first).
**	* The file name is provisional (no assert names it).
*/
