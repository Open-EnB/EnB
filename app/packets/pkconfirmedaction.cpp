// Earth & Beyond client (reconstruction): app\packets\pkconfirmedaction.cpp. Notes at the end of the file.
#include "pkconfirmedaction.h"
#include <stdio.h>

ConfirmedActionOfferPacket::ConfirmedActionOfferPacket(int id, const std::string &message, ConfirmedActionEnum type) :
	ID(id),
	Message(message),
	Type(type)
{
}

ConfirmedActionOfferPacket::ConfirmedActionOfferPacket(PacketClass *raw) :
	ID(0),
	Message(""),
	Type(CONFIRMED_ACTION_NONE)
{
	int value;
	raw->Get(value);
	Type = (ConfirmedActionEnum)value;
	raw->Get(ID);
	raw->Get(&Message);
}

PacketClass *ConfirmedActionOfferPacket::Encode()
{
	PacketClass *raw = new PacketClass(190, Get_Data_Size());
	raw->Add((int)Type);
	raw->Add(ID);
	raw->Add(Message);
	return(raw);
}

int ConfirmedActionOfferPacket::Get_Data_Size()
{
	return(PacketClass::Get_Size(Message) + sizeof(int) + sizeof(ID));
}

int ConfirmedActionOfferPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Confirmed Action Offer {Type=%d ID=%d, Message=%s}", Type, ID, Message.c_str()));
}

ConfirmedActionTimeoutPacket::ConfirmedActionTimeoutPacket(ConfirmedActionEnum type, int id) :
	ID(id),
	Type(type)
{
}

ConfirmedActionTimeoutPacket::ConfirmedActionTimeoutPacket(PacketClass *raw) :
	ID(0),
	Type(CONFIRMED_ACTION_NONE)
{
	int value;
	raw->Get(value);
	Type = (ConfirmedActionEnum)value;
	raw->Get(ID);
}

PacketClass *ConfirmedActionTimeoutPacket::Encode()
{
	PacketClass *raw = new PacketClass(191, Get_Data_Size());
	raw->Add((int)Type);
	raw->Add(ID);
	return(raw);
}

int ConfirmedActionTimeoutPacket::Get_Data_Size()
{
	return(sizeof(int) + sizeof(ID));
}

int ConfirmedActionTimeoutPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Confirmed Action Timeout {Type=%d, ID=%d}", Type, ID));
}

ConfirmedActionResponsePacket::ConfirmedActionResponsePacket(int avatar_id, int id, bool response) :
	AvatarID(avatar_id),
	ID(id),
	Response(response)
{
}

ConfirmedActionResponsePacket::ConfirmedActionResponsePacket(PacketClass *raw) :
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

PacketClass *ConfirmedActionResponsePacket::Encode()
{
	PacketClass *raw = new PacketClass(192, Get_Data_Size());
	raw->Add(AvatarID);
	raw->Add(ID);
	int value = Response;
	raw->Add(value);
	return(raw);
}

int ConfirmedActionResponsePacket::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(ID) + sizeof(int));
}

int ConfirmedActionResponsePacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Confirmed Action Response {AvatarID = %d, ID=%d, Response=%d}", AvatarID, ID, Response));
}


/*
**	Reconstruction notes (pkconfirmedaction.cpp)
**
**	* Client 0x8703C0-0x870E60 (0x8703A0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x870E60): CONFIRMED_ACTION_OFFER 190, 191 and 192, written by hand
**	  (builder P4, 2026-10-27). 191's implicit destructor is this object's (0x8709C0), 192's a copy
**	  (0x74FA10).
**	* The file name is provisional (no assert names it).
*/
