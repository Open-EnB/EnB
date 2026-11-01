// Earth & Beyond client (reconstruction): app\client\ui_avsel.h, the character select window (the client's
// ui_avsel.cpp per its asserts; this header's name is provisional, NAME 1). Notes at the end of ui_avsel.cpp.
#ifndef UI_AVSEL_H  // NAME 1 [ui_avsel.h]: provisional header name for ui_avsel.cpp
#define UI_AVSEL_H

#include "intrface.h"
#include "wwstring.h"
#include "..\..\wwlib\ref_ptr.h"

class GadgetClass;
class LayerClass;
class LoginTaskClass;
class RenderObjClass;
class FlashingGadgetEvent;
class UIAvselCharacterViewClass;
class UIAvselAvatarClass;
class UIAvselEditClass;

// The character select window (ui_avsel.cpp's __FILE__; the client's RTTI: the type name of its FlashingGadgetEvent
// callback, 0xB7C068), 0x124 bytes, tables 0xAEFB20 / 0xAEFB14 / 0xAEFB08
class AvatarSelectionInterface : public InterfaceClass	// NAME 3: the client's RTTI (0xB7C068: SignalBaseClass::MemberCallback<GadgetClass,AvatarSelectionInterface,...>)
{
public:
	AvatarSelectionInterface(UIViewClass *view, LayerClass *layer);	// 0x55C650 (charview.cpp's 0x679160, step 2)  // NAME 1 [view, layer]: neutral
	virtual ~AvatarSelectionInterface();				// 0x55CCC0 (0x55CC90 the deleting one)
	virtual void Unknown_60();							// +0x60 0x55CEA0 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x55D060 (the events: the buttons 1-17, the referral dialog's)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_88();							// +0x88 0x55EBD0
	virtual void Unknown_8C();							// +0x8C 0x55ECB0
	void Unknown_55D9C0();								// 0x55D9C0 (the referral's result, every frame)
	void Unknown_55E070();								// 0x55E070 ('Avatar_Highlight', every frame)
	void Unknown_55E2E0(int text, const char *string, bool selected);	// 0x55E2E0 (FAIL_IF '!textptr', line 769)  // NAME 1 [text, string, selected]: neutral
	void Unknown_55E370(bool active);					// 0x55E370 (G_ASSERT 'chars', line 782: the five slots' avatars and names)  // NAME 1 [active]: neutral
	void Unknown_55E8E0();								// 0x55E8E0
	void Unknown_55ED20();								// 0x55ED20 (the buttons lit)
	void Unknown_55EF90();								// 0x55EF90 (the buttons dimmed)
	void Unknown_55F140();								// 0x55F140 (the selected character's texts)
	void Unknown_55F680();								// 0x55F680
	void Unknown_55F6D0(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x55F6D0 (the FlashingGadgetEvent callback: 'Login_Mouse_Over_Buttons')  // NAME 1: neutral (D10); NAME 1 [sender]: neutral
	void Unknown_55F760();								// 0x55F760 (G_ASSERT 'BuddyReferralUI', line 1160)
	void Unknown_55FBB0();								// 0x55FBB0 (G_ASSERT 'BuddyReferralUI', line 1215)
	void Unknown_55FDC0(bool on);						// 0x55FDC0 (the referral dialog's buttons)  // NAME 1 [on]: neutral
	void Unknown_55FE50(bool on);						// 0x55FE50 ('click_me', the referral button and text shown)  // NAME 1 [on]: neutral

	UIAvselCharacterViewClass *Unknown084;				// +0x84 (the view's slot +0x28)
	LoginTaskClass *Unknown088;							// +0x88 (the character view's task)
	int Unknown08C;										// +0x8C
	int Unknown090;										// +0x90 (the selected slot, -1 for none)
	bool Unknown094;									// +0x94
	bool Unknown095;									// +0x95
	bool Unknown096;									// +0x96
	bool Unknown097;									// +0x97
	bool Unknown098;									// +0x98
	GadgetClass *Unknown09C[8];							// +0x9C (Unknown_B7B8E0's buttons)
	UIAvselAvatarClass *Unknown0BC[5];					// +0xBC (the five avatars)
	GadgetClass *Unknown0D0[5];							// +0xD0
	bool Unknown0E4[5];									// +0xE4
	float Unknown0EC;									// +0xEC (slot +0x60: a countdown, then +0xF4 is selected)
	float Unknown0F0;									// +0xF0 (slot +0x60: a countdown, 30 after a selection)
	int Unknown0F4;										// +0xF4
	bool Unknown0F8;									// +0xF8
	bool Unknown0F9;									// +0xF9
	bool Unknown0FA;									// +0xFA
	bool Unknown0FB;									// +0xFB
	bool Unknown0FC;									// +0xFC
	bool Unknown0FD;									// +0xFD
	int Unknown100;										// +0x100
	LayerClass *Layer;									// +0x104 (the constructor's)  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> BuddyReferralUI;		// +0x108 (G_ASSERT 'BuddyReferralUI')  // NAME 3: the assert's
	GadgetClass *Unknown10C[2];							// +0x10C (Unknown_B7B960's buttons)
	UIAvselEditClass *Unknown114;						// +0x114
	GadgetClass *Unknown118;							// +0x118 ('click_me')
	StringClass Unknown11C;								// +0x11C
	bool Unknown120;									// +0x120
};

#endif // UI_AVSEL_H
