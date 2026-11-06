#include "ui_item_examination_specials.h"
#include "dynamic_window_text.h"
#include "..\shared\unknown_debug.h"
#include <algorithm>

UIItemExaminationSpecialsClass::UIItemExaminationSpecialsClass(UIDynamicWindowUnknown84Class *listener) :
	DynamicWindowBackgroundClass("ITEM_EXAMINATION_SPECIALS", 6),
	Listener(listener)
{
	RefCountPtr<DynamicWindowVerticalClass> contents = Create_NEW(new DynamicWindowVerticalClass(6));
	RefCountPtr<DynamicWindowTextClass> title = DynamicWindowTextClass::Create("SPECIALS_IEI00", "Specials", 0);
	contents->Add_Element(title);
	List = Create_NEW(new DynamicWindowVerticalClass(0x16));
	contents->Add_Element(List);
	Set_Contents(contents);
}

UIItemExaminationSpecialsClass::~UIItemExaminationSpecialsClass()
{
	List->Remove_All_Children(true);
	Set_Contents(NULL);
	Specials.clear();
	List = NULL;
}

void UIItemExaminationSpecialsClass::Add_Special(ClientItemPropertySpecialClass *special)
{
	RefCountPtr<ClientItemPropertySpecialClass> ptr = Create_Peek(special);
	Specials.push_back(ptr);
}

void UIItemExaminationSpecialsClass::Rebuild()
{
	std::sort(Specials.begin(), Specials.end());
	for (std::vector< RefCountPtr<ClientItemPropertySpecialClass> >::iterator it = Specials.begin(); it != Specials.end(); ++it) {
		RefCountPtr<UIItemExaminationSpecialClass> row = Create_NEW(new UIItemExaminationSpecialClass(*it, Listener));
		List->Add_Element(row);
	}
}

void UIItemExaminationSpecialsClass::Clear()
{
	List->Remove_All_Children(true);
	Specials.clear();
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: assumed e:\build\beta\G\app\Client\ui_item_examination_specials.cpp (interim, D137: the client
**	holds no string of this file; named after its window section 'ITEM_EXAMINATION_SPECIALS', as
**	ui_item_examination_attributes.cpp, whose code it repeats for ClientItemPropertySpecialClass).
**
**	Partial (IE2, run 2026-11-05-night): the constructor 0x537AD0, destructor 0x537F00 (deleting 0x537ED0), Add_Special
**	0x5380B0, Rebuild 0x538150 and Clear 0x5382B0 exact. Not yet here: the row's constructor 0x538350 and the rest of
**	the object up to the title panel (0x539790 is the sort's insertion step with the special's operator< inlined).
*/
