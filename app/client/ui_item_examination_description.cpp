#include "ui_item_examination_description.h"

// (the dead Text.Peek() on the assignment's left spends the inline budget: with the named Vector2 local, the client's
// frame, the string's _Eos called and its destructor inline; D272, attempts.csv 0x534CA0)
UIItemExaminationDescriptionClass::UIItemExaminationDescriptionClass() :
	DynamicWindowBackgroundClass("ITEM_EXAMINATION_DESCRIPTION", 0x16)
{
	Vector2 size(150.0f, 60.0f);
	(Text.Peek(), Text) = DynamicWindowMultiLineTextClass::Create("DETAILS00_IEI00", size, 0x56);
	Set_Contents(Text);
}

UIItemExaminationDescriptionClass::~UIItemExaminationDescriptionClass()
{
	Remove_All_Children(true);
}

void UIItemExaminationDescriptionClass::Clear()
{
	Description = "";
	Text->Set_Text("");
}

void UIItemExaminationDescriptionClass::Set_Description(const std::string &text)
{
	if (Description != text) {
		Description = text;
		Vector2 min_size(150.0f, 60.0f);
		Text->Set_Min_Size(min_size);
		Text->Set_Text(Description);
	}
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: assumed e:\build\beta\G\app\Client\ui_item_examination_description.cpp (interim, D137: the client
**	holds no string of this file; named after its window section 'ITEM_EXAMINATION_DESCRIPTION', as
**	ui_item_examination_attributes.cpp). The object 0x534CA0-0x5352BF, between the attributes panel and
**	ui_item_examination_options.cpp: the constructor 0x534CA0, the destructor 0x534F80 (deleting 0x534F50),
**	Clear 0x535060 and Set_Description 0x5351D0 exact; its guarded initialiser 0x5352A0 is byte-identical to
**	ui_item_examination.cpp's 0x532B40, which takes the pair.
*/
