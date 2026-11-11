// Earth & Beyond client (reconstruction): acticon.cpp. Notes at the end of the file.
#include <math.h>
#include "acticon.h"
#include "rendobj.h"
#include "vector2.h"
#include "vector3.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\g_assert.h"

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.h's






















ActIconClass::ActIconClass(RenderObjClass *robj, RenderObjClass *bar, const LayerClass &layer) :
	Unknown_671300Class(robj, (int)bar, layer, 0, 0, 3),
	Bar(Unknown_497300(bar)),
	State(0),
	Duration(0.0f),
	EndTime(0),
	Flash(0.0f)
{
	if (Bar) {
		Unknown_496110(Bar.Peek(), 1.0f, 0.0f, 0.0f);
	}
}







void ActIconClass::Set_State(int state, unsigned int end_time)
{
	State = state;
	unsigned int now = WW3D::Get_Sync_Time();
	EndTime = end_time;
	Unknown6C = 0.0f;
	if (state == 1 || state == 4) {
		Unknown70 = 0.0f;
		if (end_time < 0x80000000 && end_time > now) {
			Duration = (end_time - now) * 0.001f;
		} else {
			State = 0;
		}
	} else if (state == 2 || state == 3) {
		Flash = 1.0f;
	}
	if (Unknown68) {
		if (State == 5) {
			Unknown68->Unknown_4A13A0(Vector2(0.0f, 0.46f));
		} else if (State == 0) {
			Unknown68->Unknown_4A13A0(Vector2(0.0f, 0.0f));
		}
	}
}


void ActIconClass::Unknown_AC()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	if (Flash > 0.0f) {
		float fade = 1.0f - Flash;
		Unknown_66C220(1.0f, fade, fade);
		float flash = Flash - time;
		if (0.0f > flash) {
			flash = 0.0f;
		}
		Flash = flash;
		if (Flash == 0.0f) {
			Unknown_66C260(Unknown_60() ? Unknown_A0() : Unknown_9C());
		}
	}
	if (Unknown68 && Unknown74) {
		if (State == 1) {
			Unknown6C += time;
			float f = Unknown6C / Duration;
			if (1.0f < f) {
				f = 1.0f;
			}
			Unknown68->Unknown_4A13A0(Vector2(0.0f, f * 0.46f));
			return;
		}
		if (State == 4) {
			Unknown6C += time;
			float f = Unknown6C / Duration;
			if (1.0f < f) {
				f = 1.0f;
			}
			Unknown68->Unknown_4A13A0(Vector2(0.0f, (1.0f - f) * 0.46f));
			Unknown_496110(Bar.Peek(), 1.0f, 0.0f, 0.0f);
			return;
		}
		Unknown6C += time * Unknown70 * 8.0f;
		Unknown_496110(Bar.Peek(), (float)((cos(Unknown6C) + 1.0) * 0.5), 0.0f, 0.0f);
	}
}
