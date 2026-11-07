// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_registration.h (PROVISIONAL: name assumed
**	from ui_registration.cpp per D137)
**
**	ui_registration: registration UI dialogs and checks.
**	0x5FAD50: reads the "Registered" bool from the registry key
**	0x5FADE0: reads the "Trial" bool from the registry key
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_REGISTRATION_H  // NAME 3 [ui_registration.h]: header name assumed from ui_registration.cpp
#define UI_REGISTRATION_H

// 0x5FAD50: reads the "Registered" bool from the registry key (ui_registration.cpp)
bool Unknown_5FAD50();

// 0x5FADE0: reads the "Trial" bool from the registry key (ui_registration.cpp)
bool Unknown_5FADE0();

// 0x5FAE70: registration predicate (ui_registration.cpp)
bool Unknown_5FAE70();

#include <string>

#ifdef __UI_REGISTRATION_MEMBERS
// ui_registration.cpp's classes (provisional place, D137: only that file sees them)
class Sub14CClass {  // NAME 1 [Sub14CClass]: vtable 0xAF7130 (0x5F03B0, _purecall, the shared 0x5F0170 / 0x5F0280 / 0x5F02A0)
public:
	virtual ~Sub14CClass() {}
	virtual void Slot1() = 0;
	virtual void Slot2() = 0;
	virtual void Slot3() = 0;
	virtual void Slot4() = 0;
	virtual void Slot5() = 0;
	virtual void Slot6() = 0;
	virtual void Slot7() = 0;
	virtual void Slot8() = 0;
	virtual void Slot9() = 0;
	virtual std::string Slot10() { return std::string(""); }  // NAME 1 [Slot10]: slot 0x28, 0x5F0170
	virtual int Slot11() = 0;  // NAME 1 [Slot11]: slot 0x2C (0x5FB950 hands its result to slot 0x30)
	virtual const char *Slot12(int arg) = 0;  // NAME 1 [Slot12]: slot 0x30
	virtual void Slot13() = 0;
	virtual void Slot14() {}  // NAME 1 [Slot14]: slot 0x38, 0x5F0280
	virtual std::string Slot15() { return std::string(""); }  // NAME 1 [Slot15]: slot 0x3C, 0x5F02A0
	virtual void Slot16() = 0;
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

// What the implementation holds at +0x18 (0x48 bytes: table 0xB073C8, a second 0xB073B0 at +4; 0x7E1E40 / 0x7E1F10)
class Sub14CUnknown18Class  // NAME 1 [Sub14CUnknown18Class]: neutral (D10)
{
public:
	Sub14CUnknown18Class();		// 0x7E1E40
	~Sub14CUnknown18Class();	// 0x7E1F10
	char Unknown00[0x48];
};

// What the implementation holds at +0x10: released through slot +0x08 (0x7E0070 fills it from a COM call)
class Sub14CUnknown10Class  // NAME 1 [Sub14CUnknown10Class]: neutral (D10)
{
public:
	virtual long __stdcall Slot0() = 0;
	virtual unsigned long __stdcall Slot1() = 0;
	virtual unsigned long __stdcall Release() = 0;
};

struct Sub14CUnknown10Ptr  // NAME 1 [Sub14CUnknown10Ptr]: neutral (D10): 0x5F0590's own EH state around the release
{
	Sub14CUnknown10Ptr() : Pointer(0) {}
	~Sub14CUnknown10Ptr() { if (Pointer) Pointer->Release(); }
	Sub14CUnknown10Class *Pointer;  // NAME 1 [Pointer]: neutral
};

// The implementation at +0x14C (0x60 bytes; tables 0xAF70DC / 0xAF70C8, destructor 0x5F0590, deleting 0x5F0630).
// Slots 0x7E0070-0x7E15B0, 0x7DFED0-0x7E0050 live in their own object.
class Sub14CImplClass : public Sub14CClass, public RegistrationCallbackClass  // NAME 1 [Sub14CImplClass]: neutral (D10)
{
public:
	Sub14CImplClass() : Unknown08(0), Unknown14(0) { Slot1(); }
	virtual ~Sub14CImplClass() { Slot2(); }
	virtual void Slot1();	// 0x7E0070
	virtual void Slot2();	// 0x7E0180
	virtual void Slot3();	// 0x7E01B0
	virtual void Slot4();	// 0x7E0960
	virtual void Slot5();	// 0x7E09F0
	virtual void Slot6();	// 0x7E0A80
	virtual void Slot7();	// 0x7E0BC0
	virtual void Slot8();	// 0x7E1200
	virtual void Slot9();	// 0x7E1940
	virtual int Slot11();	// 0x7E1C60
	virtual const char *Slot12(int arg);	// 0x7E1C80
	virtual void Slot13();	// 0x7E1290
	virtual void Slot14();	// 0x7E1A20
	virtual std::string Slot15();	// 0x7E1310
	virtual void Slot16();	// 0x7E15B0
	virtual void Unknown_5F00C0();	// 0x7DFED0
	virtual void Unknown_5F00E0();	// 0x7E0030
	virtual void Unknown_5F0100();	// 0x7E0050

	int Unknown08;			// +0x08
	int Unknown0C;			// +0x0C
	Sub14CUnknown10Ptr Unknown10;	// +0x10
	int Unknown14;			// +0x14
	Sub14CUnknown18Class Unknown18;	// +0x18
};

// What the reply reader 0x7D1AC0 returns (0x5FB9F0 assigns it member by member to +0x390)
struct RegistrationErrorStruct  // NAME 1 [RegistrationErrorStruct]: neutral (D10)
{
	RegistrationErrorStruct() : Unknown10(0) {}
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
	TextGadgetClass *Gadget;	// +0x00  // NAME 1 [Gadget]: what 0x5FB0C0 calls Get_Text / Set_Text on
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
	RegistrationUnknown578Struct() : Unknown00(""), Unknown10(""), Unknown20(0), Unknown24(""), Unknown34(""), Unknown44(0), Unknown48(0) {}
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

	bool Is_Prefix_Of(const char *text) const;	// 0x5F2950  // NAME 1 [Is_Prefix_Of, text]: what it does
};

// The account details the interface holds at +0x3C4 (0xB4 bytes; the constructor 0x5EF0B0, 0x600000's copy from
// 0x7D21C0; 0x7D4E30 reads +0x58, +0x60, +0x70, +0x7C..+0x7E, +0x90, +0x91)
struct RegistrationUnknown3C4Struct  // NAME 1 [RegistrationUnknown3C4Struct]: neutral (D10)
{
	RegistrationUnknown3C4Struct() : Unknown50(0x41), Unknown54(0), Unknown58(0), Unknown7C(0), Unknown7D(0), Unknown7E(0), Unknown90(0), Unknown91(0) {}
	std::string Unknown00[5];	// +0x00
	int Unknown50;				// +0x50 (+0x414): 'A'
	int Unknown54;				// +0x54
	int Unknown58;				// +0x58
	std::string Unknown5C;		// +0x5C
	std::string Unknown6C;		// +0x6C
	char Unknown7C;				// +0x7C: the day of the month (the constructor)
	char Unknown7D;				// +0x7D: the month (from 1)
	short Unknown7E;			// +0x7E: the year
	std::string Unknown80;		// +0x80 (+0x444 in the interface): 'US' when empty (0x5FA900)
	bool Unknown90;				// +0x90
	bool Unknown91;				// +0x91
	std::string Unknown94;		// +0x94
	std::string UnknownA4;		// +0xA4
};

// What the interface holds at +0x478 (0xE8 bytes; destructor 0x5F0CC0)
struct RegistrationUnknown478Struct  // NAME 1 [RegistrationUnknown478Struct]: neutral (D10)
{
	RegistrationUnknown478Struct() : Unknown00(0) {}
	int Unknown00;				// +0x00
	std::string Unknown04;		// +0x04
	std::string Unknown14;		// +0x14
	std::string Unknown24;		// +0x24
	std::string Unknown34;		// +0x34
	std::string Unknown44;		// +0x44
	std::string Unknown54;		// +0x54
	std::string Unknown64;		// +0x64
	std::string Unknown74;		// +0x74
	std::string Unknown84;		// +0x84
	RegistrationUnknown5A8Struct Unknown94;	// +0x94 (implicit constructor 0x5F03E0, destructor 0x5F0410)
	std::string UnknownA8;		// +0xA8
	std::string UnknownB8;		// +0xB8
	std::string UnknownC8;		// +0xC8
	std::string UnknownD8;		// +0xD8
};

// The 0xF4-byte entries of the interface's vector +0x568 (destructor 0x449CC0, not this object's; nothing here reads them)
struct RegistrationUnknown568Struct  // NAME 1 [RegistrationUnknown568Struct]: neutral (D10)
{
	~RegistrationUnknown568Struct();
	char Unknown00[0xF4];
};

// What the interface holds at +0x1AC (the function-static 0xBDA960: 0x5FB9F0 / 0x600000 ask it for the reply)
class RegistrationUnknown1ACClass  // NAME 1 [RegistrationUnknown1ACClass]: neutral (D10)
{
public:
	RegistrationUnknown1ACClass() : Unknown04(0) {}
	virtual ~RegistrationUnknown1ACClass() { Unknown_7CF190(); }	// 0x5F0680; the atexit 0x5F0660
	static void Unknown_7CF190();					// 0x7CF190 (clears its globals; the constructor's and 0x5F1660's)
	void Unknown_7CEF30(Sub14CClass *a, const char *url, const char *page, const char *version, RegistrationCallbackClass *callback);	// 0x7CEF30 (the constructor)  // NAME 1 [a, url, page, version, callback]: the [Connection] keys
	RegistrationErrorStruct Unknown_7D1AC0();	// 0x7D1AC0 (the reply's '/status', '/message', '/url')
	std::vector<RegistrationUnknown578Struct> Unknown_7D3570();	// 0x7D3570 (0x5FA0B0, 0x5FA5F0, 0x600000)
	bool Unknown_7D4E30(const char *a, const char *b, RegistrationUnknown3C4Struct *info, const char *c, bool d, int e, int f);	// 0x7D4E30 ('addAcctInfo')  // NAME 1 [a, b, info, c, d, e, f]: neutral
	bool Unknown_7D5600(const char *a, const char *b, RegistrationUnknown3C4Struct *info, bool d, int e);	// 0x7D5600  // NAME 1 [a, b, info, d, e]: neutral
	bool Unknown_7D9790(const char *a, const char *b, const char *c, int payment_type, const char *code, const char *gt_pricing, const char *gt_code, bool send_email, int f);	// 0x7D9790 ('payment_type' itoa, 'gt_pricing_selection_id', 'gt_validation_code', 'send_email')  // NAME 1 [a, b, c, payment_type, code, gt_pricing, gt_code, send_email, f]: the request keys they fill

	char Unknown04;
};


// The buttons' map value (0x18 bytes: 0x5F2B20 fills it, 0x5F3470 / 0x5F1A10 pick by the group +0x12)
struct RegistrationButtonStruct  // NAME 1 [RegistrationButtonStruct]: neutral (D10)
{
	ButtonGadgetClass *Gadget;	// +0x00
	RenderObjClass *RObj;		// +0x04: the sub-object (released after the insert)  // NAME 1 [RObj]: what 0x5F2B20 stores
	const char *Name;			// +0x08: the sub-object's name  // NAME 1 [Name]: 0x5F2B20's
	const char *Style;			// +0x0C: the font style dimmed with it (0x5F2990)  // NAME 1 [Style]: 0x6D25D0's argument
	unsigned short Key;			// +0x10: its control key  // NAME 1 [Key]: 0x721E60's
	unsigned short Group;		// +0x12: the radio group  // NAME 1 [Group]: 0x5F3470
	bool Selected;				// +0x14: radio on  // NAME 1 [Selected]: what the on texture goes with
	bool InFrame;				// +0x15: in the frame model, not the page's  // NAME 1 [InFrame]: 0x5F2990's model choice
};

class RegistrationInterface : public InterfaceClass, public RegistrationCallbackClass  // the constructor 0x5EF0B0: InterfaceClass, then the table 0xAF7184 at +0x84
{
public:
	RegistrationInterface(UIViewClass *view, LayerClass *layer, int mode);	// 0x5EF0B0 (charview.cpp's modes 9-12 pass 3, 20, 3, 0)  // NAME 1 [mode]: neutral
	virtual ~RegistrationInterface();	// 0x5F0F10 (deleting 0x5F06C0)
	virtual void Unknown_60();			// 0x5F1730
	virtual bool Unknown_68(int key);	// 0x5F1A10
	virtual void Unknown_5F00E0();	// 0x5FB950 (RegistrationCallbackClass's slot +0x08)
	virtual void Unknown_5F0100();	// 0x5FB9F0 (RegistrationCallbackClass's slot +0x0C)

	std::vector<RegistrationCardPrefixStruct> Unknown88;	// +0x88: RG_REGDATA.INI's [CardPrefixes] (0x5F8FD0)
	StringClass Unknown98;		// +0x98: [Connection] regserverurl (0x5F8FD0)
	StringClass Unknown9C;		// +0x9C: [Connection] regserverpage
	StringClass UnknownA0;		// +0xA0: [Connection] regserverver
	StringClass UnknownA4;		// +0xA4: [Settings] defaultcountry, then that entry of [Countries]
	StringClass UnknownA8;		// +0xA8: [Settings] csrphone
	std::vector<StringClass> UnknownAC;	// +0xAC: the countries (0x5F8FD0 reads them from RG_REGDATA.INI; 0x5F9700 fills the combo box 0x19)
	CharacterViewClass *UnknownBC;	// +0xBC: the constructor's +0x48 slot +0x28 (0x5FB9F0 calls CharacterViewClass::Unknown_67A8D0 on it); the message box +0x34
	LoginTaskClass *UnknownC0;	// +0xC0: the constructor's view +0xBC's task +0x14, slot +0x08 (LoginTaskClass's returns this)
	bool UnknownC4;				// +0xC4
	bool UnknownC5;				// +0xC5: a message box is up: the keys and gadgets are off (0x5F2280)
	bool UnknownC6;				// +0xC6
	bool UnknownC7;				// +0xC7 (0x5FB9F0 sets it)
	bool UnknownC8;				// +0xC8
	bool UnknownC9;				// +0xC9
	bool UnknownCA;				// +0xCA (0x5FB9F0 sets it)
	int UnknownCC;				// +0xCC
	LayerClass *Layer;			// +0xD0: the constructor's layer
	int UnknownD4;				// +0xD4: the screen's state (0x5FAFF0's 0x5F3770(0x16); 0x5FB950 tests 0x16)
	std::vector<int> UnknownD8;	// +0xD8: the states to go back to (0x5FB9F0 pops one into 0x5F3770)
	std::map<int, RegistrationButtonStruct> UnknownE8;	// +0xE8: the buttons by id (0x5F2B20)
	std::map<int, RegistrationTextStruct> UnknownF8;	// +0xF8: the text fields by id (0x5FB0C0: 0x2B, the card number)
	std::map<int, RegistrationComboStruct> Unknown108;	// +0x108: the combo boxes by id (0x5F9700 0x19, 0x5F98C0 0x16, 0x5F9EB0 0x2A; 0x5FB9F0 calls 0x57B3C0 on all)
	bool Unknown118;			// +0x118: the page's fonts are loaded (0x5F3600)
	bool Unknown119;			// +0x119: the 'YES_NO' box of key 0x2D is up (0x5F2280)
	bool Unknown11A;			// +0x11A: the account site's box is up (0x5F2280)
	bool Unknown11B;			// +0x11B (0x5FB9F0: with state 1)
	bool Unknown11C;			// +0x11C
	bool Unknown11D;			// +0x11D (0x5FB9F0: with state 0; the constructor sets it with state 0x17)
	bool Unknown11E;			// +0x11E
	RefCountPtr<RenderObjClass> DialogRenderObj;  // NAME 1 [DialogRenderObj]: +0x120 (the page's model, the font manager's key; destructor 0x4740B0)
	struct tm Unknown124;		// +0x124: today (the constructor's gmtime)
	int Unknown148;				// +0x148: the text field with the focus (0x5F8CD0)
	Sub14CImplClass Sub14C;   // NAME 1 [Sub14C]: +0x14C
	RegistrationUnknown1ACClass *Unknown1AC;	// +0x1AC (0x5FB9F0's 0x7D1AC0)
	int StatusIndex;      // NAME 1 [StatusIndex]: +0x1B0
	RefCountPtr<TextureClass> Unknown1B4;	// +0x1B4: 'RG_Radio_on.tga'
	RefCountPtr<TextureClass> Unknown1B8;	// +0x1B8: 'RG_Radio_off.tga'
	HAnimComboClass *Unknown1BC;	// +0x1BC: state 0x16's animation (0x5F1730; 0x5F3600 deletes it)
	RefCountPtr<HAnimClass> Unknown1C0;	// +0x1C0: its motion (destructor 0x57B6E0)
	int Unknown1C4;				// +0x1C4: frames to the web page (0x5F1730)
	bool Unknown1C8;			// +0x1C8
	bool Unknown1C9;			// +0x1C9 (0x5FA900: +0x208 handed to 0x7D4E30 when set, else 0)
	bool Unknown1CA;
	bool Unknown1CB;
	bool Unknown1CC;
	bool Unknown1CD;			// +0x1CD: written as 'Trial' (0x5FAC90); 0x5FA0B0 sets it when it picks an 'EB-TRIAL' entry
	bool Unknown1CE;
	bool Unknown1CF;			// +0x1CF (0x5FA0B0: the 'EB-TRIAL' entry is taken when clear, or with +0x11D)
	bool Unknown1D0;			// +0x1D0 (0x5FA900 sets it)
	bool Unknown1D1;			// +0x1D1 (0x5FA900: set or +0x1D3 set, 0x7D5600 instead of 0x7D4E30)
	bool Unknown1D2;
	bool Unknown1D3;			// +0x1D3
	bool Unknown1D4;
	bool Unknown1D5;
	std::string Unknown1D8;		// +0x1D8 (0x5FA900: the network calls' first string)
	std::string Unknown1E8;		// +0x1E8 (0x5FA900: their second string)
	std::string Unknown1F8;		// +0x1F8
	std::string Unknown208;		// +0x208 (0x5FA900: 0x7D4E30's fourth argument)
	std::string Unknown218;		// +0x218
	std::string Unknown228;		// +0x228
	std::string Unknown238;		// +0x238 (0x5FA900: through 0x5FB330 to 0x7D9790)
	std::string Unknown248;		// +0x248
	std::string Unknown258;		// +0x258
	int Unknown268;				// +0x268: 2
	int Unknown26C;				// +0x26C: 8
	RegistrationUnknown578Struct Unknown270;	// +0x270: +0x578's 'EB-TRIAL' entry (0x5FA0B0)
	RegistrationUnknown578Struct Unknown2CC;	// +0x2CC: the 'EB' entry offering payment type 3 (0x5FA0B0)
	RegistrationUnknown578Struct Unknown328;	// +0x328: +0x588's first after the sort (0x5FA5F0); 0x5FA900 hands +0x00 and +0x4C's [0] (the out-of-line operator[] 0x604130) to 0x7D9790
	int Unknown384;				// +0x384: combo box 0x29's selection (0x5F1A10)
	int Unknown388;				// +0x388: combo box 0x2A's selection (0x5F1A10)
	int Unknown38C;				// +0x38C: the id 0x5F9C90 looks for
	RegistrationErrorStruct Unknown390;	// +0x390 (0x5FB9F0 fills it from the reply)
	RegistrationUnknown3C4Struct Unknown3C4;	// +0x3C4 (0x5FA900 hands its address to 0x7D4E30 / 0x7D5600)
	RegistrationUnknown478Struct Unknown478;	// +0x478
	int Unknown560;				// +0x560
	int Unknown564;				// +0x564
	std::vector<RegistrationUnknown568Struct> Unknown568;	// +0x568
	std::vector<RegistrationUnknown578Struct> Unknown578;	// +0x578 (0x5F9C90)
	std::vector<RegistrationUnknown578Struct> Unknown588;	// +0x588: what 0x7D3570 returns, sorted by +0x48 (0x5FA5F0)
	std::vector<int> Unknown598;	// +0x598: the payment types offered (0x5F9EB0)
	std::vector<RegistrationUnknown5A8Struct> Unknown5A8;	// +0x5A8 (0x5F9A90)

	void Unknown_5F1660();
	void Unknown_5F1940();
	void Unknown_5F2280();
	void Unknown_5F2990(bool on);  // NAME 1 [on]: the gadgets' slot +0x40
	void Unknown_5F2A80(int on, UIComboBoxClass *except);  // NAME 1 [on, except]: neutral
	void Unknown_5F2B20(int id, const char *name, const char *style, unsigned short key, unsigned short group, bool in_frame);  // NAME 1 [id, name, style, key, group, in_frame]: neutral
	void Unknown_5F2D30(int id, const char *name, const char *style, int length, const char *text, int next, bool password, unsigned int flags, const char *chars);  // NAME 1 [id, name, style, length, text, next, password, flags, chars]: neutral
	void Unknown_5F2F90(int id, const char *name, int a, int b);  // NAME 1 [id, name, a, b]: neutral
	void Unknown_5F3110();
	void Unknown_5F3250(int id);  // NAME 1 [id]: the button's
	RenderObjClass *Unknown_5F33E0(unsigned short group);  // NAME 1 [group]: neutral
	void Unknown_5F3470(unsigned short group);  // NAME 1 [group]: neutral
	void Unknown_5F3510(int id, bool on);  // NAME 1 [id, on]: neutral
	void Unknown_5F3600();
	void Unknown_5F3770(int arg);
	void Unknown_5F84E0(const char *name, const char *text);  // NAME 1 [name, text]: 0x6D0A70's
	void Unknown_5F8520(const char *name, const char *text);  // NAME 1 [name, text]: 0x6CDBC0's
	void Unknown_5F8560();
	void Unknown_5F86F0();
	void Unknown_5F87D0();
	int Unknown_5F89F0();
	int Unknown_5F8A90();
	void Unknown_5F8AD0();
	void Unknown_5F8CD0(int id, bool on);  // NAME 1 [id, on]: neutral
	void Unknown_5F8FD0();
	void Unknown_5F8DB0(bool arg);  // 0x5F8DB0 (ecx = this in 0x5FA900; 'Button_Privacy')  // NAME 1 [arg]: neutral
	bool Unknown_5F8EA0(int month, int day, int year);  // 0x5F8EA0 (a date check: false before 1900)  // NAME 1 [month, day, year]: 0x5F9460 passes 2, 29, the year box's year
	bool Unknown_5F8F60(const char *text);  // NAME 1 [text]: neutral
	void Unknown_5F9460();
	void Unknown_5F9700();
	void Unknown_5F98C0(int from, int to);  // NAME 1 [from, to]: the loop's bounds
	void Unknown_5F9A90();
	void Unknown_5F9C90();
	void Unknown_5F9EB0();
	void Unknown_5FA0B0();
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
	void Unknown_5FBD30();
	bool Unknown_603820(const char *number);  // NAME 1 [number]: a card number's Luhn check
	int Unknown_6039E0(const char *number);  // NAME 1 [number]: neutral
	bool Unknown_603A60(int month, int day, int year, int age);  // NAME 1 [month, day, year, age]: neutral
	void Unknown_603B20();
};
#endif // __UI_REGISTRATION_MEMBERS

#endif // UI_REGISTRATION_H
