// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown. PLACEMENT: provisional (D137): the object 0x6F25A0-0x6F27C0 has no __FILE__ and
**	no assert; it links after eff_seek.cpp (whose guarded <string> static is 0x6F2570) and ends with its own
**	(0x6F27A0), so its name sorts after eff_seek's. A string, or another object's name that narrows the order,
**	would place it.
**
**	The warp effect: see eff_warp.h.
*/

#include "eff_warp.h"
#include "co.h"
#include "wwmath.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight  // NAME 1 [robj, a, b, c]: as acticon.cpp's

// 0x6F25A0
WarpEffectClass::WarpEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj) :
	RObjEffectClass(client, id, start_time, duration, cycle_time, robj),
	UnknownDC(0.0f)
{
	Unknown_496AE0(RObj, false);
}

// 0x6F2690
void WarpEffectClass::Unknown_1C()
{
	RObjEffectClass::Unknown_1C();
	bool fading = false;
	if (SourceObject == NULL || ((int)SourceObject->Get_Game_ID() >= 0 && SourceObject->UnknownF8 != 2)) {
		UnknownDC = WWMath::Max(0.0f, UnknownDC - 0.05f);
		fading = true;
		if (UnknownDC < 0.1f) {
			Unknown_6E6E30();
			return;
		}
	}
	if (UnknownDC != 1.0f) {
		if (!fading) {
			UnknownDC = WWMath::Min(1.0f, UnknownDC + 0.05f);
		}
		float level = UnknownDC;
		Unknown_496110(RObj, level, level, level);
	}
}
