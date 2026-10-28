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


// NOT EXACT: 0x710530 shape: the speed block as Set_Motion 0x7102B0's (the flag store before the fst, the negation folded)
Unknown710530Class::Unknown710530Class(AdvancedPositionalUpdatePacketClass *packet)
{
	Time = packet->TimeStamp;
	Transform = Build_Matrix3D(packet->Orientation);
	Transform.Set_Translation(packet->Position);
	Speed = packet->CurrentSpeed * 100.0f;
	Unknown3C = packet->SetSpeed * 100.0f;
	float diff = Unknown3C - Speed;
	Unknown40 = true;
	if (fabs(diff) < 0.0001f) {
		Unknown44 = 0.0f;
		Unknown40 = false;
	} else if (diff > 0.0f) {
		Unknown44 = packet->Acceleration * 10000.0f;
	} else if (diff < 0.0f) {
		Unknown44 = -packet->Acceleration * 10000.0f;
	}
	Unknown6C = packet->ImpartedVelocity * 100.0f;
	if (WWMath::Fabs(Unknown6C.X) >= 0.000001f || WWMath::Fabs(Unknown6C.Y) >= 0.000001f || WWMath::Fabs(Unknown6C.Z) >= 0.000001f) {
		Unknown78 = (float)pow(0.5, 0.1);
		Unknown68 = true;
	} else {
		Unknown78 = 0.0f;
		Unknown68 = false;
	}
	Unknown80 = packet->ImpartedSpin * 100.0f;
	if (fabs(Unknown80) > 0.000001f) {
		Unknown7C = true;
		Unknown84 = Unknown78;
	} else {
		Unknown84 = 0.0f;
		Unknown7C = false;
	}
	Unknown8C = packet->ImpartedRoll * 100.0f;
	if (fabs(Unknown8C) > 0.000001f) {
		Unknown88 = true;
		Unknown90 = Unknown78 * Unknown78;
	} else {
		Unknown90 = 0.0f;
		Unknown88 = false;
	}
	Unknown98 = packet->ImpartedPitch * 100.0f;
	if (fabs(Unknown98) > 0.000001f) {
		Unknown94 = true;
		Unknown9C = Unknown78 * Unknown78;
	} else {
		Unknown9C = 0.0f;
		Unknown94 = false;
	}
	Unknown5C = (float)acos(Transform[2][2]);
	if (Transform[2][0] > 0.0f) {
		Unknown5C *= -1.0f;
	}
	Unknown60 = packet->RotY * 100.0f;
	Unknown58 = (fabs(Unknown60) > 0.000001f);
	Unknown64 = packet->DesiredY;
	if (fabs(Unknown64) > WWMATH_PI) {
		Unknown64 = (Unknown60 >= 0.0f) ? WWMATH_PI : -WWMATH_PI;
	}
	Unknown50 = packet->RotZ * 100.0f;
	Unknown4C = Transform.Get_Z_Rotation();
	Unknown48 = (fabs(Unknown50) > 0.000001f);
	Unknown54 = packet->DesiredZ;
	if (fabs(Unknown54) > 3.0f * WWMATH_PI && Unknown50 < 0.0f) {
		Unknown54 = -fabs(Unknown54);
	}
}


// NOT EXACT: 0x710A20 order: the turn's spill to the stack and the operand order of three products differ (the client stores the turn once and reloads it)
void Unknown710530Class::Unknown_08()
{
	Transform.Translate_X(Speed);
	if (Unknown40) {
		Speed += Unknown44;
		if (Unknown44 > 0.0f) {
			Speed = WWMath::Min(Speed, Unknown3C);
		} else {
			Speed = WWMath::Max(Speed, Unknown3C);
		}
	}
	if (Unknown68) {
		Unknown6C *= Unknown78;
		if (WWMath::Fabs(Unknown6C.X) >= 0.000001f || WWMath::Fabs(Unknown6C.Y) >= 0.000001f || WWMath::Fabs(Unknown6C.Z) >= 0.000001f) {
			Transform.Adjust_Translation(Unknown6C);
		} else {
			Unknown68 = false;
		}
	}
	if (Unknown48) {
		float diff = Unknown54 - Unknown4C;
		if (fabs(diff) < fabs(Unknown50)) {
			Unknown50 = diff;
		}
		if (fabs(Unknown50) < 0.000001f) {
			Unknown48 = false;
		}
	}
	if (Unknown7C) {
		Unknown80 *= Unknown84;
		if (fabs(Unknown80) < 0.000001f) {
			Unknown7C = false;
		}
	}
	if (Unknown48 || Unknown7C) {
		Transform.In_Place_Pre_Rotate_Z(Unknown80 + Unknown50);
		Unknown4C += Unknown80 + Unknown50;
	}
	if (Unknown88) {
		Unknown8C = Unknown90 * Unknown8C - Transform[2][1] / 2048.0f;
		Transform.Rotate_X(Unknown8C);
		if (fabs(Unknown8C) < 0.000001f) {
			Unknown88 = false;
		}
	}
	if (Unknown94) {
		Unknown98 *= Unknown9C;
		Transform.Rotate_Y(Unknown98);
		if (fabs(Unknown98) < 0.000001f) {
			Unknown94 = false;
		}
	}
	if (Unknown58) {
		float diff = Unknown64 - Unknown5C;
		if (diff * Unknown60 < 0.0f || fabs(diff) < 0.000001f) {
			Unknown60 = 0.0f;
		} else if (fabs(diff) < fabs(Unknown60)) {
			Unknown60 = diff;
		}
		if (fabs(Unknown5C) > 1.2217305f) {
			if (Unknown5C * Unknown60 > 0.0f) {
				Unknown60 = 0.0f;
			}
		} else {
			Unknown60 = WWMath::Clamp(Unknown60, -1.2217305f - Unknown5C, 1.2217305f - Unknown5C);
		}
		if (fabs(Unknown60) > 0.000001f) {
			Transform.Rotate_Y(Unknown60);
			Unknown5C += Unknown60;
		} else {
			Unknown58 = false;
		}
	}
	Time += 100;
}


void Unknown710530Class::Unknown_711050()
{
	Time -= 100;
	if (Unknown58) {
		Transform.Rotate_Y(-Unknown60);
		Unknown5C -= Unknown60;
	}
	if (Unknown8C != 0.0f) {
		Transform.Rotate_X(-Unknown8C);
		Unknown8C = (Transform[2][1] * (1.0f / 2048.0f) + Unknown8C) / Unknown90;
	}
	if (Unknown98 != 0.0f) {
		Transform.Rotate_Y(-Unknown98);
		Unknown98 = (Unknown98 - Transform[2][0] * (1.0f / 2048.0f)) / Unknown9C;
	}
	if (Unknown48 || Unknown7C) {
		float turn = Unknown80;
		turn += Unknown50;
		Transform.In_Place_Pre_Rotate_Z(-turn);
		Unknown4C -= turn;
		if (Unknown7C) {
			Unknown80 /= Unknown84;
		}
	}
	if (Unknown68) {
		Transform.Adjust_Translation(-Unknown6C);
		Unknown6C /= Unknown78;
	}
	if (Unknown40) {
		Speed -= Unknown44;
	}
	Transform.Translate_X(-Speed);
}


bool Unknown710530Class::Unknown_711490()
{
	if (Unknown6C.Length2() <= 0.000001f && fabs(Unknown44) <= 0.000001f && fabs(Speed) >= 0.000001f && fabs(Unknown3C) >= 0.000001f) {
		return(true);
	}
	return(false);
}


// NOT EXACT: 0x711520 shape: the client keeps Time in ecx and tests it again before stepping back (a top-tested loop); here the else branch knows Time > time
void Unknown710530Class::Unknown_04(unsigned int time)
{
	if (Time <= time) {
		while (Time < time) {
			Unknown_08();
		}
	} else if (Unknown_711490()) {
		while (Time > time) {
			Unknown_711050();
		}
	}
}


Unknown710030Class *Unknown710530Class::Unknown_0C()
{
	return(new Unknown710530Class(*this));
}


Matrix3D *Unknown710530Class::Unknown_10()
{
	if (!Transform.Is_Orthogonal()) {
		Transform.Re_Orthogonalize();
	}
	return(&Transform);
}


Vector3 Unknown710530Class::Unknown_14()
{
	Vector3 forward(Speed, 0.0f, 0.0f);
	return(Build_Quaternion(Transform).Rotate_Vector(forward) + Unknown6C);
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
























































































































Unknown7119B0Class::Unknown7119B0Class(StateNode *start, StateNode *end, unsigned int start_time, unsigned int end_time) :
	StartState(start),
	StartTime(start_time),
	EndState(end),
	EndTime(end_time)
{
	G_ASSERT(StartState && EndState);
	G_ASSERT(EndTime >= StartTime);
	Quaternion orientation;
	float start_speed;
	StartState->Unknown_1C(start_time, &Unknown08, &orientation, &Unknown14, &start_speed, 0, true);
	float duration = (float)(EndTime - StartTime);
	Unknown14 *= duration;
	Unknown44 = Build_Matrix3D(orientation);
	Unknown44.Set_Translation(Unknown08);
	float end_speed;
	EndState->Unknown_1C(end_time, &Unknown28, 0, &Unknown34, &end_speed, 0, true);





	G_ASSERT(end_speed * start_speed >= 0.0f);
	if (end_speed < 0.0f) {
		Unknown74 = true;
	} else {
		Unknown74 = false;
	}
	Unknown28 -= Unknown08;
	Unknown34 *= duration;
	Unknown75 = false;
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

























void Unknown7119B0Class::Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown)
{
	unsigned int when = Unknown75 ? StartTime : time;
	if (when <= StartTime) {
		G_ASSERT(StartState);
		StartState->Unknown_1C(when, position, orientation, velocity, speed, rotrate, unknown);
		return;
	}


	if (when >= EndTime) {
		G_ASSERT(EndState);
		EndState->Unknown_1C(when, position, orientation, velocity, speed, rotrate, unknown);
		return;
	}
	Vector3 point(0.0f, 0.0f, 0.0f);
	Vector3 tangent(0.0f, 0.0f, 0.0f);
	Unknown_712170(when, &point, &tangent);
	if (position) {
		*position = point;
	}
	if (velocity) {
		*velocity = tangent;
	}
	if (orientation) {
		Vector3 target;
		if (Unknown74) {
			target = point - tangent;
		} else {
			target = tangent + point;
		}
		if (tangent.Length2() > 0.0001f) {
			Unknown44.Obj_Look_At(point, target, 0.0f);
		}
		*orientation = Build_Quaternion(Unknown44);
	}
	if (speed) {
		*speed = tangent.Length();
		if (Unknown74) {
			*speed = -*speed;
		}
	}
	if (rotrate) {
		float t = (float)(when - StartTime) / (float)(EndTime - StartTime);
		Vector3 start_rotrate, end_rotrate;
		start_rotrate = StartState->Unknown_14(when);
		end_rotrate = EndState->Unknown_14(when);
		Vector3::Lerp(start_rotrate, end_rotrate, t, rotrate);
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











// NOT EXACT: 0x712170 order: the tangent's three products come in the client's order but its sums pair (V0 + D) first and use other slots
void Unknown7119B0Class::Unknown_712170(int time, Vector3 *position, Vector3 *velocity)
{
	G_ASSERT(time >= StartTime && time <= EndTime);
	float t = (float)(time - StartTime) / (float)(EndTime - StartTime);
	float t2 = t * t;
	float t3 = t2 * t;
	if (position) {
		float h = 3.0f * t2 - 2.0f * t3;
		float c = t3 - 2.0f * t2 + t;
		float d = t3 - t2;
		*position = Unknown28 * h + Unknown14 * c + Unknown34 * d + Unknown08;
	}
	if (velocity) {
		float dh = 6.0f * t - 6.0f * t2;
		float dc = 3.0f * t2 - 4.0f * t + 1.0f;
		float dd = 3.0f * t2 - 2.0f * t;
		*velocity = (1.0f / (float)(EndTime - StartTime)) * (Unknown34 * dd + Unknown14 * dc + Unknown28 * dh);
	}
}






















void Unknown7119B0Class::Unknown_712480(int time)
{
	Vector3 position;
	Vector3 velocity;
	float start_speed;
	Unknown_1C(time, &position, 0, &velocity, &start_speed, 0, true);
	Unknown08 = position;
	Unknown14 = velocity;
	StartTime = time;
	float duration = (float)(EndTime - time);
	Unknown14 *= duration;
	float end_speed;
	EndState->Unknown_1C(EndTime, &Unknown28, 0, &Unknown34, &end_speed, 0, true);
	G_ASSERT(start_speed * end_speed >= 0.0f);
	Unknown28 -= Unknown08;
	Unknown34 *= duration;
	Unknown75 = true;
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









// NOT EXACT: 0x712780 shape: the client's speed is (s - s) * t + s of EndState's speed alone (its other slot reused for t), a form that folds here
void Unknown7125F0Class::Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown)
{
	if (time <= StartTime) {
		G_ASSERT(StartState);
		StartState->Unknown_1C(time, position, orientation, velocity, speed, rotrate, unknown);
		return;
	}


	if (time >= EndTime) {
		G_ASSERT(EndState);
		EndState->Unknown_1C(time, position, orientation, velocity, speed, rotrate, unknown);
		return;
	}




	float end_speed, start_speed;
	Vector3 start_position, start_velocity, start_rotrate;
	Vector3 end_position, end_velocity, end_rotrate;
	Quaternion start_orientation, end_orientation;
	G_ASSERT(StartState && EndState);
	StartState->Unknown_1C(time, position ? &start_position : 0, orientation ? &start_orientation : 0, velocity ? &start_velocity : 0, speed ? &start_speed : 0, rotrate ? &start_rotrate : 0, unknown);
	EndState->Unknown_1C(time, position ? &end_position : 0, orientation ? &end_orientation : 0, velocity ? &end_velocity : 0, speed ? &end_speed : 0, rotrate ? &end_rotrate : 0, unknown);
	float t = (float)(time - StartTime) / (float)(EndTime - StartTime);
	if (position) {
		Vector3::Lerp(start_position, end_position, t, position);
	}
	if (orientation) {
		if (Equal_Within_Epsilon(start_orientation, end_orientation, 0.0001f)) {
			*orientation = start_orientation;
		} else {
			Slerp(*orientation, start_orientation, end_orientation, t);
		}
	}
	if (velocity) {
		Vector3::Lerp(start_velocity, end_velocity, t, velocity);
	}
	if (speed) {
		*speed = WWMath::Lerp(start_speed, end_speed, t);
	}
	if (rotrate) {
		Vector3::Lerp(start_rotrate, end_rotrate, t, rotrate);
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






















void Unknown712DB0Class::Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown)
{
	if (time <= StartTime) {
		G_ASSERT(StartState);
		StartState->Unknown_1C(time, position, orientation, velocity, speed, rotrate, unknown);
		return;
	}


	if (time >= EndTime) {
		G_ASSERT(EndState);
		EndState->Unknown_1C(time, position, orientation, velocity, speed, rotrate, unknown);
		return;
	}
	float t = (float)(time - StartTime) / (float)(EndTime - StartTime);
	if (position) {
		Vector3::Lerp(Start.Position, End.Position, t, position);
	}
	if (orientation) {
		if (Equal_Within_Epsilon(Start.Orientation, End.Orientation, 0.0001f)) {
			*orientation = Start.Orientation;
		} else {
			Slerp(*orientation, Start.Orientation, End.Orientation, t);
		}
	}
	if (velocity) {
		Vector3::Lerp(Start.Velocity, End.Velocity, t, velocity);
	}
	if (speed) {
		*speed = WWMath::Lerp(Start.Speed, End.Speed, t);
	}
	if (rotrate) {
		Vector3::Lerp(Start.RotRate, End.RotRate, t, rotrate);
	}
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


Unknown713D70Class::Unknown713D70Class(const Unknown713D70Class &other) :
	Unknown710030Class(other)
{
	Transform = other.Transform;
	Velocity = other.Velocity;
	ImpartedDecay = other.ImpartedDecay;
	TractorSpeed = other.TractorSpeed;
	TractorID = other.TractorID;
	Client = other.Client;
}


Unknown710030Class *Unknown713D70Class::Unknown_0C()
{
	return(new Unknown713D70Class(*this));
}


// NOT EXACT: 0x7140E0 order: the translation's stores, the roll's push and the state's vtable load are scheduled differently
bool Unknown713D70Class::Unknown_7140E0(Vector3 *result)
{
	if (TractorID) {
		ClientObjectClass *object = Client->Find_Object(TractorID);
		if (object) {
			Unknown98Class *state = object->Unknown98;
			if (state) {
				Matrix3D look;
				look.Look_At(Transform.Get_Translation(), state->Unknown_1C(Time), 0.0f);
				*result = look.Rotate_Vector(Vector3(0.0f, 0.0f, -TractorSpeed));
				return(true);
			}
		}
	}
	return(false);
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


// NOT EXACT: 0x714410 order: Matrix3D::Rotate_Vector's rows sum their products in another order in the client (x z y, z y x, z y x)
void Unknown714340Class::Unknown_1C(int time, Vector3 *position, Quaternion *orientation, Vector3 *velocity, float *speed, Vector3 *rotrate, bool unknown)
{
	ClientObjectClass *leader = Client->Find_Object(LeaderID);
	if (leader == 0) {
		if (position) {
			*position = Unknown18.Get_Translation();
		}
		if (orientation) {
			*orientation = Build_Quaternion(Unknown18);
		}
		if (velocity) {
			velocity->Set(0.0f, 0.0f, 0.0f);
		}
		if (speed) {
			*speed = 0.0f;
		}
		if (rotrate) {
			rotrate->Set(0.0f, 0.0f, 0.0f);
		}
		return;
	}
	Unknown98Class *state = leader->Unknown98;
	if (position || orientation) {
		Matrix3D transform = unknown ? state->Unknown_20(time) : state->Unknown_08(time);
		if (position) {
			Matrix3D::Rotate_Vector(transform, Offset, position);
			*position += transform.Get_Translation();
		}
		if (orientation) {
			*orientation = Build_Quaternion(transform);
		}
	}
	if (velocity || speed) {
		Vector3 move = state->Unknown_10(Client->Unknown10BC);
		if (velocity) {
			*velocity = move;
		}
		if (speed) {
			*speed = move.Length2();
		}
	}
	if (rotrate) {
		*rotrate = state->Unknown_14(Client->Unknown10BC);
	}
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
