// Earth & Beyond client (reconstruction)
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
#include "..\..\..\app\client\ui_talkt.h"
#define Unknown_563760 Clienttest_Unknown_563760	// as in sclient.cpp's object (clienttest.py build)
#include "..\..\..\app\client\ui_chat_panel.h"
#undef Unknown_563760
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
unsigned SOptionsClass::Set_Camera_Mode(unsigned cameramode) { Trace("stub SOptionsClass::Set_Camera_Mode %u", cameramode); return(cameramode); }
void SOptionsClass::Unknown_75A570() { Trace("stub SOptionsClass::Unknown_75A570"); }
void SOptionsClass::Unknown_75A5A0() { Trace("stub SOptionsClass::Unknown_75A5A0"); }
unsigned SOptionsClass::Unknown_75A5D0() { Trace("stub SOptionsClass::Unknown_75A5D0"); return(0); }
void SOptionsClass::Load_Camera_Mode(INIClass &, const char *section) { Trace("stub SOptionsClass::Load_Camera_Mode %s", section); }

// ------------------------------------------------------------------------- the main view's radar (mainview.cpp's 0x512A00)

// the radar's 0x7BCA70: written by hand for its trace (whether the scene is set); gen_stubs.py writes the +0x84 object's 0x7B6BD0
void RadarSystemClass::Unknown_7BCA70(GutterRadarClass *, StarmapPipClass *arg, const RefCountPtr<RefCountClass> &scene)
{
	Trace("stub RadarSystemClass::Unknown_7BCA70 %s, scene %s", arg ? "an object" : "NULL", scene.Peek() ? "set" : "NULL");
}

// ------------------------------------------------------------------------- SClient's methods the handlers call (sclient.cpp, not rebuilt)

// SClient's chat line 0x74D990 is sclient.cpp's real one: its call of the chat panel's 0x563760 (the line with its
// type's prefix) comes here, renamed in sclient.cpp's object only (clienttest.py build; ui_chat_panel.obj keeps its own)
// MESSAGE_TIME's and NOTE_CONTENT's lines carry the local time: the trace keeps the text up to it
static std::string Chat_Trace_Text(const char *text)
{
	std::string t(text ? text : "(null)");
	std::string::size_type at = t.find("Local Time: ");
	if (at != std::string::npos) {
		t = t.substr(0, at) + "Local Time: (now)";
	}
	at = t.find("Sent: ");
	if (at != std::string::npos) {
		t = t.substr(0, at) + "Sent: (the date)";	// NOTE_CONTENT 184: ctime of the note's time, in the local time zone
	}
	while (!t.empty() && (t[t.size() - 1] == '\n' || t[t.size() - 1] == '\r')) {
		t.erase(t.size() - 1);
	}
	return(t);
}
void ChatPanelInterface::Clienttest_Unknown_563760(int type, const std::string &text)
{
	Trace("stub ChatPanelInterface::Unknown_563760 0x%X '%s'", type, Chat_Trace_Text(text.c_str()).c_str());
}
// (the chat colour 0x765DF0 SClient 0x750F70 reads is rebuilt: app/client/unknown_764fd0.cpp, GS1)

// SClient's slots declared only (sclient.h): never called by the tests

// ------------------------------------------------------------------------- co.cpp's client object
// (co.cpp's rebuilt members link from app.lib: the type tests, the model accessor 0x6F3BC0, the aux-data
// accessors 0x6FFE90-0x6FFFA0; what remains here is not rebuilt yet)

// (the constructor 0x6F2820 and destructor 0x6F2FE0 link from app.lib: clienttest.cpp's TestObjectClass logs
// the destruction and takes its fakes back first; the slots +0x04-+0x18 are the base's, client.cpp's)
// client_lego_root.cpp's lego root (app.lib) needs cbasset.ini's bone maps, which clienttest does not load: no root, as
// before; both of co.cpp's references stand in, so client_lego_root.obj is not pulled from app.lib (no LNK2005)
RefCountPtr<ClientLegoRootClass> Unknown_731FA0(ClientObjectClass *) { return(RefCountPtr<ClientLegoRootClass>(NULL)); }
void ClientLegoRootClass::Unknown_732690() {}
// (co.h's +0x134 AccelClass links from app.lib: accel.cpp)
// (co.cpp's SUBPARTS 0x6FEF70 gets a sub part's lego from 0x7294D0: app.lib's unknown_7294d0.cpp)

// co.cpp's per-frame update 0x6F3C60 prints the player's position on wwlib's mono screen (the client's global at
// 0xC1ECC0, constructed next to mono.cpp's code; no tree file defines it)
#include "_mono.h"
MonoClass Mono;

// SClient::Do_Frame's data globals (gen_stubs --link skips data): the frame's toggle and the input flags
int Unknown_BE350C = 0;
int Unknown_BE5F34 = 0;

// Do_Frame's callees that take an input event by value (gen_stubs skips a class by value)
#include "..\..\..\app\kernel\kernel.h"
void SClient::Unknown_744240(KernelEventClass event) { Trace("stub SClient::Unknown_744240 key 0x%X", event.Key); }
// (ScriptedMovieViewClass::Unknown_802960 is rebuilt in scriptedmovie.cpp, UI1; the trace never reaches it)

// ------------------------------------------------------------------------- eff_o.cpp's effect, the third list's records

// eff_o.cpp (app.lib) defines EffectObjectClass's members and UnknownB0009CEffectClass's Unknown_28 / _30 / _38 since EF1; no stand-ins here
// influence.cpp (app.lib) defines Unknown86CRecordClass, the third list's records, since O5b (2026-11-13b): no stand-in here

// ------------------------------------------------------------------------- ClientClass's members (client.h)

// Unknown831340Class (the item templates) is rebuilt in app\common\item_template.cpp (IT1): no stand-ins here

// ------------------------------------------------------------------------- cpacket.cpp's callees in other files

// sound_manager.cpp's real SoundManagerClass (app.lib); the global WinMain makes (0xBE82A4) is a static here
#include "..\..\app\client\sound_manager.h"
static SoundManagerClass The_Sound_Manager;
SoundManagerClass *SoundManager = &The_Sound_Manager;

// ui_line_display.cpp (the handlers 32-36 through SClient +0x1280): the members cpacket.cpp calls, as
// ui_line_display.h declares them (non-virtual: same mangling); clienttest.cpp points LineDisplay at a fake. The
// constructor Init calls stands in here too: from app.lib it would pull ui_line_display.obj (LNK2005 on these)
class LineDisplayClientClass;
class UILineDisplayClass
{
public:
	UILineDisplayClass(LineDisplayClientClass *client);
	void Add_Line(const char *text, int display_time, const char *font, int priority);
	void Add_Line_7(const char *text, int display_time, const char *font);
	void Add_Line_3(const char *text, int display_time, const char *font);
	void Show_Ephemeral(const char *type);
	void Flush_Lines();
};
UILineDisplayClass::UILineDisplayClass(LineDisplayClientClass *) { Trace("stub UILineDisplayClass::UILineDisplayClass"); }
void UILineDisplayClass::Add_Line(const char *text, int display_time, const char *font, int priority) { Trace("stub UILineDisplayClass::Add_Line '%s' %d '%s' %d", text ? text : "(null)", display_time, font ? font : "(null)", priority); }
void UILineDisplayClass::Add_Line_7(const char *text, int display_time, const char *font) { Trace("stub UILineDisplayClass::Add_Line_7 '%s' %d '%s'", text ? text : "(null)", display_time, font ? font : "(null)"); }
void UILineDisplayClass::Add_Line_3(const char *text, int display_time, const char *font) { Trace("stub UILineDisplayClass::Add_Line_3 '%s' %d '%s'", Chat_Trace_Text(text).c_str(), display_time, font ? font : "(null)"); }
void UILineDisplayClass::Show_Ephemeral(const char *type) { Trace("stub UILineDisplayClass::Show_Ephemeral '%s'", type ? type : "(null)"); }
void UILineDisplayClass::Flush_Lines() { Trace("stub UILineDisplayClass::Flush_Lines"); }

// shiputils.cpp is compiled into clienttest (clienttest.py's APP) with its real 0x80FF20 renamed: the real one builds a
// Font32Class from ship_font.tga (SurfaceClass -> DX8Wrapper::_Create_DX8_Surface), which needs a device
void Unknown_80FF20(RenderObjClass *, const char *, float *)
{
	Trace("gen Unknown_80FF20");	// the line gen_stubs.py's stand-in wrote
}

// ui_talkt.cpp (compiled in, its 0x64EED0 renamed): the real one would close the fake object's ActiveUI
void UITalkTreeClass::Unknown_64EED0() { Trace("gen UITalkTreeClass::Unknown_64EED0"); }

// ------------------------------------------------------------------------- the asserts and the debug log

void G_Assert_Report(const char *expr, const char *, int line) { Trace("assert %s (line %d)", expr, line); }
void Fail_If_Report(const char *expr, const char *, int line) { Trace("fail_if %s (line %d)", expr, line); }
void Unknown_Debug_Begin_Step(const char *text) { Trace("step: %s", text); }
void Unknown_Debug_End_Step(const char *text) { Trace("step end: %s", text); }
void Unknown_Debug_Print(const char *text) { Trace("log: %s", text); }
void Unknown_Debug_Printf(const char *, ...) {}	// 0x509DC0 (app\dbgmgr\dbgmgr.cpp is real; this keeps dbgmgr.obj, and its step log, out)

// ------------------------------------------------------------------------- data gen_stubs.py --link can't write

RectClass Unknown_C07550;	// 0xC07550 (sclient.cpp's Init)
struct TestEffectStruct { char Unknown00[0x64]; };	// sclient.cpp's testeffects.ini record (0x64 bytes): only the name and size matter here
std::vector<TestEffectStruct> Unknown_BE7D80;	// 0xBE7D80 (sclient.cpp's 0x750C20, which nothing calls; 0x74CA90 fills it)



// ------------------------------------------------------------------------- auxdata.h's lookup (co.cpp)

// the bool field's type global (client 0xC416AC; app.lib has it in aux_roots.obj, which the harness
// doesn't link)
#include "..\..\..\app\client\aux_roots.h"
template<> int DeltaTemplateClass<bool>::UnknownGlobal = 0;
// ui_buff.cpp (BUFF) names these two classes' per-class globals; app.lib has them in aux_roots.obj
int DeltaElementsClass::UnknownGlobal;
int DeltaBuffsClass::UnknownGlobal;
// (co.cpp's constructor asks the factory 0x6F6000 for its aux root: clienttest.cpp's stand-in hands out the harness's)

// ------------------------------------------------------------------------- extrap.cpp's state nodes (postate.cpp's)

// postate.cpp and extrap.cpp are in app.lib and define every state node: nothing stands in here
#include "..\..\..\app\client\postate.h"
// client.cpp's mount buffer insert 0x725E70 (not rebuilt; Add reaches it since f384d62, a nested struct gen_stubs.py cannot write)
void UnknownB02C90Class::BufferStruct::Insert(const ElementStruct &, int index) { Trace("stub UnknownB02C90Class::BufferStruct::Insert %d", index); }
// eff_o.cpp (real code, in the link since the testeffects.ini loader) calls it; co.cpp is not in the harness
Matrix3D UnknownB02C90Class::Unknown_725AD0() { Trace("stub UnknownB02C90Class::Unknown_725AD0"); return(Matrix3D(1)); }

// ------------------------------------------------------------------------- the aux-data roots' decode (AUX_DATA 27 / 28)

// ------------------------------------------------------------------------- eff_o.cpp's effect factory (OBJECT_EFFECT 9)

// Unknown_6E9490 and the unary / binary ends are eff_o.cpp's real ones (app.lib); the concrete effects the
// factory makes are clienttest.cpp's stand-ins, and effect.ini is the fixture in tests/clienttest/data

// ui_missn.cpp / ui_manu.cpp (via ui_ship_cust.obj's chain) call GadgetClass's slots +0x08 / +0x14 as void functions; gen_stubs
// writes one signature a name (the pointer-returning one) and clienttest.py does not apply link_aliases.txt, so these two
// names link here. Never called: clienttest opens no such window.
class GadgetClass { public: virtual void As_RObjGadget(); virtual void As_UIScutProxyGadget(); };
void GadgetClass::As_RObjGadget() {}
void GadgetClass::As_UIScutProxyGadget() {}

// (rs_eff_p.cpp's 'effect_pass' state 0x458C10 links from app.lib)
