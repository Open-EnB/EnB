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
int SClient::Do_Frame(bool focused) { Trace("stub SClient::Do_Frame %d", focused); return(0); }
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

ClientObjectClass::~ClientObjectClass() { Trace("stub ClientObjectClass::~ClientObjectClass %u", GameID); }
void ClientObjectClass::Unknown_04() { Trace("stub ClientObjectClass::Unknown_04 %u", GameID); }
void ClientObjectClass::Unknown_08() { Trace("stub ClientObjectClass::Unknown_08 %u", GameID); }
void ClientObjectClass::Unknown_0C(ClientObjectClass *child, int a, int b) { Trace("stub ClientObjectClass::Unknown_0C %u child %u %d %d", GameID, child ? child->GameID : 0, a, b); }
Matrix3D ClientObjectClass::Unknown_28(int unknown) { Trace("stub ClientObjectClass::Unknown_28 %u %d", GameID, unknown); Matrix3D m(true); return(m); }
void ClientObjectClass::Unknown_6FEF70(SubPartsPacket *) { Trace("stub ClientObjectClass::Unknown_6FEF70 %u", GameID); }
void ClientObjectClass::Unknown_70E750(int id) { Trace("stub ClientObjectClass::Unknown_70E750 %u target %d", GameID, id); }

// ------------------------------------------------------------------------- eff_o.cpp's effect, the third list's records

EffectObjectClass::~EffectObjectClass() { Trace("stub EffectObjectClass::~EffectObjectClass %u", ID); }
void EffectObjectClass::Unknown_6E74B0(int arg) { Trace("stub EffectObjectClass::Unknown_6E74B0 %u %d", ID, arg); }
void EffectObjectClass::Unknown_6E6ED0(int arg) { Trace("stub EffectObjectClass::Unknown_6E6ED0 %u %d", ID, arg); }
void EffectObjectClass::Unknown_6E6E30() { Trace("stub EffectObjectClass::Unknown_6E6E30 %u", ID); }
Unknown86CRecordClass::~Unknown86CRecordClass() { Trace("stub Unknown86CRecordClass::~Unknown86CRecordClass %u", ID); }

// ------------------------------------------------------------------------- ClientClass's members (client.h)

Unknown831340Class::Unknown831340Class(int, int, int, const RefCountPtr<RefCountClass> &, int, int, int, int, int, int, int, int,
	const std::string &name, const std::string &, const std::string &)
{
	Trace("stub Unknown831340Class::Unknown831340Class '%s'", name.c_str());
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
// the bool field's type global (client 0xC416AC; app.lib has it in aux_roots.obj, which the harness
// doesn't link)
#include "..\..\..\app\client\aux_roots.h"
template<> int DeltaTemplateClass<bool>::UnknownGlobal = 0;

// ------------------------------------------------------------------------- co.h's +0x98 state (postate.cpp)

// co.cpp doesn't define Unknown98Class's slots yet: these stand in. CONSTANT_POSITIONAL_UPDATE 64 news
// postate.cpp's constant state (declared as cpacket.cpp declares it, plus the two slots the handler calls:
// its type, 1, and the deleting destructor) and updates it
Unknown98Class::~Unknown98Class() {}
int Unknown98Class::Unknown_04() { Trace("stub Unknown98Class::Unknown_04 -> 0"); return(0); }
Matrix3D Unknown98Class::Unknown_08(int) { Matrix3D m(true); return(m); }
Vector3 Unknown98Class::Unknown_0C(int) { return(Vector3(0, 0, 0)); }
Vector3 Unknown98Class::Unknown_10(int) { return(Vector3(0, 0, 0)); }
Vector3 Unknown98Class::Unknown_14(int) { return(Vector3(0, 0, 0)); }
class Unknown714B50Class : public Unknown98Class
{
public:
	Unknown714B50Class(ConstantPositionalUpdatePacketClass *packet);
	virtual ~Unknown714B50Class();
	virtual int Unknown_04();
	void Unknown_714D50(ConstantPositionalUpdatePacketClass *packet);
	int GameID;
};
Unknown714B50Class::Unknown714B50Class(ConstantPositionalUpdatePacketClass *packet) : GameID(packet->GameID)
{
	Trace("stub Unknown714B50Class::Unknown714B50Class %u (%.2f %.2f %.2f)", GameID, packet->Unknown10[0], packet->Unknown10[1], packet->Unknown10[2]);
}
Unknown714B50Class::~Unknown714B50Class() { Trace("stub Unknown714B50Class::~Unknown714B50Class %u", GameID); }
int Unknown714B50Class::Unknown_04() { return(1); }
void Unknown714B50Class::Unknown_714D50(ConstantPositionalUpdatePacketClass *packet)
{
	Trace("stub Unknown714B50Class::Unknown_714D50 %u (%.2f %.2f %.2f)", packet->GameID, packet->Unknown10[0], packet->Unknown10[1], packet->Unknown10[2]);
}

// shiputils.cpp's colorization helper (COLORIZATION 17)
class SClientUnknown1338Class;
void Unknown_80EFF0(RenderObjClass *, std::vector<ColorizationRegionStruct> &regions, SClientUnknown1338Class *)
{
	Trace("stub Unknown_80EFF0 %d regions", (int)regions.size());
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
