// Earth & Beyond client (reconstruction): app\Client\postate.cpp. Notes at the end of the file.
#include "postate.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"
#include "..\packets\pkupdate.h"
#include "..\shared\unknown_debug.h"
#include "sclient.h"
#include "wwmath.h"
#include "rendobj.h"
#include "hermitespline.h"
#include <stdlib.h>

bool Unknown_B9489C = true;		// (0x716400 tests it)  // NAME 1: neutral (D10)
bool Unknown_B9489D = true;		// NAME 1: neutral (D10)
bool Unknown_B9489E = true;		// (the type 4 and 8 updates test it)  // NAME 1: neutral (D10)
bool Unknown_B9489F = true;		// (the update period's lag: 0x716F70)  // NAME 1: neutral (D10)
bool Unknown_B948A0 = true;		// (0x7179A0: continue from Unknown24)  // NAME 1: neutral (D10)
bool Unknown_B948A1 = true;		// (0x7179A0: 0x712DB0 rather than 0x7125F0)  // NAME 1: neutral (D10)
unsigned int Unknown_BE71D4;	// (0x7179A0's blend time; .bss, set elsewhere?)  // NAME 1: neutral (D10)
const float Unknown_B026D0 = 150.0f;	// (0x716400 squares it)  // NAME 1: neutral (D10)





























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

Matrix3D Unknown714E10Struct::Unknown_714E10(int time)
{
	Vector3 position = Unknown_714F40(time);
	Matrix3D transform = Build_Matrix3D(Orientation);
	transform.Set_Translation(position);
	return(transform);
}

Vector3 Unknown714E10Struct::Unknown_714F40(int time)
{
	float elapsed = (float)(unsigned int)(time - Time);
	if (elapsed < 1.0f) {
		return(Position);
	}
	return(Position + Velocity * elapsed);
}

Vector3 Unknown714E10Struct::Unknown_714FE0(int time)
{
	return(Velocity);
}

void Unknown714E10Struct::Unknown_715010(int time, const Matrix3D &transform)
{
	Position = transform.Get_Translation();
	Orientation = Build_Quaternion(transform);
	Time = time;
}

void Unknown714E10Struct::Unknown_715090(int time, const Vector3 &position)
{
	Position = position;
	Time = time;
}

void Unknown714E10Struct::Unknown_7150C0(int time, const Quaternion &orientation)
{
	Orientation = orientation;
}

void Unknown714E10Struct::Unknown_7150F0(int time, const Vector3 &velocity)
{
	Position = Unknown_714F40(time);
	Velocity = velocity;
	Time = time;
}

Vector3 Unknown714E10Struct::Unknown_7151D0(int time)
{
	float elapsed = (float)(unsigned int)(time - Time);
	if (elapsed < 1.0f) {
		return(Position);
	}
	return(Position + Velocity * elapsed);
}

Unknown715270Class::Unknown715270Class(ClientClass *client, int game_id, const Vector3 &offset, const Quaternion &orientation) :
	Client(client),
	GameID(game_id),
	Offset(offset),
	Orientation(orientation),
	Position(0.0f, 0.0f, 0.0f)
{
}

Unknown98Class *Unknown715270Class::Unknown_715360()
{
	ClientObjectClass *object = Client->Find_Object(GameID);
	if (!object) {
		return(0);
	}
	return(object->Unknown98);
}

Vector3 Unknown715270Class::Unknown_0C(int time)
{
	Unknown98Class *state = Unknown_715360();
	if (!state) {
		return(Position + Offset);
	}
	Position = state->Unknown_0C(time);
	return(Position + Offset);
}

Matrix3D Unknown715270Class::Unknown_08(int time)
{
	Matrix3D transform = Build_Matrix3D(Orientation);
	transform.Set_Translation(Unknown_0C(time));
	return(transform);
}

Vector3 Unknown715270Class::Unknown_10(int time)
{
	Unknown98Class *state = Unknown_715360();
	if (!state) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	return(state->Unknown_10(time));
}

Vector3 Unknown715270Class::Unknown_1C(int time)
{
	Unknown98Class *state = Unknown_715360();
	if (!state) {
		return(Position + Offset);
	}
	return(Offset + state->Unknown_1C(time));
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

Unknown715EF0Class::Unknown715EF0Class(int time, AdvancedPositionalUpdatePacketClass *packet) :
	Period(time, 0, time + packet->UpdatePeriod * 2, packet->UpdatePeriod),
	Unknown18(false)
{
	Extrap = new Unknown710110Class(packet);
}

Unknown715EF0Class::Unknown715EF0Class(SClient *client, FormationPositionalUpdatePacketClass *packet) :
	Period(0, 0, 0, 0),
	Unknown18(false)
{
	Extrap = new Unknown714340Class(client, packet);
}

void Unknown715EF0Class::Unknown_716140(int time, AdvancedPositionalUpdatePacketClass *packet)
{
	if (Unknown_B9489E) {
		Unknown_30(time, packet);
		unsigned int period = packet->UpdatePeriod;
		if (period != Period.EndValue) {
			unsigned int value = Period.Get(time);
			Period = UnknownLerpStruct(time, value, time + abs((int)(period - value)) * 2, period);
		}
	}
}

void Unknown715EF0Class::Unknown_716220(SClient *client, FormationPositionalUpdatePacketClass *packet)
{
	if (Unknown_B9489E) {
		unsigned int time = client->Unknown10BC;
		ExtrapClass *next = new Unknown714340Class(client, packet);
		unsigned int end = time + 5000;
		Extrap = new Unknown7125F0Class(Extrap, next, time, end);
		unsigned int value = Period.Get(time);
		Period = UnknownLerpStruct(time, value, time + value, 0);
	}
}

bool Unknown715EF0Class::Unknown_716390(AdvancedPositionalUpdatePacketClass *packet, ExtrapClass *extrap, int time, int other_time)
{
	if (packet->ImpartedVelocity.Length2() > 0.000001f) {
		return(false);
	}
	return(Unknown_716400(extrap, time, other_time));
}

bool Unknown715EF0Class::Unknown_716400(ExtrapClass *extrap, int time, int other_time)
{
	if (!Unknown_B9489C) {
		return(false);
	}
	Vector3 position(0.0f, 0.0f, 0.0f);
	float speed = 0.0f;
	Extrap->Unknown_1C(time, &position, 0, 0, &speed, 0, true);
	Vector3 other_position(0.0f, 0.0f, 0.0f);
	float other_speed = 0.0f;
	extrap->Unknown_1C(other_time, &other_position, 0, 0, &other_speed, 0, true);
	float distance2 = (position - other_position).Length2();
	if (distance2 < Unknown_B026D0 * Unknown_B026D0) {
		return(false);
	}
	if (other_speed * speed < 0.0f) {
		return(false);
	}
	bool moving = speed > 0.000001f;
	bool other_moving = other_speed > 0.000001f;
	if (moving != other_moving) {
		return(false);
	}
	if (!Unknown18 && distance2 < 1000000.0f) {
		return(false);
	}
	return(true);
}

bool Unknown715EF0Class::Unknown_7165D0(AdvancedPositionalUpdatePacketClass *packet, ExtrapClass *extrap, unsigned int time, unsigned int other_time)
{
	if (time < packet->TimeStamp) {
		return(false);
	}
	return(Unknown_7166A0(extrap, time, other_time));
}

bool Unknown715EF0Class::Unknown_7166A0(ExtrapClass *extrap, int time, int other_time)
{
	float speed = 0.0f;
	Extrap->Unknown_1C(time, 0, 0, 0, &speed, 0, true);
	float other_speed = 0.0f;
	extrap->Unknown_1C(other_time, 0, 0, 0, &other_speed, 0, true);
	bool moving = speed > 0.000001f;
	bool other_moving = other_speed > 0.000001f;
	if (moving == other_moving) {
		return(true);
	}
	return(false);
}

void Unknown715EF0Class::Unknown_30(int time, AdvancedPositionalUpdatePacketClass *packet)
{
	Vector3 position;
	float speed;
	Extrap->Unknown_1C(packet->TimeStamp, &position, 0, 0, &speed, 0, true);
	unsigned int duration;
	if ((position - packet->Position).Length2() > 6250000.0f) {
		duration = 0;
	} else if (!Unknown_B9489F) {
		if (!packet->Unknown_8ADB20() && WWMath::Fabs(speed) <= 0.0f) {
			duration = 500;
		} else {
			duration = 5000;
		}
	} else {
		duration = Unknown_38(time);
	}
	Unknown_34(time, packet, duration);
}

void Unknown715EF0Class::Unknown_34(int time, AdvancedPositionalUpdatePacketClass *packet, unsigned int duration)
{
	if (duration == 0) {
		delete Extrap;
		Extrap = new Unknown710110Class(packet);
		return;
	}
	ExtrapClass *next = new Unknown710110Class(packet);
	unsigned int lag = Unknown_B9489F ? Unknown_38(time) : 0;
	unsigned int start = ((unsigned int)time > packet->TimeStamp) ? time : packet->TimeStamp;
	start -= lag;
	unsigned int end = start + duration;
	if (Unknown_716390(packet, next, start, end)) {
		Extrap = new Unknown7119B0Class(Extrap, next, start, end);
	} else if (Unknown_7165D0(packet, next, start, end)) {
		Extrap = new Unknown7125F0Class(Extrap, next, start, end);
	} else {
		Extrap = new Unknown712DB0Class(Extrap, next, start, end);
	}
}

Vector3 Unknown715EF0Class::Unknown_0C(int time)
{
	time -= Unknown_38(time);
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap = Extrap->Unknown_28(time);
	return(Extrap->Unknown_04(time));
}

Vector3 Unknown715EF0Class::Unknown_10(int time)
{
	time -= Unknown_38(time);
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap = Extrap->Unknown_28(time);
	return(Extrap->Unknown_0C(time));
}

Vector3 Unknown715EF0Class::Unknown_14(int time)
{
	time -= Unknown_38(time);
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap = Extrap->Unknown_28(time);
	return(Extrap->Unknown_14(time));
}

Matrix3D Unknown715EF0Class::Unknown_08(int time)
{
	time -= Unknown_38(time);
	if (!Extrap) {
		return(Matrix3D::Identity);
	}
	Extrap = Extrap->Unknown_28(time);
	Vector3 position;
	Quaternion orientation;
	Extrap->Unknown_1C(time, &position, &orientation, 0, 0, 0, false);
	return(Matrix3D(orientation, position));
}

Vector3 Unknown715EF0Class::Unknown_1C(int time)
{
	time -= Unknown_38(time);
	Vector3 position(0.0f, 0.0f, 0.0f);
	if (!Extrap) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	Extrap->Unknown_1C(time, &position, 0, 0, 0, 0, true);
	return(position);
}

Matrix3D Unknown715EF0Class::Unknown_20(int time)
{
	time -= Unknown_38(time);
	if (!Extrap) {
		return(Matrix3D::Identity);
	}
	Vector3 position;
	Quaternion orientation;
	Extrap->Unknown_1C(time, &position, &orientation, 0, 0, 0, true);
	return(Matrix3D(orientation, position));
}

unsigned int Unknown715EF0Class::Unknown_38(int time)
{
	if (Unknown_B9489F) {
		return(Period.Get(time));
	}
	return(0);
}

Unknown717010Class::Unknown717010Class(int time, AdvancedPositionalUpdatePacketClass *packet) :
	Unknown715EF0Class(time, packet),
	MovementID(packet->MovementID),
	Unknown20(false),
	Unknown24(Extrap)
{
	Unknown18 = true;
}

Unknown717010Class::Unknown717010Class(SClient *client, FormationPositionalUpdatePacketClass *packet) :
	Unknown715EF0Class(client, packet),
	MovementID(0),
	Unknown20(false),
	Unknown24(0)
{
	Unknown18 = true;
}

void Unknown717010Class::Unknown_717290(AdvancedPositionalUpdatePacketClass *packet, int time)
{
	if (Unknown_B9489E) {
		bool moving = packet->Unknown_8ADB20();
		Vector3 position;
		float speed;
		Extrap->Unknown_1C(packet->TimeStamp, &position, 0, 0, &speed, 0, true);
		float distance2 = (position - packet->Position).Length2();
		if (distance2 > 6250000.0f) {
			MovementID = packet->MovementID;
			Unknown20 = false;
			Unknown_34(time, packet, 0);
			return;
		}
		bool settled = speed < 0.0001f && !moving && distance2 < 10.0f;
		if (Unknown20) {
			if (packet->MovementID < MovementID) {
				return;
			}
			if (packet->MovementID == MovementID && settled) {
				return;
			}
		}
		MovementID = packet->MovementID;
		Unknown20 = false;
		Unknown_34(time, packet, moving ? 2000 : 500);
	}
}

void Unknown717010Class::Unknown_717400(FormationPositionalUpdatePacketClass *packet, SClient *client)
{
	Unknown_716220(client, packet);
	Unknown24 = 0;
}
















































































































































































































































































































































// NOT EXACT: inline-budget (the client calls the float field's Cast 0x546970 out of line in the first lookups, ours inlines all; 286 vs 314 instructions; one form; attempts.py show 0x717430)
void Unknown717010Class::Unknown_717430(int unknown, DeltaCompoundClass *aux_data, int time, float *turn, float *tilt, int *move_direction, bool fixed_speed)
{
	int count = 0;
	float turn_rate_value = 0.0f;
	float *turn_rate = 0;
	float tilt_rate_value = 0.0f;
	float *tilt_rate = 0;
	float speed_value = 0.0f;
	float *speed = 0;
	float acceleration_value = 0.0f;
	float *acceleration = 0;
	if (turn) {
		NameClass name("MaxTurnRate");
		DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux_data, name);
		if (t && t->m_Unknown70) {
			turn_rate_value = -(t->Value * *turn) * 0.001f;
			turn_rate = &turn_rate_value;
			count = 1;
		}
	}
	if (tilt) {
		NameClass name("MaxTiltRate");
		DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux_data, name);
		if (t && t->m_Unknown70) {
			tilt_rate_value = -(t->Value * *tilt) * 0.001f;
			tilt_rate = &tilt_rate_value;
			count++;
		}
	}
	if (move_direction) {
		bool known = true;
		if (*move_direction == 1) {
			if (!fixed_speed) {
				NameClass name("MaxSpeed");
				DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux_data, name);
				if (t && t->m_Unknown70) {
					speed_value = t->Value * 0.001f;
				} else {
					known = false;
				}
			} else {
				speed_value = 10.0f;
			}
		} else if (*move_direction == -1) {
			if (!fixed_speed) {
				NameClass name("MinSpeed");
				DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux_data, name);
				if (t && t->m_Unknown70) {
					speed_value = t->Value * 0.001f;
				} else {
					known = false;
				}
			} else {
				speed_value = -10.0f;
			}
		} else {
			G_ASSERT(*move_direction == 0);
			speed_value = 0.0f;
		}
		if (known) {
			speed = &speed_value;
			NameClass name("Acceleration");
			DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >(aux_data, name);
			if (t && t->m_Unknown70) {
				acceleration_value = t->Value * 0.000001f;
				acceleration = &acceleration_value;
			}
			if (acceleration) {
				count++;
			}
		}
	}
	if (count == 0) {
		return;
	}
	if (Unknown_B948A0 && Unknown24) {
		ExtrapClass *next = Unknown24->Unknown_710260();
		next->Unknown_20(time + (Unknown_BE71D4 >> 1), speed, acceleration, turn_rate, tilt_rate);
		if (Unknown_B948A1) {
			Extrap = new Unknown712DB0Class(Extrap, next, time, time + Unknown_BE71D4);
		} else {
			Extrap = new Unknown7125F0Class(Extrap, next, time, time + Unknown_BE71D4);
		}
		Unknown24 = next;
	} else {
		Extrap->Unknown_20(time, speed, acceleration, turn_rate, tilt_rate);
	}
	Unknown20 = true;
	MovementID += count;
}

// NOT EXACT: inline-budget (the client calls the bool field's Cast 0x584FF0 out of line for 'AutoLevel', ours inlines it; register-only after; one form; attempts.py show 0x7179A0)
void Unknown717010Class::Unknown_7179A0(int unknown, DeltaCompoundClass *aux_data, int time, bool level, bool auto_level)
{
	float zero = 0.0f;
	float tilt_rate = 0.0f;
	if (auto_level) {
		NameClass name("AutoLevel");
		DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >(aux_data, name);
		if (t && t->m_Unknown70 && t->Value) {
			NameClass rate_name("MaxTiltRate");
			DeltaTemplateClass<float> *r = Unknown_Find_Field<DeltaTemplateClass<float> >(aux_data, rate_name);
			if (r && r->m_Unknown70) {
				tilt_rate = r->Value * 0.001f;
			}
		}
	}
	if (Unknown_B948A0 && Unknown24) {
		ExtrapClass *next = Unknown24->Unknown_710260();
		if (level) {
			next->Unknown_20(time + (Unknown_BE71D4 >> 1), 0, 0, &zero, 0);
		}
		if (auto_level) {
			next->Unknown_24(time + (Unknown_BE71D4 >> 1), tilt_rate);
		}
		if (Unknown_B948A1) {
			Extrap = new Unknown712DB0Class(Extrap, next, time, time + Unknown_BE71D4);
		} else {
			Extrap = new Unknown7125F0Class(Extrap, next, time, time + Unknown_BE71D4);
		}
		Unknown24 = next;
	} else {
		if (level) {
			Extrap->Unknown_20(time, 0, 0, &zero, 0);
		}
		if (auto_level) {
			Extrap->Unknown_24(time, tilt_rate);
		}
	}
	if (level) {
		MovementID++;
	}
	if (auto_level) {
		MovementID++;
	}
	Unknown20 = true;
}

void Unknown717010Class::Unknown_34(int time, AdvancedPositionalUpdatePacketClass *packet, unsigned int duration)
{
	if (duration == 0) {
		delete Extrap;
		Unknown24 = new Unknown710110Class(packet);
		Extrap = Unknown24;
		return;
	}
	unsigned int start = ((unsigned int)time > packet->TimeStamp) ? time : packet->TimeStamp;
	unsigned int end = start + duration;
	ExtrapClass *next = new Unknown710110Class(packet);
	Unknown24 = next;
	if (Unknown_7165D0(packet, next, start, end)) {
		Extrap = new Unknown7125F0Class(Extrap, next, start, end);
	} else {
		Extrap = new Unknown712DB0Class(Extrap, next, start, end);
	}
}

unsigned int Unknown717010Class::Unknown_38(int time)
{
	return(0);
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

Unknown718160Class::Unknown718160Class(Vector3 start, Vector3 end, float speed, int time, Unknown98Class *state) :
	Start(start),
	End(end),
	Speed(speed),
	StartTime(time),
	State(state)
{
	Direction = End - Start;
	Direction.Normalize();
}

int Unknown718160Class::Unknown_04()
{
	return(12);
}

Matrix3D Unknown718160Class::Unknown_08(int time)
{
	Matrix3D transform;
	transform.Obj_Look_At(Unknown_0C(time), End, 0.0f);
	return(transform);
}

Vector3 Unknown718160Class::Unknown_0C(int time)
{
	return(Start + (float)(unsigned int)(time - StartTime) * Unknown_10(time));
}

Vector3 Unknown718160Class::Unknown_10(int time)
{
	return(Speed * Direction);
}

Vector3 Unknown718160Class::Unknown_14(int time)
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}

Unknown718510Class::Unknown718510Class(int start_time, int end_time, Vector3 start_up, Vector3 end_up, Unknown98Class *state) :
	StartTime(start_time),
	EndTime(end_time),
	StartUp(start_up),
	EndUp(end_up),
	State(state)
{
	Spline = new HermiteSpline3DClass;
	unsigned int duration = EndTime - StartTime;
	if (!duration) {
		duration = 1;
	}
	Rate = 1.0f / (float)duration;
}

Unknown718510Class::~Unknown718510Class()
{
	delete Spline;
}

int Unknown718510Class::Unknown_04()
{
	return(13);
}

// NOT EXACT: shape (17 rows: the client's first Normalize and the up-vector cross product load their operands in another order; five forms; attempts.py show 0x718830)
Matrix3D Unknown718510Class::Unknown_08(int time)
{
	Vector3 position = Unknown_0C(time);
	Vector3 x = Unknown_10(time);
	x.Normalize();
	float t = (float)(unsigned int)(time - StartTime) * Rate;
	if (t > 1.0f) {
		t = 1.0f;
	}
	Quaternion up(StartUp.X, StartUp.Y, StartUp.Z, 1.0f);
	Quaternion end_up(EndUp.X, EndUp.Y, EndUp.Z, 1.0f);
	Slerp(up, up, end_up, t);
	Vector3 y = Vector3::Cross_Product((const Vector3 &)up, x);
	y.Normalize();
	Vector3 z = Vector3::Cross_Product(x, y);
	z.Normalize();
	Matrix3D transform;
	transform.Set(x, y, z, position);
	return(transform);
}

Vector3 Unknown718510Class::Unknown_0C(int time)
{
	Vector3 position;
	Spline->Evaluate((float)(unsigned int)(time - StartTime) * Rate, &position);
	return(position);
}

Vector3 Unknown718510Class::Unknown_10(int time)
{
	Vector3 velocity;
	Spline->Evaluate_Derivative((float)(unsigned int)(time - StartTime) * Rate, &velocity);
	return(velocity);
}

Vector3 Unknown718510Class::Unknown_14(int time)
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}

Unknown718D70Class::Unknown718D70Class(int time, const Matrix3D &transform, Unknown98Class *state) :
	State(state),
	Time(time),
	LastTime(time),
	Transform(transform),
	LastTransform(transform)
{
}

int Unknown718D70Class::Unknown_04()
{
	return(14);
}

Matrix3D Unknown718D70Class::Unknown_08(int time)
{
	return(Transform);
}

Vector3 Unknown718D70Class::Unknown_0C(int time)
{
	return(Transform.Get_Translation());
}

Vector3 Unknown718D70Class::Unknown_10(int time)
{
	Vector3 position = Transform.Get_Translation();
	Vector3 last_position = LastTransform.Get_Translation();
	Vector3 velocity(0.0f, 0.0f, 0.0f);
	unsigned int elapsed = Time - LastTime;
	if (elapsed) {
		float scale = 1.0f / (float)elapsed;
		velocity = (position - last_position) * scale;
	}
	return(velocity);
}

Vector3 Unknown718D70Class::Unknown_14(int time)
{
	Vector3 spin;
	spin.X = Transform.Get_X_Rotation() - LastTransform.Get_X_Rotation();
	spin.Y = Transform.Get_Y_Rotation() - LastTransform.Get_Y_Rotation();
	spin.Z = Transform.Get_Z_Rotation() - LastTransform.Get_Z_Rotation();
	unsigned int elapsed = Time - LastTime;
	if (elapsed) {
		float scale = 1.0f / (float)elapsed;
		spin *= scale;
	}
	return(spin);
}

void Unknown718D70Class::Unknown_30(int time, const Matrix3D &transform)
{
	LastTime = Time;
	LastTransform = Transform;
	Time = time;
	Transform = transform;
}

// (blank lines: type 15 below sits on the client's lines 1422-1457, D25; the functions not rebuilt yet are
// listed in the notes at the end)























































































































































Unknown7192A0Class::Unknown7192A0Class(RenderObjClass *render_object, Unknown98Class *state, const char *bone_name) :
	RenderObject(render_object),
	BoneIndex(-1)
{
	delete state;
	if (bone_name) {
		G_ASSERT(RenderObject);

		BoneIndex = RenderObject->Get_Bone_Index(bone_name);

		G_ASSERT(BoneIndex > 0);
	}
}

int Unknown7192A0Class::Unknown_04()
{
	return(15);
}




Matrix3D Unknown7192A0Class::Unknown_08(int time)
{

	G_ASSERT(RenderObject);
	if (BoneIndex > 0) {
		if (RenderObject->Get_Container()) {
			RenderObject->Get_Container()->Update_Sub_Object_Transforms();
		}
		return(RenderObject->Get_Bone_Transform(BoneIndex));
	}
	return(RenderObject->Get_Transform());
}




Vector3 Unknown7192A0Class::Unknown_0C(int time)
{

	G_ASSERT(RenderObject);
	if (BoneIndex > 0) {
		return(RenderObject->Get_Bone_Transform(BoneIndex).Get_Translation());
	}
	return(RenderObject->Get_Transform().Get_Translation());
}

Vector3 Unknown7192A0Class::Unknown_10(int time)
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}

Vector3 Unknown7192A0Class::Unknown_14(int time)
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}

/*
**	Reconstruction notes
**
**	The client's window (tools/re/appfile.py show app\client\postate.cpp): 0x714820 (the file's static
**	PermString) to 0x719700; its asserts sit on the client's lines (D25): the forwarding state's
**	'PositionState' on 53-90, type 4's '*move_direction == 0' on 977, type 15's on 1422-1457.
**	Built: the forwarding state (vtable 0xB026EC), type 1 (0xB02728), the record Unknown714E10Struct
**	(0x714E10-0x7151D0, no callers), type 9 (0xB02764), the extrapolator base (0xB027A0) with types 3
**	(0xB027DC), 5 (0xB02818), 8 (0xB02854), 4 (0xB028A4; its input steps 0x717430 and 0x7179A0 NOT EXACT)
**	and 11 (0xB028F0), type 12 (0xB0292C), type 13 (0xB029C0, a HermiteSpline3DClass path; its transform
**	0x718830 NOT EXACT), type 14 (0xB02A00), type 15 (0xB02A40). Not rebuilt: the statics 0x714820 and
**	after 0x719620, 0x7196C0. extrap.cpp's classes are declared in postate.h only.
*/
