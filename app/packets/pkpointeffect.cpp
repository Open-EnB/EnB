// Earth & Beyond client (reconstruction): app\packets\pkpointeffect.cpp. Notes at the end of the file.
#include "pkpointeffect.h"
#include <stdio.h>

const float DefaultScale = 1.0f;	// 0xB0A394: the effects' default Scale  // NAME 2: what it holds
static Vector3 DefaultHSVShift(0.0f, 0.0f, 0.0f);	// 0xBF4150 (its initialiser 0x8826B0 opens the object)  // NAME 2: what it holds


ObjectEffectPacketClass::ObjectEffectPacketClass(int effect_id, const DurationClass &time_stamp, int game_id, const DurationClass &duration, short effect_desc_id, float scale, const Vector3 &hsv_shift) :
	EffectID(effect_id),
	TimeStamp(time_stamp.As_Int()),
	GameID(game_id),
	Duration((short)duration.As_Int()),
	EffectDescID(effect_desc_id),
	Scale(scale),
	HSVShift(hsv_shift)
{
	if (Duration < 0) {
		Duration = -1;
	}
	Channel = Unknown_14(GameID, -1);
}

ObjectEffectPacketClass::ObjectEffectPacketClass(RawPacketClass *raw) :
	EffectID(0),
	TimeStamp(0),
	GameID(0),
	Duration(0),
	EffectDescID(0),
	Scale(DefaultScale),
	HSVShift(DefaultHSVShift)
{
	unsigned int flags = 0;
	Read_Flags(raw, &flags);
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&EffectDescID, sizeof(EffectDescID));
	Get_Flagged(raw, flags, 0, &EffectID, sizeof(EffectID));
	Get_Flagged(raw, flags, 1, &TimeStamp, sizeof(TimeStamp));
	Get_Flagged(raw, flags, 2, &Duration, sizeof(Duration));
	Get_Flagged(raw, flags, 3, &Scale, sizeof(Scale));
	Get_Flagged(raw, flags, 4, &HSVShift.X, sizeof(HSVShift.X));
	Get_Flagged(raw, flags, 5, &HSVShift.Y, sizeof(HSVShift.Y));
	Get_Flagged(raw, flags, 6, &HSVShift.Z, sizeof(HSVShift.Z));
}

RawPacketClass *ObjectEffectPacketClass::Build()
{
	unsigned int flags = Get_Flags();
	int size = Get_Size(flags);
	RawPacketClass *raw = new RawPacketClass(9, size);
	Write_Flags(raw, flags);
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&EffectDescID, sizeof(EffectDescID));
	Add_Flagged(raw, flags, 0, &EffectID, sizeof(EffectID));
	Add_Flagged(raw, flags, 1, &TimeStamp, sizeof(TimeStamp));
	Add_Flagged(raw, flags, 2, &Duration, sizeof(Duration));
	Add_Flagged(raw, flags, 3, &Scale, sizeof(Scale));
	Add_Flagged(raw, flags, 4, &HSVShift.X, sizeof(HSVShift.X));
	Add_Flagged(raw, flags, 5, &HSVShift.Y, sizeof(HSVShift.Y));
	Add_Flagged(raw, flags, 6, &HSVShift.Z, sizeof(HSVShift.Z));
	return(raw);
}

int ObjectEffectPacketClass::Get_Size(unsigned int flags)
{
	int size = Get_Flag_Size() + sizeof(GameID) + sizeof(EffectDescID);
	size += Flagged_Size(flags, 0, sizeof(EffectID));
	size += Flagged_Size(flags, 1, sizeof(TimeStamp));
	size += Flagged_Size(flags, 2, sizeof(Duration));
	size += Flagged_Size(flags, 3, sizeof(Scale));
	size += Flagged_Size(flags, 4, sizeof(HSVShift.X));
	size += Flagged_Size(flags, 5, sizeof(HSVShift.Y));
	size += Flagged_Size(flags, 6, sizeof(HSVShift.Z));
	return(size);
}

unsigned int ObjectEffectPacketClass::Get_Flags()
{
	unsigned int flags = 0;
	Set_Flag(&flags, 0, EffectID != 0);
	Set_Flag(&flags, 1, TimeStamp != 0);
	Set_Flag(&flags, 2, Duration != 0);
	Set_Flag(&flags, 3, Scale != DefaultScale);
	Set_Flag(&flags, 4, HSVShift.X != DefaultHSVShift.X);
	Set_Flag(&flags, 5, HSVShift.Y != DefaultHSVShift.Y);
	Set_Flag(&flags, 5, HSVShift.Z != DefaultHSVShift.Z);
	return(flags);
}

int ObjectEffectPacketClass::Get_Data_Size()
{
	return(Get_Size(Get_Flags()));
}

int ObjectEffectPacketClass::Get_Flag_Count()
{
	return(7);
}

int ObjectEffectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EffectID=%d TimeStamp=%u GameID=%d Duration=%hd EffectDescID=%hdw Scale=%f HSVShift=%f %f %f}", "Object_Effect", EffectID, TimeStamp, GameID, Duration, EffectDescID, Scale, HSVShift.X, HSVShift.Y, HSVShift.Z));
}


PointEffectPacketClass::PointEffectPacketClass(int effect_id, const DurationClass &time_stamp, const Vector3 &location, const DurationClass &duration, short effect_desc_id, float scale, const Vector3 &hsv_shift) :
	EffectID(effect_id),
	TimeStamp(time_stamp.As_Int()),
	Location(location),
	Duration((short)duration.As_Int()),
	EffectDescID(effect_desc_id),
	Scale(scale),
	HSVShift(hsv_shift)
{
	if (Duration < 0) {
		Duration = -1;
	}
}

PointEffectPacketClass::PointEffectPacketClass(RawPacketClass *raw)
{
	raw->Get(&EffectID, sizeof(EffectID));
	raw->Get(&TimeStamp, sizeof(TimeStamp));
	raw->Get(&Location, sizeof(Location));
	raw->Get(&Duration, sizeof(Duration));
	raw->Get(&EffectDescID, sizeof(EffectDescID));
	raw->Get(&Scale, sizeof(Scale));
	raw->Get(&HSVShift, sizeof(HSVShift));
}

RawPacketClass *PointEffectPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(10, Get_Data_Size());
	raw->Add(&EffectID, sizeof(EffectID));
	raw->Add(&TimeStamp, sizeof(TimeStamp));
	raw->Add(&Location, sizeof(Location));
	raw->Add(&Duration, sizeof(Duration));
	raw->Add(&EffectDescID, sizeof(EffectDescID));
	raw->Add(&Scale, sizeof(Scale));
	raw->Add(&HSVShift, sizeof(HSVShift));
	return(raw);
}

int PointEffectPacketClass::Get_Data_Size()
{
	return(sizeof(EffectID) + sizeof(TimeStamp) + sizeof(Location) + sizeof(Duration) + sizeof(EffectDescID) + sizeof(Scale) + sizeof(HSVShift));
}

int PointEffectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EffectID=%d TimeStamp=%u Location=%f %f %f Duration=%hd EffectDescID=%hd Scale=%f HSVShift=%f %f %f}", "Point_Effect", EffectID, TimeStamp, Location.X, Location.Y, Location.Z, Duration, EffectDescID, Scale, HSVShift.X, HSVShift.Y, HSVShift.Z));
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
**	* Opcode 9 (OBJECT_EFFECT) and PointEffect's value constructor written by hand (builder P3, 2026-10-26):
**	  9 derives from FlagPacketClass (pkflag.cpp); Get_Flags 0x882F20 sets bit 5 for HSVShift's Y and
**	  again for its Z, so bit 6 is counted and read but never sent (the client's). The defaults Scale
**	  and HSVShift are the client's data (0xB0A394, the static 0xBF4150): compared from memory, not as
**	  constants. The value constructors take the times as 64-bit spans (duration.h's As_Int, its
**	  FAIL_IF on line 190).
**	* Not rebuilt yet: opcode 11, 12, 13, 14 (declared in pk_classes.h).
*/
