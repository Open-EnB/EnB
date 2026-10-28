/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\eff_o.cpp (the client's __FILE__)
**
**	The effect object's base (client 0x6E6710-0x6E6F60 so far): see eff_o.h. Notes at the end.
*/

#include "eff_o.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "camera.h"
#include "random.h"
#include "..\..\wwaudio\Sound3D.h"

void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 (unknown_debug.h's reporter)
EffectObjectClass *Unknown_6E9490(int desc_id, int effect_id, int duration, int time, ClientClass *client, float scale, const Vector3 &hsv_shift, float speedup, int linked_desc_id, bool unknown);	// 0x6E9490 (the factory, not rebuilt yet)  // NAME 1 [desc_id .. unknown]: what the effect packets pass
extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as ui_line_display.cpp's

// 0x6E6710
EffectObjectClass::EffectObjectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	Client(client),
	StartTime(start_time),
	CycleTime(cycle_time),
	ID(id),
	Unknown34(NULL),
	Unknown38(NULL),
	Unknown3C(0),
	Unknown40(0),
	Unknown44(0),
	Unknown48(0),
	Unknown4C(NULL),
	Camera(NULL),
	StartLinkID(-1),
	NextLinkID(-1),
	StartLink(NULL),
	NextLink(NULL),
	Unknown64(0),
	Scale(1.0f),
	HSVShift(0, 0, 0),
	Speedup(1.0f),
	Flags(0)
{
	Unknown_6E6F90(duration);
	Node.Set(this);
	Flag.State = 0;
}

// 0x6E6850
void EffectObjectClass::Unknown_04(int)
{
}

// 0x6E6870
Vector3 EffectObjectClass::Unknown_08(const Vector3 &)
{
	return(Vector3(0, 0, 0));
}

// 0x6E68A0
void EffectObjectClass::Unknown_18()
{
}

// 0x6E68C0
void EffectObjectClass::Unknown_1C()
{
}

// 0x6E68E0
void EffectObjectClass::Unknown_20()
{
}

// 0x6E6A90
void Unknown_6E6A90(EffectObjectClass *effect)
{
	if (effect->Flag.State == 1) {
		effect->Unknown_6E7330();
		effect->Unknown_20();
	}
	delete effect;
}

// 0x6E6930
EffectObjectClass::~EffectObjectClass()
{
	REF_PTR_RELEASE(Unknown4C);
	REF_PTR_RELEASE(Camera);
	if (Unknown34) {
		delete [] Unknown34;
		Unknown34 = NULL;
	}
	if (StartLink) {
		Unknown_6E6A90(StartLink);
	}
	if (NextLink) {
		Unknown_6E6A90(NextLink);
	}
}














































// NOT EXACT: register-only (the NextLink factory call: the client loads the time into ecx first and the floats through edi; the NextLink delete in ecx; 9 forms)
// 0x6E6AD0
bool EffectObjectClass::Unknown_6E6AD0()
{
	FAIL_IF(!Camera) return(true);
	bool done = false;
	switch (Flag.State) {
		case 0:
			if (Client->Unknown10BC - StartTime >= 0x80000000) {
				break;
			}
			StartTime = Client->Unknown10BC;
			Flag.State = 1;
			if (!(bool)Flag.Unknown3) {
				Unknown_6E70F0();
			}
			Unknown_18();
			Unknown_6E6FE0();
		case 1:
			if (Duration != (unsigned)-1 && (done = (Client->Unknown10BC - StartTime - Duration < 0x80000000)) != false) {
				Flag.State = 2;
				Unknown_20();
				if (NextLinkID >= 0) {
					NextLink = Unknown_6E9490(NextLinkID, 0, 0, Client->Unknown10BC, Client, Scale, HSVShift, Speedup, -1, false);
					Unknown_24(1);
				}
				Unknown_6E7330();
			} else {
				Unknown_1C();
				if ((bool)Flag.Unknown3) {
					Vector3 camera = Camera->Get_Position();
					if ((Unknown_08(camera) - camera).Length2() <= Unknown3C && Client->Unknown10BC - Unknown48 < 0x80000000) {
						Unknown_6E70F0();
						int delay = Unknown_BDBB84(Unknown40, Unknown44);
						Unknown48 = Client->Unknown10BC + delay;
					}
				}
				if (Unknown38) {
					Sound3DClass *sound = Unknown38->As_Sound3DClass();
					if (sound) {
						sound->Set_Position(Unknown_08(Camera->Get_Position()));
					}
				}
				break;
			}
		case 2:
			done = true;
			if (NextLink && !NextLink->Unknown_6E6AD0()) {
				Unknown_6E6A90(NextLink);
				NextLink = NULL;
			}
			break;
	}
	if (StartLink && !StartLink->Unknown_6E6AD0()) {
		Unknown_6E6A90(StartLink);
		StartLink = NULL;
	}
	return(!done || StartLink || NextLink);
}

// 0x6E6E30
void EffectObjectClass::Unknown_6E6E30()
{
	unsigned elapsed = Client->Unknown10BC - StartTime;
	if (elapsed < 0x80000000) {
		Unknown_6E6F90(elapsed);
	} else {
		Unknown_6E6F90(0);
	}
	Set_Flag(4, true);
	if (StartLink) {
		StartLink->Unknown_6E6E30();
	}
	if (NextLink) {
		NextLink->Unknown_6E6E30();
	}
}

// 0x6E6ED0
void EffectObjectClass::Unknown_6E6ED0(int arg)
{
	CameraClass *camera = (CameraClass *)arg;
	REF_PTR_SET(Camera, camera);
}

// 0x6E6F10
void EffectObjectClass::Unknown_0C(unsigned duration)
{
	Unknown_6E6F90(duration);
	if (StartLink && (bool)Flag.Unknown5) {
		StartLink->Unknown_0C(duration);
	}
}

// 0x6E74B0
void EffectObjectClass::Unknown_6E74B0(int arg)
{
	RenderObjClass *robj = (RenderObjClass *)arg;
	if (robj != Unknown4C) {
		if (Flag.State == 1) {
			Unknown_20();
			REF_PTR_SET(Unknown4C, robj);
			Unknown_18();
		} else {
			REF_PTR_SET(Unknown4C, robj);
		}
	}
}

// 0x6E7540
UnknownB00060EffectClass::UnknownB00060EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	EffectObjectClass(client, id, start_time, duration, cycle_time),
	SourceObject(NULL),
	SourceNode(NULL),
	Transform(true)
{
}

// 0x6E7690
void UnknownB00060EffectClass::Unknown_04(int arg)
{
	if ((DataNode<UnknownAF0DD0Class *> *)arg == &SourceNode) {
		SourceObject = NULL;
		if ((bool)Flag.Unknown8) {
			Unknown_6E6E30();
		}
	}
}







































































































































































































// 0x6E7770
void UnknownB00060EffectClass::Unknown_28(int game_id)
{
	SourceObject = Client->Find_Object(game_id);
	if (SourceObject) {
		Unknown_73FDB0(&SourceNode, SourceObject);
		Unknown_6E7AA0();
		return;
	}
	Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Set_Source_Object_ID couldn't find the object!");
	Flag.State = 2;
	Unknown_20();
	Unknown_6E7330();
}

// 0x6E7810
void UnknownB00060EffectClass::Unknown_6E7810(int game_id, const LegoIdentifierClass &lego)
{
	SourceLego = lego;
	Unknown_28(game_id);
}

// 0x6E7850
void UnknownB00060EffectClass::Unknown_18()
{
	if (SourceObject) {
		Unknown_6E7AA0();
	}
}

// 0x6E7870
void UnknownB00060EffectClass::Unknown_1C()
{
	if (SourceObject && (bool)Flag.Unknown7) {
		Unknown_6E7AA0();
	}
}

// 0x6E7D70
UnknownB0009CEffectClass::UnknownB0009CEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	EffectObjectClass(client, id, start_time, duration, cycle_time),
	SourceObject(NULL),
	TargetObject(NULL),
	SourceNode(NULL),
	TargetNode(NULL),
	SourcePosition(0, 0, 0),
	TargetPosition(0, 0, 0),
	TargetOffset(0, 0, 0)
{
}

// NOT EXACT: inline-budget (the client calls Set_Flag 0x6E8090 out of line in the source branch's inlined 0x6E6E30; ours inlines both)
// 0x6E7ED0
void UnknownB0009CEffectClass::Unknown_04(int arg)
{
	if ((DataNode<UnknownAF0DD0Class *> *)arg == &SourceNode) {
		SourceObject = NULL;
		if ((bool)Flag.Unknown9) {
			Unknown_6E6E30();
		}
	} else if ((DataNode<UnknownAF0DD0Class *> *)arg == &TargetNode) {
		TargetObject = NULL;
		if ((bool)Flag.Unknown10) {
			Unknown_6E6E30();
		}
	}
}



















































































// 0x6E80D0
void UnknownB0009CEffectClass::Unknown_28(int game_id)
{
	SourceObject = Client->Find_Object(game_id);
	if (SourceObject) {
		Unknown_73FDB0(&SourceNode, SourceObject);
		Unknown_6E8F80();
		return;
	}
	Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Set_Source_Object_ID couldn't find the object!");
	Flag.State = 2;
	Unknown_20();
	Unknown_6E7330();
}

// 0x6E8170
void UnknownB0009CEffectClass::Unknown_2C(int game_id, const LegoIdentifierClass &lego)
{
	SourceLego = lego;
	Unknown_28(game_id);
}






// 0x6E81B0
void UnknownB0009CEffectClass::Unknown_34(int game_id)
{
	TargetObject = Client->Find_Object(game_id);
	Unknown_73FDB0(&TargetNode, TargetObject);
	if (TargetObject) {
		TargetPosition = TargetObject->Unknown_24() + TargetOffset;
		return;
	}
	Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Set_Target_Object_ID couldn't find the object!");
	Flag.State = 2;
	Unknown_20();
	Unknown_6E7330();
}

// 0x6E87C0
void UnknownB0009CEffectClass::Unknown_6E87C0(const Vector3 &offset)
{
	TargetOffset = offset;
}

// 0x6E8800
void UnknownB0009CEffectClass::Unknown_18()
{
	if (SourceObject) {
		Unknown_6E8F80();
	}
	if (TargetObject) {
		TargetPosition = TargetObject->Unknown_24() + TargetOffset;
	}
}

























































































































































































// 0x6E9190
void UnknownB0009CEffectClass::Unknown_6E9190()
{
	FAIL_IF(!TargetObject) return;
	TargetPosition = TargetObject->Unknown_24() + TargetOffset;
}

/*
**	Reconstruction notes (EF1, 2026-10-28)
**
**	The window: eff_line.cpp's <string> guarded static ends at 0x6E66E0; the base's constructor
**	0x6E6710 is this file's first function. The base's slots +0x10 / +0x14 are inline in eff_o.h
**	(their copies 0x6E5420 / 0x6E4860 sit in eff_line.cpp's and eff_attach.cpp's windows), the
**	empty slots +0x04 / +0x18 / +0x1C / +0x20 are here. Unknown_6E6A90 is called directly by
**	SClient (Shutdown, Process_AI) and inlined here, so it comes before the destructor. Slot +0x0C
**	tests flag 0x20 as (bool) (shr 5 / test 1; the plain bitfield test is a test byte).
*/
