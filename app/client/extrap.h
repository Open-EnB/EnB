/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\extrap.h (name assumed from extrap.cpp)
**
**	extrap.cpp's state nodes: the extrapolators that postate.cpp's positional states keep (postate.h's
**	UnknownB027A0Class::Extrap) and ask for the position, orientation, velocity, speed and rotation rate
**	at a time. The base is the client's StateNode (its asserts 'Bad function call! StateNode::Set_Motion()'
**	and '... StateNode::End_Tilt()'); its vtable is 0xB02398. Declared from the constructors and the
**	vtables 0xB02364-0xB02628. Names: D10, D24, D56.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXTRAP_H  // NAME 3 [extrap.h]: header name assumed from the client's extrap.cpp
#define __EXTRAP_H

#include "vector3.h"
#include "matrix3d.h"
#include "quat.h"

class ClientClass;
class SClient;
class SimplePositionalUpdatePacketClass;
class PlanetPositionalUpdatePacketClass;
class ComponentPositionalUpdatePacketClass;
class AdvancedPositionalUpdatePacketClass;
class FormationPositionalUpdatePacketClass;

// The base of every extrapolator (vtable 0xB02398, 4 bytes)
class StateNode	// NAME 3: the client's assert 'Bad function call! StateNode::Set_Motion()'
{
public:
	virtual ~StateNode() {}	// 0x70F320 (scalar deleting), 0x70F610
	virtual Vector3 Unknown_04(int time);	// +0x04 0x70F040 (Unknown_1C's position)  // NAME 2 [time]: what Unknown_1C takes
	virtual Quaternion Unknown_08(int time);	// +0x08 0x70F090 (Unknown_1C's orientation)  // NAME 2 [time]: as Unknown_04's
	virtual Vector3 Unknown_0C(int time);	// +0x0C 0x70F100 (Unknown_1C's velocity)  // NAME 2 [time]: as Unknown_04's
	virtual float Unknown_10(int time);		// +0x10 0x70F150 (Unknown_1C's speed)  // NAME 2 [time]: as Unknown_04's
	virtual Vector3 Unknown_14(int time);	// +0x14 0x70F180 (Unknown_1C's rotation rate)  // NAME 2 [time]: as Unknown_04's
	virtual Vector3 Unknown_18(int time) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x18 0x70F2D0  // NAME 2 [time]: as Unknown_04's
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown) = 0;	// +0x1C (pure)  // NAME 3 [position, orientation, velocity, speed, rotrate]: the client's asserts 'position->Is_Valid()' ... (0x70F630)  // NAME 1 [unknown]: neutral
	virtual void Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate);	// +0x20 0x70F1D0 (the base asserts)  // NAME 3: the client's assert text  // NAME 2 [time, speed, acceleration, turn_rate, tilt_rate]: the aux fields postate's 0x717430 reads for them
	virtual void End_Tilt(int time, float rate);	// +0x24 0x70F210 (the base asserts)  // NAME 3: the client's assert text  // NAME 2 [time, rate]: what postate's 0x7179A0 passes
	virtual StateNode *Unknown_28(int time) { return(this); }	// +0x28 0x70F300 (the node to use from now on: a blend returns its end node once it is over)  // NAME 2 [time]: as Unknown_04's
};

// A node moving at a constant velocity from a simple positional update (vtable 0xB02364, 0x30 bytes)
class Unknown70F250Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown70F250Class(SimplePositionalUpdatePacketClass *packet);	// 0x70F250  // NAME 1 [packet]: what postate's state passes
	virtual Vector3 Unknown_18(int time) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x18 0x70F4C0  // NAME 2 [time]: as the base's
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x70F3A0  // NAME 2: as the base's

	int Time;					// +0x04 (the packet's TimeStamp; signed: Unknown_1C's fild)  // NAME 2: what it holds
	Vector3 Position;			// +0x08  // NAME 2: the packet's Position
	Quaternion Orientation;		// +0x14  // NAME 2: the packet's Orientation
	Vector3 Velocity;			// +0x24  // NAME 2: the packet's Velocity
};

// The motion objects the advanced and component nodes step (vtable 0xB02400, 8 bytes)
class Unknown710030Class	// NAME 1: neutral (D10)
{
public:
	Unknown710030Class();	// 0x710030
	Unknown710030Class(const Unknown710030Class &other);	// 0x7100B0  // NAME 2 [other]: a copy
	virtual ~Unknown710030Class() {}	// 0x710080 (scalar deleting), 0x7109B0
	virtual void Unknown_04(unsigned int time);	// +0x04 0x7100E0 (steps until Time reaches time)  // NAME 2 [time]: what it compares with Time
	virtual void Unknown_08() = 0;	// +0x08 (one step)
	virtual Unknown710030Class *Unknown_0C() = 0;	// +0x0C (a copy: 0x710260)
	virtual Matrix3D *Unknown_10() = 0;	// +0x10 (the transform)
	virtual Vector3 Unknown_14() = 0;	// +0x14 (one step's move)
	virtual float Unknown_18() = 0;	// +0x18 (the speed)
	virtual Vector3 Unknown_1C() = 0;	// +0x1C
	virtual Vector3 Unknown_20() { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x20 0x710050

	unsigned int Time;	// +0x04  // NAME 2: what Unknown_04 compares
};

// The advanced node's motion object (vtable 0xB02480, 0xA0 bytes): an advanced positional update stepped every 100 ms
class Unknown710530Class : public Unknown710030Class	// NAME 1: neutral (D10)
{
public:
	Unknown710530Class(AdvancedPositionalUpdatePacketClass *packet);	// 0x710530  // NAME 1 [packet]: what 0x710110 passes
	virtual void Unknown_04(unsigned int time);	// +0x04 0x711520  // NAME 2 [time]: as the base's
	virtual void Unknown_08();	// +0x08 0x710A20
	virtual Unknown710030Class *Unknown_0C();	// +0x0C 0x7115F0
	virtual Matrix3D *Unknown_10();	// +0x10 0x7117A0
	virtual Vector3 Unknown_14();	// +0x14 0x7117D0
	virtual float Unknown_18();	// +0x18 0x711930
	virtual Vector3 Unknown_1C();	// +0x1C 0x711950
	virtual Vector3 Unknown_20();	// +0x20 0x711980
	bool Unknown_711490();	// 0x711490 (inlined in 0x7104A0 and 0x711520)

	Matrix3D Transform;		// +0x08 (Build_Matrix3D of the packet's Orientation, at its Position)  // NAME 2: what it holds
	float Speed;			// +0x38 (the packet's CurrentSpeed * 100)  // NAME 2: what Unknown_18 returns
	float Unknown3C;		// +0x3C (the packet's SetSpeed * 100)
	bool Unknown40;			// +0x40 (Unknown3C differs from Speed)
	float Unknown44;		// +0x44 (the acceleration towards Unknown3C)
	bool Unknown48;			// +0x48
	float Unknown4C;		// +0x4C
	float Unknown50;		// +0x50 (the turn rate * 100)
	float Unknown54;		// +0x54 (-1000 or 1000 by Unknown50's sign)
	bool Unknown58;			// +0x58
	float Unknown5C;		// +0x5C
	float Unknown60;		// +0x60 (the tilt rate * 100)
	float Unknown64;		// +0x64 (-70 or 70 degrees by Unknown60's sign)
	bool Unknown68;			// +0x68
	Vector3 Unknown6C;		// +0x6C (the packet's ImpartedVelocity * 100)
	char Unknown78[0xA0 - 0x78];
};

// The component node's motion object (vtable 0xB025FC, 0x54 bytes)
class Unknown713D70Class : public Unknown710030Class	// NAME 1: neutral (D10)
{
public:
	Unknown713D70Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x713D70  // NAME 1 [packet, client]: what 0x713C80 passes
	virtual void Unknown_08();	// +0x08 0x7141D0
	virtual Unknown710030Class *Unknown_0C();	// +0x0C 0x714010
	virtual Matrix3D *Unknown_10();	// +0x10 0x714250
	virtual Vector3 Unknown_14();	// +0x14 0x7142C0
	virtual float Unknown_18();	// +0x18 0x714270
	virtual Vector3 Unknown_1C();	// +0x1C 0x713ED0
	bool Unknown_7140E0(Vector3 *result);	// 0x7140E0 (the tractor's pull)  // NAME 1 [result]: neutral

	Matrix3D Transform;		// +0x08 (Build_Matrix3D of the packet's Orientation, at its Position)  // NAME 2: what it holds
	Vector3 Velocity;		// +0x38 (the packet's Velocity / 10)  // NAME 2: what it holds
	float ImpartedDecay;	// +0x44 (the packet's ImpartedDecay / 10)  // NAME 2: what it holds
	float TractorSpeed;		// +0x48 (the packet's TractorSpeed / 10)  // NAME 2: what it holds
	int TractorID;			// +0x4C  // NAME 2: the packet's TractorID
	ClientClass *Client;	// +0x50  // NAME 2: what the node passes
};

// The base of the nodes that step motion objects (vtable 0xB023CC, 0x14 bytes)
class Unknown70F4F0Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown70F4F0Class();	// 0x70F4F0
	virtual ~Unknown70F4F0Class();	// 0x70F550 (0x70F520 scalar deleting)
	virtual Vector3 Unknown_18(int time);	// +0x18 0x70FFE0  // NAME 2 [time]: as the base's
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x70F630  // NAME 2: as the base's

	Unknown710030Class *Current;	// +0x04  // NAME 3: the client's assert 'Current' (0x710110)
	Unknown710030Class *Unknown08;	// +0x08 (asked before Current by Unknown_18)
	Unknown710030Class *Unknown0C;	// +0x0C
	Unknown710030Class *Unknown10;	// +0x10
};

// The advanced node (vtable 0xB0242C, 0x14 bytes): one motion object from an advanced positional update
class Unknown710110Class : public Unknown70F4F0Class	// NAME 1: neutral (D10)
{
public:
	Unknown710110Class();	// 0x710230 (inlined in 0x710260)
	Unknown710110Class(AdvancedPositionalUpdatePacketClass *packet);	// 0x710110  // NAME 1 [packet]: what postate's state passes
	virtual void Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate);	// +0x20 0x7102B0  // NAME 2: as the base's
	virtual void End_Tilt(int time, float rate);	// +0x24 0x710410  // NAME 2: as the base's
	bool Unknown_7104A0();	// 0x7104A0 (Current's 0x711490)
	StateNode *Unknown_710260();	// 0x710260 (a new node on a copy of Current; postate's 0x7179A0 continues from it)
};

// The component node (vtable 0xB025C8, 0x14 bytes)
class Unknown713C80Class : public Unknown70F4F0Class	// NAME 1: neutral (D10)
{
public:
	Unknown713C80Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x713C80  // NAME 1 [packet, client]: what postate's state passes
};

// A blend from one node to another between two times (vtable 0xB024C0, 0x78 bytes)
class Unknown7119B0Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown7119B0Class(StateNode *start, StateNode *end, unsigned int start_time, unsigned int end_time);	// 0x7119B0 (type 8's change of course)  // NAME 3 [start, end, start_time, end_time]: the client's asserts 'StartState && EndState', 'EndTime >= StartTime'
	virtual ~Unknown7119B0Class();	// 0x711C80 (0x711C50 scalar deleting)
	virtual Vector3 Unknown_18(int time);	// +0x18 0x7120A0  // NAME 2 [time]: as the base's
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x711D60  // NAME 2: as the base's
	virtual void Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate);	// +0x20 0x7120F0  // NAME 2: as the base's
	virtual StateNode *Unknown_28(int time);	// +0x28 0x711D10  // NAME 2 [time]: as the base's
	void Unknown_712480(int time);	// 0x712480  // NAME 2 [time]: what Set_Motion passes

	StateNode *StartState;		// +0x04  // NAME 3: the client's assert 'StartState && EndState'
	Vector3 Unknown08;			// +0x08 (StartState's position at StartTime)
	Vector3 Unknown14;			// +0x14 (StartState's velocity at StartTime, times the blend's length)
	unsigned int StartTime;		// +0x20  // NAME 3: the client's assert 'EndTime >= StartTime'
	StateNode *EndState;		// +0x24  // NAME 3: as StartState
	char Unknown28[0x40 - 0x28];
	unsigned int EndTime;		// +0x40  // NAME 3: as StartTime
	Matrix3D Unknown44;			// +0x44 (Build_Matrix3D of StartState's orientation)
	float Unknown74;			// +0x74 (StartState's speed)
};

// A blend from one node to another between two times (vtable 0xB024F8, 0x14 bytes)
class Unknown7125F0Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown7125F0Class(StateNode *start, StateNode *end, unsigned int start_time, unsigned int end_time);	// 0x7125F0 (type 3's update: the old and the new, from the packet's time for 2 s)  // NAME 3 [start, end, start_time, end_time]: the client's asserts 'StartState && EndState', 'EndTime >= StartTime'
	virtual ~Unknown7125F0Class();	// 0x7126F0 (0x7126C0 scalar deleting)
	virtual Vector3 Unknown_18(int time);	// +0x18 0x712C10  // NAME 2 [time]: as the base's
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x712780  // NAME 2: as the base's
	virtual void Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate);	// +0x20 0x712C60  // NAME 2: as the base's
	virtual void End_Tilt(int time, float rate);	// +0x24 0x712CF0  // NAME 2: as the base's
	virtual StateNode *Unknown_28(int time);	// +0x28 0x712D60  // NAME 2 [time]: as the base's

	StateNode *StartState;		// +0x04  // NAME 3: the client's assert 'StartState && EndState'
	StateNode *EndState;		// +0x08  // NAME 3: as StartState
	unsigned int StartTime;		// +0x0C  // NAME 3: the client's assert 'EndTime >= StartTime'
	unsigned int EndTime;		// +0x10  // NAME 3: as StartTime
};

// One node's state at a time (0x38 bytes: Unknown712DB0Class keeps two)
struct Unknown712DB0Struct	// NAME 1: neutral (D10)
{
	Vector3 Position;		// +0x00  // NAME 2: Unknown_1C's position
	Vector3 Velocity;		// +0x0C  // NAME 2: Unknown_1C's velocity
	Quaternion Orientation;	// +0x18  // NAME 2: Unknown_1C's orientation
	float Speed;			// +0x28  // NAME 2: Unknown_1C's speed
	Vector3 RotRate;		// +0x2C  // NAME 2: Unknown_1C's rotrate
};

// Unknown7125F0Class with both nodes' states kept at the two times (vtable 0xB0252C, 0x84 bytes)
class Unknown712DB0Class : public Unknown7125F0Class	// NAME 1: neutral (D10)
{
public:
	Unknown712DB0Class(StateNode *start, StateNode *end, unsigned int start_time, unsigned int end_time);	// 0x712DB0  // NAME 2 [start, end, start_time, end_time]: as the base's
	virtual Vector3 Unknown_18(int time);	// +0x18 0x7132A0  // NAME 2 [time]: as the base's
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x712FA0  // NAME 2: as the base's
	virtual void Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate);	// +0x20 0x713370  // NAME 2: as the base's
	virtual void End_Tilt(int time, float rate);	// +0x24 0x713450  // NAME 2: as the base's
	void Unknown_7132F0(unsigned int start_time, unsigned int end_time);	// 0x7132F0 (takes both states again)  // NAME 2 [start_time, end_time]: the members it sets

	Unknown712DB0Struct Start;	// +0x14 (StartState at StartTime)  // NAME 2: what it holds
	Unknown712DB0Struct End;	// +0x4C (EndState at EndTime)  // NAME 2: what it holds
};

// A planet's orbit (vtable 0xB02560, 0x34 bytes)
class Unknown713520Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown713520Class(PlanetPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x713520  // NAME 1 [packet, client]: what postate's state passes
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x713670  // NAME 2: as the base's
	bool Unknown_7135F0(int time, int unknown, Vector3 *result);	// 0x7135F0 (the orbit object's position; no callers in the client)  // NAME 1 [time, unknown, result]: neutral

	int Time;				// +0x04 (the packet's TimeStamp)  // NAME 2: what it holds
	ClientClass *Client;	// +0x08  // NAME 2: what the state passes
	Vector3 Unknown0C;		// +0x0C (the packet's Unknown14)
	int OrbitID;			// +0x18  // NAME 2: the packet's OrbitID
	float OrbitDist;		// +0x1C  // NAME 2: the packet's OrbitDist
	float OrbitAngle;		// +0x20  // NAME 2: the packet's OrbitAngle
	float OrbitRate;		// +0x24  // NAME 2: the packet's OrbitRate
	float RotateAngle;		// +0x28  // NAME 2: the packet's RotateAngle
	float RotateRate;		// +0x2C  // NAME 2: the packet's RotateRate
	float TiltAngle;		// +0x30  // NAME 2: the packet's TiltAngle
};

// A node seen through another at a time offset (vtable 0xB02594, 0x0C bytes)
class Unknown713B00Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown713B00Class(StateNode *state, int offset);	// 0x713B00 (no callers in the client)  // NAME 1 [state, offset]: neutral
	virtual ~Unknown713B00Class();	// 0x713BC0 (0x713B90 scalar deleting)
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x713C30  // NAME 2: as the base's

	StateNode *State;	// +0x04  // NAME 1: neutral
	int Offset;			// +0x08 (subtracted from the time)  // NAME 2: what it does
};

// A formation member: the leader's state plus an offset (vtable 0xB02628, 0x48 bytes)
class Unknown714340Class : public StateNode	// NAME 1: neutral (D10)
{
public:
	Unknown714340Class(SClient *client, FormationPositionalUpdatePacketClass *packet);	// 0x714340  // NAME 1 [client, packet]: what postate's state passes
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown);	// +0x1C 0x714410  // NAME 2: as the base's
	virtual void Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate) {}	// +0x20 0x7147B0  // NAME 2: as the base's
	virtual void End_Tilt(int time, float rate) {}	// +0x24 0x7147D0  // NAME 2: as the base's

	SClient *Client;		// +0x04  // NAME 2: what the state passes
	int LeaderID;			// +0x08  // NAME 2: the packet's LeaderID
	Vector3 Offset;			// +0x0C  // NAME 2: the packet's Offset
	Matrix3D Unknown18;		// +0x18 (identity)
};

#endif
