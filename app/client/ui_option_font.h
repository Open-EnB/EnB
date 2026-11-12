// Earth & Beyond client (reconstruction): app\client\ui_option_font.h, the options' fonts page (its model 'opt04a',
// 'fc_option_font.ini'; the tooltips' section 'UI_OPTION_FONT'). The client's asserts name the file
// (e:\build\beta\G\app\Client\ui_option_font.cpp); this header's name follows it (NAME 1, provisional).
#ifndef UI_OPTION_FONT_H  // NAME 1 [ui_option_font.h]: provisional header name for ui_option_font.cpp
#define UI_OPTION_FONT_H

#include <string>
#include <vector>
#include "ui_chat_panel_base.h"

class FontSys;
class GadgetSlider;
class UIManuCheckButtonClass;	// unknown_7e7e00.h's check box (0x7E8030: checked or not; +0x64)

#define OPTION_FONT_SLIDERS	5	// NAME 1: neutral (D10): 'slider_text_opt_font_%02d' / 'track%02d' 00-04 (ids 6-10)

// What the page sends when a font changed (an empty type: the chat panel's 0x56C7B0 handles it)
class OptionFontInterfaceEvent {};	// NAME 5: the client's RTTI (.?AVOptionFontInterfaceEvent@@ 0xB7D3B0, the Event_Type of the callback table 0xAEFFDC)

// One font's settings as the page found them (0x10 bytes; Unknown_5C sets them back unless ok was pressed)
struct OptionFontSettingStruct	// NAME 1: neutral (D10)
{
	OptionFontSettingStruct(int index, float scale, int spacing, int unknown98) : Index(index), Scale(scale), Spacing(spacing), Unknown98(unknown98) {}	// NAME 1 [index, scale, spacing, unknown98]: FontDatabaseClass's getters
	int Index;											// +0x00 (Get_Font_File_Index)  // NAME 2: what it holds
	float Scale;										// +0x04 (Get_Scale)  // NAME 2: what it holds
	int Spacing;										// +0x08 (Get_Spacing)  // NAME 2: what it holds
	int Unknown98;										// +0x0C (Get_Unknown98)
};

// The fonts page (0x138 bytes, tables 0xAF4554 / 0xAF4548 / 0xAF453C): the chat panel's request 12 (0x56C7E0 makes it
// with the chat fonts 'FONT_CHAT_LINE_00' / '01', their labels and the title 'Chat Font Settings'). Five sliders pick
// the font shown and its file, scale, letter and line spacing; the check box applies them to every font.
class OptionFontInterface : public ChatPanelUIBase	// NAME 5: the client's RTTI (.?AVOptionFontInterface@@, the sender of MemberCallback<OptionFontInterface, ChatPanelInterface, ...> 0xB7D308)
{
public:
	OptionFontInterface(SClient *client, const char **font_names, const char **labels, const std::string &title);	// 0x5CC350 (deleting 0x5CC750; NULL-ended lists)  // NAME 1 [client, font_names, labels, title]: what it does with them
	virtual ~OptionFontInterface();						// 0x5CC780
	virtual void Unknown_54();							// +0x54 0x5CC9D0 ('opt04a': FAIL_IF 'FontNames.empty()'; the title, the buttons, the check box, the sliders, Escape)
	virtual void Unknown_58();							// +0x58 0x5CCFC0 (the base's)
	virtual void Unknown_5C();							// +0x5C 0x5CCFE0 (ok: the fonts kept; else set back; the event sent)
	virtual void Unknown_60();							// +0x60 0x5CD720 (empty)
	virtual bool Unknown_68(int key);					// +0x68 0x5CD0C0 (1 exit, 2 ok, 3 cancel, 4 the defaults; 6-10 the sliders; 12 the check box)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_9C();							// +0x9C 0x5CD740 (the window moved: the sliders laid out again)

	void Update_Slider(int index, bool init);			// 0x5CD780 (FAIL_IF '!::fontDatabase', '!slider': one slider and its text from the font shown)  // NAME 1 [Update_Slider, index, init]: what it does
	void Apply_To_All();								// 0x5CDCC0 (the font shown's settings onto the others)  // NAME 1: what it does

	UIManuCheckButtonClass *SameCheck;					// +0xA0 'check00' (id 12: every font the same)  // NAME 2: what it does
	GadgetClass *Buttons[4];							// +0xA4 'exit', 'ok', 'cancel', 'resolution' (ids 1-4)  // NAME 2: what they are
	GadgetSlider *Sliders[OPTION_FONT_SLIDERS];			// +0xB4 (ids 6-10)  // NAME 2: what they are
	RefCountPtr<RenderObjClass> Tracks[OPTION_FONT_SLIDERS];	// +0xC8 'track%02d'  // NAME 2: the sub-objects' names
	RefCountPtr<FontSys> SliderText[OPTION_FONT_SLIDERS];	// +0xDC 'slider_text_opt_font_%02d'  // NAME 2: the styles' names
	std::vector<std::string> FontNames;					// +0xF0 (the chat fonts)  // NAME 3: the FAIL_IF 'FontNames.empty()'
	std::vector<std::string> Labels;					// +0x100 (slider 0's text per font)  // NAME 2: what they are
	std::string Title;									// +0x110 ('title_opt_font')  // NAME 2: what it is
	std::vector<OptionFontSettingStruct> Settings;		// +0x120 (per font, as found)  // NAME 2: what they are
	unsigned int Selected;								// +0x130 (the font shown: slider 0)  // NAME 2: what it holds
	bool Accepted;										// +0x134 (ok pressed: Unknown_5C keeps the fonts)  // NAME 2: what it holds
};

#endif // UI_OPTION_FONT_H
