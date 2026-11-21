// Earth & Beyond client (reconstruction): app\client\eff_line.cpp. Notes at the end of the file.
#include "eff_line.h"
#include "client.h"
#include "co.h"
#include "segline.h"
#include "camera.h"
#include "lineseg.h"
#include "vector2.h"
#include "wwmath.h"
#include <math.h>
#include "..\shared\g_assert.h"























LineEffectClass::LineEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, SegmentedLineClass *line, float start_offset, float end_offset, int grow_time, int shrink_time, float grow_accel, float shrink_accel, float tiling_min_length) :
	UnknownB0009CEffectClass(client, id, start_time, duration, cycle_time),
	StartOffset(start_offset),
	EndOffset(end_offset),
	GrowTime((grow_time == 0) ? -1 : grow_time),
	ShrinkTime((shrink_time == 0) ? -1 : shrink_time),
	GrowA(0.0f),
	ShrinkA(0.0f),
	GrowB(0.0f),
	ShrinkB(0.0f),
	TilingMinLength(WWMath::Max(tiling_min_length, 0.0f)),
	InvTilingMinLength((tiling_min_length > 0.0f) ? 1.0f / tiling_min_length : 0.0f),
	TextureTileFactor(0.0f),
	SubdivisionLevels(0)
{
	SegLineRObj = line;
	FAIL_IF(!SegLineRObj) return;
	SegLineRObj->Add_Ref();
	SegLineRObj->Set_Transform(Matrix3D(true));
	Vector3 points[2] = { Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f) };
	SegLineRObj->Set_Points(2, points);
	if (GrowTime != -1) {
		grow_accel = WWMath::Clamp(grow_accel, 0.0f, 1.0f);
		float inv = 1.0f / GrowTime;
		GrowA = inv * inv * grow_accel;
		GrowB = (1.0f - grow_accel) * inv;
	}
	if (ShrinkTime != -1) {
		shrink_accel = WWMath::Clamp(shrink_accel, 0.0f, 1.0f);
		float inv = 1.0f / ShrinkTime;
		ShrinkA = inv * inv * shrink_accel;
		ShrinkB = (1.0f - shrink_accel) * inv;
	}
	TextureTileFactor = SegLineRObj->Get_Texture_Tile_Factor();
	SubdivisionLevels = SegLineRObj->Get_Subdivision_Levels();
	Flags |= 0x180;
}

LineEffectClass::~LineEffectClass()
{
	REF_PTR_RELEASE(SegLineRObj);
}

Vector3 LineEffectClass::Unknown_08(const Vector3 &position)
{
	LineSegClass line(SourcePosition, TargetPosition);
	return(line.Find_Point_Closest_To(position));
}

void LineEffectClass::Set_Source_Object_ID(int game_id)
{
	UnknownB0009CEffectClass::Set_Source_Object_ID(game_id);
	unsigned avatar = Client->AvatarID;
	bool source = (avatar == game_id);
	Set_Flag(0x10000, source);
	bool target = (TargetObject != NULL && TargetObject->GameID == avatar);
	Set_Flag(0x20000, target);
}

void LineEffectClass::Set_Target_Object_ID(int game_id)
{
	UnknownB0009CEffectClass::Set_Target_Object_ID(game_id);
	unsigned avatar = Client->AvatarID;
	bool target = (avatar == game_id);
	Set_Flag(0x20000, target);
	bool source = (SourceObject != NULL && SourceObject->GameID == avatar);
	Set_Flag(0x10000, source);
}

































void LineEffectClass::Unknown_18()
{
	FAIL_IF(!SegLineRObj) return;
	UnknownB0009CEffectClass::Unknown_18();
	SegLineRObj->Add(Scene);
}


void LineEffectClass::Unknown_1C()
{
	FAIL_IF(!SegLineRObj) return;
	UnknownB0009CEffectClass::Unknown_1C();
	Vector3 start = SourcePosition;
	Vector3 end = TargetPosition;
	Unknown_6E5BB0(&start, &end);
	Unknown_6E5E00(&start, &end);
	float length = (end - start).Length();
	Unknown_6E6030(&start, &end);
	SegLineRObj->Set_Point_Location(0, start);
	SegLineRObj->Set_Point_Location(1, end);
	if (length < TilingMinLength) {
		float scale = length * InvTilingMinLength;
		SegLineRObj->Set_Texture_Tile_Factor(scale * TextureTileFactor);
		int levels = SubdivisionLevels;
		while (levels > 0 && scale < 0.5f) {
			levels--;
			scale += scale;
		}
		SegLineRObj->Set_Subdivision_Levels(levels);
	}
}











void LineEffectClass::Unknown_20()
{
	FAIL_IF(!SegLineRObj) return;
	SegLineRObj->Remove();
}



void LineEffectClass::Unknown_6E5BB0(Vector3 *start, Vector3 *end)
{
	FAIL_IF(!start || !end) return;
	if (StartOffset != 0.0f || EndOffset != 0.0f) {
		Vector3 dir = *end - *start;
		dir.Normalize();
		if (StartOffset != 0.0f && SourceObject != NULL) {
			float offset = SourceObject->Unknown_30() * StartOffset;
			*start = *start + dir * offset;
		}
		if (EndOffset != 0.0f && TargetObject != NULL) {
			float offset = TargetObject->Unknown_30() * EndOffset;
			*end = *end - dir * offset;
		}
	}
}











void LineEffectClass::Unknown_6E5E00(Vector3 *start, Vector3 *end)
{
	FAIL_IF(!start || !end) return;
	unsigned now = Client->Unknown10BC;
	if (now < StartTime) {
		return; }
	unsigned elapsed = now - StartTime;
	if (GrowTime != -1 && elapsed < GrowTime) {
		float f = (elapsed * GrowA + GrowB) * elapsed;
		if (Get_Flag(14)) {
			Vector3::Lerp(*end, *start, f, start);
		} else {
			Vector3::Lerp(*start, *end, f, end);
		}
		return; }
	if (Duration <= ShrinkTime) {
		return; }
	unsigned shrink_start = Duration - ShrinkTime;
	if (Duration != -1 && ShrinkTime != -1 && elapsed > shrink_start) {
		unsigned t = elapsed - shrink_start;
		float f = (t * ShrinkA + ShrinkB) * t;
		if (Get_Flag(15)) {
			Vector3::Lerp(*end, *start, f, end);
		} else {
			Vector3::Lerp(*start, *end, f, start);
		} }
}


















// NOT EXACT: stack slots only (every instruction is the client's once stack offsets are masked; the ax < ay result temporary stores at 0x20 where the client coalesces it into p at 0x2c, 0x6E6406 / 0x6E6434); attempts.py show 0x6E6030
void LineEffectClass::Unknown_6E6030(Vector3 *start, Vector3 *end)
{
	FAIL_IF(!start || !end) return;
	FAIL_IF(!Camera) return;
	if (Get_Flag(16) || Get_Flag(17)) {
		Vector3 view_start;
		Vector3 view_end;
		Matrix3D::Transform_Vector(Camera->Get_View_Matrix(), *start, &view_start);
		Matrix3D::Transform_Vector(Camera->Get_View_Matrix(), *end, &view_end);
		float znear;
		float zfar;
		Camera->Get_Clip_Planes(znear, zfar);
		view_start.Z += znear;
		view_end.Z += znear;
		if (view_end.Z * view_start.Z <= 0.0f) {
			Vector2 vmin;
			Vector2 vmax;
			Camera->Get_View_Plane(vmin, vmax);
			Vector2 half(vmax.X * znear, vmax.Y * znear);
			float margin = SegLineRObj->Get_Noise_Amplitude();
			margin += SegLineRObj->Get_Width();
			Vector3 delta = view_end - view_start;
			Vector2 xz(delta.X, delta.Z);
			xz.Normalize();
			Vector2 yz(delta.Y, delta.Z);
			yz.Normalize();
			half.X += margin / fabs(xz.Y);
			half.Y += margin / fabs(yz.Y);
			Vector2 inv(1.0f / half.X, 1.0f / half.Y);
			view_start.X *= inv.X;
			view_start.Y *= inv.Y;
			view_end.X *= inv.X;
			view_end.Y *= inv.Y;
			float depth = view_start.Z - view_end.Z;
			Vector2 p;
			if (fabs(depth) >= 0.001f) {
				float k = (1.0f / depth) * view_start.Z;
				p.X = view_start.X + (view_end.X - view_start.X) * k;
				p.Y = view_start.Y + (view_end.Y - view_start.Y) * k;
			} else {
				p.X = view_start.X;
				p.Y = view_start.Y;
			}
			float ax = (float)fabs(p.X);
			float ay = (float)fabs(p.Y);
			if (ax < 1.0f && ay < 1.0f) {
				if (ax < ay) {
					p = p / ay - p;
				} else {
					p = p / ax - p;
				}
				if (Get_Flag(16)) {
					float s = (float)fabs(depth / view_end.Z);
					p *= s;
					Vector3 o(p.X * half.X, p.Y * half.Y, 0.0f);
					const Matrix3D &tm = Camera->Get_Transform();
					*start += Vector3(tm[0][0] * o.X + tm[0][1] * o.Y, tm[1][1] * o.Y + tm[1][0] * o.X, tm[2][1] * o.Y + tm[2][0] * o.X);
				} else {
					float s = (float)fabs(depth / view_start.Z);
					p *= s;
					Vector3 o;
					o.X = p.X * half.X;
					o.Y = p.Y * half.Y;
					const Matrix3D &tm = Camera->Get_Transform();
					*end += Vector3(tm[0][0] * o.X + tm[0][1] * o.Y, tm[1][1] * o.Y + tm[1][0] * o.X, tm[2][1] * o.Y + tm[2][0] * o.X);
				}
			}
		}
	}
}

/*
**	Notes (reconstruction)
**
**	The client's eff_line.obj: 0x6E5040-0x6E6710 (its <string> guarded static ends at 0x6E66E0; eff_o.cpp's
**	EffectObjectClass constructor 0x6E6710 opens the next object). __FILE__ is
**	'e:\build\beta\G\app\Client\eff_line.cpp' (0xB92DDC): FAIL_IF '!SegLineRObj' on lines 51, 138, 146 and
**	180, '!start || !end' on 188, 215 and 261, '!Camera' on 262. effect.ini's EffectClass 'LineEffect'
**	(0xB92F4C) is this class: eff_o.cpp's helper 0x6EAC50 makes it for a segmented line model.
**
**	Emitted here as well: the base's inline virtuals EffectObjectClass::Unknown_10 0x6E5420,
**	UnknownB0009CEffectClass::Unknown_14 0x6E5440, Unknown_30 0x6E5460 and Unknown_38 0x6E54A0, the
**	scalar deleting destructor 0x6E54E0 and the binary end's implicit destructor 0x6E5510 (inlined by
**	0x6E5600).
**
**	The line's two getters 0x9691B0 and 0x9692B0 (segline.cpp, not yet named in the client's map) are
**	called as Get_Noise_Amplitude and Get_Width: their order in segline.cpp (0x9692B0 after Set_Shader
**	0x969290) and their reads (+0xC4, +0xAC); the call bytes match either way. 0x9691D0 (+0x8C, an int)
**	is Get_Subdivision_Levels: Unknown_1C gives the value back through Set_Subdivision_Levels.
**
**	Unknown_6E6030's two camera-offset arms differ in the client: the Get_Flag(16) arm loads rows 2 and 3 X term
**	first (0x6E649A), the other arm Y term first (0x6E6529). VC6 orders those products by how o was made: by
**	Vector3's constructor (the Z store is dead) in the first arm, by member stores in the second.
*/
