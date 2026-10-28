/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\postate.h (name assumed from postate.cpp)
**
**	postate.cpp's positional states: the object at ClientObjectClass +0x98 (co.h) that the positional
**	packets replace or update (cpacket.cpp) and that the object's slot +0x28 0x6F4610 asks for its
**	Matrix3D at the client's clock (docs/packet-flow.md section 2.5). Declared from the constructors
**	and the vtables (0xB026EC-0xB02A40). Names: D10, D24, D56.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __POSTATE_H  // NAME 3 [postate.h]: header name assumed from the client's postate.cpp
#define __POSTATE_H

#include "vector3.h"
#include "matrix3d.h"
#include "quat.h"
#include "co.h"

class ClientClass;
class ConstantPositionalUpdatePacketClass;
class SimplePositionalUpdatePacketClass;
class PlanetPositionalUpdatePacketClass;
class ComponentPositionalUpdatePacketClass;
class AdvancedPositionalUpdatePacketClass;
class FormationPositionalUpdatePacketClass;
class SClient;
class ExtrapClass;


// A state that forwards every slot to a second one (vtable 0xB026EC; no constructor call found in the client)
class UnknownB026ECClass : public Unknown98Class	// NAME 1: neutral (D10), named by its vtable
{
public:
	UnknownB026ECClass(Unknown98Class *state);	// 0x714870  // NAME 1 [state]: neutral
	virtual ~UnknownB026ECClass();				// 0x714940 (scalar deleting 0x714910)
	virtual int Unknown_04();					// 0x7149D0
	virtual Matrix3D Unknown_08(int unknown);	// 0x714A10  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_0C(int unknown);	// 0x714A60  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_10(int unknown);	// 0x714AB0  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_14(int unknown);	// 0x714B00  // NAME 1 [unknown]: neutral

	Unknown98Class *PositionState;				// +0x04  // NAME 3: the client's assert 'PositionState'
};

// Type 1, CONSTANT_POSITIONAL_UPDATE 64's state (vtable 0xB02728, 0x40 bytes): a fixed transform
class Unknown714B50Class : public Unknown98Class	// NAME 1: neutral (D10)
{
public:
	Unknown714B50Class(ConstantPositionalUpdatePacketClass *packet);	// 0x714B50  // NAME 1 [packet]: what the handler passes
	void Unknown_714D50(ConstantPositionalUpdatePacketClass *packet);	// 0x714D50  // NAME 1 [packet]: what the handler passes

	virtual int Unknown_04() { return(1); }									// +0x04 0x714BE0
	virtual Matrix3D Unknown_08(int unknown) { return(Transform); }			// +0x08 0x714C00  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_0C(int unknown) { return(Position); }			// +0x0C 0x714C70  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_10(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x10 0x714CA0  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_14(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x14 0x714CD0  // NAME 1 [unknown]: neutral

	Matrix3D Transform;		// +0x04 (the packet's orientation and position)  // NAME 2: what it holds
	Vector3 Position;		// +0x34 (the packet's position)  // NAME 2: what it holds
};

// extrap.cpp's extrapolators, declared as far as the states call them (extrap.cpp is not rebuilt)
class ExtrapClass	// NAME 3: class-from-file (D24) for app\client\extrap.cpp
{
public:
	virtual ~ExtrapClass();
	virtual Vector3 Unknown_04(int time);	// NAME 2 [time]: the state's time
	virtual void Unknown_08();
	virtual Vector3 Unknown_0C(int time);	// NAME 2 [time]: the state's time
	virtual void Unknown_10();
	virtual Vector3 Unknown_14(int time);	// NAME 2 [time]: the state's time
	virtual Vector3 Unknown_18(int time);	// NAME 2 [time]: the state's time
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *a, float *speed, Vector3 *c, bool unknown);	// NAME 2 [time, position, orientation, speed]: what the states do with them  // NAME 1 [a, c, unknown]: neutral
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual ExtrapClass *Unknown_28(int time);	// +0x28 (the extrapolator to use from now on: the state keeps it)  // NAME 2 [time]: the state's time
};
class Unknown70F250Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown70F250Class(SimplePositionalUpdatePacketClass *packet);	// 0x70F250  // NAME 1 [packet]: what the state passes
	char Unknown04[0x30 - 0x04];
};
class Unknown7125F0Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown7125F0Class(ExtrapClass *from, ExtrapClass *to, unsigned int start, unsigned int end);	// 0x7125F0 (type 3's update: the old and the new, from the packet's time for 2 s)  // NAME 1 [from, to, start, end]: what the state passes
	char Unknown04[0x14 - 0x04];
};
class Unknown713520Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown713520Class(PlanetPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x713520  // NAME 1 [packet, client]: what the state passes
	char Unknown04[0x34 - 0x04];
};
class Unknown713C80Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown713C80Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x713C80  // NAME 1 [packet, client]: what the state passes
	char Unknown04[0x14 - 0x04];
};
class Unknown710110Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown710110Class(AdvancedPositionalUpdatePacketClass *packet);	// 0x710110  // NAME 1 [packet]: what the state passes
	char Unknown04[0x14 - 0x04];
};
class Unknown714340Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown714340Class(SClient *client, FormationPositionalUpdatePacketClass *packet);	// 0x714340  // NAME 1 [client, packet]: what the state passes
	char Unknown04[0x48 - 0x04];
};
class Unknown7119B0Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown7119B0Class(ExtrapClass *from, ExtrapClass *to, unsigned int start, unsigned int end);	// 0x7119B0 (type 8's change of course)  // NAME 1 [from, to, start, end]: as 0x7125F0's
	char Unknown04[0x78 - 0x04];
};
class Unknown712DB0Class : public ExtrapClass	// NAME 1: neutral (D10)
{
public:
	Unknown712DB0Class(ExtrapClass *from, ExtrapClass *to, unsigned int start, unsigned int end);	// 0x712DB0  // NAME 1 [from, to, start, end]: as 0x7125F0's
	char Unknown04[0x84 - 0x04];
};

// The base of the states that keep an extrap.cpp object (vtable 0xB027A0, 8 bytes): every slot asks it
class UnknownB027A0Class : public Unknown98Class	// NAME 1: neutral (D10), named by its vtable
{
public:
	UnknownB027A0Class();						// 0x715620 (inlined in the derived constructors)
	virtual ~UnknownB027A0Class();				// 0x715670 (inlined in the derived destructors)

	virtual Matrix3D Unknown_08(int time);		// +0x08 0x7156F0  // NAME 2 [time]: the client's +0x10BC
	virtual Vector3 Unknown_0C(int time);		// +0x0C 0x715800  // NAME 2 [time]: as Unknown_08's
	virtual Vector3 Unknown_10(int time);		// +0x10 0x715850  // NAME 2 [time]: as Unknown_08's
	virtual Vector3 Unknown_14(int time);		// +0x14 0x7158A0  // NAME 2 [time]: as Unknown_08's
	virtual Vector3 Unknown_18(int time);		// +0x18 0x7158F0  // NAME 2 [time]: as Unknown_08's
	virtual Vector3 Unknown_1C(int time);		// +0x1C 0x715930  // NAME 2 [time]: as Unknown_08's
	virtual Matrix3D Unknown_20(int time);		// +0x20 0x7159C0  // NAME 2 [time]: as Unknown_08's

	ExtrapClass *Extrap;						// +0x04  // NAME 2: an extrap.cpp object
};

// Type 3, SIMPLE_POSITIONAL_UPDATE 8's state (vtable 0xB027DC, 8 bytes)
class Unknown715AB0Class : public UnknownB027A0Class	// NAME 1: neutral (D10)
{
public:
	Unknown715AB0Class(SimplePositionalUpdatePacketClass *packet);	// 0x715AB0  // NAME 1 [packet]: what the handler passes
	void Unknown_715C20(SimplePositionalUpdatePacketClass *packet);	// 0x715C20  // NAME 1 [packet]: what the handler passes

	virtual int Unknown_04() { return(3); }		// +0x04 0x715B50
};

// Type 5, PLANET_POSITIONAL_UPDATE 63's state (vtable 0xB02818, 0x0C bytes)
class Unknown715CF0Class : public UnknownB027A0Class	// NAME 1: neutral (D10)
{
public:
	Unknown715CF0Class(PlanetPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x715CF0  // NAME 1 [packet, client]: what the handler passes
	void Unknown_715E60(PlanetPositionalUpdatePacketClass *packet);	// 0x715E60  // NAME 1 [packet]: what the handler passes

	virtual int Unknown_04() { return(5); }		// +0x04 0x715D90

	ClientClass *Client;						// +0x08  // NAME 2: what the handler passes
};

// Type 11, COMPONENT_POSITIONAL_UPDATE 70's state (vtable 0xB028F0, 0x10 bytes)
class Unknown717EF0Class : public UnknownB027A0Class	// NAME 1: neutral (D10)
{
public:
	Unknown717EF0Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x717EF0  // NAME 1 [packet, client]: what the handler passes
	void Unknown_718080(ComponentPositionalUpdatePacketClass *packet, ClientClass *client);	// 0x718080  // NAME 1 [packet, client]: what the handler passes

	virtual int Unknown_04() { return(11); }	// +0x04 0x717FB0

	int TractorID;								// +0x08 (the packet's)  // NAME 2: the packet's TractorID
	int TractorEffectID;						// +0x0C (the packet's)  // NAME 2: the packet's TractorEffectID
};

// A value moving linearly from StartValue at StartTime to EndValue at EndTime (types 4 and 8 keep the
// packets' update period in one, 0x716F70)
struct UnknownLerpStruct	// NAME 1: neutral (D10)
{
	UnknownLerpStruct(unsigned int start_time, unsigned int start_value, unsigned int end_time, unsigned int end_value) :
		StartTime(start_time), StartValue(start_value), EndTime(end_time), EndValue(end_value) {}  // NAME 2 [start_time, start_value, end_time, end_value]: the members

	unsigned int Get(unsigned int time)	// (inline in 0x716140, 0x716220, 0x716F70)  // NAME 2 [Get, time]: what it does
	{
		if (time <= StartTime) {
			return(StartValue);
		}
		if (time >= EndTime) {
			return(EndValue);
		}
		return(StartValue + (int)((float)(time - StartTime) / (float)(EndTime - StartTime) * (int)(EndValue - StartValue)));
	}

	void Change(unsigned int time, unsigned int value);	// (inline in 0x716140: from the value at that time, 2 ms a unit)  // NAME 2 [Change, time, value]: what it does

	unsigned int StartTime;		// +0x00  // NAME 2: what it holds
	unsigned int StartValue;	// +0x04  // NAME 2: what it holds
	unsigned int EndTime;		// +0x08  // NAME 2: what it holds
	unsigned int EndValue;		// +0x0C  // NAME 2: what it holds
};
inline void UnknownLerpStruct::Change(unsigned int time, unsigned int value)
{
	unsigned int current = Get(time);
	StartTime = time;
	StartValue = current;
	EndTime = time + abs((int)(value - current)) * 2;
	EndValue = value;
}

// Type 8, ADVANCED_POSITIONAL_UPDATE 62's state for the other objects (vtable 0xB02854, 0x1C bytes);
// FORMATION_POSITIONAL_UPDATE 65 makes it too
class Unknown715EF0Class : public UnknownB027A0Class	// NAME 1: neutral (D10)
{
public:
	Unknown715EF0Class(int time, AdvancedPositionalUpdatePacketClass *packet);	// 0x715EF0  // NAME 1 [time, packet]: what the handler passes
	Unknown715EF0Class(SClient *client, FormationPositionalUpdatePacketClass *packet);	// 0x716080  // NAME 1 [client, packet]: what the handler passes
	void Unknown_716140(int time, AdvancedPositionalUpdatePacketClass *packet);	// 0x716140  // NAME 1 [time, packet]: what the handler passes
	void Unknown_716220(SClient *client, FormationPositionalUpdatePacketClass *packet);	// 0x716220  // NAME 1 [client, packet]: what the handler passes
	bool Unknown_716390(AdvancedPositionalUpdatePacketClass *packet, ExtrapClass *extrap, int time, int other_time);	// 0x716390  // NAME 1 [packet, extrap, time, other_time]: neutral
	bool Unknown_716400(ExtrapClass *extrap, int time, int other_time);	// 0x716400  // NAME 1 [extrap, time, other_time]: neutral
	bool Unknown_7165D0(AdvancedPositionalUpdatePacketClass *packet, ExtrapClass *extrap, unsigned int time, unsigned int other_time);	// 0x7165D0  // NAME 1 [packet, extrap, time, other_time]: neutral
	bool Unknown_7166A0(ExtrapClass *extrap, int time, int other_time);	// 0x7166A0  // NAME 1 [extrap, time, other_time]: neutral

	virtual int Unknown_04() { return(8); }		// +0x04 0x715FB0
	virtual Matrix3D Unknown_08(int time);		// +0x08 0x716CA0  // NAME 2 [time]: as UnknownB027A0Class's
	virtual Vector3 Unknown_0C(int time);		// +0x0C 0x716B50  // NAME 2 [time]: as UnknownB027A0Class's
	virtual Vector3 Unknown_10(int time);		// +0x10 0x716BC0  // NAME 2 [time]: as UnknownB027A0Class's
	virtual Vector3 Unknown_14(int time);		// +0x14 0x716C30  // NAME 2 [time]: as UnknownB027A0Class's
	virtual Vector3 Unknown_1C(int time);		// +0x1C 0x716DC0  // NAME 2 [time]: as UnknownB027A0Class's
	virtual Matrix3D Unknown_20(int time);		// +0x20 0x716E60  // NAME 2 [time]: as UnknownB027A0Class's
	virtual void Unknown_30(int time, AdvancedPositionalUpdatePacketClass *packet);	// +0x30 0x716760  // NAME 1 [time, packet]: what 0x716140 passes
	virtual void Unknown_34(int time, AdvancedPositionalUpdatePacketClass *packet, unsigned int duration);	// +0x34 0x7168A0  // NAME 1 [time, packet, duration]: neutral (0, 500 or 5000)
	virtual unsigned int Unknown_38(int time);	// +0x38 0x716F70 (the update period at that time)  // NAME 2 [time]: as Unknown_08's

	UnknownLerpStruct Period;					// +0x08 (0 to the packets' UpdatePeriod)  // NAME 2: what it holds
	bool Unknown18;								// +0x18 (true in type 4)
};

// Type 4, ADVANCED_POSITIONAL_UPDATE 62's state for the player's own object (vtable 0xB028A4, 0x28 bytes)
class Unknown717010Class : public Unknown715EF0Class	// NAME 1: neutral (D10)
{
public:
	Unknown717010Class(int time, AdvancedPositionalUpdatePacketClass *packet);	// 0x717010  // NAME 1 [time, packet]: what the handler passes
	Unknown717010Class(SClient *client, FormationPositionalUpdatePacketClass *packet);	// 0x7171C0  // NAME 1 [client, packet]: what the handler passes
	void Unknown_717290(AdvancedPositionalUpdatePacketClass *packet, int time);	// 0x717290  // NAME 1 [packet, time]: what the handler passes
	void Unknown_717400(FormationPositionalUpdatePacketClass *packet, SClient *client);	// 0x717400  // NAME 1 [packet, client]: what the handler passes

	virtual int Unknown_04() { return(4); }		// +0x04 0x7170F0
	virtual void Unknown_34(int time, AdvancedPositionalUpdatePacketClass *packet, unsigned int duration);	// +0x34 0x717CC0  // NAME 1 [time, packet, duration]: as the base's
	virtual unsigned int Unknown_38(int time);	// +0x38 0x717ED0 (0)  // NAME 2 [time]: as the base's

	unsigned int MovementID;					// +0x1C (the packet's)  // NAME 2: the packet's MovementID
	bool Unknown20;								// +0x20
	ExtrapClass *Unknown24;						// +0x24 (the last packet's extrapolator)
};

// Type 12, a straight move from Start to End at Speed from StartTime (vtable 0xB0292C, 0x34 bytes); it holds
// a second state (+0x24). co.cpp's 0x6F4C20 treats types 12-14 alike
class Unknown718160Class : public Unknown98Class	// NAME 1: neutral (D10)
{
public:
	Unknown718160Class(Vector3 start, Vector3 end, float speed, int time, Unknown98Class *state);	// 0x718160  // NAME 2 [start, end, speed, time, state]: the members they set

	virtual Unknown98Class *Unknown_28() { return(State); }			// +0x28 0x7182A0
	virtual void Unknown_24(Unknown98Class *state) { State = state; }	// +0x24 0x7182C0  // NAME 2 [state]: what it sets
	virtual bool Unknown_2C() { return(true); }						// +0x2C 0x7182E0
	virtual int Unknown_04();					// +0x04 0x718350 (12)
	virtual Matrix3D Unknown_08(int time);		// +0x08 0x718370 (looking at End)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_0C(int time);		// +0x0C 0x718410 (the position at that time)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_10(int time);		// +0x10 0x7184A0 (the velocity)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_14(int time);		// +0x14 0x7184E0 (zero)  // NAME 2 [time]: as the others'

	Vector3 Start;					// +0x04  // NAME 2: what it holds
	Vector3 End;					// +0x10  // NAME 2: what it holds
	float Speed;					// +0x1C  // NAME 2: what it holds
	int StartTime;					// +0x20  // NAME 2: what it holds
	Unknown98Class *State;			// +0x24  // NAME 2: what it holds
	Vector3 Direction;				// +0x28 (End - Start, normalised)  // NAME 2: what it holds
};

// Type 14, a transform set from outside (vtable 0xB02A00, 0x70 bytes): the last two transforms and their
// times give the velocity and the spin; it holds a second state (+0x04)
class Unknown718D70Class : public Unknown98Class	// NAME 1: neutral (D10)
{
public:
	Unknown718D70Class(int time, const Matrix3D &transform, Unknown98Class *state);	// 0x718D70  // NAME 2 [time, transform, state]: the members they set

	virtual Unknown98Class *Unknown_28() { return(State); }			// +0x28 0x718E60
	virtual void Unknown_24(Unknown98Class *state) { State = state; }	// +0x24 0x718E80  // NAME 2 [state]: what it sets
	virtual bool Unknown_2C() { return(true); }						// +0x2C 0x718EA0
	virtual int Unknown_04();					// +0x04 0x718F10 (14)
	virtual Matrix3D Unknown_08(int time);		// +0x08 0x718F30 (Transform)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_0C(int time);		// +0x0C 0x718FA0 (Transform's translation)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_10(int time);		// +0x10 0x718FD0 (the velocity)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_14(int time);		// +0x14 0x7190E0 (the spin)  // NAME 2 [time]: as the others'
	virtual void Unknown_30(int time, const Matrix3D &transform);	// +0x30 0x7191C0 (the new transform)  // NAME 2 [time, transform]: the members it sets

	Unknown98Class *State;			// +0x04  // NAME 2: what it holds
	int Time;						// +0x08  // NAME 2: what it holds
	int LastTime;					// +0x0C  // NAME 2: what it holds
	Matrix3D Transform;				// +0x10  // NAME 2: what it holds
	Matrix3D LastTransform;			// +0x40  // NAME 2: what it holds
};

// Type 15, a render object's bone (or the object itself) as the transform (vtable 0xB02A40, 0x0C bytes);
// the constructor deletes the state it replaces
class RenderObjClass;
class Unknown7192A0Class : public Unknown98Class	// NAME 1: neutral (D10)
{
public:
	Unknown7192A0Class(RenderObjClass *render_object, Unknown98Class *state, const char *bone_name);	// 0x7192A0  // NAME 2 [render_object, state, bone_name]: what it does with them

	virtual bool Unknown_2C() { return(true); }	// +0x2C 0x719380
	virtual int Unknown_04();					// +0x04 0x7193F0 (15)
	virtual Matrix3D Unknown_08(int time);		// +0x08 0x719410 (the bone's transform, or the object's)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_0C(int time);		// +0x0C 0x719550 (its translation)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_10(int time);		// +0x10 0x7195E0 (zero)  // NAME 2 [time]: as the others'
	virtual Vector3 Unknown_14(int time);		// +0x14 0x719610 (zero)  // NAME 2 [time]: as the others'

	RenderObjClass *RenderObject;	// +0x04  // NAME 3: the client's assert 'RenderObject'
	int BoneIndex;					// +0x08 (-1 without a bone)  // NAME 3: the client's assert 'BoneIndex > 0'
};

#endif
