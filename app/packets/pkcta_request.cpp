// Earth & Beyond client (reconstruction): app\packets\pkcta_request.cpp. Notes at the end of the file.
#include "pkcta_request.h"
#include <stdio.h>


CTA_Request_PacketClass::CTA_Request_PacketClass(int avatar_id, int target_id, int request_type) : AvatarID(avatar_id), TargetID(target_id), RequestType(request_type)
{
}

CTA_Request_PacketClass::CTA_Request_PacketClass(RawPacketClass *raw) : AvatarID(-1), TargetID(-1), RequestType(-1)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
	raw->Get(&TargetID, sizeof(TargetID));
	raw->Get(&RequestType, sizeof(RequestType));
}

RawPacketClass *CTA_Request_PacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(188, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	raw->Add(&TargetID, sizeof(TargetID));
	raw->Add(&RequestType, sizeof(RequestType));
	return(raw);
}

int CTA_Request_PacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(TargetID) + sizeof(RequestType));
}

int CTA_Request_PacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "CTA_Request_Packet {AvatarID=%d TargetID=%d RequestType=%d}", AvatarID, TargetID, RequestType));
}


CTAResponsePacketClass::CTAResponsePacketClass(int avatar_id, int request_type, bool success) : AvatarID(avatar_id), RequestType(request_type), Success(success)
{
}

CTAResponsePacketClass::CTAResponsePacketClass(RawPacketClass *raw) : AvatarID(-1), RequestType(-1), Success(false)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
	raw->Get(&RequestType, sizeof(RequestType));
	raw->Get(&Success, sizeof(Success));
}

RawPacketClass *CTAResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(189, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	raw->Add(&RequestType, sizeof(RequestType));
	raw->Add(&Success, sizeof(Success));
	return(raw);
}

int CTAResponsePacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID) + sizeof(RequestType) + sizeof(Success));
}

int CTAResponsePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "CTA_Response_Packet {AvatarID=%d RequestType=%d Success=%s}", AvatarID, RequestType, Success ? "true" : "false"));
}


/*
**	Reconstruction notes (pkcta_request.cpp)
**
**	* Client 0x86F070-0x86F600 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 189 (declared in pk_classes.h).
*/
