// Earth & Beyond client (reconstruction): app\packets\pkpointeffect.cpp. Notes at the end of the file.
#include "pkpointeffect.h"
#include <stdio.h>


PointEffectPacketClass::PointEffectPacketClass(RawPacketClass *raw)
{
	raw->Get(&EffectID, sizeof(EffectID));
	raw->Get(&TimeStamp, sizeof(TimeStamp));
	raw->Get(Location, sizeof(Location));
	raw->Get(&Duration, sizeof(Duration));
	raw->Get(&EffectDescID, sizeof(EffectDescID));
	raw->Get(&Scale, sizeof(Scale));
	raw->Get(HSVShift, sizeof(HSVShift));
}

RawPacketClass *PointEffectPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(10, Get_Data_Size());
	raw->Add(&EffectID, sizeof(EffectID));
	raw->Add(&TimeStamp, sizeof(TimeStamp));
	raw->Add(Location, sizeof(Location));
	raw->Add(&Duration, sizeof(Duration));
	raw->Add(&EffectDescID, sizeof(EffectDescID));
	raw->Add(&Scale, sizeof(Scale));
	raw->Add(HSVShift, sizeof(HSVShift));
	return(raw);
}

int PointEffectPacketClass::Get_Data_Size()
{
	return(sizeof(EffectID) + sizeof(TimeStamp) + sizeof(Location) + sizeof(Duration) + sizeof(EffectDescID) + sizeof(Scale) + sizeof(HSVShift));
}

int PointEffectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EffectID=%d TimeStamp=%u Location=%f %f %f Duration=%hd EffectDescID=%hd Scale=%f HSVShift=%f %f %f}", "Point_Effect", EffectID, TimeStamp, Location[0], Location[1], Location[2], Duration, EffectDescID, Scale, HSVShift[0], HSVShift[1], HSVShift[2]));
}

RemoveEffectPacketClass::RemoveEffectPacketClass(RawPacketClass *raw)
{
	raw->Get(&EffectID, sizeof(EffectID));
}

RawPacketClass *RemoveEffectPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(15, Get_Data_Size());
	raw->Add(&EffectID, sizeof(EffectID));
	return(raw);
}

int RemoveEffectPacketClass::Get_Data_Size()
{
	return(sizeof(EffectID));
}

int RemoveEffectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EffectID=%d}", "Remove_Effect", EffectID));
}


/*
**	Reconstruction notes (pkpointeffect.cpp)
**
**	* Client 0x882710-0x885FD0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 9, 11, 12, 13, 14 (declared in pk_classes.h).
*/
