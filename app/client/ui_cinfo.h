// Earth & Beyond client (reconstruction): app\client\ui_cinfo.h, the character info window (the client's ui_cinfo.cpp,
// 'e:\build\beta\G\app\Client\ui_cinfo.cpp' at 0xB7DCA4; this header's name is provisional, NAME 1). Laid out from the
// constructor 0x5734D0 and the tabs' builder 0x5738B0.
#ifndef UI_CINFO_H  // NAME 1 [ui_cinfo.h]: provisional header name for ui_cinfo.cpp
#define UI_CINFO_H
#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"
class ButtonGadgetClass;
class Unknown549640Class;	// the first tab (0x124 bytes; 'ATTRIBUTES_MENU')
class Unknown5C4170Class;	// the second tab (0xEC bytes; 'MISSIONS_MENU')
class Unknown58D080Class;	// the third tab (0xE4 bytes; 'FACTION_MENU')
class UIBioClass;			// ui_bio.h's bio tab (0xB0 bytes)
// The character info window (0xD8 bytes, tables 0xAF06E4 / 0xAF06D8 / 0xAF06CC; 0x693230 makes it): a 'TSI15A' mesh,
// the strings of 'fa_cinfo.ini', three tab buttons and an exit button (tooltips from section 'UI_CINFO') and four tabs
class UICinfoClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\Client\ui_cinfo.cpp
{
public:
	enum { MAX_BUTTONS = 5 };							// NAME 1: the constructor's loop (buttons 1..4)
	UICinfoClass(SClient *client);						// 0x5734D0 (0xD8 bytes; deleting 0x573740)  // NAME 1 [client]: what the base's 0x5BB320 takes
	virtual ~UICinfoClass();							// 0x573770 ('fa_cinfo.ini' unloaded)
	virtual void Unknown_54();							// 0x5738B0 (the four tabs made, parented and initialised)
	virtual void Unknown_58();							// 0x573CE0 (activated: the mesh's fonts, the tab +0xB0 shown, the exit button on)
	virtual void Unknown_5C();							// 0x573D50 (deactivated: the shown tab kept in +0xB0)
	virtual void Unknown_60();							// 0x573DA0 (the base's)
	virtual bool Unknown_68(int key);					// 0x573DC0 (keys 1..3 the tabs, 4 the PDA, 6 taken)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x573C70 (the base's, then each tab's with false)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_573E80(int tab);						// 0x573E80 (the shown tab, 1..3 or 0: the old one off, the new one on)  // NAME 1 [tab]: neutral
	void Unknown_5741A0();								// 0x5741A0 (the menu fonts: the shown tab's lit)
	bool Unknown_574220(int index);						// 0x574220 (index 0..2: that tab shown, or kept to show; G_ASSERT 'false' otherwise)  // NAME 1 [index]: neutral
	void Unknown_5742B0(int index);						// 0x5742B0 (index 0..2: that tab shown, or kept for activation)  // NAME 1 [index]: neutral
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: 0x573E80's FAIL_IF '!Get_Client()')  // NAME 3: the FAIL_IF's

	int UnknownAC;										// +0xAC (the shown tab, 1..3; 0 none)
	int UnknownB0;										// +0xB0 (the tab to show on activation; the constructor: 1)
	RefCountPtr<Unknown549640Class> UnknownB4;			// +0xB4 (the first tab)
	RefCountPtr<Unknown5C4170Class> UnknownB8;			// +0xB8 (the second tab)
	RefCountPtr<Unknown58D080Class> UnknownBC;			// +0xBC (the third tab)
	RefCountPtr<UIBioClass> UnknownC0;					// +0xC0 (the bio tab)
	ButtonGadgetClass *UnknownC4[MAX_BUTTONS];			// +0xC4 (the constructor fills 1..4: 'TSI15A.BUTTON00'..'02', 'TSI15A.EXIT')
};
#endif // UI_CINFO_H
