// Earth & Beyond client (reconstruction): app\packets\pkincapacitancerequest.cpp. Notes at the end of the file.
#include "pkincapacitancerequest.h"
#include "..\shared\strconv_lib.h"
#include <stdio.h>


IncapacitanceRequestPacketClass::IncapacitanceRequestPacketClass(int game_id) : GameID(game_id)
{
}

IncapacitanceRequestPacketClass::IncapacitanceRequestPacketClass(RawPacketClass *raw)
{
	GameID = -1;
	raw->Get(GameID);
}

RawPacketClass *IncapacitanceRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(141, Get_Data_Size());
	raw->Add(GameID);
	return(raw);
}

int IncapacitanceRequestPacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int IncapacitanceRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Incapacitance Details Request {GameID=%d}", GameID));
}

IncapacitanceDistressBeaconPacketClass::IncapacitanceDistressBeaconPacketClass(int game_id, bool on) : GameID(game_id), On(on)
{
}

IncapacitanceDistressBeaconPacketClass::IncapacitanceDistressBeaconPacketClass(RawPacketClass *raw) : GameID(-1), On(true)
{
	raw->Get(GameID);
	raw->Get(&On, sizeof(On));
}

RawPacketClass *IncapacitanceDistressBeaconPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(142, Get_Data_Size());
	raw->Add(GameID);
	raw->Add(&On, sizeof(On));
	return(raw);
}

int IncapacitanceDistressBeaconPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(On));
}

int IncapacitanceDistressBeaconPacketClass::Print(char *buffer, int len)
{
	char on[16];
	Value_To_String(on, sizeof(on), On);
	return(_snprintf(buffer, len, "Incapacitance Distress Beacon {GameID=%d, On=%s}", GameID, on));
}

Packet_008FClass::Packet_008FClass(int game_id) : GameID(game_id)
{
}

Packet_008FClass::Packet_008FClass(RawPacketClass *raw)
{
	GameID = -1;
	raw->Get(GameID);
}

RawPacketClass *Packet_008FClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(143, Get_Data_Size());
	raw->Add(GameID);
	return(raw);
}

int Packet_008FClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_008FClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Request Rescue {GameID=%d}", GameID));
}

IncapacitanceDetailsPacket::IncapacitanceDetailsPacket(const std::string &unknown0C, const AvatarDescClass &desc, const std::string &unknown128, const std::string &unknown138, const std::string &unknown148, const std::string &unknown158, const std::string &unknown168, const std::string &unknown178, const std::string &unknown188) :
	Unknown0C(unknown0C),
	Desc(desc),
	Unknown128(unknown128),
	Unknown138(unknown138),
	Unknown148(unknown148),
	Unknown158(unknown158),
	Unknown168(unknown168),
	Unknown178(unknown178),
	Unknown188(unknown188)
{
}

static std::string Get_String(RawPacketClass *raw);

IncapacitanceDetailsPacket::IncapacitanceDetailsPacket(RawPacketClass *raw)
{
	Unknown0C = Get_String(raw);
	((Unknown_PacketDataClass &)Desc).Read(&raw->Data);
	Unknown128 = Get_String(raw);
	Unknown138 = Get_String(raw);
	Unknown148 = Get_String(raw);
	Unknown158 = Get_String(raw);
	Unknown168 = Get_String(raw);
	Unknown178 = Get_String(raw);
	Unknown188 = Get_String(raw);
}

// 0x872E30: a string from the message (Get_String's new[]'d copy freed with delete)  // NAME 2: what it does
static std::string Get_String(RawPacketClass *raw)
{
	char *string = raw->Get_String();
	std::string str(string);
	delete string;
	return(str);
}

RawPacketClass *IncapacitanceDetailsPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(144, Get_Data_Size());
	raw->Add_String(Unknown0C.c_str());
	((Unknown_PacketDataClass &)Desc).Write(&raw->Data);
	raw->Add_String(Unknown128.c_str());
	raw->Add_String(Unknown138.c_str());
	raw->Add_String(Unknown148.c_str());
	raw->Add_String(Unknown158.c_str());
	raw->Add_String(Unknown168.c_str());
	raw->Add_String(Unknown178.c_str());
	raw->Add_String(Unknown188.c_str());
	return(raw);
}

int IncapacitanceDetailsPacket::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Unknown0C.c_str()) + ((Unknown_PacketDataClass &)Desc).Get_Size() + RawPacketClass::Get_Size(Unknown128.c_str()) + RawPacketClass::Get_Size(Unknown138.c_str()) + RawPacketClass::Get_Size(Unknown148.c_str()) + RawPacketClass::Get_Size(Unknown158.c_str()) + RawPacketClass::Get_Size(Unknown168.c_str()) + RawPacketClass::Get_Size(Unknown178.c_str()) + RawPacketClass::Get_Size(Unknown188.c_str()));
}

int IncapacitanceDetailsPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Incapacitance Details"));
}

IncapacitanceRescueConfirmationPacket::IncapacitanceRescueConfirmationPacket(const std::string &unknown) :
	Unknown0C(unknown)
{
}

IncapacitanceRescueConfirmationPacket::IncapacitanceRescueConfirmationPacket(RawPacketClass *raw)
{
	char *string = raw->Get_String();
	Unknown0C = string;
	delete [] string;
}

RawPacketClass *IncapacitanceRescueConfirmationPacket::Build()
{
	RawPacketClass *raw = new RawPacketClass(145, Get_Data_Size());
	raw->Add_String(Unknown0C.c_str());
	return(raw);
}

int IncapacitanceRescueConfirmationPacket::Get_Data_Size()
{
	return(Unknown0C.length() + 1);
}

int IncapacitanceRescueConfirmationPacket::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "Incapacitance Rescue Confirmation"));
}


/*
**	Reconstruction notes (pkincapacitancerequest.cpp)
**
**	* Client 0x871F70-0x873770 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* 144 written by hand (builder P7, 2026-10-27): eight strings and an AvatarDescClass.
*/
