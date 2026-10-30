// Earth & Beyond client (reconstruction): app\packets\pkmasterjoin.cpp. Notes at the end of the file.
#include "pkmasterjoin.h"
#include <stdio.h>

MasterJoinPacketClass::MasterJoinPacketClass(const TicketClass &ticket) :
	Ticket(ticket),
	Unknown58(0)
{
}

MasterJoinPacketClass::MasterJoinPacketClass(RawPacketClass *raw) :
	Ticket(0),
	Unknown58(0)
{
	raw->Get((Unknown_PacketDataClass *)&Ticket);
}

MasterJoinPacketClass::~MasterJoinPacketClass()
{
}

RawPacketClass *MasterJoinPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(53, Get_Data_Size());
	raw->Add((Unknown_PacketDataClass &)Ticket);
	return(raw);
}

int MasterJoinPacketClass::Get_Data_Size()
{
	return(((Unknown_PacketDataClass &)Ticket).Get_Size());
}

int MasterJoinPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Master Join"));
}


/*
**	Reconstruction notes (pkmasterjoin.cpp)
**
**	* Client 0x8974C0-0x8977D0 (0x897490 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x8977D0): MASTER_JOIN 53, written by hand (builder P6, 2026-10-27): the login ticket
**	  (unknown_login_data.h's TicketClass) through RawPacketClass's Get / Add of its wire interface.
**	* The file name is provisional (no assert names it).
*/
