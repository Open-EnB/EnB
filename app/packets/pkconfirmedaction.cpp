// Earth & Beyond client (reconstruction): app\packets\pkconfirmedaction.cpp. Notes at the end of the file.
#include "pkconfirmedaction.h"
#include <stdio.h>

ConfirmedActionTimeoutPacketClass::ConfirmedActionTimeoutPacketClass(ConfirmedActionEnum type, int id) :
	ID(id),
	Type(type)
{
}

ConfirmedActionTimeoutPacketClass::ConfirmedActionTimeoutPacketClass(RawPacketClass *raw) :
	ID(0),
	Type(CONFIRMED_ACTION_NONE)
{
	int value;
	raw->Get(value);
	Type = (ConfirmedActionEnum)value;
	raw->Get(ID);
}

RawPacketClass *ConfirmedActionTimeoutPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(191, Get_Data_Size());
	raw->Add((int)Type);
	raw->Add(ID);
	return(raw);
}

int ConfirmedActionTimeoutPacketClass::Get_Data_Size()
{
	return(sizeof(int) + sizeof(ID));
}

int ConfirmedActionTimeoutPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Confirmed Action Timeout {Type=%d, ID=%d}", Type, ID));
}

ConfirmedActionResponsePacketClass::ConfirmedActionResponsePacketClass(int avatar_id, int id, bool response) :
	AvatarID(avatar_id),
	ID(id),
	Response(response)
{
}

ConfirmedActionResponsePacketClass::ConfirmedActionResponsePacketClass(RawPacketClass *raw) :
	AvatarID(0),
	ID(0),
	Response(false)
{
	raw->Get(AvatarID);
	raw->Get(ID);
	int value = 0;
	raw->Get(value);
	Response = value > 0;
}

RawPacketClass *ConfirmedActionResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(192, Get_Data_Size());
	raw->Add(AvatarID);
	raw->Add(ID);
	int value = Response;
	raw->Add(value);
	return(raw);
}

int ConfirmedActionResponsePacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(ID) + sizeof(int));
}

int ConfirmedActionResponsePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Confirmed Action Response {AvatarID = %d, ID=%d, Response=%d}", AvatarID, ID, Response));
}


/*
**	Reconstruction notes (pkconfirmedaction.cpp)
**
**	* Client 0x8703C0-0x870E60 (0x8703A0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x870E60): CONFIRMED_ACTION_OFFER 190 (not rebuilt yet), then 191 and 192, written by hand
**	  (builder P4, 2026-10-27). 191's implicit destructor is this object's (0x8709C0), 192's a copy
**	  (0x74FA10).
**	* The file name is provisional (no assert names it).
*/
