// Earth & Beyond client (reconstruction): app\packets\pkpointeffect.cpp. Notes at the end of the file.
#include "pkpointeffect.h"
#include <stdio.h>

const float DefaultScale = 1.0f;	// 0xB0A394: the effects' default Scale  // NAME 2: what it holds
static Vector3 DefaultHSVShift(0.0f, 0.0f, 0.0f);	// 0xBF4150 (its initialiser 0x8826B0 opens the object)  // NAME 2: what it holds
static Vector3 DefaultTargetOffset(0.0f, 0.0f, 0.0f);	// 0xBF4160 (its initialiser 0x8826E0)  // NAME 2: what it holds
const float DefaultSpeedup = 1.0f;	// 0xB0A398: the effects' default Speedup  // NAME 2: what it holds


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


ObjectToObjectEffectPacketClass::ObjectToObjectEffectPacketClass(int effect_id, const DurationClass &time_stamp, int source_game_id, const LegoIdentifierClass &source_lego, int target_game_id, const DurationClass &duration, short effect_desc_id, const Vector3 &target_offset, short unknown34, short unknown36, bool outside_target_radius, float scale, const Vector3 &hsv_shift, float speedup) :
	EffectID(effect_id),
	TimeStamp(time_stamp.As_Int()),
	SourceGameID(source_game_id),
	SourceLego(source_lego),
	TargetGameID(target_game_id),
	Duration((short)duration.As_Int()),
	EffectDescID(effect_desc_id),
	Unknown34(unknown34),
	Unknown36(unknown36),
	OutsideTargetRadius(outside_target_radius),
	TargetOffset(target_offset),
	Scale(scale),
	HSVShift(hsv_shift),
	Speedup(speedup)
{
	Channel = Unknown_14(TargetGameID, -1);
}

ObjectToObjectEffectPacketClass::ObjectToObjectEffectPacketClass(RawPacketClass *raw) :
	EffectID(0),
	TimeStamp(0),
	SourceGameID(0),
	TargetGameID(0),
	Duration(0),
	EffectDescID(0),
	Unknown34(0),
	Unknown36(0),
	OutsideTargetRadius(false),
	TargetOffset(DefaultTargetOffset),
	Scale(DefaultScale),
	HSVShift(DefaultHSVShift),
	Speedup(DefaultSpeedup)
{
	unsigned int flags = 0;
	Read_Flags(raw, &flags);
	raw->Get(&SourceGameID, sizeof(SourceGameID));
	raw->Get(&TargetGameID, sizeof(TargetGameID));
	raw->Get(&EffectDescID, sizeof(EffectDescID));
	char *name = raw->Get_String();
	if (name != NULL) {
		SourceLego = std::string(name);
		delete [] name;
	}
	Get_Flagged(raw, flags, 0, &EffectID, sizeof(EffectID));
	Get_Flagged(raw, flags, 1, &TimeStamp, sizeof(TimeStamp));
	Get_Flagged(raw, flags, 2, &Duration, sizeof(Duration));
	Get_Flagged(raw, flags, 6, &TargetOffset, sizeof(TargetOffset));
	Get_Flagged(raw, flags, 3, &Unknown34, sizeof(Unknown34));
	Get_Flagged(raw, flags, 4, &Unknown36, sizeof(Unknown36));
	Get_Flagged(raw, flags, 7, &Scale, sizeof(Scale));
	Get_Flagged(raw, flags, 8, &HSVShift.X, sizeof(HSVShift.X));
	Get_Flagged(raw, flags, 9, &HSVShift.Y, sizeof(HSVShift.Y));
	Get_Flagged(raw, flags, 10, &HSVShift.Z, sizeof(HSVShift.Z));
	Get_Flagged(raw, flags, 11, &Speedup, sizeof(Speedup));
	if (Is_Flag_Set(flags, 5)) {
		OutsideTargetRadius = true;
	}
}

RawPacketClass *ObjectToObjectEffectPacketClass::Build()
{
	unsigned int flags = Get_Flags();
	int size = Get_Size(flags);
	RawPacketClass *raw = new RawPacketClass(11, size);
	Write_Flags(raw, flags);
	raw->Add(&SourceGameID, sizeof(SourceGameID));
	raw->Add(&TargetGameID, sizeof(TargetGameID));
	raw->Add(&EffectDescID, sizeof(EffectDescID));
	raw->Add_String(SourceLego.Get_Name().c_str());
	Add_Flagged(raw, flags, 0, &EffectID, sizeof(EffectID));
	Add_Flagged(raw, flags, 1, &TimeStamp, sizeof(TimeStamp));
	Add_Flagged(raw, flags, 2, &Duration, sizeof(Duration));
	Add_Flagged(raw, flags, 6, &TargetOffset, sizeof(TargetOffset));
	Add_Flagged(raw, flags, 3, &Unknown34, sizeof(Unknown34));
	Add_Flagged(raw, flags, 4, &Unknown36, sizeof(Unknown36));
	Add_Flagged(raw, flags, 7, &Scale, sizeof(Scale));
	Add_Flagged(raw, flags, 8, &HSVShift.X, sizeof(HSVShift.X));
	Add_Flagged(raw, flags, 9, &HSVShift.Y, sizeof(HSVShift.Y));
	Add_Flagged(raw, flags, 10, &HSVShift.Z, sizeof(HSVShift.Z));
	Add_Flagged(raw, flags, 11, &Speedup, sizeof(Speedup));
	return(raw);
}

int ObjectToObjectEffectPacketClass::Get_Data_Size()
{
	return(Get_Size(Get_Flags()));
}

int ObjectToObjectEffectPacketClass::Get_Size(unsigned int flags)
{
	int size = Get_Flag_Size() + sizeof(SourceGameID) + sizeof(TargetGameID) + sizeof(EffectDescID);
	size += RawPacketClass::Get_Size(SourceLego.Get_Name().c_str());
	size += Flagged_Size(flags, 0, sizeof(EffectID));
	size += Flagged_Size(flags, 1, sizeof(TimeStamp));
	size += Flagged_Size(flags, 2, sizeof(Duration));
	size += Flagged_Size(flags, 6, sizeof(TargetOffset));
	size += Flagged_Size(flags, 3, sizeof(Unknown34));
	size += Flagged_Size(flags, 4, sizeof(Unknown36));
	size += Flagged_Size(flags, 7, sizeof(Scale));
	size += Flagged_Size(flags, 8, sizeof(HSVShift.X));
	size += Flagged_Size(flags, 9, sizeof(HSVShift.Y));
	size += Flagged_Size(flags, 10, sizeof(HSVShift.Z));
	size += Flagged_Size(flags, 11, sizeof(Speedup));
	return(size);
}

unsigned int ObjectToObjectEffectPacketClass::Get_Flags()
{
	unsigned int flags = 0;
	Set_Flag(&flags, 0, EffectID != 0);
	Set_Flag(&flags, 1, TimeStamp != 0);
	Set_Flag(&flags, 2, Duration != 0);
	Set_Flag(&flags, 6, TargetOffset != DefaultTargetOffset);
	Set_Flag(&flags, 3, Unknown34 != 0);
	Set_Flag(&flags, 4, Unknown36 != 0);
	Set_Flag(&flags, 7, Scale != DefaultScale);
	Set_Flag(&flags, 8, HSVShift.X != DefaultHSVShift.X);
	Set_Flag(&flags, 9, HSVShift.Y != DefaultHSVShift.Y);
	Set_Flag(&flags, 10, HSVShift.Z != DefaultHSVShift.Z);
	Set_Flag(&flags, 11, Speedup != DefaultSpeedup);
	Set_Flag(&flags, 5, OutsideTargetRadius);
	return(flags);
}

int ObjectToObjectEffectPacketClass::Get_Flag_Count()
{
	return(12);
}

int ObjectToObjectEffectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {EffectID=%d TimeStamp=%u SourceGameID=%d TargetGameID=%d Duration=%hd EffectDescID=%hd OutsideTargetRadius=%d TargetOffset=%f %f %f Scale=%f HSVShift=%f %f %f Speedup=%f}", "Object_To_Object_Effect",
		EffectID, TimeStamp, SourceGameID, TargetGameID, Duration, EffectDescID, OutsideTargetRadius, TargetOffset.X, TargetOffset.Y, TargetOffset.Z, Scale, HSVShift.X, HSVShift.Y, HSVShift.Z, Speedup));
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
