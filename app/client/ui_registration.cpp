// Earth & Beyond client (reconstruction): app\Client\ui_registration.cpp. Notes at the end of the file.
#include "ui_registration.h"
#include "..\..\wwlib\registry.h"
#include "fontmanager.h"
#include <stdio.h>
#include <stdarg.h>
#include <string>

extern FontManagerClass *Unknown_BE67C4;  // NAME 1: 0xBE67C4

static const char *RegistrationKey = "Software\\Westwood Studios\\Earth and Beyond\\Registration";  // NAME 1 [RegistrationKey]: client .data 0xB85EFC

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
	0,
	0,
	"Credit Card",
	"GameTime",
	0,
	"Check",
	0,
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
	virtual void Slot11() {}
	virtual const char *Slot12(int arg) { return 0; }  // NAME 1 [Slot12]: slot 0x30
};

struct RegistrationInterface;  // NAME 1 [RegistrationInterface]: provisional (D137)

void __stdcall Unknown_5F8DB0(int arg);  // NAME 1 [Unknown_5F8DB0]: ui_registration.cpp

struct RegistrationInterface {
	char pad_000[0x120];
	RenderObjClass *FontId;  // NAME 1 [FontId]: +0x120 (the font manager's key, a render object)
	char pad_124[0x14C - 0x124];
	Sub14CClass Sub14C;   // NAME 1 [Sub14C]: +0x14C
	char pad_150[0x1B0 - (0x14C + sizeof(Sub14CClass))];
	int StatusIndex;      // NAME 1 [StatusIndex]: +0x1B0

	void Unknown_5F3770(int arg);
	void Unknown_5FAF60(int arg);
	void Unknown_5FAFF0(const char *text);
};

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

// NOT EXACT: register
// @0x5FAF60
void RegistrationInterface::Unknown_5FAF60(int arg)
{
	char buffer[1024];
	const char *str = Sub14C.Slot12(arg);
	_snprintf(buffer, 1024, "%s...", str);
	FontManagerClass *fm = Unknown_BE67C4;
	ManagedFontClass *font = fm->Find(FontId);
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

/*
**	Reconstruction notes (builder ureg2, 2026-11-02)
**
**	ui_registration.cpp (the client's object: asserts lines 369-3637; client 0x5EF0B0-0x5FBD30+).
**	- 0x5FAD50 / 0x5FADE0 (UREG): demo/registration predicates reading "Registered" and "Trial".
**	- 0x5FAE70: trivial registration predicate returning true (byte-exact, short, 3 bytes).
**	- 0x5FAE90: wrap text formatter helper (byte-exact, 154 bytes).
**	- 0x5FAF60: status text formatter (functional, register choice).
**	- 0x5FAFF0: registration window status helper (byte-exact, 37 bytes).
**	- 0x5FB330: hyphen stripper string helper (byte-exact, 401 bytes).
**	- 0x5FB540: space stripper string helper (byte-exact, 401 bytes).
*/
