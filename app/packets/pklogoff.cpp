// Earth & Beyond client (reconstruction): app\packets\pklogoff.cpp. Notes at the end of the file.
#include "pklogoff.h"
#include <stdio.h>


/*
** LogoffPacketClass (opcode 3)
*/
LogoffPacketClass::LogoffPacketClass(int player_id) : PlayerID(player_id)
{
}

LogoffPacketClass::LogoffPacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
}

RawPacketClass *LogoffPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(3, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	return(raw);
}

int LogoffPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int LogoffPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d}", "Logoff", PlayerID));
}


/*
** LogoffRequestPacketClass (opcode 185)
*/
LogoffRequestPacketClass::LogoffRequestPacketClass(int player_id) : PlayerID(player_id)
{
}

LogoffRequestPacketClass::LogoffRequestPacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
}

RawPacketClass *LogoffRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(185, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	return(raw);
}

int LogoffRequestPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID));
}

int LogoffRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Logoff Request {PlayerID=%d}", PlayerID));
}


/*
** LogoffConfirmationPacketClass (opcode 186)
*/
LogoffConfirmationPacketClass::LogoffConfirmationPacketClass()
{
}

LogoffConfirmationPacketClass::LogoffConfirmationPacketClass(RawPacketClass *)
{
}

RawPacketClass *LogoffConfirmationPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(186, Get_Data_Size());
	return(raw);
}

int LogoffConfirmationPacketClass::Get_Data_Size()
{
	return(0);
}

int LogoffConfirmationPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Logoff Confirmation"));
}


/*
** Packet_00BBClass (opcode 187)
*/
Packet_00BBClass::Packet_00BBClass()
{
}

Packet_00BBClass::Packet_00BBClass(RawPacketClass *)
{
}

RawPacketClass *Packet_00BBClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(187, Get_Data_Size());
	return(raw);
}

int Packet_00BBClass::Get_Data_Size()
{
	return(0);
}

int Packet_00BBClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Logoff Confirmation"));
}

bool Packet_00BBClass::Close_Channel()
{
	return(true);
}


/*
**	Reconstruction notes (pklogoff.cpp)
**
**	* The file: client 0x89AB30-0x89B340, between Login's (Print 0x89AAC0, then its file's static
**	  0x89AB00) and ManufactureItemCategory's; its name assumed (NAME 2): app\packets's objects are
**	  linked in alphabetical order (pk_player_object_relationship.cpp 0x874790, pk_sub_parts.cpp
**	  0x875910, pkdebug.cpp 0x880340, pkfindmember.cpp 0x886640, pkjoblist.cpp 0x895F50, pktask.cpp
**	  0x8A9BB0, pkupdate.cpp 0x8ABED0) and the four classes aren't in that order among themselves.
**	* PacketClass's constructor (pk.h) is in every constructor here: the serial number reloaded
**	  after its store, the base vtable's store dropped; the members set in the initialiser list
**	  (body assignments come after the vtable's store).
**	* Build: `new RawPacketClass(opcode, Get_Data_Size())`, the size read through the vtable after
**	  the allocation, then the fields (`Add(&x, sizeof(x))`: no byte order) in order.
**	* The file's last function, after 0x89B2F0, is the guarded static initialiser every object
**	  including <string> has (`std::ctype<unsigned short>::id`, atexit 0x42B320): compiled, not
**	  countable (the same bytes in every file).
*/
