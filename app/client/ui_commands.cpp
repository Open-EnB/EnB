// Earth & Beyond client (reconstruction): app\client\ui_commands.cpp, the slash commands window ('tsi29a').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "ui_commands.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "..\input\input.h"
#include "..\audio\soundlib.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The buttons Unknown_54 makes: their models' names (0xB7E4D4), also their tooltips' entries
static const char *Unknown_B7E4D4[2] = { "exit", "add" };	// NAME 1: neutral (D10)

// @0x57BAD0
UICommandsClass::UICommandsClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	Selected(-1)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// @0x57BBE0
UICommandsClass::~UICommandsClass()
{
	Unknown_5C();
	if (Slider != NULL) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
	Commands.clear();
}

// @0x57BE50
void UICommandsClass::Unknown_54()
{
	if (Unknown_98("tsi29a", "fa_tsi29a.ini")) {
		int i;
		for (i = 0; i < 2; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B7E4D4[i], NULL);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_COMMANDS", Unknown_B7E4D4[i]);
		}
		for (i = 0; i < 7; i++) {
			char name[64];
			_snprintf(name, sizeof(name), "name%02d", i);
			Rows[i] = Add_Button(i + 4, name, NULL);
			Rows[i]->Unknown2C_0A = true;
			_snprintf(name, sizeof(name), "name%02d_tsi29a", i);
			Texts[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh.Peek()));
		}
		Slider = Add_Slider(12, false);
		Slider->Unknown_66E460(InterfaceMesh, 0, 0);
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		Unknown_57C480();
	}
}

// @0x57C080
void UICommandsClass::Unknown_58()
{
	if (!Active) {
		ChatPanelUIBase::Unknown_58();
		Top = -1;
		Unknown_57C8B0(Commands.size());
		Unknown_57CB20(true);
		if (Buttons[0] != NULL) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
	}
}

// @0x57C1C0
void UICommandsClass::Unknown_5C()
{
	if (Active) {
		Unknown_57CB20(false);
		InterfaceClass::Unknown_5C();
	}
}

// @0x57C210
void UICommandsClass::Unknown_60()
{
	Unknown_57C980();
	Unknown_57CA20();
}

// @0x57C320
bool UICommandsClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Closed = 1;
			return(true);
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_57CB60(key - 4);
			return(true);
	}
	return(false);
}

// @0x57C450
void UICommandsClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// @0x57C480
void UICommandsClass::Unknown_57C480()
{
	Commands.clear();
	INIClass ini("slash_commands.ini");
	if (!ini.Is_Loaded()) {
		return;
	}
	int i = 0;
	while (true) {
		{
			char section[64];
			_snprintf(section, sizeof(section), "SLASH-%03d", i);
			if (ini.Find_Section(section) == NULL) {
				return;
			}
			SlashCommandStruct command;
			StringClass value;
			ini.Get_String(value, section, "Command", "Undefined");
			if ((const char *)value != NULL) {
				command.Command = (const char *)value;
			}
			ini.Get_String(value, section, "Desc", "No Description available");
			if ((const char *)value != NULL) {
				command.Desc = (const char *)value;
			}
			Commands.push_back(command);
		}
		i++;
	}
}

// @0x57C8B0
void UICommandsClass::Unknown_57C8B0(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_57C980();
}

// @0x57C980
void UICommandsClass::Unknown_57C980()
{
	int value = (int)Slider->Value;
	if (Top != value) {
		Top = value;
		Unknown_57CAA0();
	}
}

// @0x57CA20
void UICommandsClass::Unknown_57CA20()
{
	float x, y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		if (Top + i == Selected) {
			Texts[i]->Unknown_6CE910(true);
		} else {
			Texts[i]->Unknown_6CE910(false);
		}
	}
}

// @0x57CAA0
void UICommandsClass::Unknown_57CAA0()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;
		if (index < Commands.size()) {
			Texts[i]->Change_Text(Commands[index].Command.c_str(), 0);
		} else {
			Texts[i]->Change_Text("", 0);
		}
	}
}

// @0x57CB20
void UICommandsClass::Unknown_57CB20(bool on)
{
	for (int i = 0; i < 2; i++) {
		if (Buttons[i] != NULL) {
			Buttons[i]->Unknown_40(on);
		}
	}
}

// @0x57CB60
void UICommandsClass::Unknown_57CB60(int row)
{
	Selected = Top + row;
	if ((unsigned int)Selected < Commands.size()) {
		ManagedFontClass *font = Unknown_BE67C4->Find(InterfaceMesh.Peek());	// NAME 1: neutral (D10)
		if (font != NULL) {
			font->Unknown_6D0A70("WRAP_DESC_TSI29A", Commands[Selected].Desc.c_str());
		}
	}
}

/*
** Notes.
**	The object 0x57BAD0-0x57D530: the constructor first, the guarded <string> initialiser 0x57D4F0 and the adjustor
**	thunk 0x57D520 (the second table's deleting destructor) last; ui_combobox.cpp's thunks 0x57BAB0 / 0x57BAC0 before
**	it, the next object's PermStringInitClass initialiser 0x57D530 after. 0x57CC00 (the vector's destructor, the
**	constructor's unwind), 0x57CC50, 0x57CC90, 0x57D070, 0x57D340 are the std::vector<SlashCommandStruct> instances
**	its code makes; 0x57C820 SlashCommandStruct's destructor.
**	Forms the bytes settle (appfile.py match, every function exact): clear() in the destructor and in 0x57C480 (an
**	explicit erase inlines std::copy, the client calls 0x57D070 from 0x57C480); 0x57C480's loop body in its own block
**	with the index counted after it (the entry's strings freed before the increment), the index declared before the
**	loop, a NULL test on each Get_String result; the rows' if / else calls (0x57CA20, 0x57CAA0: two pushes, not a
**	sete or a ?:); memset of Buttons in the constructor (both stores through ecx).
*/
