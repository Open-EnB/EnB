// Earth & Beyond client (reconstruction): app\client\ui_option_font.cpp, the options' fonts page ('opt04a').
// The client's asserts name the file; the asserts sit on its lines (D25). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_font.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_slider.h"
#include "unknown_7e7e00.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "control_keys_manager.h"
#include "unknown_signalbase.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
#define fontDatabase Unknown_BE67B8		// NAME 3: the FAIL_IF '!::fontDatabase' (0x5CD780); fontdatabase.h still calls it Unknown_BE67B8

class UnknownEventDispatcherClass;
UnknownEventDispatcherClass *Unknown_540A20();			// 0x540A20 (intrface.cpp: the interfaces' signal map)  // NAME 1: neutral (D10)

// The buttons Unknown_54 makes, ids 1-4: their models' names and their texts (0xB832D8 / 0xB832E8)
static const char *Unknown_B832D8[4] = { "exit", "ok", "cancel", "resolution" };	// NAME 1: neutral (D10)
static const char *Unknown_B832E8[4] = { NULL, "ok_opt_font", "cancel_opt_font", "next_opt_font" };	// NAME 1: neutral (D10)

// kernel.h's input event, as ui_option_main.cpp's (Unknown_54's Escape)
struct OptionFontKeyEventStruct	// NAME 1: neutral (D10)
{
	OptionFontKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
// NOT EXACT: shape (similarity 0.49; forms: indexed for, for with break, two-pointer while): the client builds the first string's copy with _Tidy out of line and the second inline
// 0x5CC350
OptionFontInterface::OptionFontInterface(SClient *client, const char **font_names, const char **labels, const std::string &title) :
	ChatPanelUIBase(client),
	SameCheck(NULL),
	Title(title),
	Selected(0),
	Accepted(false)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Sliders, 0, sizeof(Sliders));
	for (int i = 0; font_names[i] && *font_names[i] && labels[i] && *labels[i]; i++) {
		const char *name = font_names[i];				// NAME 1: neutral (D10)
		FontNames.push_back(std::string(name));
		Labels.push_back(std::string(labels[i]));
		Settings.push_back(OptionFontSettingStruct(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name)));
	}
}

// 0x5CC780
OptionFontInterface::~OptionFontInterface()
{
	for (int i = 0; i < OPTION_FONT_SLIDERS; i++) {
		if (Sliders[i]) {
			Sliders[i]->Unknown_66E460(RefCountPtr<RenderObjClass>(), false, 0);
			Sliders[i] = NULL;
		}
	}
}


























































// NOT EXACT: frame (similarity 0.74): the client's frame is 0x10 bytes larger (0xD4); the rest aligns
// 0x5CC9D0
void OptionFontInterface::Unknown_54()
{
	if (Unknown_98("opt04a", "fc_option_font.ini")) {
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&OptionFontKeyEventStruct(0x1B), (void *)1);
		FAIL_IF(FontNames.empty()) return;

		RefCountPtr<FontSys> title = Unknown_BE67C4->Unknown_6D2700("title_opt_font", InterfaceMesh);	// NAME 1: neutral (D10)
		if (title) {
			title->Change_Text(Title.c_str(), 0);
		} else {
			G_ASSERT(false);
		}
		for (int i = 0; i < 4; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B832D8[i], Unknown_B832E8[i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_FONT", Unknown_B832D8[i]);
		}
		SameCheck = Add_Check_Button(12, "check00", NULL);

		const char *name = FontNames[0].c_str();		// NAME 1: neutral (D10)
		OptionFontSettingStruct first(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name));	// NAME 1: neutral (D10)
		bool same = true;								// NAME 1: neutral (D10)
		for (int j = 1; j < (int)FontNames.size(); j++) {
			name = FontNames[j].c_str();
			same = false;
			if (first.Index != fontDatabase->Get_Font_File_Index(name)) break;
			if (first.Scale != fontDatabase->Get_Scale(name)) break;
			if (first.Spacing != fontDatabase->Get_Spacing(name)) break;
			if (first.Unknown98 != fontDatabase->Get_Unknown98(name)) break;
			same = true;
		}
		SameCheck->Unknown_7E8030(same);
		if (FontNames.size() <= 1) {
			SameCheck->Unknown_40(false);
			SameCheck->Unknown_8C(true);
			RefCountPtr<FontSys> same_text = Unknown_BE67C4->Unknown_6D2700("same_opt_font", InterfaceMesh);	// NAME 1: neutral (D10)
			if (same_text) {
				same_text->Set_Hidden(true);
			}
		}
		for (int k = 0; k < OPTION_FONT_SLIDERS; k++) {
			char text_name[128];							// NAME 1: neutral (D10)
			sprintf(text_name, "slider_text_opt_font_%02d", k);
			SliderText[k] = Unknown_BE67C4->Unknown_6D2700(text_name, InterfaceMesh);
			Sliders[k] = Add_Slider(k + 6, false);
			Sliders[k]->Unknown_66E460(InterfaceMesh, true, k);
			char track_name[32];						// NAME 1: neutral (D10)
			_snprintf(track_name, sizeof(track_name), "track%02d", k);
			Tracks[k] = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(track_name));
			Update_Slider(k, true);
		}
		Add_Back_Gadget(1, Vector3(0.0f, 0.0f, 0.0f));
		InterfaceClass::Unknown_54();
	}
}

// 0x5CCFC0
void OptionFontInterface::Unknown_58()
{
	ChatPanelUIBase::Unknown_58();
}

// 0x5CCFE0
void OptionFontInterface::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
	if (Accepted) {
		fontDatabase->Unknown_6C49C0();
		return;
	}
	for (unsigned int i = 0; i < FontNames.size(); i++) {
		fontDatabase->Set_Font(FontNames[i].c_str(), Settings[i].Index, Settings[i].Scale, Settings[i].Spacing, Settings[i].Unknown98, false);
	}
	((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
}

// 0x5CD0C0
bool OptionFontInterface::Unknown_68(int key)
{
	switch (key) {
		case 1:
		case 3:
			Accepted = false;
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			Accepted = true;
			Unknown_5C();
			Closed = 1;
			return(true);
		case 4: {
			fontDatabase->Restore_Font(FontNames[Selected].c_str());
			for (int i = 0; i < OPTION_FONT_SLIDERS; i++) {
				Update_Slider(i, false);
			}
			if (SameCheck->Checked) {
				Apply_To_All();
			}
			((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
			return(false); }
		case 12:
			Update_Slider(0, false);
			if (SameCheck->Checked) {
				Apply_To_All();
				((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
			}
			return(false);
		case 6: {
			Selected = (int)Sliders[0]->Get_Value();
			if (Selected >= FontNames.size()) {
				Selected = 0;
			}
			for (int j = 0; j < OPTION_FONT_SLIDERS; j++) {
				Update_Slider(j, false);
			}
			return(false); }
		case 7: {
			int index = (int)Sliders[1]->Get_Value();	// NAME 1: neutral (D10)
			if (SameCheck->Checked) {
				for (unsigned int i = 0; i < FontNames.size(); i++) {
					fontDatabase->Set_Font(FontNames[i].c_str(), index, 16777215.0f, 0xFFFFFF, 0xFFFFFF, false);
				}
			} else {
				fontDatabase->Set_Font(FontNames[Selected].c_str(), index, 16777215.0f, 0xFFFFFF, 0xFFFFFF, false);
			}
			Update_Slider(1, false);
			((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
			return(false);
		}
		case 8: {
			float scale = Sliders[2]->Get_Value() * 0.1f + 0.5f;	// NAME 1: neutral (D10)
			if (SameCheck->Checked) {
				for (unsigned int i = 0; i < FontNames.size(); i++) {
					fontDatabase->Set_Font(FontNames[i].c_str(), 0xFFFFFF, scale, 0xFFFFFF, 0xFFFFFF, false);
				}
			} else {
				fontDatabase->Set_Font(FontNames[Selected].c_str(), 0xFFFFFF, scale, 0xFFFFFF, 0xFFFFFF, false);
			}
			Update_Slider(2, false);
			((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
			return(false);
		}
		case 9: {
			int spacing = (int)Sliders[3]->Get_Value();	// NAME 1: neutral (D10)
			if (SameCheck->Checked) {
				for (unsigned int i = 0; i < FontNames.size(); i++) {
					fontDatabase->Set_Font(FontNames[i].c_str(), 0xFFFFFF, 16777215.0f, spacing - 3, 0xFFFFFF, false);
				}
			} else {
				fontDatabase->Set_Font(FontNames[Selected].c_str(), 0xFFFFFF, 16777215.0f, spacing - 3, 0xFFFFFF, false);
			}
			Update_Slider(3, false);
			((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
			return(false);
		}
		case 10: {
			int value = (int)Sliders[4]->Get_Value();	// NAME 1: neutral (D10)
			if (SameCheck->Checked) {
				for (unsigned int i = 0; i < FontNames.size(); i++) {
					fontDatabase->Set_Font(FontNames[i].c_str(), 0xFFFFFF, 16777215.0f, 0xFFFFFF, value - 12, false);
				}
			} else {
				fontDatabase->Set_Font(FontNames[Selected].c_str(), 0xFFFFFF, 16777215.0f, 0xFFFFFF, value - 12, false);
			}
			Update_Slider(4, false);
			((SignalMap *)Unknown_540A20())->emit(this, OptionFontInterfaceEvent());
			return(false);
		}
	}
	return(false);
}

// 0x5CD720
void OptionFontInterface::Unknown_60()
{
}

// 0x5CD740
void OptionFontInterface::Unknown_9C()
{
	for (int i = 0; i < OPTION_FONT_SLIDERS; i++) {
		Sliders[i]->Unknown_66FD80(true);
	}
	Update_Slider(0, false);
}
























































// NOT EXACT: layout (similarity 0.77): the code aligns instruction for instruction; the difference is in the switch's jump table
// 0x5CD780
void OptionFontInterface::Update_Slider(int index, bool init)
{
	FAIL_IF(!::fontDatabase) return;

	GadgetSlider *slider = Sliders[index];
	FAIL_IF(!slider) return;
	RefCountPtr<FontSys> text = SliderText[index];		// NAME 1: neutral (D10)
	switch (index) {
		case 0:
			if (init) {
				slider->Unknown_66FA90((float)Labels.size(), 1.0f, true);
			}
			slider->Unknown_A4((float)Selected, false);
			if (SameCheck->Checked) {
				if (text) {
					text->Set_Hidden(true);
				}
				slider->Unknown_8C(true);
				Tracks[0]->Set_Hidden(true);
			} else {
				if (text) {
					text->Change_Text(Labels[Selected].c_str(), 0);
					text->Set_Hidden(false);
				}
				slider->Unknown_8C(false);
				Tracks[0]->Set_Hidden(false);
			}
			break;
		case 1: {
			if (init) {
				slider->Unknown_66FA90((float)fontDatabase->AllFonts.size(), 1.0f, true);
			}
			int file_index = fontDatabase->Get_Font_File_Index(FontNames[Selected].c_str());	// NAME 1: neutral (D10)
			slider->Unknown_A4((float)file_index, false);
			if (text) {
				int length = strlen("font_");			// NAME 1: neutral (D10)
				char *file_name = (char *)fontDatabase->Get_Font_File_Name(file_index);	// NAME 1: neutral (D10)
				if (strnicmp(file_name, "font_", length) == 0) file_name += length;
				char *dot = strchr(file_name, '.');		// NAME 1: neutral (D10)
				if (dot) {
					*dot = '\0';
				}
				text->Unknown_6CE420(0, "Select Font (%s)", file_name);
				if (dot) {
					*dot = '.';
				}
			}
			break;
		}
		case 2: {
			if (init) {
				slider->Unknown_66FA90(16.0f, 1.0f, true);
			}
			float scale = fontDatabase->Get_Scale(FontNames[Selected].c_str());	// NAME 1: neutral (D10)
			if (text) {
				text->Unknown_6CE420(0, "Scale Font (%1.1f)", scale);
			}
			float position = (scale - 0.5f) / 0.1f;		// NAME 1: neutral (D10)
			slider->Unknown_A4(position, false);
			break;
		}
		case 3: {
			if (init) {
				slider->Unknown_66FA90(8.0f, 1.0f, true);
			}
			int spacing = fontDatabase->Get_Spacing(FontNames[Selected].c_str());	// NAME 1: neutral (D10)
			if (text) {
				text->Unknown_6CE420(0, "Letter Spacing (%1d)", spacing);
			}
			slider->Unknown_A4((float)(spacing + 3), false);
			break;
		}
		case 4: {
			if (init) {
				slider->Unknown_66FA90(18.0f, 1.0f, true);
			}
			int value = fontDatabase->Get_Unknown98(FontNames[Selected].c_str());	// NAME 1: neutral (D10)
			if (text) {
				text->Unknown_6CE420(0, "Line Spacing (%1d)", value);
			}
			slider->Unknown_A4((float)(value + 12), false);
			break;
		}
	}
}

// 0x5CDCC0
void OptionFontInterface::Apply_To_All()
{
	const char *name = FontNames[Selected].c_str();		// NAME 1: neutral (D10)
	OptionFontSettingStruct shown(fontDatabase->Get_Font_File_Index(name), fontDatabase->Get_Scale(name), fontDatabase->Get_Spacing(name), fontDatabase->Get_Unknown98(name));	// NAME 1: neutral (D10)
	for (unsigned int i = 0; i < FontNames.size(); i++) {
		if (i != Selected) {
			fontDatabase->Set_Font(FontNames[i].c_str(), shown.Index, shown.Scale, shown.Spacing, shown.Unknown98, false);
		}
	}
}

/*
**	Notes.
**	Object 0x5CC350-0x5CE23F: the constructor 0x5CC350 opens it, after ui_option_control_keys.cpp's adjustor thunks; the
**	template instances 0x5CDDD0 (a vector's _Tidy) and 0x5CDE10 (vector<OptionFontSettingStruct>::insert) and this page's
**	adjustor thunks 0x5CE220 / 0x5CE230 close it; ui_option_main.cpp's 0x5CE240 follows. The chat panel's 0x56C7E0
**	(request 12, not written yet) makes it and listens for OptionFontInterfaceEvent (0x56C7B0).
*/
