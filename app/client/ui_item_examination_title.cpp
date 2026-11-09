#include "..\shared\g_assert.h"
#include "ui_item_examination_title.h"
#include <stdio.h>

static Vector2 IconSize(32.0f, 32.0f);	// 0xBE5A90 (0x539950)  // NAME 1: what it sizes

UIItemExaminationTitleClass::UIItemExaminationTitleClass() :
	DynamicWindowHorizontalClass(6)
{
	std::string icon_name = Unknown_7EA190(100);	// NAME 1 [icon_name]: neutral
	Icon = DynamicWindowIconClass::Create(icon_name, IconSize, 0);
	Icon->Clear_Flags(6);
	RefCountPtr<DynamicWindowVerticalClass> column = Create_NEW(new DynamicWindowVerticalClass(6));	// NAME 1 [column]: neutral
	Text = DynamicWindowTextClass::Create("TITLE_IEI00", "", 6);
	Subtitles = Create_NEW(new DynamicWindowHorizontalClass(6));
	column->Insert_Element(Text, 0);
	column->Insert_Element(Subtitles, 1);
	Insert_Element(Icon, 0);
	Insert_Element(column, 1);
}

UIItemExaminationTitleClass::~UIItemExaminationTitleClass()
{
	Remove_All_Children(true);
}

void UIItemExaminationTitleClass::Clear()
{
	Text->Set_Text("");
	Subtitles->Remove_All_Children(true);
}

void UIItemExaminationTitleClass::Set_Name(const ClientItemPropertyNameClass &name)
{
	Text->Set_Text(name.Get_Name());
}

void UIItemExaminationTitleClass::Set_Icon(const ClientItemPropertyIconClass &icon)
{
	Icon->Unknown_519FE0(icon.IconTexture);
}







































void UIItemExaminationTitleClass::Set_Subtitles(const UIItemExaminationTitleSubtitlesClass &info)
{
	Subtitles->Remove_All_Children(true);
	char level[32];
	_snprintf(level, sizeof(level), "Level %d ", info.Get_Level());
	RefCountPtr<DynamicWindowTextClass> text = DynamicWindowTextClass::Create("SUBTITLE_TECH_LEVEL_IEI00", level, 0x20);
	FAIL_IF(!text) return;
	Subtitles->Insert_Element(text, 0);
	if (info.Unknown_530310()) {
		std::string damage = Unknown_49E210(info.Get_Damage_Type());	// NAME 1 [damage]: neutral
		if (damage.size() != 0) {
			damage += std::string(" ");




			text = DynamicWindowTextClass::Create("SUBTITLE_DAMAGE_TYPE_IEI00", damage, 0);
			FAIL_IF(!text) return;
			Subtitles->Add_Element(text);
		}
	}



	std::string type = Unknown_7EB4D0(info.Get_Type());	// NAME 1 [type]: neutral
	text = DynamicWindowTextClass::Create("SUBTITLE_TYPE_IEI00", type, 0x10);
	FAIL_IF(!text) return;
	Subtitles->Add_Element(text);
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_item_examination_title.cpp (its __FILE__ 0xB78C04: Set_Subtitles'
**	FAIL_IFs '!text' lines 87 / 98 / 107; the panel's code before it is taken to be this file's).
**
**	Partial (IE2, run 2026-11-05-night): the title panel's constructor 0x539980, destructor 0x539E80 (deleting 0x539E50),
**	Clear 0x539F70, Set_Name 0x53A0A0 and Set_Icon 0x53A2C0 and Set_Subtitles 0x53A2F0 exact, with the icon size's static 0xBE5A90
**	(32 x 32, its initialiser 0x539950). Not yet here: 0x53A140 (the name's text by value). The class with
**	tables 0xAEE924 / 0xAEE918 / 0xAEE90C (0x53AA30-0x53AF30) is backint.cpp's BackInterfaceClass (WE2).
*/
