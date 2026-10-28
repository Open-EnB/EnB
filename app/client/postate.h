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

class ClientClass;
class ConstantPositionalUpdatePacketClass;
class SimplePositionalUpdatePacketClass;
class PlanetPositionalUpdatePacketClass;
class ComponentPositionalUpdatePacketClass;
class ExtrapClass;

#ifndef __CO_H
// The state's base, co.h's Unknown98Class (vtable 0xB00768; its trivial slots 0x6F2D40-0x6F2E10 are
// emitted with co.cpp's default state). A copy of co.h's declaration while co.h does not include this
// header: a file includes one of the two.
class Unknown98Class							// NAME 1: neutral (D10)
{
public:
	virtual ~Unknown98Class() {}
	virtual int Unknown_04() = 0;				// +0x04 the state's type
	virtual Matrix3D Unknown_08(int unknown) = 0;	// NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_0C(int unknown) = 0;	// NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_10(int unknown) = 0;	// NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_14(int unknown) = 0;	// NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_18(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x18 0x6F2D40  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_1C(int unknown) { return(Unknown_0C(unknown)); }		// +0x1C 0x6F2D70  // NAME 1 [unknown]: neutral
	virtual Matrix3D Unknown_20(int unknown) { return(Unknown_08(unknown)); }		// +0x20 0x6F2DA0  // NAME 1 [unknown]: neutral
	virtual void Unknown_24(Unknown98Class *state) {}							// +0x24 0x6F2DD0  // NAME 1 [state]: neutral
	virtual Unknown98Class *Unknown_28() { return(0); }						// +0x28 0x6F2DF0
	virtual bool Unknown_2C() { return(false); }								// +0x2C 0x6F2E10
};
#endif

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

#endif
