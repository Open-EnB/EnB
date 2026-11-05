// Earth & Beyond client (reconstruction): app\client\ui_missn.h, the character info window's missions tab (the client's
// ui_missn.cpp, 'e:\build\beta\G\app\Client\ui_missn.cpp' at 0xB82708; this header's name is provisional, NAME 1).
// Laid out from the constructor 0x5C4170 and the destructor 0x5C42E0.
#ifndef UI_MISSN_H  // NAME 1 [ui_missn.h]: provisional header name for ui_missn.cpp
#define UI_MISSN_H
#include "sclient.h"
class GadgetSlider;			// gd_slider.h's slider (+0xB8)
class GadgetClass;
class TextureClass;
class RenderObjClass;
class ViewUnknown34Class;
// The missions tab (0xEC bytes, tables 0xAF3C9C / 0xAF3C90 / 0xAF3C84; 0x5738B0 makes it, the second of the character
// info window's tabs): a 'TSI22A' mesh, the strings of 'fa_TSI22A.ini', a slider, a description panel and the current
// missions (three a page)
class UIMissnClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\Client\ui_missn.cpp
{
public:
	enum { MISSIONS_PER_PAGE = 3 };						// NAME 3: the G_ASSERTs' (l.397, 473, 528); its value from 0x5C4C90's loop
	enum { NO_CURRENT_MISSION = -1 };					// NAME 3: the G_ASSERT's (l.688); its value from the constructor's +0xC8
	enum { MAX_GADGETS = 5 };							// NAME 1: the constructor clears 5 (+0xD8..+0xE8)
	UIMissnClass(SClient *client);						// 0x5C4170 (0xEC bytes; deleting 0x5C42B0)  // NAME 1 [client]: as UIBioClass's
	virtual ~UIMissnClass();							// 0x5C42E0 ('fa_TSI22A.ini' unloaded)
	virtual void Unknown_54();							// 0x5C4610 (FAIL_IFs '!DescriptionPanel' / '!forfeit', G_ASSERTs on the button textures)
	virtual void Unknown_58();							// 0x5C4BB0 (activated: 0x5C5330(true) under a sort scope)
	virtual void Unknown_5C();							// 0x5C4C40 (deactivated: the description panel's +0x5C, 0x5C5DB0(-1))
	virtual void Unknown_60();							// 0x5C4C90 (FAIL_IFs '!sclient' / '!ad_player')
	virtual bool Unknown_68(int key);					// 0x5C4EA0 (keys 1..3 a mission, 6 the slider)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x5C4490 (the description panel made again over the view)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5C5330(bool arg);						// 0x5C5330 (G_ASSERTs l.397-450)  // NAME 1 [arg]: neutral
	void Unknown_5C5DB0(int index);						// 0x5C5DB0 (FAIL_IF '!sclient' l.663)  // NAME 1 [index]: neutral
	void Unknown_5C6610();								// 0x5C6610 (G_ASSERTs l.688-690)
	void Unknown_5C66D0();								// 0x5C66D0

	RenderObjClass *UnknownAC;							// +0xAC (its fonts added to the font manager; 'Forfeit_TSI22A')
	TextureClass *StandardBttnTexture;					// +0xB0  // NAME 3: the G_ASSERT's (l.147)
	TextureClass *SelectedBttnTexture;					// +0xB4  // NAME 3: the G_ASSERT's (l.151)
	GadgetSlider *UnknownB8;							// +0xB8 (the destructor takes its model off)
	int UnknownBC;										// +0xBC (the slider's value, as an int; 0 on activation)
	bool UnknownC0;										// +0xC0
	ViewUnknown34Class *DescriptionPanel;				// +0xC4 (made again by +0x7C)  // NAME 3: the FAIL_IF's (l.123)
	int CurrentMission;									// +0xC8 (NO_CURRENT_MISSION, else 0..2)  // NAME 3: the G_ASSERT's (l.688)
	void *CurrentMissions[MISSIONS_PER_PAGE];			// +0xCC  // NAME 3: the G_ASSERT's (l.689)
	GadgetClass *UnknownD8[MAX_GADGETS];				// +0xD8 (+0xE8: the forfeit button, 'Forfeit_TSI22A')
};
#endif // UI_MISSN_H
