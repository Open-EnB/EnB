// Earth & Beyond client (reconstruction): app\client\ui_log.cpp, the log book window (no __FILE__; client
// 0x5AAE40-0x5ACB7F between ui_line_display.cpp's tables and ui_login.cpp's constructor 0x5ACB80). Notes at the end.
// PLACEMENT: provisional (D137): the file name from the tooltip section 'UI_LOG' (the windows' UI_<file> pattern) and the model 'log00a'; a __FILE__ or an assert would place it
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include "ui_chat_panel.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "gd_text_editor.h"
#include "textgadg.h"
#include "option.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// The text gadgets' flags 0x40 and 4 (textgadg.h's run 0xB08C90-0xB08CC4)
extern const int Unknown_B08C98;			// 0xB08C98 (4)  // NAME 1: neutral (D10)
extern const int Unknown_B08CA8;			// 0xB08CA8 (0x40)  // NAME 1: neutral (D10)

// 0x8192C0: a printf into a std::string (ui_guild.cpp's)
std::string Unknown_8192C0(const char *format, ...);	// NAME 1: neutral (D10)

// The log book: a page of free text per character and page number, saved as player%d_log-%d.ini (0xCC bytes;
// constructor 0x5AAE40 over ui_chat_panel_base.cpp's 0x56D280, destructor 0x5AAF60, tables 0xAF2A74 / 0xAF2A68 / 0xAF2A5C)
class UILogClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltip section 'UI_LOG'
{
public:
	UILogClass(SClient *client);						// 0x5AAE40  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UILogClass();								// 0x5AAF60 (deleting 0x5AAF30)
	virtual void Unknown_54();							// +0x54 0x5AB030 ('log00a', 'fa_log00a.ini': the buttons, the slider, the editor, the page entry)
	virtual void Unknown_58();							// +0x58 0x5AB3F0 (activated: the page from the options, loaded)
	virtual void Unknown_60();							// +0x60 0x5AB510 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x5AB720 (the buttons 1-5, the editor 7, the entry's 8 and the slider 9)  // NAME 1 [key]: InterfaceClass's
	void Previous_Page();								// 0x5AB9E0  // NAME 2: what it does
	void Next_Page();									// 0x5AB8B0  // NAME 2: what it does
	void Insert_Date();									// 0x5ABB10 ('%B %d, %Y: ' at the cursor)  // NAME 2: what it does
	void Load_Page();									// 0x5ABCC0 (player%d_log-%d.ini's LINE-%04d)  // NAME 2: what it does
	void Save_Page();									// 0x5AC0D0 (the same, unless the text is '<blank>')  // NAME 2: what it does
	std::string Escape(const std::string &text);		// 0x5AC580 (';' to '/:,': the ini's comment character)  // NAME 2: what it does; NAME 1 [text]: neutral
	std::string Unescape(const std::string &text);		// 0x5AC750 ('/:,' back to ';')  // NAME 2: what it does; NAME 1 [text]: neutral

	GadgetClass *Buttons[6];							// +0xA0 by id ('exit2', 'previous', 'next', 'date_stamp', 'Entry_Field00', ids 1-5; [0] unused)  // NAME 1: neutral
	GadgetSlider *Slider;								// +0xB8 (id 9)  // NAME 1: neutral
	TextEditorGadgetClass *Editor;						// +0xBC ('message', id 7)  // NAME 1: neutral
	TextGadgetClass *PageEntry;							// +0xC0 ('PAGE_EDIT_log00a' on 'Entry_Field00', id 8)  // NAME 1: neutral
	int Page;											// +0xC4 (1-999; the option 'Current_Log_Page')  // NAME 2: what it holds
	bool Unknown0C8;									// +0xC8 (a date inserted: the editor redrawn next frame)
	bool Unknown0C9;									// +0xC9 (the page entry open)
};

// The buttons' models and their texts (0xB80F58, 0xB80F74)
static const char *Unknown_B80F58[] = { "exit2", "previous", "next", "date_stamp", "Entry_Field00" };	// NAME 1: neutral (D10)
static const char *Unknown_B80F74[] = { NULL, "PREV_log00a", "NEXT_log00a", "DATESTAMP_log00a", NULL };	// NAME 1: neutral (D10)

// 0x5AAE40
UILogClass::UILogClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Editor(NULL),
	Page(1),
	Unknown0C8(false),
	Unknown0C9(false)
{
	for (int i = 0; i < 6; i++) {
		Buttons[i] = NULL;
	}
	Page = OptionClass::Instance->Get_Int("Current_Log_Page");
	Page = (Page >= 999) ? 999 : ((Page < 1) ? 1 : Page);
}

// 0x5AAF60
UILogClass::~UILogClass()
{
	Save_Page();
	GadgetClass::Destroy(Editor);
	Editor = NULL;
	GadgetClass::Destroy(PageEntry);
	PageEntry = NULL;
	GadgetClass::Destroy(Slider);
	Slider = NULL;
}

// NOT EXACT: the buttons' loop (the client steps the table offset and the id in two registers) and the registers after it (0x5AB030)
// 0x5AB030
void UILogClass::Unknown_54()
{
	if (Unknown_98("log00a", "fa_log00a.ini")) {
		InterfaceSortScopeClass sort(this);
		for (int i = 0; i < 5; i++) {
			Buttons[i + 1] = Unknown_56D510(i + 1, Unknown_B80F58[i], Unknown_B80F74[i]);
			GadgetClass::Set_Tooltip(Buttons[i + 1], "UI_LOG", Unknown_B80F58[i]);
		}
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
		Slider = new GadgetSlider(false);
		Slider->Unknown_66E460(*(const RefCountPtr<RenderObjClass> *)&Unknown84, 0, 0);
		Slider->Unknown_66FA90(0, 0, true);
		RefCountPtr<RenderObjClass> message = Create_NEW(Unknown84->Get_Sub_Object_By_Name("message"));
		Editor = new TextEditorGadgetClass(this, message, "UI_LOG_FONT", Slider, 0x400, "");
		GadgetClass::Set_Tooltip(Editor, "UI_LOG", "DETAILS");
		Editor->Client = Client->As_Space_Client();
		RefCountPtr<RenderObjClass> entry = Create_Get(Unknown84->Get_Sub_Object_By_Name("Entry_Field00"));
		PageEntry = new TextGadgetClass(this, 3, "PAGE_EDIT_log00a", entry.Peek(), Unknown_B08CA8 | Unknown_B08C98, 0);
		PageEntry->Set_Text("", 0);
		PageEntry->Unknown58 = 0.95f;
		PageEntry->Changed = true;
		GadgetClass::Set_Tooltip(PageEntry, "UI_LOG", "PAGE");
		Unknown64.Unknown_6870E0(Slider, 9, View, 1, 1);
		Unknown64.Unknown_6870E0(Editor, 7, View, 1, 1);
		Unknown64.Unknown_6870E0(PageEntry, 8, View, 1, 1);
		Unknown_10();
	}
}

// 0x5AB3F0
void UILogClass::Unknown_58()
{
	Page = OptionClass::Instance->Get_Int("Current_Log_Page");
	Page = (Page >= 999) ? 999 : ((Page < 1) ? 1 : Page);
	PageEntry->Set_Text(Unknown_8192C0("%d", Page).c_str(), 0);
	Load_Page();
	ChatPanelUIBase::Unknown_58();
}

// 0x5AB510
void UILogClass::Unknown_60()
{
	Editor->Unknown_672AD0();
	PageEntry->Unknown_6AAD10();
	if (Unknown0C8 && Editor->Unknown_6732A0()) {
		((RObjGadgetClass *)Buttons[4])->Unknown_66C360();
		Editor->Update_Lines(true);
		Editor->Unknown_672A60();
		Unknown0C8 = false;
	}
	if (Page <= 1) {
		Buttons[2]->Unknown_40(false);
	} else {
		Buttons[2]->Unknown_40(true);
	}
	if (Page >= 999) {
		Buttons[3]->Unknown_40(false);
	} else {
		Buttons[3]->Unknown_40(true);
	}
	if (Unknown0C9 && !PageEntry->Active) {
		Save_Page();
		int page = atoi(PageEntry->Get_Text());
		Page = (page >= 999) ? 999 : ((page < 1) ? 1 : page);
		PageEntry->Set_Text(Unknown_8192C0("%d", Page).c_str(), 0);
		Load_Page();
		Unknown0C9 = false;
	}
}

// 0x5AB720
bool UILogClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
		{
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Save_Page();
			Unknown_5C();
			Unknown98 = 1;
			IntOptionStruct *option = OptionClass::Instance->Find_Int("Current_Log_Page");
			if (option) {
				option->Value = Page;
				OptionClass::Instance->Save();
			}
			return(true);
		}
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Previous_Page();
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Next_Page();
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Insert_Date();
			return(true);
		case 5:
			PageEntry->Set_Text("", 0);
			PageEntry->Unknown_74(true);
			Unknown0C9 = true;
			return(true);
		case 7:
			if (PageEntry->Active) {
				PageEntry->Unknown_74(false);
			}
			return(true);
		case 9:
			Editor->Unknown_672A60();
			break;
	}
	return(false);
}

// 0x5AB8B0
void UILogClass::Next_Page()
{
	if (PageEntry->Active) {
		PageEntry->Unknown_74(false);
		return;
	}
	if (Page < 999) {
		Save_Page();
		Page++;
		Load_Page();
		PageEntry->Set_Text(Unknown_8192C0("%d", Page).c_str(), 0);
	}
}

// 0x5AB9E0
void UILogClass::Previous_Page()
{
	if (PageEntry->Active) {
		PageEntry->Unknown_74(false);
		return;
	}
	if (Page > 1) {
		Save_Page();
		Page--;
		Load_Page();
		PageEntry->Set_Text(Unknown_8192C0("%d", Page).c_str(), 0);
	}
}

// NOT EXACT: registers only (the client keeps this in ebp, the length in ebx) (0x5ABB10)
// 0x5ABB10
void UILogClass::Insert_Date()
{
	if (PageEntry->Active) {
		PageEntry->Unknown_74(false);
		return;
	}
	time_t now = time(NULL);
	char date[100];
	strftime(date, sizeof(date), "%B %d, %Y: ", localtime(&now));
	std::string text(date);
	Editor->Insert_Text(text);
	Editor->Unknown_6732A0();
	Unknown0C8 = true;
}

// 0x5ABCC0
void UILogClass::Load_Page()
{
	std::string text("<blank>");
	Editor->Set_Unknown60(true);
	char name[0x40];
	_snprintf(name, sizeof(name), "player%d_log-%d.ini", OptionClass::Instance->Get_Character_ID(), Page);
	INIClass ini(name);
	int count = ini.Get_Int("NUM_LINES", "LINES", 0);
	int i = 0;
	while (i < count) {
		char key[0x40];
		_snprintf(key, sizeof(key), "LINE-%04d", i);
		StringClass line;
		ini.Get_String(line, key, "TEXT", "");
		Editor->Set_Unknown60(false);
		if (i == 0) {
			text = line;
		} else {
			text += line;
		}
		if (i != count - 1) {
			text += "\n";
		}
		i++;
	}
	Editor->Set_Text(Unescape(text));
	Editor->Unknown_672A60();
}

// 0x5AC0D0
void UILogClass::Save_Page()
{
	std::string blank("<blank>");
	std::string text = Editor->Get_Text();
	std::vector<std::string> lines;
	char buffer[0x1000];
	char line[0x1000];
	_snprintf(buffer, sizeof(buffer), "%s", text.c_str());
	int n = 0;
	for (char *p = buffer; ; p++) {
		if (*p == '\n' || *p == '\0') {
			line[n] = '\0';
			std::string s("");
			if (line && n > 0) {
				s = line;
			}
			lines.push_back(s);
			n = 0;
			if (*p == '\0') {
				break;
			}
		} else {
			line[n] = *p;
			n++;
		}
	}
	if (text == blank) {
		return;
	}
	char name[0x40];
	_snprintf(name, sizeof(name), "player%d_log-%d.ini", OptionClass::Instance->Get_Character_ID(), Page);
	INIClass ini(name);
	int count = lines.size();
	ini.Put_Int("NUM_LINES", "LINES", count, 0);
	for (int i = 0; i < count; i++) {
		char key[0x40];
		_snprintf(key, sizeof(key), "LINE-%04d", i);
		ini.Put_String(key, "TEXT", Escape(lines[i]).c_str());
	}
	ini.Save(name);
}

// 0x5AC580
std::string UILogClass::Escape(const std::string &text)
{
	std::string result(text);
	int pos;
	while ((pos = result.find(";", 0, strlen(";"))) >= 0) {
		result.replace(pos, 1, "/:,");
	}
	return(result);
}

// 0x5AC750
std::string UILogClass::Unescape(const std::string &text)
{
	std::string result(text);
	int pos;
	while ((pos = result.find("/:,", 0, strlen("/:,"))) >= 0) {
		result.replace(pos, strlen("/:,"), ";");
	}
	return(result);
}

/*
**	Reconstruction notes (TM1, 2026-11-09)
**
**	The object runs 0x5AAE40-0x5ACB7F: the constructor, the deleting destructor 0x5AAF30, the destructor, the slots
**	+0x54-+0x68 and the page helpers in address order, then std::string's replace(pos, n, s, n) 0x5AC930 (the
**	escapes' out-of-line instance) and the deleting destructor's two this-adjusting entries 0x5ACB60 / 0x5ACB70
**	(tables 0xAF2A68, 0xAF2A5C). ui_login.cpp's constructor follows at 0x5ACB80. Its data runs 0xB80F58-0xB810D7
**	('Missions' at 0xB80F44 is ui_line_display.cpp's), before ui_login.cpp's button names 0xB810DC.
*/
