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
#include "rendobj.h"
#include "camera.h"

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





















































// 0x6E6AD0: not rebuilt yet (its FAIL_IF '!Camera' sits on the client's line 154, D25)






















































































// 0x6E6E30
void EffectObjectClass::Unknown_6E6E30()
{
	unsigned elapsed = Client->Unknown10BC - StartTime;
	if (elapsed < 0x80000000) {
		Unknown_6E6F90(elapsed);
	} else {
		Unknown_6E6F90(0);
	}
	Flag.Unknown2 = 1;
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
