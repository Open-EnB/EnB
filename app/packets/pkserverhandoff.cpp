// Earth & Beyond client (reconstruction): app\packets\pkserverhandoff.cpp. Notes at the end of the file.
#include "pkserverhandoff.h"
#include <stdio.h>

ServerHandoffPacketClass::ServerHandoffPacketClass(const TicketClass &ticket, const std::string &source_sector,
	const std::string &source_system, const std::string &name, const std::string &system) :
	Ticket(ticket),
	Source_Sector(source_sector),
	Source_System(source_system),
	Name(name),
	System(system)
{
}

ServerHandoffPacketClass::ServerHandoffPacketClass(RawPacketClass *raw) :
	Ticket(0)
{
	raw->Get((Unknown_PacketDataClass *)&Ticket);
	raw->Get(&Source_Sector);
	raw->Get(&Source_System);
	raw->Get(&Name);
	raw->Get(&System);
}

RawPacketClass *ServerHandoffPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(58, Get_Data_Size());
	raw->Add((Unknown_PacketDataClass &)Ticket);
	raw->Add(Source_Sector);
	raw->Add(Source_System);
	raw->Add(Name);
	raw->Add(System);
	return(raw);
}

int ServerHandoffPacketClass::Get_Data_Size()
{
	return(((LoginPacketDataClass &)Ticket).Get_Size() + RawPacketClass::Get_Size(Source_Sector) +
		RawPacketClass::Get_Size(Source_System) + RawPacketClass::Get_Size(Name) + RawPacketClass::Get_Size(System));
}

int ServerHandoffPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Server_Handoff { Dest Sector=%d  Name=%s  System=%s --- Source Sector=%s  Source System=%s}",
		Ticket.Unknown18, Name.c_str(), System.c_str(), Source_Sector.c_str(), Source_System.c_str()));
}


/*
**	Reconstruction notes (pkserverhandoff.cpp)
**
**	* Client 0x8943C0-0x8949D0 (0x894390 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x8949D0): SERVER_HANDOFF 58, written by hand (builder P6, 2026-10-27): the login ticket and four
**	  strings. Print passes its format as an argument of "%s" (the client prints the format text itself).
**	* The file name is provisional (no assert names it).
*/
