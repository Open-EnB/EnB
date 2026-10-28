/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkupdate.h (the .cpp named by an assert)
**
**	The packet classes of client 0x8ABA80-0x8AE7E0 rebuilt so far (opcode 64, 62, 63); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUPDATE_H
#define PKUPDATE_H

#include "pkflag.h"
#include "vector3.h"
#include "quat.h"

// opcode 64 (0x0040, CONSTANT_POSITIONAL_UPDATE): vtable 0xB0C0B4, constructor 0x8ABBF0, 0x34 bytes
class ConstantPositionalUpdatePacketClass : public PacketClass
{
public:
	ConstantPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8ABBF0
	virtual RawPacketClass *Build();	// +0x04 0x8ABCB0
	virtual bool Process_Packet(void *client);	// +0x08 0x733C80 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8ABDB0
	virtual int Print(char *buffer, int len);	// +0x10 0x8ABDD0
	virtual int Get_Channel() { return(Unknown2C); }	// +0x18 0x8ABB60
	virtual int Get_Priority() { return(Unknown30); }	// +0x1C 0x8ABB80
	virtual int Get_Type();	// +0x28 0x8ABD90  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	float Unknown10[3];	// +0x10
	char Unknown1C[16];	// +0x1C
	int Unknown2C;	// +0x2C
	int Unknown30;	// +0x30
};

// opcode 8 (0x0008, SIMPLE_POSITIONAL_UDPATE): vtable 0xB0C0E8, constructor 0x8AC1B0, 0x44 bytes
class SimplePositionalUpdatePacketClass : public PacketClass  // NAME 3: the client's Print text 'Simple_Positional_Update'
{
public:
	SimplePositionalUpdatePacketClass();	// 0x8ABE20
	SimplePositionalUpdatePacketClass(int priority, int game_id, unsigned int time_stamp, const Vector3 &pos, const Quaternion &orient, const Vector3 &vel);	// 0x8ABF40 (its asserts' names)
	SimplePositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8AC1B0
	virtual RawPacketClass *Build();	// +0x04 0x8AC3E0
	virtual bool Process_Packet(void *client);	// +0x08 0x733DC0 (cpacket.cpp, NOT EXACT)
	virtual int Get_Data_Size();	// +0x0C 0x8AC510
	virtual int Print(char *buffer, int len);	// +0x10 0x8AC530
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x8ABE90
	virtual int Get_Priority() { return(Priority); }	// +0x1C 0x8ABEB0
	virtual bool Is_Reliable() { return(false); }	// +0x20 0x8ABED0
	virtual int Get_Type();	// +0x28 0x8AC4F0  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	unsigned int TimeStamp;	// +0x10  // NAME 3: the client's Print text 'TimeStamp='
	Vector3 Position;	// +0x14  // NAME 3: the client's assert 'Position.Is_Valid()'
	Quaternion Orientation;	// +0x20  // NAME 3: the client's assert 'Orientation.Is_Valid()'
	Vector3 Velocity;	// +0x30  // NAME 3: the client's assert 'Velocity.Is_Valid()'
	int Channel;	// +0x3C (PacketClass::Unknown_14 of GameID in the value constructor; not in the message)  // NAME 2: Get_Channel returns it
	int Priority;	// +0x40 (not in the message)  // NAME 2: Get_Priority returns it
};

// opcode 70 (0x0046, COMPONENT_POSITIONAL_UPDATE): vtable 0xB0C1A0, constructor 0x8AE240, 0x54 bytes
class ComponentPositionalUpdatePacketClass : public SimplePositionalUpdatePacketClass  // NAME 3: the client's Print text 'Component_Positional_Update'
{
public:
	ComponentPositionalUpdatePacketClass(int priority, int game_id, unsigned int time_stamp, const Vector3 &pos, const Quaternion &orient, const Vector3 &vel, float imparted_decay, float tractor_speed, int tractor_id, int tractor_effect_id);	// 0x8AE130
	ComponentPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8AE240
	virtual RawPacketClass *Build();	// +0x04 0x8AE2E0
	virtual bool Process_Packet(void *client);	// +0x08 0x734BF0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8AE370
	virtual int Print(char *buffer, int len);	// +0x10 0x8AE3D0
	virtual int Get_Type();	// +0x28 0x8AE350

	int Print_Tractor(char *buffer, int len);	// 0x8AE390 (Print's second half, inlined there)  // NAME 2: what it prints

	float ImpartedDecay;	// +0x44  // NAME 3: the client's Print text 'ImpartedDecay='
	float TractorSpeed;	// +0x48  // NAME 3: the client's Print text 'TractorSpeed='
	int TractorID;	// +0x4C  // NAME 3: the client's Print text 'TractorID='
	int TractorEffectID;	// +0x50  // NAME 3: the client's Print text 'TractorEffectID='
};

// opcode 65 (0x0041, FORMATION_POSITIONAL_UPDATE): vtable 0xB0C1D4, constructor 0x8AE5C0, 0x24 bytes
class FormationPositionalUpdatePacketClass : public PacketClass  // NAME 3: the client's Print text 'FormationPositionalUpdate'
{
public:
	FormationPositionalUpdatePacketClass(int game_id, int leader_id, const Vector3 &offset);	// 0x8AE4B0
	FormationPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8AE5C0
	virtual RawPacketClass *Build();	// +0x04 0x8AE690
	virtual bool Process_Packet(void *client);	// +0x08 0x734740 (cpacket.cpp, NOT EXACT)
	virtual int Get_Data_Size();	// +0x0C 0x8AE770
	virtual int Print(char *buffer, int len);	// +0x10 0x8AE790

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int LeaderID;	// +0x10  // NAME 3: the client's Print text 'LeaderID='
	Vector3 Offset;	// +0x14  // NAME 3: the client's Print text 'Offset='
	int Channel;	// +0x20 (PacketClass::Unknown_14 of GameID in the value constructor; not in the message)  // NAME 2: what it holds, as 8's
};

// opcode 62 (0x003E, ADVANCED_POSITIONAL_UPDATE): vtable 0xB0C11C, constructor 0x8ACAB0, 0x74 bytes
class AdvancedPositionalUpdatePacketClass : public FlagPacketClass
{
public:
	AdvancedPositionalUpdatePacketClass(int priority, int game_id, unsigned int time_stamp, const Vector3 &position, const Quaternion &orientation, float current_speed, float set_speed, float acceleration, float roty, float desiredy, float rotz, float desiredz, const Vector3 &imparted_velocity, float imparted_spin, float imparted_roll, float imparted_pitch, unsigned int movement_id, unsigned int update_period);	// 0x8AC5B0 (the parameter names from its asserts)
	AdvancedPositionalUpdatePacketClass();	// 0x8ACA10
	AdvancedPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8ACAB0
	virtual RawPacketClass *Build();	// +0x04 0x8AD000
	virtual bool Process_Packet(void *client);	// +0x08 0x734130 (cpacket.cpp, NOT EXACT)
	virtual int Get_Data_Size();	// +0x0C 0x8AD9A0
	virtual int Print(char *buffer, int len);	// +0x10 0x8AD9F0
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x8AC960
	virtual int Get_Priority() { return(Priority); }	// +0x1C 0x8AC980
	virtual bool Is_Reliable() { return(false); }	// +0x20 0x8AC9A0
	virtual int Get_Flag_Count();	// +0x28 0x8AD9D0 (9)
	virtual int Get_Type();	// +0x2C 0x8AD200  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Get_Size(unsigned int flags);	// 0x8AD220 (the payload's bytes for these flags)  // NAME 2: what it returns
	unsigned int Get_Flags();	// 0x8AD300 (the bit of each field that differs from its default)  // NAME 2: what it returns
	bool Has_Current_Speed();	// 0x8AD690 (flag 0; inlined in Get_Flags)  // NAME 2: what it tests
	bool Has_Set_Speed();	// 0x8AD6C0 (flag 1)  // NAME 2: what it tests
	bool Has_Acceleration();	// 0x8AD6F0 (flag 2)  // NAME 2: what it tests
	bool Has_RotY();	// 0x8AD730 (flag 3)  // NAME 2: what it tests
	bool Has_DesiredY();	// 0x8AD760 (flag 4: not the default, +-pi by RotY's sign)  // NAME 2: what it tests
	bool Has_RotZ();	// 0x8AD7E0 (flag 5)  // NAME 2: what it tests
	bool Has_DesiredZ();	// 0x8AD810 (flag 6: not the default, +-1000 by RotZ's sign)  // NAME 2: what it tests
	bool Has_Imparted();	// 0x8AD890 (flag 7: ImpartedVelocity, -Spin, -Roll, -Pitch)  // NAME 2: what it tests
	bool Has_Update_Period();	// 0x8AD980 (flag 8: not 500)  // NAME 2: what it tests
	bool Unknown_8ADB20();	// 0x8ADB20 (not rebuilt: CurrentSpeed or SetSpeed above a limit, or ImpartedVelocity not zero; postate.cpp's types 4 and 8 ask it)  // NAME 1: neutral (D10)

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	unsigned int TimeStamp;	// +0x10  // NAME 3: the client's Print text 'TimeStamp='
	Vector3 Position;	// +0x14  // NAME 3: the client's Print text 'Position='
	Quaternion Orientation;	// +0x20  // NAME 3: the client's Print text 'Orientation='
	float CurrentSpeed;	// +0x30 (flag 0) (named by the client's assert)
	float SetSpeed;	// +0x34 (flag 1; CurrentSpeed when absent) (named by the client's assert)
	float Acceleration;	// +0x38 (flag 2) (named by the client's assert)
	float RotY;	// +0x3C (flag 3) (named by the client's assert)
	float DesiredY;	// +0x40 (flag 4)  // NAME 3: the client's Print text 'DesiredY='
	float RotZ;	// +0x44 (flag 5) (named by the client's assert)
	float DesiredZ;	// +0x48 (flag 6)  // NAME 3: the client's Print text 'DesiredZ='
	Vector3 ImpartedVelocity;	// +0x4C (flag 7) (named by the client's assert)
	float ImpartedSpin;	// +0x58 (flag 7) (named by the client's assert)
	float ImpartedRoll;	// +0x5C (flag 7) (named by the client's assert)
	float ImpartedPitch;	// +0x60 (flag 7) (named by the client's assert)
	unsigned int MovementID;	// +0x64  // NAME 3: the client's Print text 'MovementID='
	unsigned int UpdatePeriod;	// +0x68 (flag 8; 500 by default)  // NAME 3: the client's Print text 'UpdatePeriod='
	int Channel;	// +0x6C (PacketClass::Unknown_14 of GameID; not read from the message)  // NAME 2: Get_Channel returns it
	int Priority;	// +0x70 (not read from the message)  // NAME 2: Get_Priority returns it
};

// opcode 63 (0x003F, PLANET_POSITIONAL_UPDATE): vtable 0xB0C16C, constructor 0x8ADDF0, 0x3C bytes
class PlanetPositionalUpdatePacketClass : public PacketClass
{
public:
	PlanetPositionalUpdatePacketClass(RawPacketClass *raw);	// 0x8ADDF0
	virtual RawPacketClass *Build();	// +0x04 0x8ADF10
	virtual bool Process_Packet(void *client);	// +0x08 0x734A40 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8AE090
	virtual int Print(char *buffer, int len);	// +0x10 0x8AE0B0
	virtual int Get_Channel() { return(-1); }	// +0x18 0x8ADD80
	virtual int Get_Type();	// +0x28 0x8AE070  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int TimeStamp;	// +0x10  // NAME 3: the client's Print text 'TimeStamp='
	char Unknown14[12];	// +0x14
	int OrbitID;	// +0x20  // NAME 3: the client's Print text 'OrbitID='
	float OrbitDist;	// +0x24  // NAME 3: the client's Print text 'OrbitDist='
	float OrbitAngle;	// +0x28  // NAME 3: the client's Print text 'OrbitAngle='
	float OrbitRate;	// +0x2C  // NAME 3: the client's Print text 'OrbitRate='
	float RotateAngle;	// +0x30  // NAME 3: the client's Print text 'RotateAngle='
	float RotateRate;	// +0x34  // NAME 3: the client's Print text 'RotateRate='
	float TiltAngle;	// +0x38  // NAME 3: the client's Print text 'TiltAngle='
};

#endif
