// Earth & Beyond client (reconstruction): app\Client\ui_registration.cpp. Notes at the end of the file.
#include "ui_registration.h"
#include "..\..\wwlib\registry.h"
#include "fontmanager.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "charview.h"
#include "avatar.h"
#include "ui_combobox.h"
#include <map>
#include <vector>
#include <algorithm>
#include "ww3d.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <string>

extern FontManagerClass *Unknown_BE67C4;  // NAME 1: 0xBE67C4

static const char *RegistrationKey = "Software\\Westwood Studios\\Earth and Beyond\\Registration";  // NAME 1 [RegistrationKey]: client .data 0xB85EFC
static const char *RegistrationServerError = "Could not communicate with registration server.\nPlease try again later.";  // NAME 1 [RegistrationServerError]: client .data 0xB85F00

static const char *StatusStrings[] = {  // NAME 1 [StatusStrings]: client .data 0xB86004
	"Please Wait...",
	"Validating Login Information...",
	"Retrieving Entitlement Status...",
	"Retrieving Billing Information...",
	"Retrieving Subscription Information...",
	"Retrieving Pricing Options...",
	"Retrieving Credit Card List...",
	"Retrieving GameTime Data...",
	"Creating EA Account...",
	"Transmitting Billing Information...",
	"Applying Registration Code...",
	"Applying GameTime Code...",
	"Validating AOL Login Information...",
	"Requesting Member Name...",
	"Requesting Member Password...",
	"Your Account has expired...",
	"Your Account has expired...",
	0
};

static const char *PaymentTypeNames[] = {  // NAME 1 [PaymentTypeNames]: client .data 0xB86048 (0x5F9EB0's G_ASSERT 'typetext')
	0,
	0,
	"Credit Card",
	"GameTime",
	0,
	"Check",
	0
};

static const char *PaymentTypeTexts[] = {  // NAME 1 [PaymentTypeTexts]: client .data 0xB86064 (the descriptions, as 0xB86048's order)
	0,
	0,
	"EA accepts most major US and International Credit Cards, including: Visa, MasterCard, Discover and American Express.",
	"GameTime is a paid block of time that allows you to play Earth & Beyond for a fixed period of time.  You need a GameTime code to use this option.",
	0,
	"EA accepts personal checks for payment for your Earth & Beyond account.  To pay by check, you must contact a customer service representative.",
	0
};

class Sub14CClass {  // NAME 1 [Sub14CClass]: vtable 0xAF7130
public:
	virtual void Slot0() {}
	virtual void Slot1() {}
	virtual void Slot2() {}
	virtual void Slot3() {}
	virtual void Slot4() {}
	virtual void Slot5() {}
	virtual void Slot6() {}
	virtual void Slot7() {}
	virtual void Slot8() {}
	virtual void Slot9() {}
	virtual void Slot10() {}
	virtual int Slot11() { return 0; }  // NAME 1 [Slot11]: slot 0x2C (0x5FB950 hands its result to slot 0x30)
	virtual const char *Slot12(int arg) { return 0; }  // NAME 1 [Slot12]: slot 0x30
};

// The second base at +0x84 (table 0xAF7184 while constructing: destructor 0x5F00A0 / 0x5F0120, three empty slots
// 0x5F00C0 / 0x5F00E0 / 0x5F0100; the interface's table 0xAF6FF4 overrides them with 0x600000 / 0x5FB950 / 0x5FB9F0)
class RegistrationCallbackClass  // NAME 1 [RegistrationCallbackClass]: neutral (D10): the server's replies reach the screen through it
{
public:
	virtual ~RegistrationCallbackClass() {}
	virtual void Unknown_5F00C0() {}
	virtual void Unknown_5F00E0() {}
	virtual void Unknown_5F0100() {}
};

// What the reply reader 0x7D1AC0 returns (0x5FB9F0 assigns it member by member to +0x390)
struct RegistrationErrorStruct  // NAME 1 [RegistrationErrorStruct]: neutral (D10)
{
	std::string Unknown00;		// +0x00
	short Unknown10;			// +0x10: the error number ('%s\n(%d)'; -1: none)
	std::string Unknown14;		// +0x14: the error text
	std::string Unknown24;		// +0x24
};

class RegistrationInterface;  // NAME 1 [RegistrationInterface]: provisional (D137)

// The combo boxes' map value (12 bytes: operator[] copies three dwords; the first is what 0x57B000 / 0x57B3C0 run on)
struct RegistrationComboStruct  // NAME 1 [RegistrationComboStruct]: neutral (D10)
{
	UIComboBoxClass *Combo;	// +0x00  // NAME 1 [Combo]: what it points to (ui_combobox.cpp's 0x57B000 / 0x57B3C0)
	int Unknown04;
	int Unknown08;
};

// The 0x14-byte entries of the interface's vector +0x5A8 (0x5F9A90 hands their text to the combo box 0x22)
struct RegistrationUnknown5A8Struct  // NAME 1 [RegistrationUnknown5A8Struct]: neutral (D10)
{
	int Unknown00;
	std::string Unknown04;		// +0x04: the text
};

// The 0x5C-byte entries of the interface's vector +0x578 (0x5F9C90 shows the ones whose id list holds +0x38C)
struct RegistrationUnknown578Struct  // NAME 1 [RegistrationUnknown578Struct]: neutral (D10)
{
	char Unknown00[0x24];
	std::string Unknown24;		// +0x24: the text handed to the combo box
	char Unknown34[0x4C - 0x34];
	std::vector<int> Unknown4C;	// +0x4C: the ids it applies to
};

// What the interface holds at +0x1AC (0x5FB9F0 / 0x600000 ask it for the reply)
class RegistrationUnknown1ACClass  // NAME 1 [RegistrationUnknown1ACClass]: neutral (D10)
{
public:
	RegistrationErrorStruct Unknown_7D1AC0();	// 0x7D1AC0 (the reply's '/status', '/message', '/url')
};

void __stdcall Unknown_5F8DB0(int arg);  // NAME 1 [Unknown_5F8DB0]: ui_registration.cpp

class RegistrationInterface : public InterfaceClass, public RegistrationCallbackClass  // the constructor 0x5EF0B0: InterfaceClass, then the table 0xAF7184 at +0x84
{
public:
	virtual void Unknown_5F00E0();	// 0x5FB950 (RegistrationCallbackClass's slot +0x08)
	virtual void Unknown_5F0100();	// 0x5FB9F0 (RegistrationCallbackClass's slot +0x0C)

	char Unknown88[0xAC - 0x88];
	std::vector<StringClass> UnknownAC;	// +0xAC: the countries (0x5F8FD0 reads them from RG_REGDATA.INI; 0x5F9700 fills the combo box 0x19)
	CharacterViewClass *UnknownBC;	// +0xBC: the constructor's +0x48 slot +0x28 (0x5FB9F0 calls CharacterViewClass::Unknown_67A8D0 on it); the message boxes' view
	LoginTaskClass *UnknownC0;	// +0xC0: the constructor's view +0xBC's task +0x14, slot +0x08 (LoginTaskClass's returns this)
	char UnknownC4[0xC7 - 0xC4];
	bool UnknownC7;				// +0xC7 (0x5FB9F0 sets it)
	char UnknownC8[0xCA - 0xC8];
	bool UnknownCA;				// +0xCA (0x5FB9F0 sets it)
	char UnknownCB[0xD4 - 0xCB];
	int UnknownD4;				// +0xD4: the screen's state (0x5FAFF0's 0x5F3770(0x16); 0x5FB950 tests 0x16)
	std::vector<int> UnknownD8;	// +0xD8: the states to go back to (0x5FB9F0 pops one into 0x5F3770)
	char UnknownE8[0x108 - 0xE8];
	std::map<int, RegistrationComboStruct> Unknown108;	// +0x108: the combo boxes by id (0x5F9700 0x19, 0x5F98C0 0x16, 0x5F9EB0 0x2A; 0x5FB9F0 calls 0x57B3C0(0) on each)
	char Unknown118[0x11B - 0x118];
	bool Unknown11B;			// +0x11B (0x5FB9F0: with state 1)
	char Unknown11C;
	bool Unknown11D;			// +0x11D (0x5FB9F0: with state 0; the constructor sets it with state 0x17)
	char Unknown11E[0x120 - 0x11E];
	RenderObjClass *FontId;  // NAME 1 [FontId]: +0x120 (the font manager's key, a render object)
	char pad_124[0x14C - 0x124];
	Sub14CClass Sub14C;   // NAME 1 [Sub14C]: +0x14C
	char pad_150[0x1AC - (0x14C + sizeof(Sub14CClass))];
	RegistrationUnknown1ACClass *Unknown1AC;	// +0x1AC (0x5FB9F0's 0x7D1AC0)
	int StatusIndex;      // NAME 1 [StatusIndex]: +0x1B0
	char Unknown1B4[0x1CD - 0x1B4];
	bool Unknown1CD;			// +0x1CD: written as 'Trial' (0x5FAC90)
	char Unknown1CE[0x38C - 0x1CE];
	int Unknown38C;				// +0x38C: the id 0x5F9C90 looks for
	RegistrationErrorStruct Unknown390;	// +0x390 (0x5FB9F0 fills it from the reply)
	char Unknown3C4[0x578 - 0x3C4];
	std::vector<RegistrationUnknown578Struct> Unknown578;	// +0x578 (0x5F9C90)
	char Unknown588[0x598 - 0x588];
	std::vector<int> Unknown598;	// +0x598: the payment types offered (0x5F9EB0)
	std::vector<RegistrationUnknown5A8Struct> Unknown5A8;	// +0x5A8 (0x5F9A90)

	void Unknown_5F2990(int arg);
	void Unknown_5F3770(int arg);
	void Unknown_5F9700();
	void Unknown_5F98C0(int from, int to);  // NAME 1 [from, to]: the loop's bounds
	void Unknown_5F9A90();
	void Unknown_5F9C90();
	void Unknown_5F9EB0();
	void Unknown_5FAC90();
	void Unknown_5FAF60(int arg);
	void Unknown_5FAFF0(const char *text);
	void Unknown_5FB030();
	void Unknown_5FB890();
};

// @0x5F9700
void RegistrationInterface::Unknown_5F9700()
{
	for (std::vector<StringClass>::iterator it = UnknownAC.begin(); it != UnknownAC.end(); it++) {
		Unknown108[0x19].Combo->Unknown_57B000(*it);
	}
}

// @0x5F98C0
void RegistrationInterface::Unknown_5F98C0(int from, int to)
{
	for (int i = from; i <= to; i++) {
		char text[6];
		_snprintf(text, sizeof(text), "%d", i);
		Unknown108[0x16].Combo->Unknown_57B000(text);
	}
}

// @0x5F9A90
void RegistrationInterface::Unknown_5F9A90()
{
	for (std::vector<RegistrationUnknown5A8Struct>::iterator it = Unknown5A8.begin(); it != Unknown5A8.end(); it++) {
		Unknown108[0x22].Combo->Unknown_57B000((*it).Unknown04.c_str());
	}
}

// @0x5F9C90
void RegistrationInterface::Unknown_5F9C90()
{
	for (std::vector<RegistrationUnknown578Struct>::iterator it = Unknown578.begin(); it != Unknown578.end(); it++) {
		if (std::find((*it).Unknown4C.begin(), (*it).Unknown4C.end(), Unknown38C) != (*it).Unknown4C.end()) {
			Unknown108[0x29].Combo->Unknown_57B000((*it).Unknown24.c_str());
		}
	}
}






























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// @0x5F9EB0
void RegistrationInterface::Unknown_5F9EB0()
{
	for (std::vector<int>::iterator it = Unknown598.begin(); it != Unknown598.end(); it++) {
		const char *typetext = PaymentTypeNames[*it];
		G_ASSERT(typetext);
		Unknown108[0x2A].Combo->Unknown_57B000(typetext);
	}
}

// @0x5FAC90
void RegistrationInterface::Unknown_5FAC90()
{
	{
		RegistryClass reg(RegistrationKey, true);
		reg.Set_Bool("Registered", true);
		reg.Set_Bool("Trial", Unknown1CD);
	}
	RegFlushKey(HKEY_LOCAL_MACHINE);
}

// @0x5FAD50
bool Unknown_5FAD50()
{
	RegistryClass reg(RegistrationKey, true);
	return reg.Get_Bool("Registered", false);
}

// @0x5FADE0
bool Unknown_5FADE0()
{
	RegistryClass reg(RegistrationKey, true);
	return reg.Get_Bool("Trial", false);
}

// @0x5FAE70
bool Unknown_5FAE70()
{
	return true;
}

// @0x5FAE90
void __cdecl Unknown_5FAE90(RegistrationInterface *self, const char *format, ...)
{
	char buffer[1024];
	if (format) {
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		FontManagerClass *fm = Unknown_BE67C4;
		RenderObjClass *font_id = self->FontId;
		ManagedFontClass *font = fm->Find(font_id);
		if (font) {
			font->Unknown_6D0A70("WRAP_TEXT_MAIN_RG_X1", buffer);
		}
	} else {
		FontManagerClass *fm = Unknown_BE67C4;
		const char *text = StatusStrings[self->StatusIndex];
		ManagedFontClass *font = fm->Find(self->FontId);
		if (font) {
			font->Unknown_6D0A70("WRAP_TEXT_MAIN_RG_X1", text);
		}
	}
}

// @0x5FAF60
void RegistrationInterface::Unknown_5FAF60(int arg)
{
	char buffer[1024];
	const char *str = Sub14C.Slot12(arg);
	_snprintf(buffer, 1024, "%s...", str);
	RenderObjClass *font_id = FontId;
	ManagedFontClass *font = Unknown_BE67C4->Find(font_id);
	if (font) {
		font->Unknown_6D0A70("WRAP_STATUS_TEXT_RG_X1", buffer);
	}
}

// @0x5FAFF0
void RegistrationInterface::Unknown_5FAFF0(const char *text)
{
	Unknown_5F8DB0(0);
	Unknown_5F3770(0x16);
	Unknown_5FAE90(this, text);
}

// @0x5FB030
void RegistrationInterface::Unknown_5FB030()
{
	int width;
	int height;
	int bits;
	bool windowed;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	RenderObjClass *label = FontId->Get_Sub_Object_By_Name("LABEL_TEXT");
	Unknown_7CA2D0()->Unknown_7CA5C0(4, label, NULL);
	UnknownC0->Unknown0018->Unknown_5153B0();
	label->Release_Ref();
}

// @0x5FB330
std::string __stdcall Unknown_5FB330(std::string str)
{
	std::string copy = str;
	for (std::string::iterator it = copy.begin(); it != copy.end(); ) {
		if (*it == '-') {
			it = copy.erase(it);
		} else {
			++it;
		}
	}
	return copy;
}

// @0x5FB540
std::string __stdcall Unknown_5FB540(std::string str)
{
	std::string copy = str;
	for (std::string::iterator it = copy.begin(); it != copy.end(); ) {
		if (*it == ' ') {
			it = copy.erase(it);
		} else {
			++it;
		}
	}
	return copy;
}
















































































































































































































































































































































































































// @0x5FB750
bool __stdcall Unknown_5FB750(const char *test)
{
	FAIL_IF(!test || *test == '\0') return false;
	for (;;) {
		if (*test == ',') {
			return true;
		}
		if (*test++ == '\0') {
			break;
		}
	}
	return false;
}

// @0x5FB7A0
void __cdecl Unknown_5FB7A0(RegistrationInterface *self, const char *format, ...)
{
	char buffer[1024];
	if (format) {
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		self->UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, buffer, 0x140, 0, 0);
	} else {
		_snprintf(buffer, 1024, "%s\n(%d)", self->Unknown390.Unknown14.c_str(), self->Unknown390.Unknown10);
		self->UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, buffer, 0x140, 0, 0);
	}
}

// @0x5FB890
void RegistrationInterface::Unknown_5FB890()
{
	char buffer[1024];
	if (Unknown390.Unknown10 != -1) {
		_snprintf(buffer, 1024, "%s\n(%d)", RegistrationServerError, Unknown390.Unknown10);
	} else {
		_snprintf(buffer, 1024, "%s", RegistrationServerError);
	}
	UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, buffer, 0x140, 0, 0);
}

// @0x5FB950
void RegistrationInterface::Unknown_5F00E0()
{
	if (UnknownD4 == 0x16) {
		Unknown_5FAF60(Sub14C.Slot11());
	}
}

// NOT EXACT: field-offset (explain.py's first mismatch; the shape is inline-budget: the client calls the map's begin 0x606960, the iterator's
// operator++(int) 0x604DF0 and pop_back's copy 0x60A060 / _Destroy 0x605950 out of line, and destroys the reply under one EH state)
// @0x5FB9F0
void RegistrationInterface::Unknown_5F0100()
{
	Unknown390 = Unknown1AC->Unknown_7D1AC0();
	if (Unknown390.Unknown10 == -1) {
		Unknown_5FB890();
	} else {
		UnknownBC->Get_Unknown34()->Unknown_588A70("OK", 0, Unknown390.Unknown14.c_str(), 0x140, 0, 0);
	}
	if ((UnknownD4 == 1 && Unknown11B) || (UnknownD4 == 0 && Unknown11D) || UnknownD4 == 0x17) {
		Unknown_5F2990(0);
		for (std::map<int, RegistrationComboStruct>::iterator it = Unknown108.begin(); it != Unknown108.end(); it++) {
			if ((*it).second.Combo) {
				(*it).second.Combo->Unknown_57B3C0(0);
			}
		}
		UnknownCA = true;
		UnknownC7 = true;
		UnknownBC->Get_Unknown198()->Unknown_784E00();
		UnknownBC->Unknown_67A8D0(true);
		UnknownC0->Go_Next_Step();
	} else {
		int state = 0;
		if (UnknownD8.size() != 0) {
			state = UnknownD8.back();
		}
		if (UnknownD8.size() != 0) {
			UnknownD8.pop_back();
		}
		Unknown_5F3770(state);
	}
}

/*
**	Reconstruction notes (builder ureg2, 2026-11-02)
**
**	ui_registration.cpp (the client's object: asserts lines 369-3637; client 0x5EF0B0-0x5FBD30+).
**	- 0x5FAD50 / 0x5FADE0 (UREG): demo/registration predicates reading "Registered" and "Trial".
**	- 0x5FAE70: trivial registration predicate returning true (byte-exact, short, 3 bytes).
**	- 0x5FAE90: wrap text formatter helper (byte-exact, 154 bytes).
**	- 0x5FAF60: status text formatter (byte-exact, builder reg 2026-11-04: the font key read into a local
**	  before FontManagerClass::Find, as 0x5FAE90 does; inlined into 0x5FB950).
**	- 0x5FAFF0: registration window status helper (byte-exact, 37 bytes).
**	- 0x5FB330: hyphen stripper string helper (byte-exact, 401 bytes).
**	- 0x5FB540: space stripper string helper (byte-exact, 401 bytes).
**
**	Builder reg, 2026-11-04:
**	- RegistrationInterface is an InterfaceClass with a second base at +0x84 (the constructor 0x5EF0B0 stores
**	  0xAF7184 there, then the final table 0xAF6FF4, whose slot 0 is the adjustor 0x4297A0 'sub ecx, 0x84' to the
**	  deleting destructor 0x5F06C0): RegistrationCallbackClass, three empty slots overridden with 0x600000 /
**	  0x5FB950 / 0x5FB9F0, which run with this at +0x84.
**	- 0x5FB750: the ',' test (FAIL_IF line 2868, also inlined into 0x5FBD30), byte-exact; __stdcall like 0x5FB330
**	  (no caller; a member that ignores this compiles the same).
**	- 0x5FB7A0 / 0x5FB890: the 'OK' message boxes through the view at +0xBC (UIViewClass::Get_Unknown34), the
**	  reply's error number +0x3A0 and text +0x3A4 ('%s\n(%d)'), byte-exact.
**	- 0x5FB030 (no caller): the 'LABEL_TEXT' sub-object handed to the settings store's 0x7CA5C0, then the login
**	  task's (+0xC0) software mouse 0x5153B0; the device resolution is read and unused, byte-exact.
**	- 0x5FB950: the status line refresh in state 0x16 (0x5FAF60 inlined with Sub14C's slot 0x2C), byte-exact.
**	- 0x5FAC90 (0x5F3770's caller): 'Registered' 1 and 'Trial' +0x1CD written, then RegFlushKey(HKLM), byte-exact.
**	- 0x5FB9F0 (slot +0x0C): the reply's error shown, then on (state 1 and +0x11B) / (state 0 and +0x11D) / state
**	  0x17 the combo boxes reset (0x57B3C0) and the login task's Go_Next_Step, else the previous state popped.
**	  Functional: the client calls the map's begin 0x606960, the iterator's operator++(int) 0x604DF0 and
**	  pop_back's copy / _Destroy out of line (as the map inserts 0x5F9700 / 0x5F98C0 / 0x5F9EB0 call begin and
**	  pair's constructor 0x6074E0), and destroys the reply under one EH state with only its first string's
**	  _Tidy inlined; ours inlines all of them. An empty ~RegistrationErrorStruct and a named local were tried.
**	- The map at +0x108: 12-byte values whose first dword is a UIComboBoxClass * (0x5FBD30 reads its Selected
**	  through the out-of-line getter 0x5FFFE0, 'mov eax, [ecx + 0xE0]'). 0x5FFFC0 is UIViewClass::Get_Unknown34
**	  emitted out of line for 0x5FBD30; 0x600000 is slot +0x04 of 0xAF6FF4 (the reply handler): all three belong
**	  to this object (triage's run 0x5FFFC0-0x603660).
**	- 0x5F0A80: a template instance the map / vector members bring in (verify matches it in this object).
**	- The combo boxes' fillers, byte-exact: 0x5F9700 (the countries +0xAC into box 0x19), 0x5F98C0 (the numbers
**	  from..to, '%d', into box 0x16), 0x5F9A90 (+0x5A8's texts into box 0x22), 0x5F9C90 (+0x578's entries whose
**	  id list holds +0x38C into box 0x29), 0x5F9EB0 (the payment types +0x598 into box 0x2A; G_ASSERT 'typetext'
**	  line 2342). Unknown108[id] is the map's operator[]: its insert inlined with begin, --, _Insert and pair's
**	  constructor out of line, as the client has them. The status table ends at 0xB86048, where the payment type
**	  names and descriptions follow (two 7-entry tables).
*/
