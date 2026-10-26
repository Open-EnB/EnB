// Earth & Beyond client (reconstruction): app\packets\pkupdate.cpp. Notes at the end of the file.
#include "pkupdate.h"
#include <stdio.h>


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
**	* Not rebuilt yet: opcode 8, 62, 70, 65 (declared in pk_classes.h).
*/
