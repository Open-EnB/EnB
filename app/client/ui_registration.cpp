// Earth & Beyond client (reconstruction): app\Client\ui_registration.cpp. Notes at the end of the file.
#include "ui_registration.h"
#include "..\..\wwlib\registry.h"
#include "..\..\wwlib\ini.h"
#include "fontmanager.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "charview.h"
#include "avatar.h"
#include "ui_combobox.h"
#include "textgadg.h"
#include <map>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "ww3d.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <string>

extern FontManagerClass *Unknown_BE67C4;  // NAME 1: 0xBE67C4

static const char *RegDataFile = "RG_REGDATA.INI";  // NAME 1 [RegDataFile]: client .data 0xB85EF8 (0x5F8FD0's INIClass)
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

static const int DaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };  // NAME 1 [DaysInMonth]: client .rdata 0xAF6F58 (0x5F9460)

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

// The text fields' map value (0x14 bytes: operator[] copies five dwords; the first is the field, 0x5FB0C0)
struct RegistrationTextStruct  // NAME 1 [RegistrationTextStruct]: neutral (D10)
{
	TextBaseGadgetClass *Gadget;	// +0x00  // NAME 1 [Gadget]: what 0x5FB0C0 calls Get_Text / Set_Text on
	int Unknown04;
	int Unknown08;
	int Unknown0C;
	int Unknown10;
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
	std::string Unknown00;		// +0x00 (0x5FA900: 0x7D9790's third string)
	std::string Unknown10;		// +0x10
	int Unknown20;				// +0x20
	std::string Unknown24;		// +0x24: the text handed to the combo box
	std::string Unknown34;		// +0x34
	int Unknown44;				// +0x44
	int Unknown48;				// +0x48: the sort key (0x60BCF0)
	std::vector<int> Unknown4C;	// +0x4C: the ids it applies to

	// 0x60BCF0 (std::sort's in 0x5FA5F0, by value: it destroys its copy)
	bool operator<(RegistrationUnknown578Struct other) const { return Unknown48 < other.Unknown48; }
};

// The 0x14-byte entries of the interface's vector +0x88 (0x5F8FD0 reads them from [CardPrefixes]; their operator= is 0x4450B0)
struct RegistrationCardPrefixStruct  // NAME 1 [RegistrationCardPrefixStruct]: the INI keys card%02dprefix / card%02did
{
	std::string Prefix;			// +0x00: card%02dprefix  // NAME 1 [Prefix]: the INI key
	int Id;						// +0x10: card%02did  // NAME 1 [Id]: the INI key
};

// The account details the interface holds at +0x3C4 (0x7D4E30 reads +0x58, +0x60, +0x70, +0x7C..+0x7E, +0x90, +0x91;
// its size is not known)
struct RegistrationUnknown3C4Struct  // NAME 1 [RegistrationUnknown3C4Struct]: neutral (D10)
{
	char Unknown00[0x80];
	std::string Unknown80;		// +0x80 (+0x444 in the interface): 'US' when empty (0x5FA900)
};

// What the interface holds at +0x1AC (0x5FB9F0 / 0x600000 ask it for the reply)
class RegistrationUnknown1ACClass  // NAME 1 [RegistrationUnknown1ACClass]: neutral (D10)
{
public:
	RegistrationErrorStruct Unknown_7D1AC0();	// 0x7D1AC0 (the reply's '/status', '/message', '/url')
	std::vector<RegistrationUnknown578Struct> Unknown_7D3570();	// 0x7D3570 (0x5FA0B0, 0x5FA5F0, 0x600000)
	bool Unknown_7D4E30(const char *a, const char *b, RegistrationUnknown3C4Struct *info, const char *c, bool d, int e, int f);	// 0x7D4E30 ('addAcctInfo')  // NAME 1 [a, b, info, c, d, e, f]: neutral
	bool Unknown_7D5600(const char *a, const char *b, RegistrationUnknown3C4Struct *info, bool d, int e);	// 0x7D5600  // NAME 1 [a, b, info, d, e]: neutral
	bool Unknown_7D9790(const char *a, const char *b, const char *c, int payment_type, const char *code, const char *gt_pricing, const char *gt_code, bool send_email, int f);	// 0x7D9790 ('payment_type' itoa, 'gt_pricing_selection_id', 'gt_validation_code', 'send_email')  // NAME 1 [a, b, c, payment_type, code, gt_pricing, gt_code, send_email, f]: the request keys they fill
};

extern int Unknown_BE8AD4;  // NAME 1 [Unknown_BE8AD4]: 0xBE8AD4 (0x7CF190 zeroes it; the registration requests' last argument)
void __cdecl Unknown_5FAE90(RegistrationInterface *self, const char *format, ...);

class RegistrationInterface : public InterfaceClass, public RegistrationCallbackClass  // the constructor 0x5EF0B0: InterfaceClass, then the table 0xAF7184 at +0x84
{
public:
	virtual void Unknown_5F00E0();	// 0x5FB950 (RegistrationCallbackClass's slot +0x08)
	virtual void Unknown_5F0100();	// 0x5FB9F0 (RegistrationCallbackClass's slot +0x0C)

	std::vector<RegistrationCardPrefixStruct> Unknown88;	// +0x88: RG_REGDATA.INI's [CardPrefixes] (0x5F8FD0)
	StringClass Unknown98;		// +0x98: [Connection] regserverurl (0x5F8FD0)
	StringClass Unknown9C;		// +0x9C: [Connection] regserverpage
	StringClass UnknownA0;		// +0xA0: [Connection] regserverver
	StringClass UnknownA4;		// +0xA4: [Settings] defaultcountry, then that entry of [Countries]
	StringClass UnknownA8;		// +0xA8: [Settings] csrphone
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
	char UnknownE8[0xF8 - 0xE8];
	std::map<int, RegistrationTextStruct> UnknownF8;	// +0xF8: the text fields by id (0x5FB0C0: 0x2B, the card number)
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
	char Unknown1B4[0x1C9 - 0x1B4];
	bool Unknown1C9;			// +0x1C9 (0x5FA900: +0x208 handed to 0x7D4E30 when set, else 0)
	char Unknown1CA[0x1CD - 0x1CA];
	bool Unknown1CD;			// +0x1CD: written as 'Trial' (0x5FAC90)
	char Unknown1CE[0x1D0 - 0x1CE];
	bool Unknown1D0;			// +0x1D0 (0x5FA900 sets it)
	bool Unknown1D1;			// +0x1D1 (0x5FA900: set or +0x1D3 set, 0x7D5600 instead of 0x7D4E30)
	char Unknown1D2;
	bool Unknown1D3;			// +0x1D3
	char Unknown1D4[0x1D8 - 0x1D4];
	std::string Unknown1D8;		// +0x1D8 (0x5FA900: the network calls' first string)
	std::string Unknown1E8;		// +0x1E8 (0x5FA900: their second string)
	char Unknown1F8[0x208 - 0x1F8];
	std::string Unknown208;		// +0x208 (0x5FA900: 0x7D4E30's fourth argument)
	char Unknown218[0x238 - 0x218];
	std::string Unknown238;		// +0x238 (0x5FA900: through 0x5FB330 to 0x7D9790)
	char Unknown248[0x328 - 0x248];
	RegistrationUnknown578Struct Unknown328;	// +0x328: +0x588's first after the sort (0x5FA5F0); 0x5FA900 hands +0x00 and +0x4C's [0] (the out-of-line operator[] 0x604130) to 0x7D9790
	char Unknown384[0x38C - 0x384];
	int Unknown38C;				// +0x38C: the id 0x5F9C90 looks for
	RegistrationErrorStruct Unknown390;	// +0x390 (0x5FB9F0 fills it from the reply)
	RegistrationUnknown3C4Struct Unknown3C4;	// +0x3C4 (0x5FA900 hands its address to 0x7D4E30 / 0x7D5600)
	char Unknown454[0x578 - (0x3C4 + sizeof(RegistrationUnknown3C4Struct))];
	std::vector<RegistrationUnknown578Struct> Unknown578;	// +0x578 (0x5F9C90)
	std::vector<RegistrationUnknown578Struct> Unknown588;	// +0x588: what 0x7D3570 returns, sorted by +0x48 (0x5FA5F0)
	std::vector<int> Unknown598;	// +0x598: the payment types offered (0x5F9EB0)
	std::vector<RegistrationUnknown5A8Struct> Unknown5A8;	// +0x5A8 (0x5F9A90)

	void Unknown_5F2990(int arg);
	void Unknown_5F3770(int arg);
	void Unknown_5F8FD0();
	void Unknown_5F8DB0(bool arg);  // 0x5F8DB0 (ecx = this in 0x5FA900; 'Button_Privacy')  // NAME 1 [arg]: neutral
	bool Unknown_5F8EA0(int month, int day, int year);  // 0x5F8EA0 (a date check: false before 1900)  // NAME 1 [month, day, year]: 0x5F9460 passes 2, 29, the year
	void Unknown_5F9460();
	void Unknown_5F9700();
	void Unknown_5F98C0(int from, int to);  // NAME 1 [from, to]: the loop's bounds
	void Unknown_5F9A90();
	void Unknown_5F9C90();
	void Unknown_5F9EB0();
	void Unknown_5FA5F0();
	bool Unknown_5FA900();
	void Unknown_5FAC90();
	void Unknown_5FAF60(int arg);
	void Unknown_5FAFF0(const char *text);
	void Unknown_5FB030();
	void Unknown_5FB0C0();
	std::string Unknown_5FB330(std::string str);  // 0x5FB330 (0x5FA900 calls it with ecx = this)
	std::string Unknown_5FB540(std::string str);
	bool Unknown_5FB750(const char *test);
	void Unknown_5FB890();
};

// @0x5F8FD0
void RegistrationInterface::Unknown_5F8FD0()
{
	INIClass ini(RegDataFile);
	ini.Get_String(Unknown98, "Connection", "regserverurl", "");
	ini.Get_String(Unknown9C, "Connection", "regserverpage", "");
	ini.Get_String(UnknownA0, "Connection", "regserverver", "");
	ini.Get_String(UnknownA4, "Settings", "defaultcountry", "");
	ini.Get_String(UnknownA4, "Countries", UnknownA4, "");
	ini.Get_String(UnknownA8, "Settings", "csrphone", "");
	int count = ini.Entry_Count("Countries");
	int i;
	for (i = 0; i < count; i++) {
		StringClass country;
		char key[15];
		_snprintf(key, 15, "country%03d", i);
		ini.Get_String(country, "Countries", key, "");
		UnknownAC.push_back(country);
	}
	int numprefixes = ini.Get_Int("CardPrefixes", "numprefixes", 0);
	Unknown88.clear();
	for (i = 0; i < numprefixes; i++) {
		StringClass prefix;
		char key[15];
		_snprintf(key, 15, "card%02dprefix", i);
		ini.Get_String(prefix, "CardPrefixes", key, "");
		RegistrationCardPrefixStruct entry;
		entry.Prefix = prefix;
		_snprintf(key, 15, "card%02did", i);
		entry.Id = ini.Get_Int("CardPrefixes", key, 0);
		Unknown88.push_back(entry);
	}
}

// NOT EXACT: shape (explain.py: 14 calls in ours, 13 in the client; the value_type temporaries' stack slots differ)
// @0x5F9460
void RegistrationInterface::Unknown_5F9460()
{
	int month = Unknown108[0x14].Combo->Selected;
	int day = Unknown108[0x15].Combo->Selected;
	int days = DaysInMonth[month];
	UIComboBoxClass *years = Unknown108[0x16].Combo;
	int year = (years->Selected < years->Items.size()) ? atoi(years->Items[years->Selected].c_str()) : 0;
	if (month == 1 && Unknown_5F8EA0(2, 29, year)) {
		days = 29;
	}
	UIComboBoxClass *days_box = Unknown108[0x15].Combo;
	days_box->Unknown_57B210();
	for (int i = 1; i <= days; i++) {
		char text[5];
		_snprintf(text, sizeof(text), "%02d", i);
		days_box->Unknown_57B000(text);
	}
	Unknown108[0x15].Combo->Unknown_57AC70(day);
}

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

// @0x5FA5F0
void RegistrationInterface::Unknown_5FA5F0()
{
	Unknown588 = Unknown1AC->Unknown_7D3570();
	std::sort(Unknown588.begin(), Unknown588.end());
	Unknown328 = Unknown588[0];
}

// @0x5FA900
bool RegistrationInterface::Unknown_5FA900()
{
	Unknown1D0 = true;
	if (Unknown11B) {
		StatusIndex = 0xB;
		Unknown_5FAE90(this, 0);
		if (Unknown1AC->Unknown_7D9790(Unknown1D8.c_str(), Unknown1E8.c_str(), Unknown328.Unknown00.c_str(), Unknown328.Unknown4C[0],
				Unknown_5FB330(Unknown238).c_str(), 0, 0, true, Unknown_BE8AD4)) {
			Unknown_5F8DB0(false);
			Unknown_5F3770(0x16);
			Unknown_5FAE90(this, "Registering GameTime...");
			UnknownBC->Get_Unknown198()->Unknown_784E00();
			return true;
		}
	} else {
		StatusIndex = 8;
		if (!Unknown1D1 && !Unknown1D3) {
			if (Unknown1AC->Unknown_7D4E30(Unknown1D8.c_str(), Unknown1E8.c_str(), &Unknown3C4,
					Unknown1C9 ? Unknown208.c_str() : 0, true, 0, Unknown_BE8AD4)) {
				Unknown_5F8DB0(false);
				Unknown_5F3770(0x16);
				Unknown_5FAE90(this, "Creating EA Account...");
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG21", false);
				return true;
			}
		} else {
			if (Unknown3C4.Unknown80.length() == 0) {
				Unknown3C4.Unknown80 = "US";
			}
			if (Unknown1AC->Unknown_7D5600(Unknown1D8.c_str(), Unknown1E8.c_str(), &Unknown3C4, false, Unknown_BE8AD4)) {
				Unknown_5F8DB0(false);
				Unknown_5F3770(0x16);
				Unknown_5FAE90(this, "Updating EA Account...");
				UnknownBC->Get_Unknown198()->Unknown_784C90("REG21", false);
				return true;
			}
		}
	}
	return false;
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

// NOT EXACT: inline-budget (explain.py: the client calls the second append(1, '-') and the first end()'s _Freeze out of line)
// @0x5FB0C0
void RegistrationInterface::Unknown_5FB0C0()
{
	if (UnknownD4 != 0x11 && UnknownD4 != 0x14) {
		return;
	}
	TextBaseGadgetClass *gadget = UnknownF8[0x2B].Gadget;
	bool changed = gadget->Changed;
	gadget->Changed = false;
	if (!changed) {
		return;
	}
	if (gadget->Active) {
		char *text = gadget->Get_Text();
		std::string number;
		int count = 1;
		for (unsigned int i = 0; i < min(strlen(text), 19u); i++) {
			if (text[i] != '-') {
				number += text[i];
				if ((count & 3) == 0) {
					number += '-';
				}
				count++;
			}
		}
		delete text;
		if (number.length() != 0 && *(number.end() - 1) == '-') {
			number.erase(number.end() - 1);
		}
		gadget->Set_Text(number.c_str(), false);
	}
	gadget->Changed = true;
}

// @0x5FB330
std::string RegistrationInterface::Unknown_5FB330(std::string str)
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
std::string RegistrationInterface::Unknown_5FB540(std::string str)
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
bool RegistrationInterface::Unknown_5FB750(const char *test)
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
**	- 0x5F9460 (functional): the day box refilled for the month (DaysInMonth 0xAF6F58; 29 when 0x5F8EA0 accepts
**	  29 February of the year box's year). 0x5FB0C0 (functional): the card number field (map +0xF8, id 0x2B)
**	  regrouped in fours with '-', at most 19 characters, in states 0x11 / 0x14.
**
**	Builder reg2, 2026-11-04:
**	- 0x5FB330 / 0x5FB540 / 0x5FB750 and 0x5F8DB0 are RegistrationInterface members (0x5FA900 sets ecx = this).
**	- 0x5FA900 (byte-exact): the account request: with +0x11B the GameTime one (0x7D9790, +0x238 through 0x5FB330;
**	  the client's own out-of-line c_str 0x553A60 and vector operator[] 0x604130 come out of ours too), else
**	  create (0x7D4E30) or, with +0x1D1 / +0x1D3, update (0x7D5600; +0x3C4's +0x80 defaults to 'US').
**	- 0x5FA5F0 (byte-exact): +0x588 = 0x7D3570's vector, sorted (operator< 0x60BCF0 compares +0x48, by value),
**	  its first copied to +0x328. That makes +0x328 (and +0x270 / +0x2CC, 0x5FA0B0) 0x5C-byte entries:
**	  RegistrationUnknown578Struct is four strings, three ints and the id vector.
**	- 0x5F8FD0 (byte-exact): RG_REGDATA.INI ([Connection], [Settings], [Countries] into +0xAC, [CardPrefixes]
**	  into +0x88). The key buffer is char[15] (with [16] the INIClass and the buffer swap stack slots).
*/
