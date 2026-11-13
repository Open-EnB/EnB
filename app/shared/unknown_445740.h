// Earth & Beyond client (reconstruction): app\shared\unknown_445740.h. Notes at the end of unknown_445740.cpp.
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_445740_H  // NAME 1 [unknown_445740.h]: provisional header name (D137: the client names neither file)
#define UNKNOWN_445740_H

#include "vector3.h"
#include "matrix3d.h"

class PxWrapper;
class KVEncoderBaseClass;

// What a steering behaviour reads of the vehicle it steers (0x48 bytes read: the transform's rotation and
// translation, then a position and a velocity of its own)
struct SteeringVehicleStruct  // NAME 1: what the behaviours read of it
{
	Matrix3D Transform;		// +0x00 (0x446000 and 0x447E20 rotate by it; 0x4469A0 transforms by it)  // NAME 1: its type
	Vector3 Position;		// +0x30 (every behaviour's origin)  // NAME 1: what the behaviours do with it
	Vector3 Velocity;		// +0x3C (every behaviour subtracts it from the desired velocity)  // NAME 1: as above
};

// What a pursuit reads of its target (0x18 bytes: 0x446E60 predicts the position from the velocity)
struct SteeringTargetStruct  // NAME 1: what the behaviours read of it
{
	Vector3 Position;		// +0x00  // NAME 1: as SteeringVehicleStruct's
	Vector3 Velocity;		// +0x0C  // NAME 1: as SteeringVehicleStruct's
};

// The steering behaviours (Reynolds's seek, flee, arrive, pursue, evade, separation, cohesion, alignment and
// obstacle avoidance): every one is a non-inline member, none is called from outside this object, and none
// reads more of the object than its first float, the speed the desired velocity is scaled to.
class SteeringClass  // NAME 1: what its members do
{
public:
	Vector3 Seek(const SteeringVehicleStruct &vehicle, const Vector3 &target);							// 0x445740  // NAME 1 [Seek, vehicle, target]: Reynolds's name for what it does
	Vector3 Flee(const SteeringVehicleStruct &vehicle, const Vector3 &target);							// 0x445860  // NAME 1 [Flee]: as Seek
	Vector3 Arrive(const SteeringVehicleStruct &vehicle, const Vector3 &target);						// 0x445980 (SlowDist)  // NAME 1 [Arrive]: as Seek
	Vector3 Arrive(const SteeringVehicleStruct &vehicle, const Vector3 &target, float distance);		// 0x445A50 (stops distance short)  // NAME 1 [distance]: what it subtracts
	Vector3 Pursue(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target);			// 0x445B30  // NAME 1 [Pursue]: as Seek
	Vector3 Evade(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target);			// 0x445C60  // NAME 1 [Evade]: as Seek
	Vector3 Evade(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, float distance);	// 0x445D90 (to a point beside the flee direction)  // NAME 1 [distance]: how far the point lies
	Vector3 Avoid_Obstacles(const SteeringVehicleStruct &vehicle, PxWrapper *proxim);					// 0x446000 (ObstacleTicks, ObstacleComfort)  // NAME 1 [Avoid_Obstacles]: as Seek; [proxim]: fp_tt.cpp's FAIL_IF 'proxim.Get_Avoidance()'
	Vector3 Avoid_Collisions(const SteeringVehicleStruct &vehicle, PxWrapper *proxim);					// 0x4463B0 (CollisionTicks, CollisionComfort)  // NAME 1 [Avoid_Collisions]: after its settings
	Vector3 Separation(const SteeringVehicleStruct &vehicle, PxWrapper *proxim);						// 0x446810 (SeparationRadius)  // NAME 1 [Separation]: as Seek
	Vector3 Cohesion(const SteeringVehicleStruct &vehicle, PxWrapper *proxim);							// 0x4469A0  // NAME 1 [Cohesion]: as Seek
	Vector3 Alignment(const SteeringVehicleStruct &vehicle, PxWrapper *proxim);						// 0x446CB0  // NAME 1 [Alignment]: as Seek
	Vector3 Predict(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target);			// 0x446E60 (the target's position when the vehicle reaches it)  // NAME 1 [Predict]: what it returns
	float Closing_Speed(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target);		// 0x447070  // NAME 1 [Closing_Speed]: what it returns
	Vector3 Follow(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, PxWrapper *proxim);	// 0x447180 (FollowUsesPursue, FollowPredictArrival)  // NAME 1 [Follow]: after its settings
	Vector3 Seek(const SteeringVehicleStruct &vehicle, const Vector3 &target, PxWrapper *proxim);		// 0x447480 (avoiding first)  // NAME 1: as Seek
	Vector3 Evade(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, PxWrapper *proxim);	// 0x447690 (avoiding first)  // NAME 1: as Evade
	Vector3 Pursue(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, PxWrapper *proxim);	// 0x4479F0 (proxim not read: avoiding as 0x447480 does is left out)  // NAME 1: as Pursue
	Vector3 Arrive(const SteeringVehicleStruct &vehicle, const Vector3 &target, PxWrapper *proxim, float distance);	// 0x447B30 (proxim not read, as 0x4479F0's)  // NAME 1: as Arrive

	static void Encode(KVEncoderBaseClass *encoder);	// 0x447CC0 (the settings' keys)  // NAME 1 [Encode, encoder]: what it does

	float MaxSpeed;			// +0x00 the desired velocity's length  // NAME 1: what the behaviours do with it
};

// The turn limits: three angles and their cosines (0x447DD0); 0x447E20 turns a desired velocity into the
// yaw and pitch toward it, limited, and its forward component
class SteeringTurnClass  // NAME 1: what it holds
{
public:
	SteeringTurnClass(float yaw, float pitch, float level);							// 0x447DD0  // NAME 1 [yaw, pitch, level]: what 0x447E20 limits with them
	Vector3 Turn(const Vector3 &desired, const SteeringVehicleStruct &vehicle);		// 0x447E20  // NAME 1 [Turn, desired]: what it does

	float MaxYaw;			// +0x00  // NAME 1: as the constructor's
	float MaxPitch;			// +0x04  // NAME 1: as above
	float MaxLevel;			// +0x08  // NAME 1: as above
	float CosYaw;			// +0x0C  // NAME 1: as above
	float CosPitch;			// +0x10  // NAME 1: as above
	float CosLevel;			// +0x14  // NAME 1: as above
};

#endif
