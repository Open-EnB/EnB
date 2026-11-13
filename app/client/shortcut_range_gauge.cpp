// Earth & Beyond client (reconstruction): app\client\shortcut_range_gauge.cpp, the range gauge on a shortcut's slot (the
// client's __FILE__ 'e:\build\beta\G\app\Client\shortcut_range_gauge.cpp', 0x6A3D60-0x6A4570). Notes at the end.
#include <math.h>
#include "shortcut_range_gauge.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "ww3d.h"
#include "..\shared\uvcontrl.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's

// 0x6A4500 (no callers: the functions below inline it; the client emits it last)
void ShortcutRangeGaugeClass::Unknown_6A4500()
{
	if (Sound) {
		Sound->Stop(1);
		Sound = NULL;
		SoundMode = 0;
	}
}
















// 0x6A3D60
ShortcutRangeGaugeClass::ShortcutRangeGaugeClass(RenderObjClass *robj) :
	Unknown04(NULL),
	Frame(-1),
	Angle(0),
	SoundMode(0)
{
	RenderObject = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("if_targ"));
	FAIL_IF(!RenderObject) return;
	Unknown_496AE0(RenderObject.Peek(), false);
	RenderObjClass *container = robj->Get_Container();
	if (!container) container = robj;
	int bone = container->Get_Sub_Object_Bone_Index(robj);
	container->Add_Sub_Object_To_Bone(RenderObject.Peek(), bone);
	Unknown04 = new Unknown4A13F0Class(RenderObject.Peek(), 0, 0);
	Unknown_6A4020(1.0f, 0.0f, 0, false);
}

// 0x6A3F10
ShortcutRangeGaugeClass::~ShortcutRangeGaugeClass()
{
	delete Unknown04;
	Unknown04 = NULL;
	RenderObject = NULL;
	Unknown_6A4500();
}

// NOT EXACT: functional from the pseudocode (D64); the hidden case and the turn's factors are laid out in another order
// 0x6A4020
void ShortcutRangeGaugeClass::Unknown_6A4020(float distance, float range, int state, bool arg)
{
	if (!Unknown04) return;
	float double_range = range * 2;
	if (!state || distance < 0.0f || range <= 0.0f) {
		if (!RenderObject->Is_Hidden()) RenderObject->Set_Hidden(1);
		Unknown_6A4500();
		return;
	}
	if (RenderObject->Is_Hidden()) RenderObject->Set_Hidden(0);
	float old_angle = Angle;
	int old_frame = Frame;
	if (distance > double_range) {
		Frame = 0;
		Unknown_6A4500();
	} else if (distance > range) {
		Frame = (int)((1.0f - (distance - range) / range) * 15.0f);
		if (Angle > 0.0001f) {
			Angle *= 0.8f;
		} else {
			Angle = 0;
		}
		if (arg) Unknown_6A43D0(1);
	} else if (state == 1) {
		Frame = 14;
		if (arg) Unknown_6A43D0(state);
	} else {
		if (Frame != 15) Frame = 15;
		Angle += (1.0f - distance / range) * (WW3D::Get_Frame_Time() * 0.002f);
		if (Angle > WWMATH_PI / 2) {
			Angle = fmod(Angle, WWMATH_PI / 2);
		}
		if (arg) Unknown_6A43D0(2);
	}
	if (Frame != old_frame) {
		Unknown04->Unknown_4A1550(Frame);
	}
	if (Angle != old_angle) {
		Matrix3D tm(RenderObject->Get_Position());
		tm.Rotate_Z(-Angle);
		RenderObject->Set_Transform(tm);
	}
}

// 0x6A43D0
void ShortcutRangeGaugeClass::Unknown_6A43D0(int mode)
{
	if (SoundMode != mode) {
		Unknown_6A4500();
		if (mode != 1) {
			Sound = Create_Get(SoundLibraryClass::Play_2D_Sound("weapon_range_green_1.wav", 1));
		}
		SoundMode = mode;
	}
}


/*
**	Reconstruction notes (O18, 2026-11-13)
**
**	The object: ship_stats_display.cpp's <string> statics end at 0x6A3D46; this one runs from the constructor 0x6A3D60
**	to Unknown_6A4500 0x6A4500 (no callers: its three callers inline it) and its guarded <string> static 0x6A4550; the
**	next object opens with permstr.h's PermStringInitClass static 0x6A4580.
**	Not exact (one round): Unknown_6A4020 puts the hidden case after the rest (the client: right after its tests) and
**	takes 1 - distance / range before the frame time (the client: the frame time first); an else for the
**	return, !(range > 0) and three orders of the turn's product did not move either.
*/
