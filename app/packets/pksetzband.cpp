// Earth & Beyond client (reconstruction): app\packets\pksetzband.cpp. Notes at the end of the file.
#include "pksetzband.h"
#include <stdio.h>


SetZBandPacketClass::SetZBandPacketClass(float min, float max) : Min(min), Max(max)
{
}

SetZBandPacketClass::SetZBandPacketClass(RawPacketClass *raw)
{
	raw->Get(&Min, sizeof(Min));
	raw->Get(&Max, sizeof(Max));
}

RawPacketClass *SetZBandPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(42, Get_Data_Size());
	raw->Add(&Min, sizeof(Min));
	raw->Add(&Max, sizeof(Max));
	return(raw);
}

int SetZBandPacketClass::Get_Data_Size()
{
	return(sizeof(Min) + sizeof(Max));
}

int SetZBandPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {Min=%f Max=%f}", "Set_ZBand", Min, Max));
}

SetBBoxPacketClass::SetBBoxPacketClass(RawPacketClass *raw)
{
	raw->Get(XMin, sizeof(XMin));
	raw->Get(XMax, sizeof(XMax));
}

RawPacketClass *SetBBoxPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(43, Get_Data_Size());
	raw->Add(XMin, sizeof(XMin));
	raw->Add(XMax, sizeof(XMax));
	return(raw);
}

int SetBBoxPacketClass::Get_Data_Size()
{
	return(sizeof(XMin) + sizeof(XMax));
}

int SetBBoxPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {XMin=%f YMin=%f XMax=%f YMax=%f}", "Set_BBox", XMin[0], XMin[1], XMax[0], XMax[1]));
}


/*
**	Reconstruction notes (pksetzband.cpp)
**
**	* Client 0x8A4250-0x8A4760 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
