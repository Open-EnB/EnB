// mouse_targeting.cpp: which object the mouse is over (0x7B77C0-0x7B7B4F); notes at the end
#include <string>
#include "mouse_targeting.h"
#include "radar_icon_data.h"
#include "rendobj.h"
#include "intersec.h"
#include <math.h>
#include "vector2.h"
#include "..\shared\g_assert.h"

// @0x7B77C0
MouseTargetClass::MouseTargetClass(ClientObjectClass *self) :
	Self(self),
	Target(NULL),
	Mouse(0.0f, 0.0f),
	HalfSize(0.0f, 0.0f),
	TargetPriority(0),
	TargetDistance(FLT_MAX),
	SelfPriority(0)
{
}

// @0x7B7810
void MouseTargetClass::Set(float x, float y, float width, float height, const LayerClass &layer)
{
	Mouse.Set(x, y);
	HalfSize.Set(width * 0.5f, height * 0.5f);
	Layer.Set(layer);
	TargetPriority = 0;
	TargetDistance = FLT_MAX;
}































// @0x7B7880
void MouseTargetClass::Test(RadarIconDataClass *rdata, const Vector3 &pos, float size, float scale)
{
	FAIL_IF(!rdata) return;

	ClientObjectClass *co = rdata->Get_Client_Object();
	int priority = 0;
	float distance = 0.0f;
	Vector2 abs_delta = rdata->Position - Mouse;
	abs_delta.X = fabs(abs_delta.X);
	abs_delta.Y = fabs(abs_delta.Y);
	if ((abs_delta.X <= HalfSize.X) && (abs_delta.Y <= HalfSize.Y)) {
		distance = abs_delta.Y + abs_delta.X;
		priority = 3;
	} else if (pos.Z < 0.0f) {
		Vector2 delta(pos.X - Mouse.X, pos.Y - Mouse.Y);
		float distance2 = delta.Length2();
		if (distance2 <= scale * scale) {
			priority = 1;
			distance = distance2;
		}
		RefCountPtr<RenderObjClass> robj = rdata->Get_Render_Obj();
		IntersectionResultClass result;
		if (robj && (distance2 <= size * size)) {
			IntersectionClass intersection;
			if (intersection.Intersect_Sub_Object(Mouse.X, Mouse.Y, Layer, robj.Peek(), &result)) {
				distance = -pos.Z;
				priority = 2;
			}
		}
	}
	if (co == Self) {
		SelfPriority = priority;
	} else if (priority != 0) {
		bool higher = (priority > TargetPriority);
		bool closer = ((priority == TargetPriority) && (distance < TargetDistance));
		if (higher || closer) {
			Target = co;
			TargetDistance = distance;
			TargetPriority = priority;
		}
	}
}

/*
**	Notes
**
**	The object: 0x7B77C0 (the client's __FILE__ 0xB9EE10; the FAIL_IF of 0x7B7880 at line 66) to 0x7B7B4F, the
**	guarded initialiser <string> leaves at each object's end (0xC416FC, atexit 0x42B320, table entry 0xB6CD04). The
**	next object is nav_list's, 0x7B7BF0. Callers: radar_system.cpp's 0x7BCA70 makes it on its stack; gutter_radar.cpp's
**	Set_Targets and Update, starmap_pip.cpp's 0x7C0440 and 0x7C0620 use it.
*/
