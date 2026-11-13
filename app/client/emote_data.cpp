// Earth & Beyond client (reconstruction): app\client\emote_data.cpp. Notes at the end of the file.
#include "emote_data.h"
#include "co.h"
#include "client.h"
#include "sclient.h"
#include "postate.h"
#include "camera_manager.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\Sound3D.h"
#include "matrix3d.h"
#include <math.h>
#include "wwmath.h"
#include "..\shared\g_assert.h"

static const float WaveTime = 2000.0f;
static const float LoopTime = 3000.0f;
static const float RollTime = 2000.0f;
static const float SpinTime = 1000.0f;

static const char *EmoteSounds[] = {
	"wave.wav",
	"loop.wav",
	"loop.wav",
	"roll.wav",
	"roll.wav",
	"spin.wav",
	"spin.wav",
};

EmoteDataClass::EmoteDataClass(ClientObjectClass *co) :
	ClientObjectDataClass(co),
	StartTime(0),
	Emote(7),
	Unknown40(true)
{
}

EmoteDataClass::~EmoteDataClass()
{
	Unknown_7E79D0();
	Sound = NULL;
}

void EmoteDataClass::Unknown_7E6650(int emote)
{
	Queue.push_back(emote);
}











































// NOT EXACT: control flow (the switch's default reaches the stop block placed after the compare in the client, before the active block in this build)
void EmoteDataClass::Unknown_7E6860()
{
	if (!Unknown40) {
		return;
	}
	if (Emote == 7) {
		if (Queue.empty()) {
			return;
		}
		Unknown_7E7780();
		Emote = Queue.front();
		Queue.pop_front();
		StartTime = Get_Client()->Unknown10BC;
		if (Peek_Client_Object() != 0 && Peek_Client_Object()->Unknown_6F5A40()) {
			SoundLibraryClass::Play_2D_Sound(EmoteSounds[Emote], 2);
		} else {
			Sound = Create_Get(SoundLibraryClass::Play_3D_Sound(EmoteSounds[Emote], Matrix3D(true), 1, 1.0f));
		}
	}
	unsigned int elapsed = Get_Client()->Unknown10BC - StartTime;
	float t;
	switch (Emote) {
		case 0:
			t = elapsed / WaveTime;
			break;
		case 1:
		case 2:
			t = elapsed / LoopTime;
			break;
		case 3:
		case 4:
			t = elapsed / RollTime;
			break;
		case 5:
		case 6:
			t = elapsed / SpinTime;
			break;
		default:
			goto done;
	}
	if (t <= 1.0f) {
		Matrix3D tm = Unknown_7E70A0(t);
		Matrix3D base(true);
		FAIL_IF(!Peek_Client_Object()->Unknown98) return;
		FAIL_IF(Peek_Client_Object()->Unknown98->Unknown_04() != 14) return;



		Unknown98Class *state = Peek_Client_Object()->Unknown98;
		Unknown98Class *inner = state->Unknown_28();
		if (inner != 0) {
			base = inner->Unknown_08(Get_Client()->Unknown10BC);
		}
		Matrix3D result = base * tm;
		((Unknown718D70Class *)state)->Unknown_30(Get_Client()->Unknown10BC, result);
		if (Sound) {
			Sound->Set_Transform(result);
		}
	} else {
done:
		Unknown_7E7620();
	}
}

// NOT EXACT: frame (the client's frame has 4 more bytes of locals; the cases' x87 code follows from it)
Matrix3D EmoteDataClass::Unknown_7E70A0(float t)
{
	Matrix3D tm(true);
	switch (Emote) {
		case 0:
			tm.Rotate_X(WWMath::Sin(t * 12.566371f) * 0.25f);
			break;
		case 1:
		{
			float radius = Peek_Client_Object()->Unknown_30() * 2.0f;
			float angle = (t - 0.25f) * 6.2831855f;
			float s = WWMath::Sin(angle);
			float c = WWMath::Cos(angle);
			Vector3 pos(c * radius, 0.0f, (s + 1.0f) * radius);
			float xs = WWMath::Sin(t * 6.2831855f);
			Vector3 x(WWMath::Cos(t * 6.2831855f), 0.0f, xs);
			x.Normalize();
			Vector3 y(0.0f, 1.0f, 0.0f);
			Vector3 z;
			Vector3::Cross_Product(x, y, &z);
			z.Normalize();
			tm.Set(x, y, z, pos);
			break;
		}
		case 2:
		{
			float radius = Peek_Client_Object()->Unknown_30() * 2.0f;
			float angle = (0.25f - t) * 6.2831855f;
			float s = WWMath::Sin(angle);
			float c = WWMath::Cos(angle);
			Vector3 pos(c * radius, 0.0f, (s - 1.0f) * radius);
			float xs = WWMath::Sin(t * -6.2831855f);
			Vector3 x(WWMath::Cos(t * -6.2831855f), 0.0f, xs);
			x.Normalize();
			Vector3 y(0.0f, 1.0f, 0.0f);
			Vector3 z;
			Vector3::Cross_Product(x, y, &z);
			z.Normalize();
			tm.Set(x, y, z, pos);
			break;
		}
		case 3:
			tm.Rotate_X(t * 6.2831855f);
			break;
		case 4:
			tm.Rotate_X(t * -6.2831855f);
			break;
		case 5:
			tm.Rotate_Z(t * -12.566371f);
			break;
		case 6:
			tm.Rotate_Z(t * 12.566371f);
			break;
	}
	return(tm);
}

void EmoteDataClass::Unknown_7E7620()
{
	Emote = 7;
	Unknown_7E79D0();
	unsigned id = Peek_Client_Object()->GameID;
	if (id == ((SClient *)Unknown_18())->AvatarID) {
		((SClient *)Unknown_18())->CameraManager->Unknown_79B2B0(0x15);
	}
}

void EmoteDataClass::Unknown_7E76E0()
{
	Unknown40 = true;
}

void EmoteDataClass::Unknown_7E7700()
{
	Unknown40 = false;
	Emote = 7;
	Unknown_7E79D0();
}































void EmoteDataClass::Unknown_7E7780()
{
	Unknown98Class *state = Peek_Client_Object()->Unknown98;
	Matrix3D tm(true);
	if (state != 0) {
		switch (state->Unknown_04()) {
			case 12:
			case 13:
			case 14:
				G_ASSERT(0);
				return;
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				tm = state->Unknown_08(Get_Client()->Unknown10BC);
				break;
			default: G_ASSERT(0);
				return;
		}
	}
	Peek_Client_Object()->Unknown98 = new Unknown718D70Class(Get_Client()->Unknown10BC, tm, state);
}

void EmoteDataClass::Unknown_7E79D0()
{
	Unknown98Class *state = Peek_Client_Object()->Unknown98;
	if (state != 0 && state->Unknown_04() == 14) {
		Peek_Client_Object()->Unknown98 = Peek_Client_Object()->Unknown98->Unknown_28();
		delete state;
	}
}

bool EmoteDataClass::Unknown_7E7A40()
{
	return((Emote == 7 && !Queue.empty()) ? true : false);
}

bool EmoteDataClass::Unknown_7E7A70()
{
	return(Emote == 1 || Emote == 2);
}

/*
**	Notes (reconstruction)
**
**	The client's emote_data.obj: 0x7E62F0-0x7E7E60, after distance_calc_client.cpp (its guarded
**	initialiser ends at 0x7E62E0) and before the check button class at 0x7E7E70 (vtable 0xB075E4).
**	__FILE__ is 'e:\build\beta\G\app\Client\emote_data.cpp' (0xBA14AC): the FAIL_IFs on lines 135
**	and 136 (0x7E6860) and the G_ASSERT(0)s on lines 275 and 291 (0x7E7780). The constructor
**	0x7E62F0 sets the vtable 0xB075CC, whose typeid name is co_data.cpp's '.PAVEmoteDataClass@@'.
**
**	The emote sounds 0xBA13D8 (wave, loop, roll, spin: two of each but the wave) and the four
**	durations 0xB07598-0xB075A4 (2000, 3000, 2000, 1000 ms, each its own float: the client divides
**	by them, fdiv, where a literal divisor compiles to a multiply) are this file's.
**	The template instances after the file's functions: std::deque<int>'s destructor 0x7E6570,
**	operator*(Matrix3D, Matrix3D) 0x7E6E40, Create_Get 0x7E7AA0 and ~RefCountPtr<Sound3DClass>
**	0x7E7AD0, deque's _Buyback 0x7E7B10, _Freefront 0x7E7CD0, _Growmap 0x7E7D70 and its iterator's
**	constructor 0x7E7E00. 0x7E79D0 is inlined by the destructor, 0x7E6860, 0x7E7620 and 0x7E7700;
**	0x7E7620 has no caller (0x7E6860 inlines it).
**
**	The asserts' text names what this file calls Unknown98, Unknown_04 and 14:
**	'Peek_Client_Object()->PositionalState' and 'Peek_Client_Object()->PositionalState->Get_Type() !=
**	PositionalStateClass::CLIENT_SIDE_PROCEDURAL' (co.h's Unknown98Class, postate.h's type 14,
**	Unknown718D70Class); the text differs from the client's until co.h takes those names.
*/
