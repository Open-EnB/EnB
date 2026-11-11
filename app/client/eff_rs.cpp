// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown. PLACEMENT: provisional (D137): the object 0x6F0200-0x6F0580 has no __FILE__ and
**	no assert; it links between eff_robj.cpp (whose guarded <string> static is 0x6F01D0) and eff_seek.cpp
**	(its own static is 0x6F0550), so its name sorts between theirs. A string, or another object's name
**	that narrows the order, would place it.
**
**	The render-state effect: see eff_rs.h.
*/

#include "eff_rs.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "..\shared\rs_track.h"

// 0x6F0200
RenderStateEffectClass::RenderStateEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, int render_state, int action, unsigned action_time) :
	UnknownB00060EffectClass(client, id, start_time, duration, cycle_time),
	RenderStateID(render_state),
	ActionType(action),
	ActionTime(action_time)
{
}

// 0x6F0380
Vector3 RenderStateEffectClass::Unknown_08(const Vector3 &)
{
	return(Transform.Get_Translation());
}

// 0x6F03C0
void RenderStateEffectClass::Unknown_18()
{
	UnknownB00060EffectClass::Unknown_18();
	if (SourceObject && RenderStateID >= 0) {
		RenderObjClass *robj = SourceObject->Unknown_6F3BC0();
		if (robj) {
			if (robj->Class_ID() == 0x10001) {
				RsTrackClass *track = (RsTrackClass *)robj;
				switch (ActionType) {
					case 0:
						if (track->Unknown_460810(RenderStateID) && Get_Flag(9)) {
							track->Unknown_460780(RenderStateID);
						}
						track->Unknown_4604D0(RenderStateID);
						break;
					case 1:
						track->Unknown_460780(RenderStateID);
						break;
					case 2:
						track->Unknown_4605A0(RenderStateID, ActionTime);
						break;
					case 3:
						if (track->Unknown_460810(RenderStateID) && Get_Flag(9)) {
							track->Unknown_460780(RenderStateID);
						}
						track->Unknown_4605E0(RenderStateID, ActionTime);
						break;
					case 4:
						track->Unknown_460850(RenderStateID);
						break;
				}
			}
			robj->Release_Ref();
		}
	}
}
