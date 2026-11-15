// Earth & Beyond client (reconstruction): app\client\eff_particlejet.cpp. Notes at the end of the file.
#include "eff_particlejet.h"
#include "part_emt.h"
#include "part_ldr.h"
#include "lineseg.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"




ParticleJetEffectClass::ParticleJetEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *emitter, float min_lifetime) :
	UnknownB0009CEffectClass(client, id, start_time, duration, cycle_time),
	Lifetime(0.0f), MinLifetime(min_lifetime), ForwardVelocity(1.0f)
{
	Emitter = (ParticleEmitterClass *)emitter;

	FAIL_IF(!Emitter) return;
	Emitter->Add_Ref();
	Emitter->Set_Transform(Matrix3D(true));
	Lifetime = Emitter->Get_Lifetime();
	if (MinLifetime < 0.0f) {
		MinLifetime = Lifetime * 0.3333333f;
	}
	ForwardVelocity = Emitter->Get_Start_Velocity().X;











	FAIL_IF(ForwardVelocity < 0.01f) ForwardVelocity = 0.01f;
	Flags |= 0x180;
}

ParticleJetEffectClass::~ParticleJetEffectClass()
{
	REF_PTR_RELEASE(Emitter);
}

Vector3 ParticleJetEffectClass::Unknown_08(const Vector3 &position)
{
	LineSegClass line(SourcePosition, TargetPosition);
	return(line.Find_Point_Closest_To(position));
}

void ParticleJetEffectClass::Unknown_18()
{
	UnknownB0009CEffectClass::Unknown_18();

















	FAIL_IF(!Emitter) return;
	Vector3 velocity = Emitter->Get_Start_Velocity();
	velocity.X = ForwardVelocity;
	Emitter->Set_Base_Velocity(velocity);
	float lifetime = (TargetPosition - SourcePosition).Length() / ForwardVelocity;
	lifetime = WWMath::Clamp(lifetime, MinLifetime, Lifetime);




	float current = Emitter->Get_Lifetime();
	if (current < lifetime * 0.8f || lifetime * 1.2f < current) {
		ParticleEmitterDefClass *definition = Emitter->Build_Definition();
		Emitter->Release_Ref();
		definition->Set_Lifetime(lifetime);
		float scale = lifetime / current;
		ParticlePropertyStruct<Vector3> color;
		ParticlePropertyStruct<float> opacity;
		ParticlePropertyStruct<float> size;
		ParticlePropertyStruct<float> rotation;
		ParticlePropertyStruct<float> frame;
		ParticlePropertyStruct<float> blur_time;
		definition->Get_Color_Keyframes(color);
		definition->Get_Opacity_Keyframes(opacity);
		definition->Get_Size_Keyframes(size);
		definition->Get_Rotation_Keyframes(rotation);
		definition->Get_Frame_Keyframes(frame);
		definition->Get_Blur_Time_Keyframes(blur_time);
		float orient_rnd = definition->Get_Initial_Orientation_Random();
		unsigned i;
		for (i = 0; i < color.NumKeyFrames; i++) color.KeyTimes[i] *= scale;
		for (i = 0; i < opacity.NumKeyFrames; i++) opacity.KeyTimes[i] *= scale;
		for (i = 0; i < size.NumKeyFrames; i++) size.KeyTimes[i] *= scale;
		for (i = 0; i < rotation.NumKeyFrames; i++) rotation.KeyTimes[i] *= scale;
		for (i = 0; i < frame.NumKeyFrames; i++) frame.KeyTimes[i] *= scale;
		for (i = 0; i < blur_time.NumKeyFrames; i++) blur_time.KeyTimes[i] *= scale;
		definition->Set_Color_Keyframes(color);
		definition->Set_Opacity_Keyframes(opacity);
		definition->Set_Size_Keyframes(size);
		definition->Set_Rotation_Keyframes(rotation, orient_rnd);
		definition->Set_Frame_Keyframes(frame);
		definition->Set_Blur_Time_Keyframes(blur_time);
		Emitter = ParticleEmitterClass::Create_From_Definition(*definition);
		delete definition;
	}
	Emitter->Add(Scene);
}

















void ParticleJetEffectClass::Unknown_1C()
{
	UnknownB0009CEffectClass::Unknown_1C();

	FAIL_IF(!Emitter) return;
	Matrix3D transform;
	if (Get_Flag(14)) {
		transform.Obj_Look_At(TargetPosition, SourcePosition, 0.0f);
	} else {
		transform.Obj_Look_At(SourcePosition, TargetPosition, 0.0f);
	}
	Emitter->Set_Transform(transform);
	float speed = (TargetPosition - SourcePosition).Length() / Emitter->Get_Lifetime();
	Vector3 velocity = Emitter->Get_Start_Velocity();
	velocity.X = speed;
	Emitter->Set_Base_Velocity(velocity);
}






void ParticleJetEffectClass::Unknown_20()
{
	FAIL_IF(!Emitter) return;
	Emitter->Remove();
}

/*
**	Notes (reconstruction)
**
**	The client's eff_particlejet.obj: 0x6ECC50-0x6ED6C0 (its <string> guarded static 0x6ED690 ends it; eff_pass.cpp's
**	constructor 0x6ED6C0 opens the next object). __FILE__ is 'e:\build\beta\G\app\Client\eff_particlejet.cpp'
**	(0xB938D4): FAIL_IF '!Emitter' on lines 18, 72, 140 and 161, 'ForwardVelocity < 0.01f' on 37. effect.ini's
**	EffectClass 'ParticleJetEffect' is this class: eff_o.cpp's helper 0x6EC950 makes it for a particle emitter
**	model (its 'MinLifetime', and 'Invert' as flag 0x4000).
**
**	Emitted here as well: the scalar deleting destructor 0x6ECEA0. Unknown_18 clamps through WWMath::Clamp
**	after the division (the division's result stored first); Unknown_1C divides before it reads the emitter's
**	start velocity. MinLifetime's default is the literal 0.3333333f (0x3EAAAAAA at 0xB00134, not 1/3).
*/
