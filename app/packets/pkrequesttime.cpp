// Earth & Beyond client (reconstruction): app\packets\pkrequesttime.cpp. Notes at the end of the file.
#include "pkrequesttime.h"
#include "..\shared\auxdata\unknown_root.h"
#include <stdio.h>

RequestTimePacketClass::RequestTimePacketClass()
{
	TimeReceived = 0;
	TimeSent = 0;
}

RequestTimePacketClass::RequestTimePacketClass(RawPacketClass *raw)
{
	raw->Get(&TimeSent, sizeof(TimeSent));
	TimeReceived = Unknown_Get_Ticks();
}

RawPacketClass *RequestTimePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(68, Get_Data_Size());
	TimeSent = Unknown_Get_Ticks();
	raw->Add(&TimeSent, sizeof(TimeSent));
	return(raw);
}

int RequestTimePacketClass::Get_Data_Size()
{
	return(sizeof(TimeSent));
}

int RequestTimePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {TimeSent=%u TimeReceived=%u}", "Request_Time", TimeSent, TimeReceived));
}

SetClientTimePacketClass::SetClientTimePacketClass(unsigned int client_sent, unsigned int server_received) :
	ClientSent(client_sent),
	ServerReceived(server_received),
	ServerSent(0),
	ClientReceived(0)
{
}

SetClientTimePacketClass::SetClientTimePacketClass(RawPacketClass *raw)
{
	raw->Get(&ClientSent, sizeof(ClientSent));
	raw->Get(&ServerReceived, sizeof(ServerReceived));
	raw->Get(&ServerSent, sizeof(ServerSent));
	ClientReceived = Unknown_Get_Ticks();
}

RawPacketClass *SetClientTimePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(52, Get_Data_Size());
	raw->Add(&ClientSent, sizeof(ClientSent));
	raw->Add(&ServerReceived, sizeof(ServerReceived));
	ServerSent = Unknown_Get_Ticks();
	raw->Add(&ServerSent, sizeof(ServerSent));
	return(raw);
}

int SetClientTimePacketClass::Get_Data_Size()
{
	return(sizeof(ClientSent) + sizeof(ServerReceived) + sizeof(ServerSent));
}

int SetClientTimePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ClientSent=%u ServerReceived=%u ServerSent=%u ClientReceived=%u}", "Set_Client_Time",
		ClientSent, ServerReceived, ServerSent, ClientReceived));
}


/*
**	Reconstruction notes (pkrequesttime.cpp)
**
**	* Client 0x8AAD50-0x8AB27F (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P4, 2026-10-26). REQUEST_TIME's destructor
**	  (0x733550) is a copy from an earlier object (COMDAT); CLIENT_SET_TIME's is this object's (0x8AB040).
**	* Both Builds stamp a field with the aux-data clock (Unknown_Get_Ticks 0x4A3B40), so a rebuilt
**	  Build can't give the received message back: udpreplay.py compares opcodes 52 and 68 with that
**	  field masked.
**	* The file name is provisional (no assert names it).
*/
