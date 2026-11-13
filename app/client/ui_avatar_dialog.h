// Earth & Beyond client (reconstruction): app\client\ui_avatar_dialog.h, the avatar dialog over the cockpit ('TLI01A':
// an avatar in a picture-in-picture, its name, what it says and up to five answers; the incapacitance question). The
// client's asserts name the file (e:\build\beta\G\app\Client\ui_avatar_dialog.cpp); this header's name follows it
// (NAME 1, provisional).
#ifndef UI_AVATAR_DIALOG_H  // NAME 1 [ui_avatar_dialog.h]: provisional header name for ui_avatar_dialog.cpp
#define UI_AVATAR_DIALOG_H

#include <string>
#include <vector>
#include "unknown_5bb250.h"
#include "..\..\wwlib\ref_ptr.h"

class AvatarDescClass;
class AvatarPipClass;
class ButtonGadgetClass;
class CharacterViewAvatarClass;
class RenderObjClass;
class FontSplitStringClass;
class FontSys;

// The dialog (0x1B4 bytes, tables 0xAEF844 / 0xAEF838 / 0xAEF82C; constructor 0x551FB0, made by ui_cpit.cpp's 0x57DD20
// as the cockpit's +0xE0). Buttons 1-3 'MORE' / 'SKILL' / 'TRADE' (off), 4 'EXIT', 5-9 'ANSWER00'-'ANSWER04'; the
// avatar's text wraps over the six 'Talk%02d_TLI01A_AV' lines. ui_cpit.cpp's 0x584690 fills it from the incapacitance
// packet (0x552FC0, 0x553370, 0x553500, 0x5537A0, slot +0x58) and reads the answer (0x553A40).
class UIAvatarDialogClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for ui_avatar_dialog.cpp
{
public:
	enum
	{
		UI_CLOSED = 0,									// (the destructor's G_ASSERT 'State == UI_CLOSED'; slot +0x5C sets it)  // NAME 3: the assert's
		UI_OPEN = 2,									// (slot +0x58 sets it)  // NAME 1: neutral (D10), as ui_analyze.h's
		NUM_ANSWERS = 5									// (the FAIL_IFs 'num > NUM_ANSWERS')  // NAME 3: the FAIL_IFs'
	};

	UIAvatarDialogClass(SClient *client);				// 0x551FB0 (deleting 0x5522B0)  // NAME 1 [client]: what ui_cpit.cpp passes
	virtual ~UIAvatarDialogClass();						// 0x552310 (G_ASSERT 'State == UI_CLOSED' l.85)
	virtual void Unknown_54();							// +0x54 0x5524F0 (G_ASSERT 'font' l.117: the buttons, the six text lines, the back gadget, the PIP)
	virtual void Unknown_58();							// +0x58 0x552980 (opens: the answers shown, the avatar in the PIP over 'PICTURE', 'message_1.wav')
	virtual void Unknown_5C();							// +0x5C 0x552C20 (closes; the PIP's model off)
	virtual void Unknown_60();							// +0x60 0x552C60 (the PIP every frame; closes once the answered avatar is done)
	virtual bool Unknown_68(int key);					// +0x68 0x552CD0 (1 / 4 close, 5-9 the answers)  // NAME 1 [key]: InterfaceClass's

	void Unknown_552D90(int answer);					// 0x552D90 (an answer chosen: its reply said, or closed)  // NAME 1 [answer]: what it holds
	void Unknown_552FC0(AvatarDescClass *desc);			// 0x552FC0 (FAIL_IF '!AvatarRObj' l.304, '!font' l.310 / l.319: the avatar and its name)  // NAME 1 [desc]: what 0x584690 passes
	void Unknown_553370(std::string text);				// 0x553370 (the text wrapped over 'TALK00''s width)  // NAME 1 [text]: neutral
	void Unknown_553500(std::vector<std::string> lines);	// 0x553500 (FAIL_IF 'num > NUM_ANSWERS' l.352: the answers)  // NAME 1 [lines]: neutral
	void Unknown_5537A0(std::vector<std::string> lines);	// 0x5537A0 (FAIL_IF 'num > NUM_ANSWERS' l.371: the replies)  // NAME 1 [lines]: neutral
	int Unknown_553A40();								// 0x553A40: the answer once it closes (-1 none; ui_cpit.cpp's slot +0x60: 0 Packet_008FClass, 1 the distress beacon)

	ButtonGadgetClass *Buttons[10];						// +0xAC (ids 1-9; [0] unused)  // NAME 2: what they are
	AvatarPipClass *Pip;								// +0xD4 (the avatar's picture-in-picture)  // NAME 2: what it holds
	int State;											// +0xD8 (UI_CLOSED / UI_OPEN)  // NAME 3: the assert's
	int Answer;											// +0xDC (the answer chosen, -1 none)  // NAME 2: what it holds
	bool Unknown0E0;									// +0xE0 (an answer was chosen: slot +0x60 closes once the avatar is done)
	RefCountPtr<RenderObjClass> AvatarRObj;				// +0xE4 (a CharacterViewAvatarClass: 0x552FC0 stores Create_NEW's through the template operator=, the base type's order)  // NAME 3: the FAIL_IF '!AvatarRObj'
	FontSplitStringClass *Split;						// +0xE8 (the text's lines)  // NAME 2: what it does
	std::string Text;									// +0xEC (the avatar's text)  // NAME 2: what it holds
	std::string Answers[NUM_ANSWERS];					// +0xFC (0x553500)  // NAME 2: what they hold
	std::string Replies[NUM_ANSWERS];					// +0x14C (0x5537A0: said when its answer is chosen)  // NAME 2: what they hold
	RefCountPtr<FontSys> Talk[6];						// +0x19C ('Talk%02d_TLI01A_AV')  // NAME 2: their style names
};

#endif // UI_AVATAR_DIALOG_H
