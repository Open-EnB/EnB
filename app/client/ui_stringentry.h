// Earth & Beyond client (reconstruction): app\client\ui_stringentry.cpp's text entry window (0xD0 bytes; tables 0xAF88BC /
// 0xAF88B0 / 0xAF88A4; client 0x649890-0x64A32F). sclient.h includes it after the class's base. The trade window's +0xB4
// (its credits), ui_playeropt.cpp's +0xAC ('Ignore Player'), ui_guild.cpp's +0xC0 / +0xC4, ui_friends.cpp's +0xB0.
// Base: 0x649890 calls 0x5BB320, and the slots +0x7C / +0x88 / +0x8C of its table are Unknown5BB320Class's.
#ifndef UI_STRINGENTRY_H
#define UI_STRINGENTRY_H

class TextGadgetClass;
class GadgetClass;
class UIPTradeCreditsClass : public Unknown5BB320Class	// NAME 1: neutral (D10): what ui_ptrade.cpp asks it for (the tooltips' section is 'UI_STRINGENTRY')
{
public:
	UIPTradeCreditsClass(SClient *client, int a, int b);	// 0x649890 (the set-up passes the client, 0, 9)  // NAME 1 [client, a, b]: what the set-up passes
	~UIPTradeCreditsClass();							// 0x649A00 (table 0xAF88BC; slot 0x6499D0)
	virtual void Unknown_54();							// +0x54 0x649AA0 (the buttons, the entry, the back)
	virtual void Unknown_58();							// +0x58 0x649E90 (activated: the entry focused and emptied)
	virtual void Unknown_5C();							// +0x5C 0x649F70 (deactivated)
	virtual void Unknown_60();							// +0x60 0x649FE0 (every frame: the entry redrawn; closed with no button: accepted)
	virtual bool Unknown_68(int key);					// +0x68 0x64A110 (the buttons: 1, 2 cancel, 3 confirm, 4 the entry again)  // NAME 1 [key]: InterfaceClass's
	int Unknown_64A220();								// 0x64A220: its +0xC0 (0x5E39F0: 0 still open, 2 accepted)
	const char *Unknown_64A240();						// 0x64A240 (0x5E39F0: the amount typed, to atoi)
	void Unknown_64A270(const char *text);				// 0x64A270 (the set-up: 'Enter Amount of Credits': the 'TITLE01' style's text)  // NAME 1 [text]: neutral
	void Unknown_64A2B0(const char *text);				// 0x64A2B0 (slot +0x68, key 4: the amount offered so far, or "")  // NAME 1 [text]: neutral

	TextGadgetClass *Unknown0AC;						// +0xAC the entry ('VALUE_BUTTON01' on the 'TEXT_BOX' model)
	GadgetClass *Unknown0B0[4];							// +0xB0 the buttons ('CANCEL', 'EXIT', 'CONFIRM', 'VALUE_BUTTON')
	int Unknown0C0;										// +0xC0 (0 open, 1 cancelled, 2 accepted)
	int Unknown0C4;										// +0xC4 (the constructor's a: the entry's flags, 0, 1 or else)
	int Unknown0C8;										// +0xC8 (the constructor's b: the entry's length)
	bool Unknown0CC;									// +0xCC (set when the entry is focused: slot +0x60 accepts when it loses it)
	bool Unknown0CD;									// +0xCD (slot +0x60 passes the text through 0x4BE9D0)
};

#endif
