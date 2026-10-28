// clienttest: logging stand-ins for what client.cpp, sclient.cpp and cpacket.cpp call in files that
// aren't rebuilt (or aren't compiled into the harness). Each appends one line to the trace and does
// nothing else; where a caller branches on the result, the result is the harness's (clienttest.h).
//
// Not run, by design (D69): SOptionsClass's constructor 0x75A280 reads the registry, so soptions.cpp is
// not compiled in and every member SClient reaches is a stand-in below (app.lib's soptions.obj is then never
// pulled: one member missing here and the linker brings it in, LNK2005 on the others).
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "clienttest.h"
#include "..\..\..\app\client\sclient.h"
#include "..\..\..\app\common\connect.h"
#include "..\..\..\app\packets\pknamedecal.h"
#include "..\..\..\app\packets\pkguildranknamesrequestclient.h"
#include "..\..\..\app\packets\pkupdate.h"
#include "..\..\..\app\packets\pkcolorization.h"
#include "rect.h"
#include "matrix3d.h"

// ------------------------------------------------------------------------- soptions.cpp (registry: never run)

SOptionsClass::SOptionsClass() { Trace("stub SOptionsClass::SOptionsClass (0x75A280, registry: not run)"); }
SOptionsClass::~SOptionsClass() { Trace("stub SOptionsClass::~SOptionsClass"); }
void SOptionsClass::Set_Camera_Mode(unsigned cameramode) { Trace("stub SOptionsClass::Set_Camera_Mode %u", cameramode); }
void SOptionsClass::Unknown_75A570() { Trace("stub SOptionsClass::Unknown_75A570"); }
void SOptionsClass::Unknown_75A5A0() { Trace("stub SOptionsClass::Unknown_75A5A0"); }
void SOptionsClass::Unknown_75A5D0() { Trace("stub SOptionsClass::Unknown_75A5D0"); }
void SOptionsClass::Load_Camera_Mode(INIClass &, const char *section) { Trace("stub SOptionsClass::Load_Camera_Mode %s", section); }

// ------------------------------------------------------------------------- SClient's methods the handlers call (sclient.cpp, not rebuilt)

void SClient::Print(int channel, const char *format, ...)
{
	char buffer[512];
	va_list va;
	va_start(va, format);
	_vsnprintf(buffer, sizeof(buffer) - 1, format, va);
	va_end(va);
	buffer[sizeof(buffer) - 1] = 0;
	Trace("stub SClient::Print 0x%X '%s'", channel, buffer);
}
void SClient::Unknown_743290() { Trace("stub SClient::Unknown_743290"); }
bool SClient::Unknown_744B60() { Trace("stub SClient::Unknown_744B60 -> %d", Stub_744B60); return(Stub_744B60); }
void SClient::Unknown_74C530(unsigned id) { Trace("stub SClient::Unknown_74C530 %u", id); }
void SClient::Unknown_74B600() { Trace("stub SClient::Unknown_74B600"); }
void SClient::Unknown_74B640() { Trace("stub SClient::Unknown_74B640"); }
void SClient::Unknown_74B680(int a, int starbase_id) { Trace("stub SClient::Unknown_74B680 %d %d", a, starbase_id); }
void SClient::Unknown_74BA30(int a, int starbase_id, int mode) { Trace("stub SClient::Unknown_74BA30 %d %d %d", a, starbase_id, mode); }
void SClient::Unknown_74D990(int color, char *text, int a)
{
	// MESSAGE_TIME's lines carry the local time: the trace keeps the text up to it
	std::string t(text ? text : "(null)");
	if (t.find("Local Time: ") == 0) {
		t = "Local Time: (now)";
	}
	while (!t.empty() && (t[t.size() - 1] == '\n' || t[t.size() - 1] == '\r')) {
		t.erase(t.size() - 1);
	}
	Trace("stub SClient::Unknown_74D990 0x%X '%s' %d", color, t.c_str(), a);
}
void SClient::Unknown_74E710(const std::string &message, int id, int type) { Trace("stub SClient::Unknown_74E710 '%s' %d %d", message.c_str(), id, type); }
void SClient::Unknown_74EB00(int type, int id) { Trace("stub SClient::Unknown_74EB00 %d %d", type, id); }
void SClient::Unknown_751460(int points_total, int points_spent) { Trace("stub SClient::Unknown_751460 %d %d", points_total, points_spent); }
void SClient::Unknown_7517E0(int a, int b, int c, int d) { Trace("stub SClient::Unknown_7517E0 %d %d %d %d", a, b, c, d); }
void SClient::Unknown_751850(std::vector<GuildRankNameStruct> &ranks) { Trace("stub SClient::Unknown_751850 %d ranks", (int)ranks.size()); }

// SClient's slots declared only (sclient.h): never called by the tests
bool SClient::Unknown_34(int arg) { Trace("stub SClient::Unknown_34 %d", arg); return(false); }
void SClient::Unknown_64() { Trace("stub SClient::Unknown_64"); }

// ------------------------------------------------------------------------- the kernel (kernel.cpp not compiled in)

bool TaskClass::Is_Focused()
{
	if (Stub_Focused) {
		Trace("stub TaskClass::Is_Focused -> 1");	// (once per message: traced only when true)
	}
	return(Stub_Focused);
}

// ------------------------------------------------------------------------- co.cpp's client object
// (co.cpp's rebuilt members link from app.lib: the type tests, the model accessor 0x6F3BC0, the aux-data
// accessors 0x6FFE90-0x6FFFA0; what remains here is not rebuilt yet)

// (the constructor 0x6F2820 and destructor 0x6F2FE0 link from app.lib: clienttest.cpp's TestObjectClass logs
// the destruction and takes its fakes back first; the slots +0x04-+0x18 are the base's, client.cpp's)
RefCountPtr<UnknownB4Class> Unknown_731FA0(ClientObjectClass *) { return(RefCountPtr<UnknownB4Class>(NULL)); }
Unknown498C10Class::Unknown498C10Class() : Unknown00(false) {}		// (silent: every object constructs one)
void Unknown498C10Class::Unknown_498F00(float, float, float) {}
// co.cpp's SUBPARTS 0x6FEF70 asks client.cpp's 0x7294D0 (not rebuilt) for a sub part's lego: a by-value RefCountPtr
// gen_stubs.py cannot write (reached only with a +0xB4 object, which Unknown_731FA0 above never gives)
#include "..\..\..\app\client\client_lego_impl.h"
RefCountPtr<LegoClass> Unknown_7294D0(const LegoIdentifierClass &, int, int value, ClientClass *, float) { Trace("stub Unknown_7294D0 %d", value); return(RefCountPtr<LegoClass>(NULL)); }
void ClientObjectClass::Unknown_70E750(int id) { Trace("stub ClientObjectClass::Unknown_70E750 %u target %d", GameID, id); }

// co.cpp's per-frame update 0x6F3C60 prints the player's position on wwlib's mono screen (the client's global at
// 0xC1ECC0, constructed next to mono.cpp's code; no tree file defines it)
#include "_mono.h"
MonoClass Mono;

// SClient::Do_Frame's data globals (gen_stubs --link skips data): the frame's toggle and the input flags
int Unknown_BE350C = 0;
int Unknown_BE5F34 = 0;

// Do_Frame's callees that take an input event by value (gen_stubs skips a class by value)
#include "..\..\..\app\kernel\kernel.h"
bool SClient::Unknown_7436A0(KernelEventClass event) { Trace("stub SClient::Unknown_7436A0 key 0x%X", event.Key); return(false); }
void SClient::Unknown_744240(KernelEventClass event) { Trace("stub SClient::Unknown_744240 key 0x%X", event.Key); }
KernelEventClass Unknown_65F780(void *) { Trace("stub Unknown_65F780"); return(KernelEventClass()); }
void ScriptedMovieViewClass::Unknown_802960(KernelEventClass event) { Trace("stub ScriptedMovieViewClass::Unknown_802960 key 0x%X", event.Key); }

// ------------------------------------------------------------------------- eff_o.cpp's effect, the third list's records

EffectObjectClass::~EffectObjectClass() { Trace("stub EffectObjectClass::~EffectObjectClass %u", ID); }
void EffectObjectClass::Unknown_6E74B0(int arg) { Trace("stub EffectObjectClass::Unknown_6E74B0 %u %d", ID, arg); }
void EffectObjectClass::Unknown_6E6ED0(int arg) { Trace("stub EffectObjectClass::Unknown_6E6ED0 %u %d", ID, arg); }
void EffectObjectClass::Unknown_6E6E30() { Trace("stub EffectObjectClass::Unknown_6E6E30 %u", ID); }
// eff_o.cpp (app.lib) defines these since EF1; standing in here keeps eff_o.obj, and its real slots, out
void Unknown_6E6A90(EffectObjectClass *effect) { Trace("stub Unknown_6E6A90 %u", effect->ID); }
bool EffectObjectClass::Unknown_6E6AD0() { Trace("stub EffectObjectClass::Unknown_6E6AD0 %u", ID); return(false); }
Unknown86CRecordClass::~Unknown86CRecordClass() { Trace("stub Unknown86CRecordClass::~Unknown86CRecordClass %u", ID); }

// ------------------------------------------------------------------------- ClientClass's members (client.h)

// (ITEM_BASE 37 asserts on the item type and name of a template it has already: the stand-in keeps both)
Unknown831340Class::Unknown831340Class(int a, int, int, const RefCountPtr<RefCountClass> &, int, int, int g, int, int, int, int, int,
	const std::string &name, const std::string &, const std::string &) : ItemType(g), Name(name)
{
	Trace("stub Unknown831340Class::Unknown831340Class %d '%s' type %d", a, name.c_str(), g);
}
Unknown831340Class::~Unknown831340Class() { Trace("stub Unknown831340Class::~Unknown831340Class"); }
PxClass::PxClass() { Trace("stub PxClass::PxClass"); }
PxClass::~PxClass() { Trace("stub PxClass::~PxClass"); }
PxProcessClass::PxProcessClass() { Trace("stub PxProcessClass::PxProcessClass"); }
PxProcessClass::~PxProcessClass() { Trace("stub PxProcessClass::~PxProcessClass"); }
Unknown763E00Class::Unknown763E00Class() { Trace("stub Unknown763E00Class::Unknown763E00Class"); }
Unknown763E00Class::~Unknown763E00Class() { Trace("stub Unknown763E00Class::~Unknown763E00Class"); }
void Unknown763E00Class::Unknown_763E70(const char *ini) { Trace("stub Unknown763E00Class::Unknown_763E70 %s", ini); }
void Unknown763E00Class::Unknown_763FF0() { Trace("stub Unknown763E00Class::Unknown_763FF0"); }

// ------------------------------------------------------------------------- cpacket.cpp's callees in other files

// rs_track.cpp and sound_manager.cpp: declared as cpacket.cpp declares them (same names, same mangling)
class RsTrackClass
{
public:
	void Unknown_4604D0(int render_state_id);
	void Unknown_460780(int render_state_id);
	void Unknown_460850(int render_state_id);
	void Unknown_460880(int render_state_id, int link_id);
};
void RsTrackClass::Unknown_4604D0(int id) { Trace("stub RsTrackClass::Unknown_4604D0 %d", id); }
void RsTrackClass::Unknown_460780(int id) { Trace("stub RsTrackClass::Unknown_460780 %d", id); }
void RsTrackClass::Unknown_460850(int id) { Trace("stub RsTrackClass::Unknown_460850 %d", id); }
void RsTrackClass::Unknown_460880(int id, int link_id) { Trace("stub RsTrackClass::Unknown_460880 %d %d", id, link_id); }

class SoundManagerClass
{
public:
	void Unknown_75AE30(const char *name, int channel);
	void Unknown_75B0D0(const char *name, int channel);
	void Unknown_75B170(int channel);
	void Unknown_75B2B0(int channel);
	char Unknown00[4];
};
void SoundManagerClass::Unknown_75AE30(const char *name, int channel) { Trace("stub SoundManagerClass::Unknown_75AE30 '%s' %d", name, channel); }
void SoundManagerClass::Unknown_75B0D0(const char *name, int channel) { Trace("stub SoundManagerClass::Unknown_75B0D0 '%s' %d", name, channel); }
void SoundManagerClass::Unknown_75B170(int channel) { Trace("stub SoundManagerClass::Unknown_75B170 %d", channel); }
void SoundManagerClass::Unknown_75B2B0(int channel) { Trace("stub SoundManagerClass::Unknown_75B2B0 %d", channel); }
static SoundManagerClass The_Sound_Manager;
SoundManagerClass *SoundManager = &The_Sound_Manager;

// ui_line_display.cpp (the handlers 32-36 through SClient +0x1280): the members cpacket.cpp calls, as
// ui_line_display.h declares them (non-virtual: same mangling); clienttest.cpp points LineDisplay at a fake
class UILineDisplayClass
{
public:
	void Add_Line(const char *text, int display_time, const char *font, int priority);
	void Add_Line_7(const char *text, int display_time, const char *font);
	void Add_Line_3(const char *text, int display_time, const char *font);
	void Show_Ephemeral(const char *type);
	void Flush_Lines();
};
void UILineDisplayClass::Add_Line(const char *text, int display_time, const char *font, int priority) { Trace("stub UILineDisplayClass::Add_Line '%s' %d '%s' %d", text ? text : "(null)", display_time, font ? font : "(null)", priority); }
void UILineDisplayClass::Add_Line_7(const char *text, int display_time, const char *font) { Trace("stub UILineDisplayClass::Add_Line_7 '%s' %d '%s'", text ? text : "(null)", display_time, font ? font : "(null)"); }
void UILineDisplayClass::Add_Line_3(const char *text, int display_time, const char *font) { Trace("stub UILineDisplayClass::Add_Line_3 '%s' %d '%s'", text ? text : "(null)", display_time, font ? font : "(null)"); }
void UILineDisplayClass::Show_Ephemeral(const char *type) { Trace("stub UILineDisplayClass::Show_Ephemeral '%s'", type ? type : "(null)"); }
void UILineDisplayClass::Flush_Lines() { Trace("stub UILineDisplayClass::Flush_Lines"); }

// shiputils.cpp 0x80E8C0 (DECAL 16)
class SClientUnknown1334Class;
void Unknown_80E8C0(RenderObjClass *, std::vector<DecalStruct> &decals, SClientUnknown1334Class *) { Trace("stub Unknown_80E8C0 %d decals", (int)decals.size()); }

// verbmanager.cpp (VERB_UPDATE 92: the handler tests VerbManager first, which Init makes)
void VerbManagerClass::Unknown_81F810(int game_id, void *, void *) { Trace("stub VerbManagerClass::Unknown_81F810 %d", game_id); }

// ------------------------------------------------------------------------- the asserts and the debug log

void Unknown_Assert_Failed(const char *expr, const char *, int line) { Trace("assert %s (line %d)", expr, line); }
void Unknown_Fail_If_Report(const char *expr, const char *, int line) { Trace("fail_if %s (line %d)", expr, line); }
void Unknown_Debug_Begin_Step(const char *text) { Trace("step: %s", text); }
void Unknown_Debug_End_Step(const char *text) { Trace("step end: %s", text); }
void Unknown_Debug_Print(const char *text) { Trace("log: %s", text); }
// Unknown_Debug_Printf: stubs/app_stubs.cpp's (stubs.lib)

// ------------------------------------------------------------------------- data gen_stubs.py --link can't write

RectClass Unknown_C07550;	// 0xC07550 (sclient.cpp's Init)

// wwaudio's instance (ClientClass's destructor: Free_Completed_Sounds): here, so that wwaudio.obj and Miles
// (mss32.dll) stay out of the program; the tests never delete the client
#include "wwaudio.h"
WWAudioClass *WWAudioClass::_theInstance = 0;
void WWAudioClass::Free_Completed_Sounds(void) { Trace("stub WWAudioClass::Free_Completed_Sounds"); }

// The object at 0xC404D8 (soundlib.cpp's neighbourhood): SClient's constructor sets its byte +0x19A5, so the
// pointer must point somewhere (declared as sclient.cpp declares it)
struct Unknown_C404D8Struct
{
	char Unknown0000[0x19A5];
	bool Unknown19A5;
};
static Unknown_C404D8Struct The_C404D8;
Unknown_C404D8Struct *Unknown_C404D8 = &The_C404D8;

// ------------------------------------------------------------------------- auxdata.h's lookup (co.cpp)

// co.cpp's aux-data accessors (0x6F58B0, 0x6F5E40, ...) inline auxdata.h's lookup: these three keep
// name.obj, delta.obj and lookup.obj out of the harness (they pull unknown_root.obj, whose
// Unknown_Get_Ticks clienttest.cpp defines, and shipcust_desc.obj). No root, no field: no trace.
#include "..\..\..\app\shared\auxdata\delta.h"
#include "..\..\..\app\shared\auxdata\lookup.h"
NameClass::NameClass(const char *) {}
LookupClass *DeltaCompoundClass::Get_Lookup() { return NULL; }
DeltaClass *LookupClass::Find(const NameClass &) { return NULL; }
// the damage state 0x6FE1E0 reads ShieldPercent through the interpolator's Get_Value: Update keeps
// delta_interpolator.obj (and with it delta.obj) out; no field, never called
#include "..\..\..\app\shared\auxdata\delta_interpolator.h"
float DeltaInterpolatorClass::Update(const DurationClass &) { return(0.0f); }
// the bool field's type global (client 0xC416AC; app.lib has it in aux_roots.obj, which the harness
// doesn't link)
#include "..\..\..\app\client\aux_roots.h"
template<> int DeltaTemplateClass<bool>::UnknownGlobal = 0;
// (co.cpp's constructor asks the factory 0x6F6000 for its aux root: clienttest.cpp's stand-in hands out the harness's)

// ------------------------------------------------------------------------- extrap.cpp's state nodes (postate.cpp's)

// postate.cpp and extrap.cpp are in app.lib and define every state node: nothing stands in here
#include "..\..\..\app\client\postate.h"
// client.cpp's mount buffer insert 0x725E70 (not rebuilt; Add reaches it since f6b75ef, a nested struct gen_stubs.py cannot write)
void UnknownB02C90Class::BufferStruct::Insert(const ElementStruct &, int index) { Trace("stub UnknownB02C90Class::BufferStruct::Insert %d", index); }
// pkupdate.cpp's 0x8ADB20 (not rebuilt): types 4 and 8 ask it
bool AdvancedPositionalUpdatePacketClass::Unknown_8ADB20() { return(false); }

// shiputils.cpp's colorization helper (COLORIZATION 17)
class SClientUnknown1338Class;
void Unknown_80EFF0(RenderObjClass *, std::vector<ColorizationRegionStruct> &regions, SClientUnknown1338Class *)
{
	Trace("stub Unknown_80EFF0 %d regions", (int)regions.size());
}
// and its colorization of a new model (co.cpp's 0x6FCB70: a vector by reference gen_stubs.py cannot write)
void Unknown_80F220(RenderObjClass *, std::vector<ColorizationRegionStruct> &regions)
{
	Trace("stub Unknown_80F220 %d regions", (int)regions.size());
}

// ------------------------------------------------------------------------- the aux-data roots' decode (AUX_DATA 27 / 28)

// unknown_root.cpp's DeltaCompoundClass::Decode (0x4A3FA0), here so that unknown_root.obj stays out: the
// harness's roots (clienttest.cpp's Aux_Root_Name names them) only log the update's size
#include "..\..\..\app\shared\auxdata\linearbu.h"
bool DeltaCompoundClass::Decode(LinearBufferClass &buf, const DurationClass &)
{
	Trace("stub DeltaCompoundClass::Decode %s, %d bytes", Aux_Root_Name(this), buf.Size);
	return(true);
}
// ------------------------------------------------------------------------- eff_o.cpp's effect factory (OBJECT_EFFECT 9)

// declared as cpacket.cpp declares them; the factory's effect has a target end that logs the object's id
class Unknown6E9490TargetClass
{
public:
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28(int game_id);
	virtual void Unknown_2C(const Matrix3D &transform);
};
void Unknown6E9490TargetClass::Unknown_00() {}
void Unknown6E9490TargetClass::Unknown_04() {}
void Unknown6E9490TargetClass::Unknown_08() {}
void Unknown6E9490TargetClass::Unknown_0C() {}
void Unknown6E9490TargetClass::Unknown_10() {}
void Unknown6E9490TargetClass::Unknown_14() {}
void Unknown6E9490TargetClass::Unknown_18() {}
void Unknown6E9490TargetClass::Unknown_1C() {}
void Unknown6E9490TargetClass::Unknown_20() {}
void Unknown6E9490TargetClass::Unknown_24() {}
void Unknown6E9490TargetClass::Unknown_28(int game_id) { Trace("stub Unknown6E9490TargetClass::Unknown_28 %d", game_id); }
void Unknown6E9490TargetClass::Unknown_2C(const Matrix3D &tm) { Trace("stub Unknown6E9490TargetClass::Unknown_2C at (%g, %g, %g)", tm[0][3], tm[1][3], tm[2][3]); }
static Unknown6E9490TargetClass The_Effect_Target;
// the binary end (OBJECT_TO_OBJECT_EFFECT 11 to the linked 14), standing in for eff_o.h's (its slots, Flags and
// positions at the same offsets): logs the ids, the positions and the offset
class UnknownB0009CEffectClass
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24();
	virtual void Unknown_28(int source_game_id);
	virtual void Unknown_2C(int source_game_id, const LegoIdentifierClass &source_lego);
	virtual void Unknown_30(const Vector3 &position);
	virtual void Unknown_34(int target_game_id);
	virtual void Unknown_38(const Vector3 &position);
	void Unknown_6E82A0(short unknown36, short unknown34, Vector3 target_offset);
	char Unknown04[0x7C - 0x04];
	unsigned Flags;
	char Unknown80[0xA8 - 0x80];
	Vector3 SourcePosition;
	Vector3 TargetPosition;
};
void UnknownB0009CEffectClass::Unknown_00() {}
void UnknownB0009CEffectClass::Unknown_04() {}
void UnknownB0009CEffectClass::Unknown_08() {}
void UnknownB0009CEffectClass::Unknown_0C() {}
void UnknownB0009CEffectClass::Unknown_10() {}
void UnknownB0009CEffectClass::Unknown_14() {}
void UnknownB0009CEffectClass::Unknown_18() {}
void UnknownB0009CEffectClass::Unknown_1C() {}
void UnknownB0009CEffectClass::Unknown_20() {}
void UnknownB0009CEffectClass::Unknown_24() {}
void UnknownB0009CEffectClass::Unknown_28(int id) { Trace("stub Unknown6E9490BinaryClass::Unknown_28 source %d", id); }
void UnknownB0009CEffectClass::Unknown_2C(int id, const LegoIdentifierClass &) { Trace("stub Unknown6E9490BinaryClass::Unknown_2C source %d with a lego", id); }
void UnknownB0009CEffectClass::Unknown_30(const Vector3 &p) { Trace("stub Unknown6E9490BinaryClass::Unknown_30 (%g, %g, %g)", p.X, p.Y, p.Z); SourcePosition = p; }
void UnknownB0009CEffectClass::Unknown_34(int id) { Trace("stub Unknown6E9490BinaryClass::Unknown_34 target %d", id); }
void UnknownB0009CEffectClass::Unknown_38(const Vector3 &p) { Trace("stub Unknown6E9490BinaryClass::Unknown_38 (%g, %g, %g)", p.X, p.Y, p.Z); TargetPosition = p; }
void UnknownB0009CEffectClass::Unknown_6E82A0(short a, short b, Vector3 o) { Trace("stub Unknown6E9490BinaryClass::Unknown_6E82A0 %d %d (%g, %g, %g) flags 0x%X", a, b, o.X, o.Y, o.Z, Flags); }
static UnknownB0009CEffectClass The_Effect_Binary;
class Unknown6E9490EffectClass : public EffectObjectClass
{
public:
	virtual void Unknown_04();
	virtual Vector3 Unknown_08(const Vector3 &position);
	virtual void Unknown_0C();
	virtual Unknown6E9490TargetClass *Unknown_10();
	virtual UnknownB0009CEffectClass *Unknown_14();
};
void Unknown6E9490EffectClass::Unknown_04() {}
Vector3 Unknown6E9490EffectClass::Unknown_08(const Vector3 &) { return(Vector3(0, 0, 0)); }
void Unknown6E9490EffectClass::Unknown_0C() {}
Unknown6E9490TargetClass *Unknown6E9490EffectClass::Unknown_10() { return(&The_Effect_Target); }
UnknownB0009CEffectClass *Unknown6E9490EffectClass::Unknown_14() { return(&The_Effect_Binary); }
// eff_o.h's declaration (cpacket.cpp's handlers and co.cpp's warp state)
EffectObjectClass *Unknown_6E9490(int desc_id, unsigned effect_id, unsigned duration, unsigned time, ClientClass *, float scale, const Vector3 &hsv_shift, float a, int b, bool c)
{
	Trace("stub Unknown_6E9490 desc %d effect %d duration %d time %d scale %.2f hsv (%.2f %.2f %.2f) %.2f %d %d", desc_id, (int)effect_id,
		(int)duration, (int)time, scale, hsv_shift.X, hsv_shift.Y, hsv_shift.Z, a, b, (int)c);
	Unknown6E9490EffectClass *effect = new Unknown6E9490EffectClass;
	effect->ID = effect_id;
	effect->Node.Set(effect);
	return(effect);
}

// CAMERA_CONTROL 146 (T19): its docking sound; app.lib's soundlib.obj would bring WWAudio's globals in
#include "..\..\..\app\audio\soundlib.h"
AudibleSoundClass *SoundLibraryClass::Play_2D_Sound(const char *name, int mode) { Trace("stub SoundLibraryClass::Play_2D_Sound '%s' %d", name, mode); return(0); }
Sound3DClass *SoundLibraryClass::Play_3D_Sound(const char *name, const Matrix3D &tm, int mode, float scale) { Trace("stub SoundLibraryClass::Play_3D_Sound '%s' at (%g, %g, %g) %d %g", name, tm[0][3], tm[1][3], tm[2][3], mode, scale); return(0); }	// co.cpp's engine sounds 0x6FDBD0
