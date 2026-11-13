// Earth & Beyond client (reconstruction): app\shared\unknown_445740.cpp. Notes at the end of the file.
#include "unknown_445740.h"
#include "px_wrapper.h"
#include "dynamic_obj.h"
#include "kv_encoder_base.h"
#include "wwmath.h"
#include <vector>

// The steering settings, in Encode's keys (0xB704BC-0xB704D5, FollowUsesPursue in the bss at 0xBDC065)
static float SlowDist = 100.0f;
static float ObstacleTicks = 180.0f;
static float ObstacleComfort = 1.4f;
static float CollisionTicks = 120.0f;
static float CollisionComfort = 1.4f;
static float SeparationRadius = 100.0f;
static bool FollowPredictArrival = true;
static bool UseTerrainAvoid = true;
static bool FollowUsesPursue;

// 0x445740
Vector3 SteeringClass::Seek(const SteeringVehicleStruct &vehicle, const Vector3 &target)
{
	Vector3 desired = target - vehicle.Position;
	desired.Normalize();
	desired *= MaxSpeed;
	return desired - vehicle.Velocity;
}

// 0x445860
Vector3 SteeringClass::Flee(const SteeringVehicleStruct &vehicle, const Vector3 &target)
{
	Vector3 desired = vehicle.Position - target;
	desired.Normalize();
	desired *= MaxSpeed;
	return desired - vehicle.Velocity;
}

// 0x445980
Vector3 SteeringClass::Arrive(const SteeringVehicleStruct &vehicle, const Vector3 &target)
{
	Vector3 offset = target - vehicle.Position;
	float distance = offset.Length();
	float speed = (distance / SlowDist) * MaxSpeed;
	if (speed >= MaxSpeed) {
		speed = MaxSpeed;
	}
	return offset * (speed / distance) - vehicle.Velocity;
}

// 0x445A50
Vector3 SteeringClass::Arrive(const SteeringVehicleStruct &vehicle, const Vector3 &target, float distance)
{
	Vector3 offset = target - vehicle.Position;
	distance = offset.Length() - distance;
	if (distance < 0.0f) {
		distance = 0.0f;
	}
	float speed = (distance / SlowDist) * MaxSpeed;
	if (speed >= MaxSpeed) {
		speed = MaxSpeed;
	}
	return offset * (speed / distance) - vehicle.Velocity;
}

// 0x445B30
Vector3 SteeringClass::Pursue(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target)
{
	return Seek(vehicle, Predict(vehicle, target));
}

// 0x445C60
Vector3 SteeringClass::Evade(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target)
{
	return Flee(vehicle, Predict(vehicle, target));
}

// 0x445D90
Vector3 SteeringClass::Evade(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, float distance)
{
	Vector3 predicted = Predict(vehicle, target);
	Vector3 away = vehicle.Position - predicted;
	away.Normalize();
	Vector3 side(-away.Y, away.X, 0.0f);
	away *= distance;
	side *= 50.0f;
	predicted += side + away;
	return Seek(vehicle, predicted);
}

#pragma auto_inline(off)
// NOT EXACT: frame (explain.py): the client keeps the avoidance vector's X (0.0f) on the FPU and its frame is 4 bytes smaller; the inverse rotation's terms load in another order
// 0x446000
Vector3 SteeringClass::Avoid_Obstacles(const SteeringVehicleStruct &vehicle, PxWrapper *proxim)
{
	std::vector<DynObj *> &list = proxim->Unknown_4568E0();
	int count = list.size();
	if (count == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	float lookahead = vehicle.Velocity.Length() * ObstacleTicks;
	bool found = false;
	Vector3 avoid(0.0f, 0.0f, 0.0f);
	float nearest = lookahead + lookahead;
	for (int i = 0; i < count; i++) {
		DynObj *obj = list[i];
		if (obj->Unknown_Dyn54().Length2() <= 0.0001f) {
			Vector3 local;
			Matrix3D::Inverse_Rotate_Vector(vehicle.Transform, obj->Unknown_Dyn48() - vehicle.Position, &local);
			if (local.X >= 0.0f && local.X <= lookahead) {
				Vector2 lateral(local.Y, local.Z);
				if (lateral.Length() <= (proxim->Owner->Unknown04 + obj->Unknown04) * ObstacleComfort) {
					found = true;
					if (local.X - obj->Unknown04 < nearest) {
						nearest = local.X - obj->Unknown04;
						avoid.Y = local.Y;
						avoid.Z = local.Z;
					}
				}
			}
		}
	}
	if (!found) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	if (avoid.Length2() < 0.0001f) {
		avoid.Y = 1.0f;
		avoid.Z = 0.0f;
	} else {
		avoid.Normalize();
	}
	avoid *= -MaxSpeed;
	Vector3 result;
	Matrix3D::Rotate_Vector(vehicle.Transform, avoid, &result);
	return result;
}

// 0x4463B0
Vector3 SteeringClass::Avoid_Collisions(const SteeringVehicleStruct &vehicle, PxWrapper *proxim)
{
	std::vector<DynObj *> &list = proxim->Unknown_4568E0();
	int count = list.size();
	if (count == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	Vector3 nearest_point(0.0f, 0.0f, 0.0f);
	float nearest_time = 480.0f;
	DynObj *nearest = NULL;
	for (int i = 0; i < count; i++) {
		DynObj *obj = list[i];
		Vector3 velocity = obj->Unknown_Dyn54();
		if (velocity.Length2() >= 0.0001f) {
			Vector3 position = obj->Unknown_Dyn48();
			Vector3 dp = vehicle.Position - position;
			Vector3 dv = vehicle.Velocity - velocity;
			float dv2 = dv.Length2();
			if (dv2 >= 0.0001f) {
				float time = -Vector3::Dot_Product(dv, dp) / dv2;
				if (time < 0.0f) {
					time = 0.0f;
				} else if (time > CollisionTicks) {
					time = CollisionTicks;
				}
				Vector3 future = vehicle.Velocity * time + vehicle.Position;
				Vector3 other_future = velocity * time + position;
				float miss = (future - other_future).Length();
				float comfort = (proxim->Owner->Unknown04 + obj->Unknown04) * CollisionComfort;
				if (miss <= comfort) {
					time -= comfort / dp.Length();
					if (time < nearest_time) {
						nearest_time = time;
						nearest_point = velocity * time + position;
						nearest = obj;
					}
				}
			}
		}
	}
	if (nearest == NULL) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	return Flee(vehicle, nearest_point);
}

#pragma auto_inline(on)
// NOT EXACT: shape (explain.py): one load order (the client loads the owner's radius and fsubr's the distance)
// 0x446810
Vector3 SteeringClass::Separation(const SteeringVehicleStruct &vehicle, PxWrapper *proxim)
{
	std::vector<DynObj *> &list = proxim->Unknown_4568E0();
	int count = list.size();
	if (count == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	Vector3 sum(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < count; i++) {
		DynObj *obj = list[i];
		Vector3 away = vehicle.Position - obj->Unknown_Dyn48();
		float distance = away.Length();
		float gap = distance - proxim->Owner->Unknown04 - obj->Unknown04;
		if (gap <= SeparationRadius) {
			float strength = (1.0f - gap * 0.01f) * MaxSpeed;
			away *= (strength + strength) / distance;
			sum += away;
		}
	}
	return sum;
}

// NOT EXACT: register-only: the inlined Inverse_Transform_Vector's terms in another FPU order (fld st(1) / fld st(0))
// 0x4469A0
Vector3 SteeringClass::Cohesion(const SteeringVehicleStruct &vehicle, PxWrapper *proxim)
{
	std::vector<DynObj *> &list = proxim->Unknown_4568E0();
	int count = list.size();
	if (count == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	int n = 0;
	Vector3 sum(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < count; i++) {
		Vector3 local;
		Matrix3D::Inverse_Transform_Vector(vehicle.Transform, list[i]->Unknown_Dyn48(), &local);
		if (local.Length() <= 300.0f) {
			sum += local;
			n++;
		}
	}
	if (n == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	sum /= (float)n;
	Vector3 center;
	Matrix3D::Transform_Vector(vehicle.Transform, sum, &center);
	return Seek(vehicle, center);
}

// NOT EXACT: order-only (explain.py): the same 127 instructions in another order (the offset's length sums)
// 0x446CB0
Vector3 SteeringClass::Alignment(const SteeringVehicleStruct &vehicle, PxWrapper *proxim)
{
	std::vector<DynObj *> &list = proxim->Unknown_4568E0();
	int count = list.size();
	if (count == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	int n = 0;
	Vector3 sum(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < count; i++) {
		DynObj *obj = list[i];
		Vector3 offset = obj->Unknown_Dyn48() - vehicle.Position;
		if (offset.Length() <= 300.0f) {
			n++;
			sum += obj->Unknown_Dyn54();
		}
	}
	if (n == 0) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	sum /= (float)n;
	return sum - vehicle.Velocity;
}

#pragma auto_inline(off)
// NOT EXACT: shape (explain.py): the inlined Closing_Speed's Normalize multiplies in another FPU order
// 0x446E60
Vector3 SteeringClass::Predict(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target)
{
	if (target.Velocity.Length2() < 0.0001f) {
		return target.Position;
	}
	float speed = Closing_Speed(vehicle, target);
	if (speed < 0.0001f) {
		return target.Position;
	}
	float time = (target.Position - vehicle.Position).Length() / speed;
	Vector3 predicted = target.Position + target.Velocity * time;
	return predicted;
}

#pragma auto_inline(on)
// 0x447070
float SteeringClass::Closing_Speed(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target)
{
	Vector3 dir = target.Position - vehicle.Position;
	dir.Normalize();
	return Vector3::Dot_Product(dir, vehicle.Velocity) - Vector3::Dot_Product(dir, target.Velocity);
}

// NOT EXACT: field-offset (explain.py): the client keeps the avoidance result's Y on the FPU (X and Z in memory)
// 0x447180
Vector3 SteeringClass::Follow(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, PxWrapper *proxim)
{
	Vector3 result = Avoid_Obstacles(vehicle, proxim);
	if (result.Length2() > 0.0001f) {
		return result;
	}
	result = Avoid_Collisions(vehicle, proxim);
	if (result.Length2() > 0.0001f) {
		return result;
	}
	if (FollowUsesPursue) {
		result = Pursue(vehicle, target);
	} else if (FollowPredictArrival) {
		result = Arrive(vehicle, Predict(vehicle, target));
	} else {
		result = Arrive(vehicle, target.Position);
	}
	return result;
}

// NOT EXACT: field-offset (explain.py): as Follow, the avoidance result's Y on the FPU in the client
// 0x447480
Vector3 SteeringClass::Seek(const SteeringVehicleStruct &vehicle, const Vector3 &target, PxWrapper *proxim)
{
	Vector3 result = Avoid_Obstacles(vehicle, proxim);
	if (result.Length2() <= 0.0001f) {
		result = Avoid_Collisions(vehicle, proxim);
		if (result.Length2() <= 0.0001f) {
			return Seek(vehicle, target);
		}
	}
	return result;
}

// NOT EXACT: frame (explain.py): as Follow; frame 0x24 against 0x30
// 0x447690
Vector3 SteeringClass::Evade(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, PxWrapper *proxim)
{
	Vector3 result = Avoid_Obstacles(vehicle, proxim);
	if (result.Length2() <= 0.0001f) {
		result = Avoid_Collisions(vehicle, proxim);
		if (result.Length2() <= 0.0001f) {
			return Evade(vehicle, target, 500.0f);
		}
	}
	return result;
}

// NOT EXACT: shape (explain.py): the client copies the result twice (a second local) and orders the length sums otherwise
// 0x4479F0
Vector3 SteeringClass::Pursue(const SteeringVehicleStruct &vehicle, const SteeringTargetStruct &target, PxWrapper *proxim)
{
	Vector3 result;
	result = Pursue(vehicle, target);
	return result;
}

// NOT EXACT: shape (explain.py): as 0x4479F0
// 0x447B30
Vector3 SteeringClass::Arrive(const SteeringVehicleStruct &vehicle, const Vector3 &target, PxWrapper *proxim, float distance)
{
	Vector3 result;
	result = Arrive(vehicle, target, distance);
	return result;
}

// 0x447CC0
void SteeringClass::Encode(KVEncoderBaseClass *encoder)
{
	encoder->Add(0x3FF, "ObstacleTicks", &ObstacleTicks);
	encoder->Add(0x3FF, "ObstacleComfort", &ObstacleComfort);
	encoder->Add(0x3FF, "SeparationRadius", &SeparationRadius);
	encoder->Add(0x3FF, "CollisionTicks", &CollisionTicks);
	encoder->Add(0x3FF, "CollisionComfort", &CollisionComfort);
	encoder->Add_Typed(0x3FF, "UseTerrainAvoid", &UseTerrainAvoid);
	encoder->Add(0x3FF, "SlowDist", &SlowDist);
	encoder->Add_Typed(0x3FF, "FollowUsesPursue", &FollowUsesPursue);
	encoder->Add_Typed(0x3FF, "FollowPredictArrival", &FollowPredictArrival);
}

// 0x447DD0
SteeringTurnClass::SteeringTurnClass(float yaw, float pitch, float level) :
	MaxYaw(yaw),
	MaxPitch(pitch),
	MaxLevel(level),
	CosYaw(cos(yaw)),
	CosPitch(cos(pitch)),
	CosLevel(cos(level))
{
}

// NOT EXACT: shape (explain.py): the client copies the cosine into the angle's slot before comparing it with the limit (an inline helper's parameter?); one round tried
// 0x447E20
Vector3 SteeringTurnClass::Turn(const Vector3 &desired, const SteeringVehicleStruct &vehicle)
{
	Vector3 local;
	Matrix3D::Inverse_Rotate_Vector(vehicle.Transform, vehicle.Velocity + desired, &local);
	local.Normalize();
	float yaw = 0.0f;
	if (local.X < -0.0001f || fabs(local.Y) > 0.0001) {
		Vector2 flat(local.X, local.Y);
		flat.Normalize();
		yaw = flat.X;
		if (yaw < CosYaw) {
			yaw = MaxYaw;
		} else {
			yaw = WWMath::Acos(WWMath::Clamp(yaw, -1.0f, 1.0f));
		}
		if (flat.Y < 0.0f) {
			yaw = -yaw;
		}
	}
	float pitch = 0.0f;
	if (fabs(desired.Z) > 0.0001) {
		Vector2 flat(local.X, local.Z);
		flat.Normalize();
		pitch = flat.X;
		if (pitch < CosPitch) {
			pitch = MaxPitch;
		} else {
			pitch = WWMath::Acos(WWMath::Clamp(pitch, -1.0f, 1.0f));
		}
		if (desired.Z > 0.0f) {
			pitch = -pitch;
		}
		if (vehicle.Transform[2][2] < CosLevel && pitch * vehicle.Transform[2][0] < 0.0f) {
			pitch = 0.0f;
		}
	}
	return Vector3(pitch, yaw, vehicle.Transform[0][0] * desired.X + vehicle.Transform[1][0] * desired.Y + vehicle.Transform[2][0] * desired.Z);
}

// 0x448260
void KVEncoderBaseClass::Add(int mask, const char *name, float *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<float>(Make_Key(name), new KVPointerAccessorClass<float>(value), even, odd));
	}
}

// 0x448390
template<class T> void KVEncoderBaseClass::Add_Typed(int mask, const char *name, T *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<T>(Make_Key(name), new KVPointerAccessorClass<T>(value), even, odd));
	}
}

/*
**	Reconstruction notes (2026-11-13, builder O17)
**
**	* The object: client 0x445740-0x448BD0 (no __FILE__, assert or RTTI names it; D137's interim name). It
**	  follows main.cpp's object (whose notes put its end at 0x445725) and ends with <string>'s guarded
**	  std::ctype<unsigned short>::id initialiser 0x448BB0 ($E37 here); fp_ita.cpp's object follows (0x448C30's
**	  constructor; its FAIL_IF 'y_axis.Z < 0.0001f' sits in 0x448D00). Objects link in name order, so the
**	  file's name sorts between main\main.cpp and shared\fp_ita.cpp; the flight-path files (fp_ita.cpp,
**	  fp_tt.cpp: 'proxim.Get_Avoidance()') are this code's users, so the client's name is probably an fp_*.cpp
**	  before fp_ita.
**	* Every SteeringClass member and Encode has an incremental-link thunk and no caller in the client: the
**	  steering code is linked but not called (the flight-path code that would call it is not in this object).
**	  The members are thiscall (this's first float, the speed, is all they read) with the Vector3 returned
**	  through a hidden pointer; Predict, Avoid_Obstacles and Avoid_Collisions are always called, never inlined
**	  (client.py callers: 7, 3 and 3 calls), so they sit under auto_inline(off); the rest inline the earlier
**	  behaviours (Pursue = Seek(Predict), 0x445D90 inlined with 500 into 0x447690).
**	* Encode 0x447CC0 names the settings: their initial values are the client's data (SlowDist 100,
**	  ObstacleTicks 180, ObstacleComfort 1.4, CollisionTicks 120, CollisionComfort 1.4, SeparationRadius 100,
**	  FollowPredictArrival and UseTerrainAvoid true, FollowUsesPursue false); UseTerrainAvoid is read nowhere.
**	* The encoder's typed adds 0x448260 (float; kv_encoder.cpp's Add_Vector3_Elements calls it) and 0x448390
**	  (bool; shipcust_desc.cpp's IsMatte and Encode call it) are compiled here, as avatar_desc.cpp's int and char
**	  adds are there. The bool one is kv_encoder_base.h's member template Add_Typed, defined here out of the
**	  class (its instance links shipcust_desc.cpp's call): a plain Add(int, const char *, bool *) overload in the
**	  header matched as well but flipped ui_group.cpp's symbol-table sensitive 0x597080 and the send pad
**	  (verify 2026-11-13), so the header stays as it was. This object's template instances follow, KVPairTemplateClass<float> / <bool> 0x4484C0-
**	  0x448800, 0x4488B0-0x4489C0, 0x448B10 / 0x448B60, and KVPointerAccessorClass<float> / <bool> 0x448640-
**	  0x448870, 0x448960 / 0x448A40, 0x448A70-0x448AE0.
**	* SteeringTurnClass (0x447DD0, 0x447E20): the constructor stores the three angles and cos() of each (the
**	  double result straight into the member); Turn's yaw and pitch come from acos of the clamped cosine.
*/
