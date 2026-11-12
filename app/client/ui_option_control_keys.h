// Earth & Beyond client (reconstruction): app\client\ui_option_control_keys.h, the options' controls page (its model
// 'tsi31a', 'tsi31a.ini'; the tooltips' section 'UI_OPTION_CONTROL_KEYS'). The client's asserts name the file
// (e:\build\beta\G\app\Client\ui_option_control_keys.cpp); this header's name follows it (NAME 1, provisional).
#ifndef UI_OPTION_CONTROL_KEYS_H  // NAME 1 [ui_option_control_keys.h]: provisional header name for ui_option_control_keys.cpp
#define UI_OPTION_CONTROL_KEYS_H

#include <vector>
#include "ui_chat_panel_base.h"

class FontSys;
class GadgetSlider;
class ViewUnknown34Class;
class KeyPressGadget;			// gd_key_press.cpp's (0x66A700, 0x48 bytes: the key pressed while it is on)
struct KeyMappingStruct;		// control_keys_manager.h's

#define CONTROL_KEYS_ROWS	11	// NAME 1: neutral (D10): the rows the page shows ('LABEL_TEXT%02d_tsi31a' 00-10)

// The controls page (0x1B0 bytes, tables 0xAF43EC / 0xAF43E0 / 0xAF43D4): the chat panel's request 8 (the options'
// 'Control' button). Eleven rows of key definitions under a slider, each with its name, its hard-coded key or its two
// mapped keys (buttons: a click lets the key-press gadget take the next key); six buttons pick the set shown.
class UIOptionControlKeysClass : public ChatPanelUIBase	// NAME 1: neutral, after its file and tooltips' section 'UI_OPTION_CONTROL_KEYS'
{
public:
	UIOptionControlKeysClass(SClient *client, bool mode);	// 0x5CA010 (deleting 0x5CA180)  // NAME 1 [client, mode]: what it does with them
	virtual ~UIOptionControlKeysClass();				// 0x5CA1B0
	virtual void Unknown_54();							// +0x54 0x5CA330 ('tsi31a': the buttons, the rows, the slider, the key-press gadget, Escape)
	virtual void Unknown_58();							// +0x58 0x5CA910 (FAIL_IF '!key_manager'; the base's when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5CA980 (the dialog box closed; InterfaceClass's when Active)
	virtual void Unknown_60();							// +0x60 0x5CB000 (the key taken, checked and set; the dialog box's answer)
	virtual bool Unknown_68(int key);					// +0x68 0x5CA9F0 (1 exit, 2 cancel, 3 ok; 4 / 5 the default sets; 6-11 the sets; 13 the slider; 15-36 the keys)  // NAME 1 [key]: InterfaceClass's

	void Set_Context(int context);						// 0x5CB8F0 (FAIL_IF '!key_manager': the mappings of one set, the slider reset)  // NAME 1 [Set_Context, context]: ControlKeyDefStruct's
	void Refresh();										// 0x5CB9D0 (the rows from the slider's position: FAIL_IF '!KeyName[lp]' ...)  // NAME 1: neutral
	static int Key_To_Context(int key);						// 0x5CBCC0 (keys 6-11 to the sets 0, 3, 4, 1, 5, 2)  // NAME 1 [key]: Unknown_68's
	static int Context_To_Key(int context);					// 0x5CBD50 (the reverse; nothing calls it)  // NAME 1 [context]: as Set_Context's
	bool Show_Dialog_Box(const char *dialog_name, const char *text);	// 0x5CBDE0 (FAIL_IF '!dialog_name', '!DialogBoxUI': the view's dialog box)  // NAME 3 [dialog_name]: the FAIL_IF's; NAME 1 [text]: 0x588A70's
	void Set_Key_Text(GadgetClass *bttn, const std::string &text);	// 0x5CC030 (FAIL_IF '!bttn', '!font': the button's text)  // NAME 3 [bttn]: the FAIL_IF's; NAME 1 [text]: neutral

	std::vector<KeyMappingStruct *> CurrentKeys;		// +0xA0 (ControlKeysManager::Find_Mappings)  // NAME 3: the FAIL_IF 'keys_it == CurrentKeys.end()'
	int Context;										// +0xB0 (the set shown: Find_Mappings' argument)  // NAME 1: ControlKeyDefStruct's 'context'
	GadgetSlider *Slider;								// +0xB4 (id 13)  // NAME 2: what it is
	int TopRow;											// +0xB8 (the slider's position at the last Refresh)  // NAME 2: what it holds
	RefCountPtr<FontSys> KeyName[CONTROL_KEYS_ROWS];	// +0xBC 'LABEL_TEXT%02d_tsi31a'  // NAME 3: the FAIL_IF '!KeyName[lp]'
	RefCountPtr<FontSys> HardCodedKey[CONTROL_KEYS_ROWS];	// +0xE8 'PRIME_KEY%02d_tsi31a'  // NAME 3: the FAIL_IF '!HardCodedKey[lp]'
	GadgetClass *Primary[CONTROL_KEYS_ROWS];			// +0x114 'Prime_Key%02d' (ids 15-25)  // NAME 3: the FAIL_IF '!Primary[lp]'
	GadgetClass *Secondary[CONTROL_KEYS_ROWS];			// +0x140 'Alt_Key%02d' (ids 26-36)  // NAME 3: the FAIL_IF '!Secondary[lp]'
	KeyPressGadget *KeyPress;							// +0x16C (id 14)  // NAME 2: its class
	KeyMappingStruct *EditKey;							// +0x170 (the mapping whose key the gadget takes)  // NAME 2: what it holds
	bool EditPrimary;									// +0x174 (true: its first key; false: its second)  // NAME 2: what it holds
	RefCountPtr<ViewUnknown34Class> DialogBoxUI;		// +0x178  // NAME 3: the FAIL_IF '!DialogBoxUI'
	const char *DialogBoxID;							// +0x17C  // NAME 3: the FAIL_IF '!DialogBoxID'
	bool Mode;											// +0x180 (the constructor's; false: ok and cancel go back to the chat panel's page 1)  // NAME 1: as UIOptionCheckClass's
	GadgetClass *Buttons[CONTROL_KEYS_ROWS];			// +0x184 'exit', 'BUTTON03', 'BUTTON00'-'BUTTON02', 'TOP_BUTTON00'-'05' (ids 1-11)  // NAME 2: what they are
};

#endif // UI_OPTION_CONTROL_KEYS_H
