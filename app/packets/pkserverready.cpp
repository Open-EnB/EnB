// Earth & Beyond client (reconstruction): app\packets\pkserverready.cpp. Notes at the end of the file.
#include "pkserverready.h"
#include <stdio.h>

ServerReadyPacketClass::ServerReadyPacketClass(int sector, int ip, unsigned short port) :
	Sector(sector),
	IP(ip),
	Port(port)
{
}

ServerReadyPacketClass::ServerReadyPacketClass(PacketClass *raw)
{
	raw->Get(&Sector, sizeof(Sector));
	raw->Get(&IP, sizeof(IP));
	raw->Get(&Port, sizeof(Port));
}

PacketClass *ServerReadyPacketClass::Build()
{
	PacketClass *raw = new PacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&Sector, sizeof(Sector));
	raw->Add(&IP, sizeof(IP));
	raw->Add(&Port, sizeof(Port));
	return(raw);
}

int ServerReadyPacketClass::Get_Data_Size()
{
	return(sizeof(Sector) + sizeof(IP) + sizeof(Port));
}

int ServerReadyPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Sector=%d IP=%x Port=%hd}", "Server_Ready", Sector, IP, Port));
}


/*
**	Reconstruction notes (pkserverready.cpp)
**
**	* Client 0x8A6140-0x8A6400 (0x8A6110 is the previous object's guarded static, 0xC416FC; this one's is 0x8A6400):
**	  opcode 59, written by hand (builder P4, 2026-10-27). The value constructor comes first.
**	* The file name is provisional (no assert names it).
*/
