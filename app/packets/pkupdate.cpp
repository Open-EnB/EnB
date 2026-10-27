// Earth & Beyond client (reconstruction): app\packets\pkupdate.cpp. Notes at the end of the file.
#include "pkupdate.h"
#include <stdio.h>
#include <math.h>

const unsigned int DEFAULT_UPDATE_PERIOD = 500;	// 0xB0C0A8: the default UpdatePeriod  // NAME 2: what it holds
const float FLAG_EPSILON = 0.000001f;	// 0xB0C0AC: the flags' threshold for angles and the imparted motion  // NAME 2: what it is


ConstantPositionalUpdatePacketClass::ConstantPositionalUpdatePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(Unknown10, sizeof(Unknown10));
	raw->Get(Unknown1C, sizeof(Unknown1C));
}

RawPacketClass *ConstantPositionalUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(Unknown10, sizeof(Unknown10));
	raw->Add(Unknown1C, sizeof(Unknown1C));
	return(raw);
}

int ConstantPositionalUpdatePacketClass::Get_Type()
{
	return(64);
}

int ConstantPositionalUpdatePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Unknown10) + sizeof(Unknown1C));
}

int ConstantPositionalUpdatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Position={%f %f %f}}", "Constant_Positional_Update", GameID, Unknown10[0], Unknown10[1], Unknown10[2]));
}






















































































































AdvancedPositionalUpdatePacketClass::AdvancedPositionalUpdatePacketClass(int priority, int game_id, unsigned int time_stamp, const Vector3 &position, const Quaternion &orientation, float current_speed, float set_speed, float acceleration, float roty, float desiredy, float rotz, float desiredz, const Vector3 &imparted_velocity, float imparted_spin, float imparted_roll, float imparted_pitch, unsigned int movement_id, unsigned int update_period) :
	GameID(game_id),
	TimeStamp(time_stamp),
	Position(position),
	Orientation(orientation),
	CurrentSpeed(current_speed),
	SetSpeed(set_speed),
	Acceleration(acceleration),
	RotY(roty),
	DesiredY(desiredy),
	RotZ(rotz),
	DesiredZ(desiredz),
	ImpartedVelocity(imparted_velocity),
	ImpartedSpin(imparted_spin),
	ImpartedRoll(imparted_roll),
	ImpartedPitch(imparted_pitch),
	MovementID(movement_id),
	UpdatePeriod(update_period),
	Channel(-1),
	Priority(priority)
{
	G_ASSERT(imparted_velocity.Is_Valid());
	G_ASSERT(WWMath::Is_Valid_Float(imparted_spin));
	G_ASSERT(WWMath::Is_Valid_Float(imparted_roll));
	G_ASSERT(WWMath::Is_Valid_Float(imparted_pitch));
	G_ASSERT(WWMath::Is_Valid_Float(rotz));
	G_ASSERT(WWMath::Is_Valid_Float(roty));
	G_ASSERT(WWMath::Is_Valid_Float(acceleration));
	G_ASSERT(WWMath::Is_Valid_Float(current_speed));
	G_ASSERT(WWMath::Is_Valid_Float(set_speed));
	Channel = Unknown_14(GameID, -1);
}

AdvancedPositionalUpdatePacketClass::AdvancedPositionalUpdatePacketClass() :
	GameID(0),
	TimeStamp(0),
	Position(0.0f, 0.0f, 0.0f),
	Orientation(0.0f, 0.0f, 0.0f, 0.0f),
	CurrentSpeed(0.0f),
	SetSpeed(0.0f),
	Acceleration(0.0f),
	RotY(0.0f),
	DesiredY(0.0f),
	RotZ(0.0f),
	DesiredZ(0.0f),
	ImpartedVelocity(0.0f, 0.0f, 0.0f),
	ImpartedSpin(0.0f),
	ImpartedRoll(0.0f),
	ImpartedPitch(0.0f),
	MovementID(0),
	UpdatePeriod(DEFAULT_UPDATE_PERIOD),
	Channel(0),
	Priority(0)
{
}















AdvancedPositionalUpdatePacketClass::AdvancedPositionalUpdatePacketClass(RawPacketClass *raw) :
	GameID(0),
	TimeStamp(0),
	Position(0.0f, 0.0f, 0.0f),
	Orientation(0.0f, 0.0f, 0.0f, 0.0f),
	CurrentSpeed(0.0f),
	SetSpeed(0.0f),
	Acceleration(0.0f),
	RotY(0.0f),
	DesiredY(0.0f),
	RotZ(0.0f),
	DesiredZ(0.0f),
	ImpartedVelocity(0.0f, 0.0f, 0.0f),
	ImpartedSpin(0.0f),
	ImpartedRoll(0.0f),
	ImpartedPitch(0.0f),
	MovementID(0),
	UpdatePeriod(DEFAULT_UPDATE_PERIOD),
	Channel(0),
	Priority(0)
{
	unsigned int flags = 0;
	Read_Flags(raw, &flags);
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&TimeStamp, sizeof(TimeStamp));
	raw->Get(&Position, sizeof(Position));
	raw->Get(&Orientation, sizeof(Orientation));
	raw->Get(&MovementID, sizeof(MovementID));
	Get_Flagged(raw, flags, 0, &CurrentSpeed, sizeof(CurrentSpeed));
	Get_Flagged(raw, flags, 1, &SetSpeed, sizeof(SetSpeed));
	Get_Flagged(raw, flags, 2, &Acceleration, sizeof(Acceleration));
	Get_Flagged(raw, flags, 3, &RotY, sizeof(RotY));
	Get_Flagged(raw, flags, 4, &DesiredY, sizeof(DesiredY));
	Get_Flagged(raw, flags, 5, &RotZ, sizeof(RotZ));
	Get_Flagged(raw, flags, 6, &DesiredZ, sizeof(DesiredZ));
	Get_Flagged(raw, flags, 7, &ImpartedVelocity, sizeof(ImpartedVelocity));
	Get_Flagged(raw, flags, 7, &ImpartedSpin, sizeof(ImpartedSpin));
	Get_Flagged(raw, flags, 7, &ImpartedRoll, sizeof(ImpartedRoll));
	Get_Flagged(raw, flags, 7, &ImpartedPitch, sizeof(ImpartedPitch));
	Get_Flagged(raw, flags, 8, &UpdatePeriod, sizeof(UpdatePeriod));
	if (!Is_Flag_Set(flags, 1)) {
		SetSpeed = CurrentSpeed;
	}
	if (!Is_Flag_Set(flags, 4)) {
		DesiredY = (RotY > 0.0f) ? WWMATH_PI : -WWMATH_PI;
	}
	if (!Is_Flag_Set(flags, 6)) {
		DesiredZ = (RotZ > 0.0f) ? 1000.0f : -1000.0f;
	}
	G_ASSERT(ImpartedVelocity.Is_Valid());
	G_ASSERT(WWMath::Is_Valid_Float(ImpartedSpin));
	G_ASSERT(WWMath::Is_Valid_Float(ImpartedRoll));
	G_ASSERT(WWMath::Is_Valid_Float(ImpartedPitch));
	G_ASSERT(WWMath::Is_Valid_Float(RotZ));
	G_ASSERT(WWMath::Is_Valid_Float(RotY));
	G_ASSERT(WWMath::Is_Valid_Float(Acceleration));
	G_ASSERT(WWMath::Is_Valid_Float(CurrentSpeed));
	G_ASSERT(WWMath::Is_Valid_Float(SetSpeed));
}

RawPacketClass *AdvancedPositionalUpdatePacketClass::Build()
{
	unsigned int flags = Get_Flags();
	int size = Get_Size(flags);
	RawPacketClass *raw = new RawPacketClass(62, size);
	Write_Flags(raw, flags);
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&TimeStamp, sizeof(TimeStamp));
	raw->Add(&Position, sizeof(Position));
	raw->Add(&Orientation, sizeof(Orientation));
	raw->Add(&MovementID, sizeof(MovementID));
	Add_Flagged(raw, flags, 0, &CurrentSpeed, sizeof(CurrentSpeed));
	Add_Flagged(raw, flags, 1, &SetSpeed, sizeof(SetSpeed));
	Add_Flagged(raw, flags, 2, &Acceleration, sizeof(Acceleration));
	Add_Flagged(raw, flags, 3, &RotY, sizeof(RotY));
	Add_Flagged(raw, flags, 4, &DesiredY, sizeof(DesiredY));
	Add_Flagged(raw, flags, 5, &RotZ, sizeof(RotZ));
	Add_Flagged(raw, flags, 6, &DesiredZ, sizeof(DesiredZ));
	Add_Flagged(raw, flags, 7, &ImpartedVelocity, sizeof(ImpartedVelocity));
	Add_Flagged(raw, flags, 7, &ImpartedSpin, sizeof(ImpartedSpin));
	Add_Flagged(raw, flags, 7, &ImpartedRoll, sizeof(ImpartedRoll));
	Add_Flagged(raw, flags, 7, &ImpartedPitch, sizeof(ImpartedPitch));
	Add_Flagged(raw, flags, 8, &UpdatePeriod, sizeof(UpdatePeriod));
	return(raw);
}

int AdvancedPositionalUpdatePacketClass::Get_Type()
{
	return(62);
}

int AdvancedPositionalUpdatePacketClass::Get_Size(unsigned int flags)
{
	int size = Get_Flag_Size() + sizeof(GameID) + sizeof(TimeStamp) + sizeof(Position) + sizeof(Orientation) + sizeof(MovementID);
	size += Flagged_Size(flags, 0, sizeof(CurrentSpeed));
	size += Flagged_Size(flags, 1, sizeof(SetSpeed));
	size += Flagged_Size(flags, 2, sizeof(Acceleration));
	size += Flagged_Size(flags, 3, sizeof(RotY));
	size += Flagged_Size(flags, 4, sizeof(DesiredY));
	size += Flagged_Size(flags, 5, sizeof(RotZ));
	size += Flagged_Size(flags, 6, sizeof(DesiredZ));
	size += Flagged_Size(flags, 7, sizeof(ImpartedVelocity));
	size += Flagged_Size(flags, 7, sizeof(ImpartedSpin));
	size += Flagged_Size(flags, 7, sizeof(ImpartedRoll));
	size += Flagged_Size(flags, 7, sizeof(ImpartedPitch));
	size += Flagged_Size(flags, 8, sizeof(UpdatePeriod));
	return(size);
}

unsigned int AdvancedPositionalUpdatePacketClass::Get_Flags()
{
	unsigned int flags = 0;
	Set_Flag(&flags, 0, Has_Current_Speed());
	Set_Flag(&flags, 1, Has_Set_Speed());
	Set_Flag(&flags, 2, Has_Acceleration());
	Set_Flag(&flags, 3, Has_RotY());
	Set_Flag(&flags, 4, Has_DesiredY());
	Set_Flag(&flags, 5, Has_RotZ());
	Set_Flag(&flags, 6, Has_DesiredZ());
	Set_Flag(&flags, 7, Has_Imparted());
	Set_Flag(&flags, 8, Has_Update_Period());
	return(flags);
}

bool AdvancedPositionalUpdatePacketClass::Has_Current_Speed()
{
	return(CurrentSpeed != 0.0f);
}

bool AdvancedPositionalUpdatePacketClass::Has_Set_Speed()
{
	return(fabs(SetSpeed - CurrentSpeed) >= WWMATH_EPSILON);
}

bool AdvancedPositionalUpdatePacketClass::Has_Acceleration()
{
	return(Acceleration != 0.0f && fabs(SetSpeed - CurrentSpeed) >= WWMATH_EPSILON);
}

bool AdvancedPositionalUpdatePacketClass::Has_RotY()
{
	return(fabs(RotY) >= FLAG_EPSILON);
}

bool AdvancedPositionalUpdatePacketClass::Has_DesiredY()
{
	if (fabs(RotY) < FLAG_EPSILON) {
		return(false);
	}
	if (RotY > 0.0f && DesiredY == WWMATH_PI) {
		return(false);
	}
	if (RotY < 0.0f && DesiredY == -WWMATH_PI) {
		return(false);
	}
	return(true);
}

bool AdvancedPositionalUpdatePacketClass::Has_RotZ()
{
	return(fabs(RotZ) >= FLAG_EPSILON);
}

bool AdvancedPositionalUpdatePacketClass::Has_DesiredZ()
{
	if (fabs(RotZ) < FLAG_EPSILON) {
		return(false);
	}
	if (RotZ > 0.0f && DesiredZ == 1000.0f) {
		return(false);
	}
	if (RotZ < 0.0f && DesiredZ == -1000.0f) {
		return(false);
	}
	return(true);
}

bool AdvancedPositionalUpdatePacketClass::Has_Imparted()
{
	return(WWMath::Fabs(ImpartedVelocity.X) >= FLAG_EPSILON || WWMath::Fabs(ImpartedVelocity.Y) >= FLAG_EPSILON || WWMath::Fabs(ImpartedVelocity.Z) >= FLAG_EPSILON
		|| fabs(ImpartedSpin) >= FLAG_EPSILON || fabs(ImpartedRoll) >= FLAG_EPSILON || fabs(ImpartedPitch) >= FLAG_EPSILON);
}

bool AdvancedPositionalUpdatePacketClass::Has_Update_Period()
{
	return(UpdatePeriod != DEFAULT_UPDATE_PERIOD);
}

int AdvancedPositionalUpdatePacketClass::Get_Data_Size()
{
	return(Get_Size(Get_Flags()));
}

int AdvancedPositionalUpdatePacketClass::Get_Flag_Count()
{
	return(9);
}

int AdvancedPositionalUpdatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d TimeStamp=%u Position={%f %f %f} Orientation={%f %f %f %f} CurrentSpeed=%f SetSpeed=%f Acceleration=%f RotY=%f DesiredY=%f RotZ=%f DesiredZ=%f ImpartedVelocity={%f %f %f) ImpartedSpin=%f ImpartedRoll=%f ImpartedPitch=%f MovementID=%u UpdatePeriod=%u}", "Advanced_Positional_Update",
		GameID, TimeStamp, Position.X, Position.Y, Position.Z, Orientation.X, Orientation.Y, Orientation.Z, Orientation.W,
		CurrentSpeed, SetSpeed, Acceleration, RotY, DesiredY, RotZ, DesiredZ,
		ImpartedVelocity.X, ImpartedVelocity.Y, ImpartedVelocity.Z, ImpartedSpin, ImpartedRoll, ImpartedPitch, MovementID, UpdatePeriod));
}

PlanetPositionalUpdatePacketClass::PlanetPositionalUpdatePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&TimeStamp, sizeof(TimeStamp));
	raw->Get(Unknown14, sizeof(Unknown14));
	raw->Get(&OrbitID, sizeof(OrbitID));
	raw->Get(&OrbitDist, sizeof(OrbitDist));
	raw->Get(&OrbitAngle, sizeof(OrbitAngle));
	raw->Get(&OrbitRate, sizeof(OrbitRate));
	raw->Get(&RotateAngle, sizeof(RotateAngle));
	raw->Get(&RotateRate, sizeof(RotateRate));
	raw->Get(&TiltAngle, sizeof(TiltAngle));
}

RawPacketClass *PlanetPositionalUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&TimeStamp, sizeof(TimeStamp));
	raw->Add(Unknown14, sizeof(Unknown14));
	raw->Add(&OrbitID, sizeof(OrbitID));
	raw->Add(&OrbitDist, sizeof(OrbitDist));
	raw->Add(&OrbitAngle, sizeof(OrbitAngle));
	raw->Add(&OrbitRate, sizeof(OrbitRate));
	raw->Add(&RotateAngle, sizeof(RotateAngle));
	raw->Add(&RotateRate, sizeof(RotateRate));
	raw->Add(&TiltAngle, sizeof(TiltAngle));
	return(raw);
}

int PlanetPositionalUpdatePacketClass::Get_Type()
{
	return(63);
}

int PlanetPositionalUpdatePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(TimeStamp) + sizeof(Unknown14) + sizeof(OrbitID) + sizeof(OrbitDist) + sizeof(OrbitAngle) + sizeof(OrbitRate) + sizeof(RotateAngle) + sizeof(RotateRate) + sizeof(TiltAngle));
}

int PlanetPositionalUpdatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d TimeStamp=%u OrbitID=%d OrbitDist=%f OrbitAngle=%f OrbitRate=%f RotateAngle=%f RotateRate=%f TiltAngle=%f}", "Planet_Positional_Update", GameID, TimeStamp, OrbitID, OrbitDist, OrbitAngle, OrbitRate, RotateAngle, RotateRate, TiltAngle));
}


/*
**	Reconstruction notes (pkupdate.cpp)
**
**	* Client 0x8ABA80-0x8AE7E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* Opcode 62 (ADVANCED_POSITIONAL_UPDATE) written by hand over FlagPacketClass (pkflag.cpp; builder P3,
**	  2026-10-26): its message has a flag mask (Get_Flags 0x8AD300: one bit per field that differs from
**	  its default, from the Has_ tests 0x8AD690-0x8AD980, which it inlines), the fields always sent, then
**	  the flagged ones; the raw constructor fills the absent ones from the others (SetSpeed from
**	  CurrentSpeed, DesiredY / DesiredZ +-pi / +-1000 by RotY's / RotZ's sign). Its asserts sit on the
**	  client's lines 179-187 and 277-285 (D25); the blank stretches above them hold opcode 8's place.
**	  Build sizes the message before the new (`int size = Get_Size(flags);`: Get_Size in the new's
**	  arguments is evaluated after the allocation) and Get_Size adds one field per statement.
**	* Not rebuilt yet: opcode 8, 70, 65 (declared in pk_classes.h).
*/
