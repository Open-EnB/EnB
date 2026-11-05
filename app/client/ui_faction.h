// Earth & Beyond client (reconstruction): app\client\ui_faction.h, the character info window's faction tab (the client's
// ui_faction.cpp, 'e:\build\beta\G\app\Client\ui_faction.cpp' at 0xB7F120; this header's name is provisional, NAME 1).
// Laid out from the constructor 0x58D080 and the destructor 0x58D1E0.
#ifndef UI_FACTION_H  // NAME 1 [ui_faction.h]: provisional header name for ui_faction.cpp
#define UI_FACTION_H
#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"
class GadgetSlider;			// gd_slider.h's slider (+0xBC)
class TextureClass;
class FontSys;
class Unknown58EB50Class;	// ui_faction_panel.cpp's panel (+0xAC; 200 bytes)
class Unknown58E210Class;	// the faction buttons (0x7C bytes; 0x58E210 makes them)
// The faction tab (0xE4 bytes, tables 0xAF1774 / 0xAF1768 / 0xAF175C; 0x5738B0 makes it, the third of the character info
// window's tabs): a 'tsi30a' mesh, the strings of 'fa_tsi30a.ini', six faction buttons, a slider (gadget 8) and a panel
class UIFactionClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\Client\ui_faction.cpp
{
public:
	enum { MAX_BUTTONS = 7 };							// NAME 1: the constructor clears 7, 0x58D400 makes 6 (keys 1..6)
	UIFactionClass(SClient *client);					// 0x58D080 (0xE4 bytes; deleting 0x58D1B0)  // NAME 1 [client]: as UIBioClass's
	virtual ~UIFactionClass();							// 0x58D1E0 ('fa_tsi30a.ini' unloaded)
	virtual void Unknown_54();							// 0x58D400 (the panel, the buttons, the slider; FAIL_IFs '!InterfaceMesh' / '!mesh')
	virtual void Unknown_58();							// 0x58D9F0 (activated: 0x58DC20(true) under a sort scope)
	virtual void Unknown_5C();							// 0x58DA80 (deactivated: the panel's +0x5C)
	virtual void Unknown_60();							// 0x58DAB0 (FAIL_IFs '!sclient' / '!ad_player': 0x58DC20(false))
	virtual bool Unknown_68(int key);					// 0x58DB30 (keys 1..6 a faction, 8 the slider, 9 taken)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x58D3C0 (the panel's with false, then the base's)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_58DC20(bool arg);						// 0x58DC20 (FAIL_IFs '!Get_Client()' / '!sclient' / '!aux_data': the reputations)  // NAME 1 [arg]: neutral
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: 0x58DC20's FAIL_IF '!Get_Client()', as UICinfoClass's)  // NAME 3: the FAIL_IF's
	void Unknown_Select(int index);						// (inline, defined in ui_faction.cpp: 0x58DB30's two forms)  // NAME 1: neutral (D10)  // NAME 1 [index]: neutral

	RefCountPtr<Unknown58EB50Class> UnknownAC;			// +0xAC (the panel, 0x58D400 makes it)
	RefCountPtr<TextureClass> StandardBttnTexture;		// +0xB0 (0x58D400: 0x66C4A0's)  // NAME 3: the G_ASSERT's (l.139)
	RefCountPtr<TextureClass> SelectedBttnTexture;		// +0xB4 ('I_blank_SEL.tga')  // NAME 3: the G_ASSERT's (l.143)
	RefCountPtr<FontSys> UnknownB8;						// +0xB8 ('AFFILIATED_TSI30A')
	GadgetSlider *UnknownBC;							// +0xBC (gadget 8; the destructor takes its model off)
	int UnknownC0;										// +0xC0 (the slider's value, as an int)
	int UnknownC4;										// +0xC4 (the selected faction, 0..5; -1 none)
	Unknown58E210Class *UnknownC8[MAX_BUTTONS];			// +0xC8 (the faction buttons, gadgets 1..6)
};
#endif // UI_FACTION_H
