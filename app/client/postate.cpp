// Earth & Beyond client (reconstruction): app\Client\postate.cpp. Notes at the end of the file.
#include "postate.h"
#include "..\packets\pkupdate.h"
#include "..\shared\unknown_debug.h"

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
	virtual void Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *a, Vector3 *b, Vector3 *c, bool unknown);	// NAME 2 [time, position, orientation]: what the states do with them  // NAME 1 [a, b, c, unknown]: neutral
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




UnknownB026ECClass::UnknownB026ECClass(Unknown98Class *state) :
	PositionState(state)
{
	G_ASSERT(PositionState);
}

UnknownB026ECClass::~UnknownB026ECClass()
{

	G_ASSERT(PositionState);
	delete PositionState;

}

int UnknownB026ECClass::Unknown_04()
{
	G_ASSERT(PositionState);
	return(PositionState->Unknown_04());
}

Matrix3D UnknownB026ECClass::Unknown_08(int unknown)
{
	G_ASSERT(PositionState);
	return(PositionState->Unknown_08(unknown));
}

Vector3 UnknownB026ECClass::Unknown_0C(int unknown)
{
	G_ASSERT(PositionState);
	return(PositionState->Unknown_0C(unknown));
}

Vector3 UnknownB026ECClass::Unknown_10(int unknown)
{
	G_ASSERT(PositionState);
	return(PositionState->Unknown_10(unknown));
}

Vector3 UnknownB026ECClass::Unknown_14(int unknown)
{
	G_ASSERT(PositionState);
	return(PositionState->Unknown_14(unknown));
}

Unknown714B50Class::Unknown714B50Class(ConstantPositionalUpdatePacketClass *packet) :
	Transform(*(Quaternion *)packet->Unknown1C, *(Vector3 *)packet->Unknown10),
	Position(*(Vector3 *)packet->Unknown10)
{
}

void Unknown714B50Class::Unknown_714D50(ConstantPositionalUpdatePacketClass *packet)
{
	Position = *(Vector3 *)packet->Unknown10;
	Transform = Matrix3D(*(Quaternion *)packet->Unknown1C, *(Vector3 *)packet->Unknown10);
}

UnknownB027A0Class::UnknownB027A0Class() :
	Extrap(0)
{
}

UnknownB027A0Class::~UnknownB027A0Class()
{
	if (Extrap) {
		delete Extrap;
		Extrap = 0;
	}
}

Matrix3D UnknownB027A0Class::Unknown_08(int time)
{
	if (!Extrap) {
		return(Matrix3D::Identity);
	}
	Extrap = Extrap->Unknown_28(time);
	Vector3 position;
	Quaternion orientation;
	Extrap->Unknown_1C(time, &position, &orientation, 0, 0, 0, false);
	return(Matrix3D(orientation, position));
}

Vector3 UnknownB027A0Class::Unknown_0C(int time)
{
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap = Extrap->Unknown_28(time);
	return(Extrap->Unknown_04(time));
}

Vector3 UnknownB027A0Class::Unknown_10(int time)
{
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap = Extrap->Unknown_28(time);
	return(Extrap->Unknown_0C(time));
}

Vector3 UnknownB027A0Class::Unknown_14(int time)
{
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap = Extrap->Unknown_28(time);
	return(Extrap->Unknown_14(time));
}

Vector3 UnknownB027A0Class::Unknown_18(int time)
{
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	return(Extrap->Unknown_18(time));
}

Vector3 UnknownB027A0Class::Unknown_1C(int time)
{
	Vector3 position(0.0f, 0.0f, 0.0f);
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap->Unknown_1C(time, &position, 0, 0, 0, 0, true);
	return(position);
}

Matrix3D UnknownB027A0Class::Unknown_20(int time)
{
	if (!Extrap) {
		return(Matrix3D::Identity);
	}
	Vector3 position;
	Quaternion orientation;
	Extrap->Unknown_1C(time, &position, &orientation, 0, 0, 0, true);
	return(Matrix3D(orientation, position));
}

Unknown715AB0Class::Unknown715AB0Class(SimplePositionalUpdatePacketClass *packet)
{
	Extrap = new Unknown70F250Class(packet);
}

void Unknown715AB0Class::Unknown_715C20(SimplePositionalUpdatePacketClass *packet)
{
	unsigned int start = packet->TimeStamp;
	unsigned int end = start + 2000;
	Extrap = new Unknown7125F0Class(Extrap, new Unknown70F250Class(packet), start, end);
}

Unknown715CF0Class::Unknown715CF0Class(PlanetPositionalUpdatePacketClass *packet, ClientClass *client) :
	Client(client)
{
	Extrap = new Unknown713520Class(packet, Client);
}

void Unknown715CF0Class::Unknown_715E60(PlanetPositionalUpdatePacketClass *packet)
{
	delete Extrap;
	Extrap = new Unknown713520Class(packet, Client);
}

Unknown717EF0Class::Unknown717EF0Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client)
{
	Extrap = new Unknown713C80Class(packet, client);
	TractorID = packet->TractorID;
	TractorEffectID = packet->TractorEffectID;
}

void Unknown717EF0Class::Unknown_718080(ComponentPositionalUpdatePacketClass *packet, ClientClass *client)
{
	unsigned int start = packet->TimeStamp;
	unsigned int end = start + 250;
	Extrap = new Unknown7125F0Class(Extrap, new Unknown713C80Class(packet, client), start, end);
	TractorID = packet->TractorID;
	TractorEffectID = packet->TractorEffectID;
}

/*
**	Reconstruction notes
**
**	The client's window (tools/re/appfile.py show app\client\postate.cpp): 0x714820 (the file's static
**	PermString, not rebuilt) to 0x719700; its asserts sit on the client's lines (D25): the forwarding
**	state's 'PositionState' on 53-90. Built so far: the forwarding state (vtable 0xB026EC), type 1
**	(0xB02728), the extrapolator base (0xB027A0) with types 3 (0xB027DC) and 5 (0xB02818).
*/
