// Earth & Beyond client (reconstruction): app\Client\extrap.cpp. Notes at the end of the file.
#include "extrap.h"
#include "..\packets\pkupdate.h"
#include "..\shared\unknown_debug.h"
#include "client.h"
#include "co.h"
#include "sclient.h"
#include <math.h>

void Unknown_Debug_Printf(const char *format, ...);	// 0x509DC0 (dbgmgr.cpp, D38)
void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line



Vector3 StateNode::Unknown_04(int time)
{
	Vector3 position;
	Unknown_1C(time, &position, 0, 0, 0, 0, false);
	return(position);
}


Quaternion StateNode::Unknown_08(int time)
{
	Quaternion orientation;
	Unknown_1C(time, 0, &orientation, 0, 0, 0, false);
	return(orientation);
}


Vector3 StateNode::Unknown_0C(int time)
{
	Vector3 velocity;
	Unknown_1C(time, 0, 0, &velocity, 0, 0, false);
	return(velocity);
}


float StateNode::Unknown_10(int time)
{
	float speed;
	Unknown_1C(time, 0, 0, 0, &speed, 0, false);
	return(speed);
}


Vector3 StateNode::Unknown_14(int time)
{
	Vector3 rotrate;
	Unknown_1C(time, 0, 0, 0, 0, &rotrate, false);
	return(rotrate);
}























































































































































































void StateNode::Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate)
{
	G_ASSERT(0);
	Unknown_Debug_Print("Bad function call! StateNode::Set_Motion()");
}



void StateNode::End_Tilt(int time, float rate)
{
	G_ASSERT(0);
	Unknown_Debug_Printf("Bad function call! StateNode::End_Tilt()");
}


Unknown70F250Class::Unknown70F250Class(SimplePositionalUpdatePacketClass *packet) :
	Time(packet->TimeStamp),
	Position(packet->Position),
	Orientation(packet->Orientation),
	Velocity(packet->Velocity)
{
}


void Unknown70F250Class::Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown)
{
	if (position) {
		*position = Position + Velocity * (float)(time - Time);
	}
	if (orientation) {
		*orientation = Orientation;
	}
	if (velocity) {
		*velocity = Velocity;
	}
	if (speed) {
		*speed = Velocity.Length();
	}
	if (rotrate) {
		rotrate->Set(0.0f, 0.0f, 0.0f);
	}
}


Unknown70F4F0Class::Unknown70F4F0Class() :
	Current(0),
	Unknown08(0),
	Unknown0C(0),
	Unknown10(0)
{
}


Unknown70F4F0Class::~Unknown70F4F0Class()
{
	if (Current) {
		delete Current;
		Current = 0;
	}
	if (Unknown08) {
		delete Unknown08;
		Unknown08 = 0;
	}
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = 0;
	}
	if (Unknown10) {
		delete Unknown10;
		Unknown10 = 0;
	}
}


Vector3 Unknown70F4F0Class::Unknown_18(int time)
{
	Unknown710030Class *motion = Unknown08;
	if (motion == 0) {
		motion = Current;
	}
	if (motion == 0) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	return(motion->Unknown_20());
}


Unknown710030Class::Unknown710030Class()
{
}


Unknown710030Class::Unknown710030Class(const Unknown710030Class &other)
{
	Time = other.Time;
}


void Unknown710030Class::Unknown_04(unsigned int time)
{
	while (Time < time) {
		Unknown_08();
	}
}






























































































































































































































































































































































































































Unknown710110Class::Unknown710110Class(AdvancedPositionalUpdatePacketClass *packet)
{
	Current = new Unknown710530Class(packet);
	G_ASSERT(Current);
}


Unknown710110Class::Unknown710110Class()
{
}


StateNode *Unknown710110Class::Unknown_710260()
{
	Unknown710110Class *node = new Unknown710110Class;
	node->Current = Current->Unknown_0C();
	return(node);
}


// NOT EXACT: 0x7102B0 shape: the client keeps Unknown3C on the stack past the flag store (fst) and negates the
// acceleration before the multiply (fchs, both branches' fmul merged); here the flag store comes first and -a * 10000 folds
void Unknown710110Class::Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate)
{
	Unknown710530Class *motion = (Unknown710530Class *)Current;
	motion->Unknown_04(time - (time - motion->Time) % 100);
	if (speed && acceleration) {
		motion->Unknown3C = *speed * 100.0f;
		float diff = motion->Unknown3C - motion->Speed;
		motion->Unknown40 = true;
		if (fabs(diff) < 0.0001f) {
			motion->Unknown44 = 0.0f;
			motion->Unknown40 = false;
		} else if (diff > 0.0f) {
			motion->Unknown44 = *acceleration * 10000.0f;
		} else if (diff < 0.0f) {
			motion->Unknown44 = -*acceleration * 10000.0f;
		}
	}
	if (turn_rate) {
		motion->Unknown50 = *turn_rate * 100.0f;
		motion->Unknown54 = (motion->Unknown50 >= 0.0f) ? 1000.0f : -1000.0f;
		motion->Unknown48 = true;
	}
	if (tilt_rate) {
		motion->Unknown60 = *tilt_rate * 100.0f;
		motion->Unknown64 = (motion->Unknown60 >= 0.0f) ? 1.2217305f : -1.2217305f;
		motion->Unknown58 = true;
	}
}


void Unknown710110Class::End_Tilt(int time, float rate)
{
	Unknown710530Class *motion = (Unknown710530Class *)Current;
	motion->Unknown_04(time - (time - motion->Time) % 100);
	if (motion->Unknown5C >= 0.0f) {
		motion->Unknown60 = -rate;
	} else {
		motion->Unknown60 = rate;
	}
	motion->Unknown60 *= 100.0f;
	motion->Unknown58 = true;
	motion->Unknown64 = 0.0f;
}


bool Unknown710110Class::Unknown_7104A0()
{
	return(((Unknown710530Class *)Current)->Unknown_711490());
}


bool Unknown710530Class::Unknown_711490()
{
	if (Unknown6C.Length2() <= 0.000001f && fabs(Unknown44) <= 0.000001f && fabs(Speed) >= 0.000001f && fabs(Unknown3C) >= 0.000001f) {
		return(true);
	}
	return(false);
}


Matrix3D *Unknown710530Class::Unknown_10()
{
	if (!Transform.Is_Orthogonal()) {
		Transform.Re_Orthogonalize();
	}
	return(&Transform);
}


float Unknown710530Class::Unknown_18()
{
	return(Speed);
}


Vector3 Unknown710530Class::Unknown_1C()
{
	return(Vector3(0.0f, Unknown60, Unknown50));
}


Vector3 Unknown710530Class::Unknown_20()
{
	return(Vector3(0.0f, Unknown64, Unknown54));
}


StateNode *Unknown7119B0Class::Unknown_28(int time)
{
	if (time >= EndTime) {
		StateNode *end = EndState;
		EndState = 0;
		delete this;
		return(end);
	}
	StartState = StartState->Unknown_28(time);
	return(this);
}


Unknown7119B0Class::~Unknown7119B0Class()
{
	if (StartState) {
		delete StartState;
		StartState = 0;
	}
	if (EndState) {
		delete EndState;
		EndState = 0;
	}
}


Vector3 Unknown7119B0Class::Unknown_18(int time)
{
	StateNode *state = EndState;
	if (state == 0) {
		state = StartState;
	}
	if (state == 0) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	return(state->Unknown_18(time));
}


void Unknown7119B0Class::Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate)
{
	if (StartState) {
		StartState->Set_Motion(time, speed, acceleration, turn_rate, tilt_rate);
	}
	if (EndState) {
		EndState->Set_Motion(time, speed, acceleration, turn_rate, tilt_rate);
	}
	Unknown_712480(time);
}


































































































































































































































































































































































































































































































































Unknown7125F0Class::Unknown7125F0Class(StateNode *start, StateNode *end, unsigned int start_time, unsigned int end_time) :
	StartState(start),
	EndState(end),
	StartTime(start_time),
	EndTime(end_time)
{
	G_ASSERT(StartState && EndState);
	G_ASSERT(EndTime >= StartTime);
}


Unknown7125F0Class::~Unknown7125F0Class()
{
	if (StartState) {
		delete StartState;
		StartState = 0;
	}
	if (EndState) {
		delete EndState;
		EndState = 0;
	}
}


Vector3 Unknown7125F0Class::Unknown_18(int time)
{
	StateNode *state = EndState;
	if (state == 0) {
		state = StartState;
	}
	if (state == 0) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	return(state->Unknown_18(time));
}






























































void Unknown7125F0Class::Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate)
{
	G_ASSERT(StartState && EndState);
	StartState->Set_Motion(time, speed, acceleration, turn_rate, tilt_rate);
	EndState->Set_Motion(time, speed, acceleration, turn_rate, tilt_rate);
}



void Unknown7125F0Class::End_Tilt(int time, float rate)
{
	G_ASSERT(StartState && EndState);
	StartState->End_Tilt(time, rate);
	EndState->End_Tilt(time, rate);
}


StateNode *Unknown7125F0Class::Unknown_28(int time)
{
	if (time >= EndTime) {
		StateNode *end = EndState;
		EndState = 0;
		delete this;
		return(end);
	}
	StartState = StartState->Unknown_28(time);
	return(this);
}


Unknown712DB0Class::Unknown712DB0Class(StateNode *start, StateNode *end, unsigned int start_time, unsigned int end_time) :
	Unknown7125F0Class(start, end, start_time, end_time)
{
	Unknown_7132F0(start_time, end_time);
}


Vector3 Unknown712DB0Class::Unknown_18(int time)
{
	StateNode *state = EndState;
	if (state == 0) {
		state = StartState;
	}
	if (state == 0) {
		return(Vector3(0.0f, 0.0f, 0.0f));
	}
	return(state->Unknown_18(time));
}


void Unknown712DB0Class::Unknown_7132F0(unsigned int start_time, unsigned int end_time)
{
	Unknown_1C(start_time, &Start.Position, &Start.Orientation, &Start.Velocity, &Start.Speed, &Start.RotRate, true);
	Unknown_1C(end_time, &End.Position, &End.Orientation, &End.Velocity, &End.Speed, &End.RotRate, true);
	StartTime = start_time;
	EndTime = end_time;
}


void Unknown712DB0Class::Set_Motion(int time, float *speed, float *acceleration, float *turn_rate, float *tilt_rate)
{
	Unknown7125F0Class::Set_Motion(time, speed, acceleration, turn_rate, tilt_rate);
	Unknown_7132F0(time, EndTime);
}


void Unknown712DB0Class::End_Tilt(int time, float rate)
{
	Unknown7125F0Class::End_Tilt(time, rate);
	Unknown_7132F0(time, EndTime);
}


Unknown713520Class::Unknown713520Class(PlanetPositionalUpdatePacketClass *packet, ClientClass *client) :
	Time(packet->TimeStamp),
	Client(client),
	Unknown0C(*(Vector3 *)packet->Unknown14),
	OrbitID(packet->OrbitID),
	OrbitDist(packet->OrbitDist),
	OrbitAngle(packet->OrbitAngle),
	OrbitRate(packet->OrbitRate),
	RotateAngle(packet->RotateAngle),
	RotateRate(packet->RotateRate),
	TiltAngle(packet->TiltAngle)
{
}


bool Unknown713520Class::Unknown_7135F0(int time, int unknown, Vector3 *result)
{
	if (OrbitID == 0) {
		return(false);
	}
	ClientObjectClass *object = Client->Find_Object(OrbitID);
	if (object == 0) {
		return(false);
	}
	*result = object->Unknown98->Unknown_1C(time);
	return(true);
}












































































































































































Unknown713B00Class::Unknown713B00Class(StateNode *state, int offset) :
	State(state),
	Offset(offset)
{
	G_ASSERT(State);
}


Unknown713B00Class::~Unknown713B00Class()
{
	delete State;
}


void Unknown713B00Class::Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown)
{
	State->Unknown_1C(time - Offset, position, orientation, velocity, speed, rotrate, unknown);
}


Unknown713C80Class::Unknown713C80Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client)
{
	Current = new Unknown713D70Class(packet, client);
}


// NOT EXACT: 0x713D70 scheduling: the epilogue's load of the saved EH link comes after `pop edi` in the client, before the last stores here
Unknown713D70Class::Unknown713D70Class(ComponentPositionalUpdatePacketClass *packet, ClientClass *client)
{
	Time = packet->TimeStamp;
	Transform = Build_Matrix3D(packet->Orientation);
	Transform.Set_Translation(packet->Position);
	TractorSpeed = packet->TractorSpeed * 0.1f;
	Velocity = packet->Velocity * 0.1f;
	ImpartedDecay = packet->ImpartedDecay * 0.1f;
	TractorID = packet->TractorID;
	Client = client;
}


Vector3 Unknown713D70Class::Unknown_1C()
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}


void Unknown713D70Class::Unknown_08()
{
	Transform.Adjust_Translation(Unknown_14());
	Velocity *= ImpartedDecay;
	Time += 100;
}


Matrix3D *Unknown713D70Class::Unknown_10()
{
	return(&Transform);
}


float Unknown713D70Class::Unknown_18()
{
	return(Unknown_14().Length());
}


Vector3 Unknown713D70Class::Unknown_14()
{
	Vector3 tractor;
	if (Unknown_7140E0(&tractor)) {
		return(tractor + Velocity);
	}
	return(Velocity);
}


Unknown714340Class::Unknown714340Class(SClient *client, FormationPositionalUpdatePacketClass *packet) :
	Client(client),
	LeaderID(packet->LeaderID),
	Offset(packet->Offset),
	Unknown18(true)
{
}

/*
**	Reconstruction notes (EX1, 2026-10-28)
**
**	client.exe 0x70F040-0x714810: the state nodes postate.cpp's positional states keep. The object ends with
**	<string>'s guarded static (0x7147F0, std::ctype<unsigned short>::id); postate.cpp starts at 0x714820.
**	The asserts sit on the client's lines (D25): 238 and 246 (the base's Set_Motion / End_Tilt), 757
**	(0x710110), 1432-1433 (0x7125F0, inlined in 0x712DB0), 1525 / 1534 (0x712C60 / 0x712CF0, inlined in
**	0x713370 / 0x713450), 1799 (0x713B00). Not written yet: the motion objects' steps and copies (0x710530,
**	0x710A20, 0x711050, 0x7115F0, 0x7117D0, 0x713F30, 0x714010, 0x7140E0), the composite's 0x70F630, the blends'
**	0x7119B0, 0x711D60, 0x712170, 0x712480, 0x712780, 0x712FA0, the orbit 0x713670 and the formation's 0x714410.
*/
