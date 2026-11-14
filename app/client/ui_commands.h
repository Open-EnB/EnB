// Earth & Beyond client (reconstruction): app\client\ui_commands.h, the slash commands window (its model 'tsi29a',
// 'fa_tsi29a.ini'; the tooltips' section 'UI_COMMANDS'; the list from 'slash_commands.ini'). No string names the file:
// ui_commands.cpp and this header are provisional names (NAME 1, D137), between ui_combobox.cpp and ui_cpit.cpp in the
// client (its object 0x57BAD0-0x57D530).
#ifndef UI_COMMANDS_H  // NAME 1 [ui_commands.h]: provisional header name
#define UI_COMMANDS_H

#include <string>
#include <vector>
#include "ui_chat_panel_base.h"
#include "..\..\wwlib\ref_ptr.h"

class FontSys;
class GadgetSlider;
class UIViewClass;

// One entry of slash_commands.ini (0x20 bytes: a section 'SLASH-%03d', its keys 'Command' and 'Desc'; destructor 0x57C820)
struct SlashCommandStruct  // NAME 1: neutral, after the file it is read from
{
	SlashCommandStruct() : Command(""), Desc("") {}		// (0x57C480 builds each with two empty strings)
	std::string Command;								// +0x00 (the key 'Command', default 'Undefined')  // NAME 3: the ini key
	std::string Desc;									// +0x10 (the key 'Desc', default 'No Description available')  // NAME 3: the ini key
};

// The slash commands window (0xFC bytes, tables 0xAF0B9C / 0xAF0B90 / 0xAF0B84; the chat panel's 0x569A00 makes it):
// seven rows of command names under a slider, the selected one's description in the 'WRAP_DESC_TSI29A' text
class UICommandsClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_COMMANDS'
{
public:
	UICommandsClass(SClient *client);					// 0x57BAD0 (deleting 0x57BBB0)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UICommandsClass();							// 0x57BBE0
	virtual void Unknown_54();							// +0x54 0x57BE50 ('tsi29a': the buttons 'exit', 'add', the rows 'name00'-'name06', the slider, the list)
	virtual void Unknown_58();							// +0x58 0x57C080 (when not Active: the base's, the rows from the top, the buttons on)
	virtual void Unknown_5C();							// +0x5C 0x57C1C0 (when Active: the buttons off, InterfaceClass's)
	virtual void Unknown_60();							// +0x60 0x57C210 (every frame: the rows from the slider, the selected one lit)
	virtual bool Unknown_68(int key);					// +0x68 0x57C320 (1 exit, 2 add: close; 4-10 the rows: select)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x57C450 (InterfaceClass's unless already on the view)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_57C480();								// 0x57C480 (the list from slash_commands.ini, 'SLASH-000' on until a section is missing)
	void Unknown_57C8B0(int count);						// 0x57C8B0 (the slider's range, then 0x57C980; inlined into 0x57C080)  // NAME 1 [count]: what 0x57C080 passes
	void Unknown_57C980();								// 0x57C980 (the top row from the slider, then the rows' texts; inlined)
	void Unknown_57CA20();								// 0x57CA20 (the selected row lit; inlined into 0x57C210)
	void Unknown_57CAA0();								// 0x57CAA0 (the rows' texts from the top row; inlined)
	void Unknown_57CB20(bool on);						// 0x57CB20 (the buttons' slot +0x40; inlined)  // NAME 1 [on]: GadgetClass's
	void Unknown_57CB60(int row);						// 0x57CB60 (a row selected: its description shown; inlined into 0x57C320)  // NAME 1 [row]: neutral

	GadgetSlider *Slider;								// +0xA0 (Add_Slider's, id 12)  // NAME 2: what it is
	int Top;											// +0xA4 (the list's index of the first row; the slider's value)  // NAME 2: what it holds
	int Selected;										// +0xA8 (the selected entry, -1 at first)  // NAME 2: what it holds
	GadgetClass *Buttons[2];							// +0xAC 'exit', 'add'  // NAME 2: what they are
	GadgetClass *Rows[7];								// +0xB4 'name00'-'name06' (ids 4-10)  // NAME 2: what they are
	RefCountPtr<FontSys> Texts[7];						// +0xD0 the rows' font styles 'name%02d_tsi29a'  // NAME 2: what they are
	std::vector<SlashCommandStruct> Commands;			// +0xEC (slash_commands.ini's entries)  // NAME 2: what it holds
};

#endif // UI_COMMANDS_H
