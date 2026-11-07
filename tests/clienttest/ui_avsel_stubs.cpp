// Earth & Beyond client (reconstruction)
// clienttest: hand stand-ins for what ui_avsel.obj, ui_login.obj and tsk_game.obj (linked from app.lib since the
// login screen) reach and gen_stubs.py --link cannot write: a pointer-to-nested-class parameter, a by-value
// class, and globals whose type is a file-local class (the client's one address, 0xBE67C4 / 0xBE67B8, has a
// different view in each file, so the mangled names differ). The int constants are textgadg.h's flags, as
// programs\clientpoc\poc_data.cpp has them (the client's .rdata values).
#include <windows.h>
#include <string>
#include "clienttest.h"
#include "..\..\..\app\client\unknown_signalbase.h"
#include "..\..\..\app\shared\avatar_desc.h"

class UIAvselNotifierClass
{
public:
	void Add(int id, SignalBaseClass::CallbackClass *callback);
	void Remove(int id, SignalBaseClass::CallbackClass *callback);
};
void UIAvselNotifierClass::Add(int id, SignalBaseClass::CallbackClass *) { Trace("stub UIAvselNotifierClass::Add %d (0x4F9740)", id); }
void UIAvselNotifierClass::Remove(int id, SignalBaseClass::CallbackClass *) { Trace("stub UIAvselNotifierClass::Remove %d (0x4F98D0)", id); }

class UIAvselCharacterViewClass { public: void Unknown_5594B0(AvatarDescClass desc); };
void UIAvselCharacterViewClass::Unknown_5594B0(AvatarDescClass) { Trace("stub UIAvselCharacterViewClass::Unknown_5594B0 (0x5594B0)"); }




// ui_registration.obj's flags (RegistrationInterface, charview modes 9-12), the client's .rdata dwords at 0xB08C90-0xB08CB4
extern const int Unknown_B08C90 = 0x1;
extern const int Unknown_B08C94 = 0x2;
extern const int Unknown_B08C98 = 0x4;
extern const int Unknown_B08CA0 = 0x10;
extern const int Unknown_B08CB4 = 0x200;
// Sub14CImplClass::Slot15 (0x7E1310) returns a std::string by value, which gen_stubs.py --link skips
// (a bare declaration: only the class name and the virtual signature reach the mangled name; ui_registration.h needs half the UI headers)
class Sub14CImplClass { public: virtual std::string Slot15(); };
std::string Sub14CImplClass::Slot15() { Trace("stub Sub14CImplClass::Slot15 (0x7E1310)"); return std::string(""); }
extern const int Unknown_B08C9C = 0x8;		// ui_shard's name gadget flag (4f8d4698), as programs/clientpoc/poc_data.cpp
extern const int Unknown_B08CAC = 0x80;
extern const int Unknown_B08CB0 = 0x100;
extern const int Unknown_B08CB8 = 0x400;
extern const int Unknown_B08CBC = 0x800;
extern const int Unknown_B08CC0 = 0x1000;
extern const int Unknown_B08CC4 = 0x2C0;
