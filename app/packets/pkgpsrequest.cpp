// Earth & Beyond client (reconstruction): app\packets\pkgpsrequest.cpp. Notes at the end of the file.
#include "pkgpsrequest.h"
#include "nstrdup.h"
#include <stdio.h>

GpsRequestPacketClass::GpsRequestPacketClass(char *unknown0c, char *unknown10, char *unknown14, char *unknown18, int unknown1c) :
	Unknown1C(unknown1c)
{
	Unknown0C = nstrdup(unknown0c);
	Unknown10 = nstrdup(unknown10);
	Unknown14 = nstrdup(unknown14);
	Unknown18 = nstrdup(unknown18);
}

GpsRequestPacketClass::GpsRequestPacketClass(RawPacketClass *raw)
{
	Unknown0C = raw->Get_String();
	Unknown10 = raw->Get_String();
	Unknown14 = raw->Get_String();
	Unknown18 = raw->Get_String();
	raw->Get(Unknown1C);
}

GpsRequestPacketClass::~GpsRequestPacketClass()
{
	delete [] Unknown0C;
	Unknown0C = 0;
	delete [] Unknown10;
	Unknown10 = 0;
	delete [] Unknown14;
	Unknown14 = 0;
	delete [] Unknown18;
	Unknown18 = 0;
}

RawPacketClass *GpsRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(221, Get_Data_Size());
	raw->Add_String(Unknown0C);
	raw->Add_String(Unknown10);
	raw->Add_String(Unknown14);
	raw->Add_String(Unknown18);
	raw->Add(Unknown1C);
	return(raw);
}

int GpsRequestPacketClass::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown0C);
	size += RawPacketClass::Get_Size(Unknown10);
	size += RawPacketClass::Get_Size(Unknown14);
	return(RawPacketClass::Get_Size(Unknown18) + size + sizeof(Unknown1C));
}

int GpsRequestPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s", "GPSRequest"));
}

GpsResponsePacketClass::GpsResponsePacketClass(char *unknown0c, char *unknown10, char *unknown14, char *unknown18, int unknown1c, int unknown20) :
	Unknown1C(unknown1c),
	Unknown20(unknown20)
{
	Unknown0C = nstrdup(unknown0c);
	Unknown10 = nstrdup(unknown10);
	Unknown14 = nstrdup(unknown14);
	Unknown18 = nstrdup(unknown18);
}

GpsResponsePacketClass::GpsResponsePacketClass(RawPacketClass *raw)
{
	Unknown0C = raw->Get_String();
	Unknown10 = raw->Get_String();
	Unknown14 = raw->Get_String();
	Unknown18 = raw->Get_String();
	raw->Get(Unknown1C);
	raw->Get(Unknown20);
}

GpsResponsePacketClass::~GpsResponsePacketClass()
{
	delete [] Unknown0C;
	Unknown0C = 0;
	delete [] Unknown10;
	Unknown10 = 0;
	delete [] Unknown14;
	Unknown14 = 0;
	delete [] Unknown18;
	Unknown18 = 0;
}

RawPacketClass *GpsResponsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(222, Get_Data_Size());
	raw->Add_String(Unknown0C);
	raw->Add_String(Unknown10);
	raw->Add_String(Unknown14);
	raw->Add_String(Unknown18);
	raw->Add(Unknown1C);
	raw->Add(Unknown20);
	return(raw);
}

int GpsResponsePacketClass::Get_Data_Size()
{
	int size = RawPacketClass::Get_Size(Unknown0C);
	size += RawPacketClass::Get_Size(Unknown10);
	size += RawPacketClass::Get_Size(Unknown14);
	return(RawPacketClass::Get_Size(Unknown18) + size + sizeof(Unknown1C) + sizeof(Unknown20));
}

int GpsResponsePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s", "GPSResponse"));
}


/*
**	Reconstruction notes (pkgpsrequest.cpp)
**
**	* Client 0x887F20-0x888670 (0x887EF0 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x888670): GPS_REQUEST 221 and its answer 222 ('GPSResponse'), written by hand (builder P4,
**	  2026-10-27): four new[]'d strings and one (221) or two (222) network-order ints.
**	* The file name is provisional (no assert names it).
*/
